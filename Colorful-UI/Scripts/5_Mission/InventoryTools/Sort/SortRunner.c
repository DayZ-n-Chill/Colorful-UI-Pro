// Runs one container sort: snapshots the cargo, asks Packer for the target layout, then makes
// the real inventory moves one at a time, waiting for each to land before planning the next.
//
// One move at a time because on a multiplayer client every cargo move is a server round trip
// (PlayerBase.NeedInventoryJunctureFromServer, playerbase.c:5344-5358); firing them all at once would
// have most of them refused against reservations the earlier ones still hold. Everything here is
// client side: the only thing the server ever sees is the ordinary moves.
//
// Ticked from InventoryMenu.Update (SortHooks.c), so it only advances while the inventory is
// open, and InventoryMenu.OnHide aborts it.
class SortRunner
{
	protected static const int ST_IDLE = 0;
	protected static const int ST_WAIT_READY = 1;
	protected static const int ST_WAIT_LAND = 2;

	// Give up if the inventory stays busy, or a move does not land, for this long.
	protected static const int READY_TIMEOUT_MS = 3000;
	protected static const int LAND_TIMEOUT_MS = 3500;
	// A frame gap longer than this means InventoryMenu.Update was not running (another screen such as
	// inspect was on top). That time must not count against the timeouts.
	protected static const int MAX_TICK_MS = 250;

	protected static EntityAI s_Owner;
	protected static int s_Idx;
	// Raw on purpose: SortButton is Managed, so this goes null if the header is destroyed mid-sort.
	protected static SortButton s_Btn;
	protected static ref array<ref PackItem> s_Plan;
	protected static ref array<EntityAI> s_Ents;
	protected static int s_State = ST_IDLE;
	protected static int s_Pending = -1;	// index into s_Plan of the move in flight
	// Where the move in flight is going: its target, a staging spot, or a gravity step.
	protected static int s_DstRow;
	protected static int s_DstCol;
	protected static bool s_DstFlip;
	// The planned layout could not be reached (too crowded to shuffle through): pack up into the top
	// left instead (Packer.NextGravity).
	protected static bool s_Gravity;
	// Cleared once the engine refuses a move onto cells the item itself covers.
	protected static bool s_SelfOk;
	protected static int s_WaitMs;
	protected static int s_LastTick;
	protected static int s_Done;
	protected static int s_Total;
	protected static int s_MoveCap;
	protected static int s_Moves;
	// Merges StackRunner made before handing over, so the final chip can say "Stacked 4, sorted".
	protected static int s_Merged;
	// How the last sort ended (a SortButton state), or -1 since ClearLast. SortAll reads it
	// to count the containers that came out sorted, since it runs them with no button of their own.
	protected static int s_LastState = -1;

	static bool IsRunning()
	{
		return s_State != ST_IDLE;
	}

	// The card whose cargo is being sorted covers its grid with a spinner until the sort is done
	// (SortCover), so the player sees the result rather than every intermediate move.
	static bool IsSortingFor(EntityAI v_Owner, int v_Idx)
	{
		return s_State != ST_IDLE && v_Owner && s_Owner == v_Owner && s_Idx == v_Idx;
	}

	static int LastState()
	{
		return s_LastState;
	}

	static void ClearLast()
	{
		s_LastState = -1;
	}

	// Whether a container gets a sort button at all: a real (not proxy) cargo, on the local player or
	// on something the player can take from - the ground, a car, a tent, a zombie or a dead body. A
	// living player's root means another player's gear, which is never the player's to rearrange.
	static bool CanOffer(EntityAI v_Owner, int v_Idx)
	{
		if (!v_Owner || v_Idx < 0)
			return false;

		CargoBase v_Cargo = v_Owner.GetInventory().GetCargoFromIndex(v_Idx);
		if (!v_Cargo || v_Cargo.IsProxyCargo())
			return false;

		Man v_Root = v_Owner.GetHierarchyRootPlayer();
		if (!v_Root || v_Root == GetGame().GetPlayer())
			return true;
		return !v_Root.IsAlive();
	}

	// v_Merged is non-zero only when StackRunner hands over after merging stacks.
	static void Start(EntityAI v_Owner, int v_Idx, SortButton v_Btn, int v_Merged = 0)
	{
		// One sort or stack at a time; clicks on any other button are ignored while one runs.
		if (IsRunning() || StackRunner.IsRunning())
			return;

		s_Merged = v_Merged;
		s_Btn = v_Btn;
		s_Owner = v_Owner;
		s_Idx = v_Idx;

		PlayerBase v_Player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!v_Player || !v_Owner || !v_Player.CanManipulateInventory() || v_Owner.GetInventory().IsInventoryLocked())
		{
			Finish(SortButton.STATE_FAILED, SortStrings.STOPPED);
			return;
		}

		CargoBase v_Cargo = GetCargo();
		if (!v_Cargo)
		{
			Finish(SortButton.STATE_FAILED, SortStrings.CHANGED);
			return;
		}

		s_Plan = new array<ref PackItem>;
		s_Ents = new array<EntityAI>;
		EntityAI v_InHands = v_Player.GetHumanInventory().GetEntityInHands();
		InventoryLocation v_Il = new InventoryLocation;

		int v_Count = v_Cargo.GetItemCount();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			EntityAI v_E = v_Cargo.GetItem(v_I);
			if (!v_E)
				continue;

			PackItem v_Item = new PackItem;
			v_Item.m_Key = s_Ents.Count();
			s_Ents.Insert(v_E);

			// The item's own unflipped size. ItemBase.GetItemSize is a script stub, so never that.
			int v_W, v_H;
			g_Game.GetInventoryItemSize(InventoryItem.Cast(v_E), v_W, v_H);
			v_Item.m_W = v_W;
			v_Item.m_H = v_H;
			v_Item.m_CanFlip = v_Item.m_W != v_Item.m_H;
			v_Item.m_Group = v_E.GetType();
			v_Item.m_Cat = SortCategory.Of(v_E);
			string v_Name = v_E.GetDisplayName();
			v_Name.ToLower();
			v_Item.m_Name = v_Name;

			v_E.GetInventory().GetCurrentInventoryLocation(v_Il);
			v_Item.m_Row = v_Il.GetRow();
			v_Item.m_Col = v_Il.GetCol();
			v_Item.m_Flip = v_Il.GetFlip();

			// Anything the player or the engine has a claim on stays exactly where it is.
			// One condition per statement: Enforce Script rejects a continuation line that starts with || or &&.
			bool v_Pinned = !v_E.GetInventory().CanRemoveEntity() || v_E.GetInventory().IsInventoryLocked();
			if (!v_Pinned && v_Player.GetHumanInventory().FindUserReservedLocationIndex(v_E) >= 0)
				v_Pinned = true;
			if (!v_Pinned && GameInventory.HasInventoryReservation(v_E, null))
				v_Pinned = true;
			if (!v_Pinned && v_E == v_InHands)
				v_Pinned = true;
			v_Item.m_Pinned = v_Pinned;

			s_Plan.Insert(v_Item);
		}

		// Ghost reservations: the cells an item in hands (or on the quickbar) will drop back into. Planned
		// around as anonymous pinned blocks so the sort never fills a spot the player is coming back to.
		int v_Ghosts = v_Cargo.GetUserReservedLocationCount();
		for (int v_G = 0; v_G < v_Ghosts; v_G++)
		{
			EntityAI v_GE;
			int v_GRow, v_GCol, v_GW, v_GH, v_GFlip;
			if (!v_Cargo.GetUserReservedLocation(v_G, v_GE, v_GRow, v_GCol, v_GW, v_GH, v_GFlip))
				continue;

			PackItem v_Ghost = new PackItem;
			v_Ghost.m_Key = -1;
			v_Ghost.m_Pinned = true;
			v_Ghost.m_Row = v_GRow;
			v_Ghost.m_Col = v_GCol;
			// Prefer the reserved item's own size with the reserved flip, the same model as every other
			// item. The record's own w/h are only used if the item is gone, and then taken as the
			// footprint as-is (whether they are pre-flipped is not verified).
			InventoryItem v_GInv = InventoryItem.Cast(v_GE);
			if (v_GInv)
			{
				int v_IW, v_IH;
				g_Game.GetInventoryItemSize(v_GInv, v_IW, v_IH);
				v_Ghost.m_W = v_IW;
				v_Ghost.m_H = v_IH;
				v_Ghost.m_Flip = v_GFlip != 0;
			}
			else
			{
				v_Ghost.m_W = v_GW;
				v_Ghost.m_H = v_GH;
				v_Ghost.m_Flip = false;
			}
			s_Plan.Insert(v_Ghost);
		}

		Packer.Order(s_Plan);
		// No full layout found (pinned items or odd shapes in the way): go straight to packing up into
		// the top left rather than refusing.
		bool v_Planned = Packer.Plan(v_Cargo.GetWidth(), v_Cargo.GetHeight(), s_Plan);
		if (v_Planned && Packer.IsSorted(s_Plan))
		{
			Finish(SortButton.STATE_DONE, SortStrings.ALREADY);
			return;
		}

		s_Total = 0;
		for (int v_K = 0; v_K < s_Plan.Count(); v_K++)
		{
			if (!v_Planned || !s_Plan[v_K].AtTarget())
				s_Total++;
		}
		// A hard stop on staging moves, so a layout the packer keeps shuffling can never loop forever.
		s_MoveCap = 4 * s_Plan.Count();
		// Worked out in full before the first real move: if the planned layout cannot be reached
		// cleanly, pack into the top left from the start rather than shuffling items back and forth.
		if (v_Planned && !Packer.Reaches(v_Cargo.GetWidth(), v_Cargo.GetHeight(), s_Plan, s_MoveCap))
			v_Planned = false;
		s_Moves = 0;
		s_Done = 0;
		s_Pending = -1;
		s_Gravity = !v_Planned;
		s_SelfOk = true;
		s_WaitMs = 0;
		s_LastTick = GetGame().GetTime();
		s_State = ST_WAIT_READY;
		ShowProgress();
	}

	// Every frame from InventoryMenu.Update.
	static void Tick()
	{
		if (s_State == ST_IDLE)
			return;

		int v_Now = GetGame().GetTime();
		int v_Dt = v_Now - s_LastTick;
		s_LastTick = v_Now;
		if (v_Dt > MAX_TICK_MS || v_Dt < 0)
			v_Dt = 0;
		s_WaitMs += v_Dt;

		PlayerBase v_Player = PlayerBase.Cast(GetGame().GetPlayer());
		CargoBase v_Cargo = GetCargo();
		if (!v_Player || !v_Cargo)
		{
			Abort(SortStrings.STOPPED);
			return;
		}

		if (s_State == ST_WAIT_LAND)
		{
			PackItem v_P = s_Plan[s_Pending];
			EntityAI v_E = s_Ents[v_P.m_Key];
			if (!v_E || v_Cargo.FindEntityInCargo(v_E) < 0)
			{
				Abort(SortStrings.STOPPED);
				return;
			}
			if (Landed(v_E, v_P))
			{
				s_Done++;
				s_Pending = -1;
				s_WaitMs = 0;
				s_State = ST_WAIT_READY;
				ShowProgress();
				return;
			}
			if (s_WaitMs > LAND_TIMEOUT_MS)
				Abort(SortStrings.STOPPED);
			return;
		}

		// ST_WAIT_READY: the previous move is fully settled (input queue free, no juncture in progress,
		// no reservation still held) before the next one is planned against live positions.
		bool v_Ready = ScriptInputUserData.CanStoreInputUserData() && v_Player.CanManipulateInventory();
		if (v_Ready && v_Player.GetDayZPlayerInventory().IsProcessing())
			v_Ready = false;
		if (v_Ready && v_Player.GetInventory().GetAnyInventoryReservationCount() != 0)
			v_Ready = false;
		if (!v_Ready)
		{
			if (s_WaitMs > READY_TIMEOUT_MS)
				Abort(SortStrings.STOPPED);
			return;
		}

		if (!Resnapshot(v_Cargo))
		{
			Abort(SortStrings.CHANGED);
			return;
		}

		int v_Row, v_Col;
		bool v_Flip;
		int v_K;
		int v_W = v_Cargo.GetWidth();
		int v_H = v_Cargo.GetHeight();
		if (!s_Gravity)
		{
			v_K = Packer.NextMove(v_W, v_H, s_Plan, v_Row, v_Col, v_Flip, s_SelfOk);
			if (v_K == Packer.MOVE_DONE)
			{
				Finish(SortButton.STATE_DONE, SortStrings.SORTED);
				return;
			}
			// Too crowded to reach the plan: pack everything up into the top left instead, which
			// needs no free space to shuffle through and always ends.
			if (v_K == Packer.MOVE_STUCK || s_Moves >= s_MoveCap)
			{
				s_Gravity = true;
				s_Moves = 0;
			}
		}
		if (s_Gravity)
		{
			v_K = Packer.NextGravity(v_W, v_H, s_Plan, v_Row, v_Col, v_Flip, s_SelfOk);
			if (v_K == Packer.MOVE_DONE || s_Moves >= s_MoveCap)
			{
				Finish(SortButton.STATE_DONE, SortStrings.PACKED);
				return;
			}
		}

		Issue(v_Player, v_Cargo, v_K, v_Row, v_Col, v_Flip);
	}

	protected static void Issue(PlayerBase v_Player, CargoBase v_Cargo, int v_K, int v_Row, int v_Col, bool v_Flip)
	{
		PackItem v_P = s_Plan[v_K];
		EntityAI v_E = s_Ents[v_P.m_Key];

		// Re-checked at the moment of the move: the item may have been picked up or locked since the
		// snapshot, and the sort must never pull an item out of the player's hands or out of a lock.
		if (!v_E || v_E == v_Player.GetHumanInventory().GetEntityInHands() || v_E.GetInventory().IsInventoryLocked() || !v_E.GetInventory().CanRemoveEntity())
		{
			Abort(SortStrings.STOPPED);
			return;
		}

		InventoryLocation v_Src = new InventoryLocation;
		if (!v_E.GetInventory().GetCurrentInventoryLocation(v_Src))
		{
			Abort(SortStrings.STOPPED);
			return;
		}

		// Same call vanilla's drop-into-a-cell uses (ContainerWithCargo.DropReceived, containerwithcargo.c:377).
		InventoryLocation v_Dst = new InventoryLocation;
		v_Dst.SetCargoAuto(v_Cargo, v_E, v_Row, v_Col, v_Flip);
		if (!GameInventory.LocationCanMoveEntity(v_Src, v_Dst))
		{
			// The engine may not count the item's own cells as free. Plan without that from here on
			// and try again next frame, instead of giving up.
			if (s_SelfOk && v_Src.GetType() == InventoryLocationType.CARGO && OverlapsSelf(v_P, v_Row, v_Col, v_Flip))
			{
				s_SelfOk = false;
				return;
			}
			Abort(SortStrings.STOPPED);
			return;
		}

		s_Pending = v_K;
		s_DstRow = v_Row;
		s_DstCol = v_Col;
		s_DstFlip = v_Flip;
		// Keeps the icon's footprint right while the move travels; RestorePendingFlip undoes it if the
		// move never lands, the same as vanilla does for a cancelled drag (icon.c:1202).
		v_E.GetInventory().SetFlipCargo(v_Flip);
		if (!v_Player.PredictiveTakeToDst(v_Src, v_Dst))
		{
			Abort(SortStrings.STOPPED);
			return;
		}

		s_Moves++;
		s_WaitMs = 0;
		s_State = ST_WAIT_LAND;
	}

	protected static bool OverlapsSelf(PackItem v_P, int v_Row, int v_Col, bool v_Flip)
	{
		int v_W = v_P.Cols(v_Flip);
		int v_H = v_P.Rows(v_Flip);
		int v_CW = v_P.Cols(v_P.m_Flip);
		int v_CH = v_P.Rows(v_P.m_Flip);
		if (v_Col + v_W <= v_P.m_Col || v_P.m_Col + v_CW <= v_Col)
			return false;
		if (v_Row + v_H <= v_P.m_Row || v_P.m_Row + v_CH <= v_Row)
			return false;
		return true;
	}

	// Landed where this move sent it: its target, a staging spot or a gravity step. (Comparing against
	// the planned target here made every staging move look lost, so the sort stopped after a timeout.)
	protected static bool Landed(EntityAI v_E, PackItem v_P)
	{
		InventoryLocation v_Il = new InventoryLocation;
		if (!v_E.GetInventory().GetCurrentInventoryLocation(v_Il))
			return false;
		if (v_Il.GetType() != InventoryLocationType.CARGO)
			return false;
		if (v_Il.GetParent() != s_Owner || v_Il.GetIdx() != s_Idx)
			return false;
		return v_Il.GetRow() == s_DstRow && v_Il.GetCol() == s_DstCol && v_Il.GetFlip() == s_DstFlip;
	}

	// Copies every item's live position into the plan, so NextMove works from what the engine actually
	// did. Returns false when the cargo's contents changed (an item added, taken or gone), which makes
	// the plan meaningless.
	protected static bool Resnapshot(CargoBase v_Cargo)
	{
		if (v_Cargo.GetItemCount() != s_Ents.Count())
			return false;

		InventoryLocation v_Il = new InventoryLocation;
		for (int v_I = 0; v_I < s_Plan.Count(); v_I++)
		{
			PackItem v_P = s_Plan[v_I];
			if (v_P.m_Key < 0)
				continue;

			EntityAI v_E = s_Ents[v_P.m_Key];
			if (!v_E || v_Cargo.FindEntityInCargo(v_E) < 0)
				return false;
			if (!v_E.GetInventory().GetCurrentInventoryLocation(v_Il))
				return false;

			v_P.m_Row = v_Il.GetRow();
			v_P.m_Col = v_Il.GetCol();
			v_P.m_Flip = v_Il.GetFlip();
		}
		return true;
	}

	// On a real server a refused move only shows up as a timeout, after SetFlipCargo already ran, so the
	// item's flip state is put back to wherever it actually is.
	protected static void RestorePendingFlip()
	{
		if (s_Pending < 0 || !s_Plan || s_Pending >= s_Plan.Count())
			return;

		PackItem v_P = s_Plan[s_Pending];
		if (v_P.m_Key < 0)
			return;
		EntityAI v_E = s_Ents[v_P.m_Key];
		if (!v_E)
			return;

		InventoryLocation v_Il = new InventoryLocation;
		if (v_E.GetInventory().GetCurrentInventoryLocation(v_Il) && v_Il.GetType() == InventoryLocationType.CARGO)
			v_E.GetInventory().SetFlipCargo(v_Il.GetFlip());
	}

	// Stops a running sort and shows why. Safe to call when nothing is running (InventoryMenu.OnHide
	// calls it unconditionally).
	static void Abort(string v_Why)
	{
		if (s_State == ST_IDLE)
			return;
		Finish(SortButton.STATE_FAILED, v_Why);
	}

	protected static void Finish(int v_State, string v_Text)
	{
		RestorePendingFlip();
		s_LastState = v_State;

		// After a stack pass, a good result leads with the merges; "Already tidy" adds nothing to that.
		if (s_Merged > 0 && v_State == SortButton.STATE_DONE)
		{
			string v_Head = SortStrings.STACKED + " " + s_Merged.ToString();
			if (v_Text == SortStrings.SORTED)
				v_Text = v_Head + SortStrings.STACKED_SORTED;
			else
				v_Text = v_Head;
		}

		if (s_Btn)
			s_Btn.SetState(v_State, v_Text);

		s_State = ST_IDLE;
		s_Owner = null;
		s_Idx = 0;
		s_Btn = null;
		s_Plan = null;
		s_Ents = null;
		s_Pending = -1;
		s_Gravity = false;
		s_SelfOk = true;
		s_WaitMs = 0;
		s_Done = 0;
		s_Total = 0;
		s_MoveCap = 0;
		s_Moves = 0;
		s_Merged = 0;
	}

	protected static CargoBase GetCargo()
	{
		if (!s_Owner)
			return null;
		return s_Owner.GetInventory().GetCargoFromIndex(s_Idx);
	}

	// Staging moves can push the done count past the planned total, so the shown number is capped.
	protected static void ShowProgress()
	{
		if (!s_Btn)
			return;
		int v_Shown = s_Done;
		if (v_Shown > s_Total)
			v_Shown = s_Total;
		s_Btn.SetState(SortButton.STATE_RUNNING, SortStrings.SORTING + " " + v_Shown.ToString() + "/" + s_Total.ToString());
	}
}
