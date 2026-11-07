// Runs one "stack" pass on a container: merges split stacks of the same class (ammo piles, nails,
// rags, anything vanilla lets you stack by dropping one onto the other), one merge at a time, then
// hands the container to SortRunner so the freed space is compacted too.
//
// Each merge is exactly what vanilla does when you drop a stack onto a matching stack
// (Icon.PerformCombination, icon.c:526-528): target.CombineItemsClient(source). On a multiplayer
// client that sends an INPUT_UDT_ITEM_MANIPULATION request (itembase.c:786-818), and the server runs
// ItemBase.CombineItems / Magazine.CombineItems after its own distance checks (playerbase.c:6648-6671).
// There is no juncture and no reply, so a merge has "landed" when the synced quantities change or the
// emptied source is deleted.
//
// Pairing is always "fullest non-full stack takes from the smallest one", recomputed from live
// quantities before every merge. Every merge then either deletes a source or fills a target, so the
// pass ends, and it leaves the fewest, fullest stacks.
//
// Ticked from InventoryMenu.Update (SortHooks.c); InventoryMenu.OnHide aborts it.
class StackRunner
{
	protected static const int ST_IDLE = 0;
	protected static const int ST_WAIT_READY = 1;
	protected static const int ST_WAIT_LAND = 2;
	// Waiting for the last merge's reservation to clear before the sort starts (see Tick).
	protected static const int ST_SETTLE = 3;

	protected static const int READY_TIMEOUT_MS = 3000;
	protected static const int LAND_TIMEOUT_MS = 3500;
	// Just past c_InventoryReservationTimeoutShortMS (3000, inventory.c:715), the reservation a
	// merge-everything request takes on its source (itembase.c:808-811).
	protected static const int SETTLE_TIMEOUT_MS = 3500;
	// Same reasoning as SortRunner: a longer frame gap means the inventory was not updating.
	protected static const int MAX_TICK_MS = 250;

	protected static EntityAI s_Owner;
	protected static int s_Idx;
	// Raw on purpose: SortButton is Managed, so this goes null if the header is destroyed.
	protected static SortButton s_Btn;
	protected static int s_State = ST_IDLE;
	protected static int s_WaitMs;
	protected static int s_LastTick;
	protected static int s_Merged;
	protected static int s_Requests;
	protected static int s_RequestCap;

	// The merge in flight. Raw pointers are fine: entities are Managed, so a source the server deletes
	// for being emptied reads as null here, which is itself the "landed" signal.
	protected static ItemBase s_Target;
	protected static ItemBase s_Source;
	protected static float s_TargetQty;
	protected static float s_SourceQty;

	static bool IsRunning()
	{
		return s_State != ST_IDLE;
	}

	static void Start(EntityAI v_Owner, int v_Idx, SortButton v_Btn)
	{
		if (IsRunning() || SortRunner.IsRunning())
			return;

		s_Btn = v_Btn;
		s_Owner = v_Owner;
		s_Idx = v_Idx;

		PlayerBase v_Player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!v_Player || !v_Owner || !v_Player.CanManipulateInventory() || v_Owner.GetInventory().IsInventoryLocked())
		{
			Fail(SortStrings.STACK_STOPPED);
			return;
		}

		CargoBase v_Cargo = GetCargo();
		if (!v_Cargo)
		{
			Fail(SortStrings.CHANGED);
			return;
		}

		s_Merged = 0;
		s_Requests = 0;
		// Each merge removes a stack or fills one, so a pass needs fewer merges than there are items.
		// Twice that is a safety stop in case the server keeps half-applying merges.
		s_RequestCap = 2 * v_Cargo.GetItemCount() + 2;
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
			Abort(SortStrings.STACK_STOPPED);
			return;
		}

		if (s_State == ST_WAIT_LAND)
		{
			if (Landed(v_Cargo))
			{
				s_Merged++;
				s_Target = null;
				s_Source = null;
				s_WaitMs = 0;
				s_State = ST_WAIT_READY;
				ShowProgress();
				return;
			}
			// The target leaving the container mid-merge means someone else is moving things.
			if (!s_Target || v_Cargo.FindEntityInCargo(s_Target) < 0)
			{
				Abort(SortStrings.CHANGED);
				return;
			}
			if (s_WaitMs > LAND_TIMEOUT_MS)
				Abort(SortStrings.STACK_STOPPED);
			return;
		}

		if (s_State == ST_SETTLE)
		{
			// A merge that empties its source reserves that source for up to 3 s. The sort's own ready
			// gate waits for zero reservations with a 3 s timeout, so it is started only once the
			// reservation is gone (or has had time to expire) rather than racing it.
			bool v_Settled = !v_Player.GetDayZPlayerInventory().IsProcessing();
			if (v_Settled && v_Player.GetInventory().GetAnyInventoryReservationCount() != 0)
				v_Settled = false;
			if (v_Settled || s_WaitMs > SETTLE_TIMEOUT_MS)
				HandOff();
			return;
		}

		// ST_WAIT_READY. No zero-reservation gate here: the only reservation a merge takes is on a
		// source the server then deletes, and every new pair is checked for reservations by
		// CanBeCombined itself (itembase.c:2188).
		bool v_Ready = ScriptInputUserData.CanStoreInputUserData() && v_Player.CanManipulateInventory();
		if (v_Ready && v_Player.GetDayZPlayerInventory().IsProcessing())
			v_Ready = false;
		if (!v_Ready)
		{
			if (s_WaitMs > READY_TIMEOUT_MS)
				Abort(SortStrings.STACK_STOPPED);
			return;
		}

		ItemBase v_Target, v_Source;
		if (s_Requests >= s_RequestCap || !FindPair(v_Player, v_Cargo, v_Target, v_Source))
		{
			s_WaitMs = 0;
			s_State = ST_SETTLE;
			return;
		}

		Issue(v_Target, v_Source);
	}

	// Picks the next merge from live state: among every pair vanilla itself would combine, the target
	// is the fullest stack that still has room and the source is the smallest stack it can take from.
	protected static bool FindPair(PlayerBase v_Player, CargoBase v_Cargo, out ItemBase v_Target, out ItemBase v_Source)
	{
		v_Target = null;
		v_Source = null;

		array<ItemBase> v_Items = new array<ItemBase>;
		int v_Count = v_Cargo.GetItemCount();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			ItemBase v_Ib = ItemBase.Cast(v_Cargo.GetItem(v_I));
			if (v_Ib && !IsPinned(v_Player, v_Ib))
				v_Items.Insert(v_Ib);
		}

		float v_BestT = -1;
		float v_BestS = -1;
		int v_N = v_Items.Count();
		for (int v_A = 0; v_A < v_N; v_A++)
		{
			ItemBase v_T = v_Items[v_A];
			float v_TQ = Qty(v_T);
			// A better target was already found; nothing this one pairs with can beat it.
			if (v_Target && v_TQ < v_BestT)
				continue;

			string v_Type = v_T.GetType();
			for (int v_B = 0; v_B < v_N; v_B++)
			{
				if (v_B == v_A)
					continue;
				ItemBase v_S = v_Items[v_B];
				// Cheap type test first; CanBeCombined does the real checks (stack room, ruined,
				// canBeSplit, reservations, slot locks) the same way vanilla's drop does.
				if (v_S.GetType() != v_Type)
					continue;

				float v_SQ = Qty(v_S);
				bool v_Better = !v_Target || v_TQ > v_BestT || (v_TQ == v_BestT && v_SQ < v_BestS);
				if (!v_Better)
					continue;
				if (!v_T.CanBeCombined(v_S, true, false))
					continue;

				v_Target = v_T;
				v_Source = v_S;
				v_BestT = v_TQ;
				v_BestS = v_SQ;
			}
		}
		return v_Target != null;
	}

	// The same claims the sort respects: in hands, locked, or holding a spot the player will drop back into.
	protected static bool IsPinned(PlayerBase v_Player, ItemBase v_Ib)
	{
		if (v_Ib == v_Player.GetHumanInventory().GetEntityInHands())
			return true;
		if (v_Ib.GetInventory().IsInventoryLocked())
			return true;
		return v_Player.GetHumanInventory().FindUserReservedLocationIndex(v_Ib) >= 0;
	}

	protected static void Issue(ItemBase v_Target, ItemBase v_Source)
	{
		s_Target = v_Target;
		s_Source = v_Source;
		s_TargetQty = Qty(v_Target);
		s_SourceQty = Qty(v_Source);

		// use_stack_max true, as vanilla's drop-onto-stack does (the default of CombineItemsClient).
		v_Target.CombineItemsClient(v_Source, true);

		s_Requests++;
		s_WaitMs = 0;
		s_State = ST_WAIT_LAND;
	}

	// Landed when the source is gone (emptied and deleted, or no longer in this cargo) or either side's
	// synced quantity moved in the merge's direction.
	protected static bool Landed(CargoBase v_Cargo)
	{
		if (!s_Source || v_Cargo.FindEntityInCargo(s_Source) < 0)
			return true;
		if (Qty(s_Source) < s_SourceQty)
			return true;
		return s_Target && Qty(s_Target) > s_TargetQty;
	}

	// Magazines (ammo piles included) keep their count as cartridges, not as item quantity.
	protected static float Qty(ItemBase v_Ib)
	{
		Magazine v_Mag = Magazine.Cast(v_Ib);
		if (v_Mag)
			return v_Mag.GetAmmoCount();
		return v_Ib.GetQuantity();
	}

	// Ends the stack pass and starts the sort on the same container, reporting on the same button, so
	// one chip tells the whole story ("Sorting 3/9", then "Stacked 4, sorted").
	protected static void HandOff()
	{
		EntityAI v_Owner = s_Owner;
		int v_Idx = s_Idx;
		SortButton v_Btn = s_Btn;
		int v_Merged = s_Merged;

		Clear();
		SortRunner.Start(v_Owner, v_Idx, v_Btn, v_Merged);
	}

	// Stops a running pass and shows why. Safe to call when idle (InventoryMenu.OnHide calls it).
	static void Abort(string v_Why)
	{
		if (s_State == ST_IDLE)
			return;
		Fail(v_Why);
	}

	protected static void Fail(string v_Why)
	{
		if (s_Btn)
			s_Btn.SetState(SortButton.STATE_FAILED, v_Why);
		Clear();
	}

	protected static void Clear()
	{
		s_State = ST_IDLE;
		s_Owner = null;
		s_Idx = 0;
		s_Btn = null;
		s_Target = null;
		s_Source = null;
		s_TargetQty = 0;
		s_SourceQty = 0;
		s_WaitMs = 0;
		s_Merged = 0;
		s_Requests = 0;
		s_RequestCap = 0;
	}

	protected static CargoBase GetCargo()
	{
		if (!s_Owner)
			return null;
		return s_Owner.GetInventory().GetCargoFromIndex(s_Idx);
	}

	protected static void ShowProgress()
	{
		if (!s_Btn)
			return;
		string v_Text = SortStrings.STACKING;
		if (s_Merged > 0)
			v_Text = v_Text + " " + s_Merged.ToString();
		s_Btn.SetState(SortButton.STATE_RUNNING, v_Text);
	}
}
