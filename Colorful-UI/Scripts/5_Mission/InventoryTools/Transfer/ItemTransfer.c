// Transfer: a multi-select move between the two columns.
//
// Each column's bar has a Transfer arrow pointing at the other column: left on EQUIPMENT, right on
// VICINITY. The first click on either turns Transfer mode on (TransferMode.c): a plain
// click on an item then picks it, or unpicks it, instead of the usual click. Header clicks still set
// the TARGET cards as they always do. A second click on an arrow moves things in its direction and
// turns the mode off:
// - what moves: the items picked on that arrow's side; with none picked, everything in that side's
//   TARGET card (the backpack, the sea chest); with neither, nothing,
//   and the mode just goes off;
// - where to: the other side's TARGET card if there is one. Otherwise, from the gear, onto the
//   ground; from the vicinity, into the worn cargo order quick transfer uses (QuickMove).
// What does not fit a TARGET card goes on the ground when it comes from the gear, and stays where it
// is when it comes from the vicinity.
//
// The picks are this client's own and last only while the mode is on; nothing is sent until the move.
// One move at a time, each waited on, like the other runners: on a multiplayer client every move is a
// server round trip. Ticked from InventoryMenu.Update (TransferHooks.c); OnHide aborts it.
class ItemTransfer
{
	protected static const int ST_IDLE = 0;
	protected static const int ST_WAIT_READY = 1;
	protected static const int ST_WAIT_LAND = 2;

	protected static const int READY_TIMEOUT_MS = 3000;
	protected static const int LAND_TIMEOUT_MS = 3500;
	// A move the server turned down settles without the item going anywhere; after this long with
	// the inventory idle again it is counted as left behind.
	protected static const int SETTLE_MS = 400;
	protected static const int MAX_TICK_MS = 250;

	protected static ref array<EntityAI> s_Picks;

	// Raw on purpose: TransferButton is Managed, so this goes null if the bar is destroyed.
	protected static TransferButton s_Btn;
	protected static ref array<EntityAI> s_Items;
	protected static int s_Side;
	protected static ClosableContainer s_Dest;
	protected static int s_Next;
	protected static int s_State = ST_IDLE;
	protected static int s_WaitMs;
	protected static int s_LastTick;
	protected static int s_Moved;
	protected static int s_Ground;
	protected static int s_Left;
	protected static bool s_PendingGround;
	protected static EntityAI s_Pending;
	protected static string s_PendingFrom;

	// ---- picks ----------------------------------------------------------------------------------
	static bool IsPicked(EntityAI v_E)
	{
		return v_E && s_Picks && s_Picks.Find(v_E) >= 0;
	}

	static void TogglePick(EntityAI v_E)
	{
		if (!v_E)
			return;
		if (!s_Picks)
			s_Picks = new array<EntityAI>;
		int v_At = s_Picks.Find(v_E);
		if (v_At >= 0)
			s_Picks.Remove(v_At);
		else
			s_Picks.Insert(v_E);
	}

	static void ClearPicks()
	{
		s_Picks = null;
	}

	// ---- the arrows -----------------------------------------------------------------------------
	static bool IsRunning()
	{
		return s_State != ST_IDLE;
	}

	// A click on either arrow: the first turns the mode on, the second moves towards the other side.
	static void OnArrow(int v_Side, TransferButton v_Btn)
	{
		if (IsRunning())
			return;
		if (!TransferMode.IsOn())
		{
			TransferMode.Toggle();
			return;
		}
		// Collected before the mode goes off, which clears the picks.
		array<EntityAI> v_Items = Collect(v_Side);
		TransferMode.Off();
		if (v_Items.Count() > 0)
			Start(v_Side, v_Items, v_Btn);
	}

	protected static array<EntityAI> Collect(int v_Side)
	{
		array<EntityAI> v_Out = new array<EntityAI>;
		PlayerBase v_Player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!v_Player)
			return v_Out;

		if (s_Picks)
		{
			foreach (EntityAI v_Pick : s_Picks)
			{
				// An item inside a picked bag travels with the bag.
				if (v_Pick && OnSide(v_Player, v_Pick, v_Side) && !HasPickedParent(v_Pick))
					v_Out.Insert(v_Pick);
			}
		}
		if (v_Out.Count() > 0)
			return v_Out;

		ClosableContainer v_Card = QuickMove.TargetOn(v_Side);
		if (!v_Card || !v_Card.TransferGetEntity())
			return v_Out;
		CargoBase v_Cargo = v_Card.TransferGetEntity().GetInventory().GetCargoFromIndex(v_Card.TransferTargetIdx());
		if (!v_Cargo)
			return v_Out;
		int v_Count = v_Cargo.GetItemCount();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			ItemBase v_Ib = ItemBase.Cast(v_Cargo.GetItem(v_I));
			if (v_Ib)
				v_Out.Insert(v_Ib);
		}
		return v_Out;
	}

	protected static bool HasPickedParent(EntityAI v_E)
	{
		EntityAI v_P = v_E.GetHierarchyParent();
		while (v_P)
		{
			if (s_Picks.Find(v_P) >= 0)
				return true;
			v_P = v_P.GetHierarchyParent();
		}
		return false;
	}

	protected static bool OnSide(PlayerBase v_Player, EntityAI v_E, int v_Side)
	{
		bool v_Mine = v_E.GetHierarchyRootPlayer() == v_Player;
		if (v_Side == QuickMove.SIDE_PLAYER)
			return v_Mine;
		return !v_Mine;
	}

	protected static int OtherSide(int v_Side)
	{
		if (v_Side == QuickMove.SIDE_PLAYER)
			return QuickMove.SIDE_VICINITY;
		return QuickMove.SIDE_PLAYER;
	}

	// ---- the run --------------------------------------------------------------------------------
	protected static void Start(int v_Side, array<EntityAI> v_Items, TransferButton v_Btn)
	{
		if (GrabAll.IsRunning())
			return;
		s_Btn = v_Btn;
		s_Side = v_Side;
		s_Items = v_Items;
		s_Dest = QuickMove.TargetOn(OtherSide(v_Side));
		s_Next = 0;
		s_Moved = 0;
		s_Ground = 0;
		s_Left = 0;
		s_Pending = null;
		s_WaitMs = 0;
		s_LastTick = GetGame().GetTime();
		s_State = ST_WAIT_READY;
		ShowProgress();
	}

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
		if (!v_Player || !v_Player.IsAlive())
		{
			Abort();
			return;
		}

		bool v_Ready = ScriptInputUserData.CanStoreInputUserData() && v_Player.CanManipulateInventory();
		if (v_Ready && v_Player.GetDayZPlayerInventory().IsProcessing())
			v_Ready = false;
		if (v_Ready && v_Player.GetInventory().GetAnyInventoryReservationCount() != 0)
			v_Ready = false;

		if (s_State == ST_WAIT_LAND)
		{
			if (!s_Pending || Where(s_Pending) != s_PendingFrom)
			{
				if (s_PendingGround)
					s_Ground++;
				else
					s_Moved++;
				Advance();
			}
			else if ((v_Ready && s_WaitMs > SETTLE_MS) || s_WaitMs > LAND_TIMEOUT_MS)
			{
				s_Left++;
				Advance();
			}
			return;
		}

		// ST_WAIT_READY: the last move fully settled before the next is planned.
		if (!v_Ready)
		{
			if (s_WaitMs > READY_TIMEOUT_MS)
				Abort();
			return;
		}

		while (s_Next < s_Items.Count())
		{
			EntityAI v_E = s_Items[s_Next];
			s_Next++;
			// Gone, or already carried across inside something moved before it.
			if (!v_E || !OnSide(v_Player, v_E, s_Side))
				continue;
			if (v_E == v_Player.GetHumanInventory().GetEntityInHands() || !QuickMove.CanGrab(v_Player, v_E))
			{
				s_Left++;
				continue;
			}
			if (Issue(v_Player, v_E))
				return;
			s_Left++;
		}

		Done();
	}

	// Sends one item on its way; false when it has nowhere to go.
	protected static bool Issue(PlayerBase v_Player, EntityAI v_E)
	{
		s_PendingFrom = Where(v_E);
		s_PendingGround = false;
		InventoryLocation v_Dst = new InventoryLocation;
		bool v_Found = false;
		if (s_Dest)
			v_Found = QuickMove.FindCardSpot(s_Dest, v_E, v_Dst);
		else if (s_Side == QuickMove.SIDE_VICINITY)
			v_Found = QuickMove.FindWornSpot(v_Player, v_E, v_Dst);

		if (v_Found)
		{
			QuickMove.IssueGrab(v_Player, v_E, v_Dst);
		}
		else
		{
			// From the gear, whatever has no place goes on the ground; from the vicinity it stays.
			if (s_Side != QuickMove.SIDE_PLAYER || !v_Player.PredictiveDropEntity(v_E))
				return false;
			s_PendingGround = true;
		}
		s_Pending = v_E;
		s_WaitMs = 0;
		s_State = ST_WAIT_LAND;
		ShowProgress();
		return true;
	}

	protected static string Where(EntityAI v_E)
	{
		InventoryLocation v_Il = new InventoryLocation;
		if (!v_E.GetInventory().GetCurrentInventoryLocation(v_Il))
			return "";
		return v_Il.DumpToString();
	}

	protected static void Advance()
	{
		s_Pending = null;
		s_WaitMs = 0;
		s_State = ST_WAIT_READY;
		ShowProgress();
	}

	protected static void Done()
	{
		if (s_Moved == 0 && s_Ground == 0)
		{
			Finish(TransferButton.STATE_FAILED, TransferStrings.GRAB_NO_ROOM);
			return;
		}
		string v_Text = TransferStrings.MOVED + " " + s_Moved.ToString();
		if (s_Moved == 0)
			v_Text = TransferStrings.DROPPED + " " + s_Ground.ToString();
		else if (s_Ground > 0)
			v_Text = v_Text + ", " + s_Ground.ToString() + " " + TransferStrings.ON_GROUND;
		if (s_Left > 0)
			v_Text = v_Text + ", " + s_Left.ToString() + " " + TransferStrings.GRAB_LEFT;
		Finish(TransferButton.STATE_DONE, v_Text);
	}

	// Stops a running transfer and reports what it managed. Safe when idle.
	static void Abort()
	{
		if (s_State == ST_IDLE)
			return;
		if (s_Moved > 0 || s_Ground > 0)
		{
			Done();
			return;
		}
		Finish(TransferButton.STATE_FAILED, TransferStrings.TRANSFER_STOPPED);
	}

	protected static void Finish(int v_State, string v_Text)
	{
		if (s_Btn)
			s_Btn.SetState(v_State, v_Text);
		s_State = ST_IDLE;
		s_Btn = null;
		s_Items = null;
		s_Dest = null;
		s_Pending = null;
		s_Next = 0;
		s_WaitMs = 0;
	}

	protected static void ShowProgress()
	{
		if (!s_Btn || !s_Items)
			return;
		int v_Done = s_Moved + s_Ground + s_Left;
		s_Btn.SetState(TransferButton.STATE_RUNNING, TransferStrings.MOVING + " " + v_Done.ToString() + "/" + s_Items.Count().ToString());
	}
}
