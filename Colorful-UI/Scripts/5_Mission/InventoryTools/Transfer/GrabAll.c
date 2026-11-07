// "Grab all" on the VICINITY bar: takes every loose item lying on the ground near the player into
// their gear, like Tarkov's "receive all" in the message centre - except that what does not fit stays
// on the ground (there is nowhere to come back to it from). Containers that cannot be picked up
// (crates, barrels, tents) and anything inside them are left alone; their contents are a Ctrl+click
// away.
//
// Each item goes where quick transfer would put it (QuickMove.FindGrabSpot): the player-side
// TARGET card, else vest, pants, jacket, backpack, the other worn cargo, else a free slot such as a
// shoulder. One move at a time, each waited on, the same as the other runners - on a multiplayer
// client every move is a server round trip. Items with no room are skipped and counted.
//
// Ticked from InventoryMenu.Update (TransferHooks.c); InventoryMenu.OnHide aborts it.
class GrabAll
{
	protected static const int ST_IDLE = 0;
	protected static const int ST_WAIT_READY = 1;
	protected static const int ST_WAIT_LAND = 2;

	protected static const int READY_TIMEOUT_MS = 3000;
	protected static const int LAND_TIMEOUT_MS = 3500;
	protected static const int MAX_TICK_MS = 250;

	// Raw on purpose: TransferButton is Managed, so this goes null if the bar is destroyed.
	protected static TransferButton s_Btn;
	protected static ref array<EntityAI> s_Items;
	protected static int s_Next;
	protected static int s_State = ST_IDLE;
	protected static int s_WaitMs;
	protected static int s_LastTick;
	protected static int s_Grabbed;
	protected static int s_Left;
	protected static EntityAI s_Pending;

	static bool IsRunning()
	{
		return s_State != ST_IDLE;
	}

	static void Start(TransferButton v_Btn)
	{
		if (IsRunning() || ItemTransfer.IsRunning())
			return;
		s_Btn = v_Btn;

		PlayerBase v_Player = PlayerBase.Cast(GetGame().GetPlayer());
		VicinityItemManager v_Vic = VicinityItemManager.GetInstance();
		if (!v_Player || !v_Vic)
		{
			Finish(TransferButton.STATE_FAILED, TransferStrings.GRAB_NOTHING);
			return;
		}

		s_Items = new array<EntityAI>;
		array<EntityAI> v_All = v_Vic.GetVicinityItems();
		for (int v_I = 0; v_I < v_All.Count(); v_I++)
		{
			EntityAI v_E = v_All[v_I];
			if (v_E && IsLoose(v_Player, v_E))
				s_Items.Insert(v_E);
		}
		if (s_Items.Count() == 0)
		{
			Finish(TransferButton.STATE_DONE, TransferStrings.GRAB_NOTHING);
			return;
		}

		s_Next = 0;
		s_Grabbed = 0;
		s_Left = 0;
		s_Pending = null;
		s_WaitMs = 0;
		s_LastTick = GetGame().GetTime();
		s_State = ST_WAIT_READY;
		ShowProgress();
	}

	// On the ground, takeable, an item and not a person or a body.
	protected static bool IsLoose(PlayerBase v_Player, EntityAI v_E)
	{
		if (v_E.GetHierarchyParent() || v_E.IsMan() || !ItemBase.Cast(v_E))
			return false;
		return v_E.IsTakeable() && v_E.GetHierarchyRootPlayer() != v_Player;
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

		if (s_State == ST_WAIT_LAND)
		{
			if (!s_Pending || s_Pending.GetHierarchyRootPlayer() == v_Player)
			{
				if (s_Pending)
					s_Grabbed++;
				Advance();
				return;
			}
			if (s_WaitMs > LAND_TIMEOUT_MS)
			{
				s_Left++;
				Advance();
			}
			return;
		}

		// ST_WAIT_READY: the last move fully settled before the next is planned.
		bool v_Ready = ScriptInputUserData.CanStoreInputUserData() && v_Player.CanManipulateInventory();
		if (v_Ready && v_Player.GetDayZPlayerInventory().IsProcessing())
			v_Ready = false;
		if (v_Ready && v_Player.GetInventory().GetAnyInventoryReservationCount() != 0)
			v_Ready = false;
		if (!v_Ready)
		{
			if (s_WaitMs > READY_TIMEOUT_MS)
				Abort();
			return;
		}

		// The next item that can still be grabbed and has somewhere to go.
		while (s_Next < s_Items.Count())
		{
			EntityAI v_E = s_Items[s_Next];
			s_Next++;
			if (!v_E || !IsLoose(v_Player, v_E))
				continue;
			if (!QuickMove.CanGrab(v_Player, v_E))
			{
				s_Left++;
				continue;
			}
			InventoryLocation v_Dst = new InventoryLocation;
			if (!QuickMove.FindGrabSpot(v_Player, v_E, v_Dst))
			{
				s_Left++;
				continue;
			}
			s_Pending = v_E;
			QuickMove.IssueGrab(v_Player, v_E, v_Dst);
			s_WaitMs = 0;
			s_State = ST_WAIT_LAND;
			ShowProgress();
			return;
		}

		Done();
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
		if (s_Grabbed == 0)
		{
			Finish(TransferButton.STATE_FAILED, TransferStrings.GRAB_NO_ROOM);
			return;
		}
		string v_Text = TransferStrings.GRABBED + " " + s_Grabbed.ToString();
		if (s_Left > 0)
			v_Text = v_Text + ", " + s_Left.ToString() + " " + TransferStrings.GRAB_LEFT;
		Finish(TransferButton.STATE_DONE, v_Text);
	}

	// Stops a running grab and reports what it managed. Safe when idle.
	static void Abort()
	{
		if (s_State == ST_IDLE)
			return;
		if (s_Grabbed > 0)
		{
			Done();
			return;
		}
		Finish(TransferButton.STATE_FAILED, TransferStrings.GRAB_STOPPED);
	}

	protected static void Finish(int v_State, string v_Text)
	{
		if (s_Btn)
			s_Btn.SetState(v_State, v_Text);
		s_State = ST_IDLE;
		s_Btn = null;
		s_Items = null;
		s_Pending = null;
		s_Next = 0;
		s_WaitMs = 0;
	}

	protected static void ShowProgress()
	{
		if (!s_Btn || !s_Items)
			return;
		int v_Done = s_Grabbed + s_Left;
		s_Btn.SetState(TransferButton.STATE_RUNNING, TransferStrings.GRABBING + " " + v_Done.ToString() + "/" + s_Items.Count().ToString());
	}
}
