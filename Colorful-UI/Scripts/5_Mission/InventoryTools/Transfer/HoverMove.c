// Hover transfer, Rust style: with Ctrl held, the moment the cursor enters an item's cell that item is
// queued. Its own spinner starts on its icon, and when the spinner has gone round once the item moves
// exactly as a Ctrl+click on it would (HoverHooks.c runs the click's own code, so the TARGET cards,
// the default worn order, NO ROOM and vanilla's drop all behave the same).
//
// A queued item no longer needs the cursor: sweeping across many items queues each one, and they move
// themselves. Letting go of Ctrl only stops new items being queued; the queue finishes. Closing the
// inventory clears it.
//
// Moves go one at a time: a finished item waits (spinner full) until the player is free again
// (QuickMove.IsPlayerFree), and finished items fire in the order they were queued, one per frame.
// Per-frame work is one pass over the queue; nothing runs while it is empty.
class HoverMove
{
	// Seconds from queueing an item to its move: one turn of its spinner.
	static const float HOLD_TIME = 0.35;

	protected static ref array<ref HoverMoveEntry> s_Queue = new array<ref HoverMoveEntry>;

	// A cargo or hands cell was entered (HoverHooks.c).
	static void EnterIcon(Icon v_Icon)
	{
		if (!v_Icon || !CanQueue())
			return;
		EntityAI v_Item = v_Icon.TransferHoverItem();
		if (!v_Item || IsQueued(v_Item))
			return;
		HoverMoveEntry v_Entry = new HoverMoveEntry(v_Item);
		v_Entry.m_Icon = v_Icon;
		// Vanilla's Ctrl+click drop (the fallback) only answers left Ctrl; Ctrl may be up by fire time.
		v_Entry.m_LeftCtrl = g_Game.IsLeftCtrlDown();
		s_Queue.Insert(v_Entry);
	}

	// A vicinity list slot was entered (HoverHooks.c); v_List is the list it sits in.
	static void EnterSlot(SlotsIcon v_Slot, VicinitySlotsContainer v_List)
	{
		if (!v_Slot || !v_List || !CanQueue())
			return;
		EntityAI v_Item = v_Slot.TransferHoverItem();
		if (!v_Item || IsQueued(v_Item))
			return;
		HoverMoveEntry v_Entry = new HoverMoveEntry(v_Item);
		v_Entry.m_Slot = v_Slot;
		v_Entry.m_List = v_List;
		s_Queue.Insert(v_Entry);
	}

	// From InventoryMenu.OnHide: entries own their spinners, so this removes those too.
	static void Reset()
	{
		s_Queue.Clear();
	}

	// Every frame from InventoryMenu.Update.
	static void Tick(float v_Dt)
	{
		if (s_Queue.Count() == 0)
			return;

		// Only one move starts per frame, and only while the player is free; the first finished entry
		// in queue order goes.
		bool v_CanFire = QuickMove.IsPlayerFree(PlayerBase.Cast(GetGame().GetPlayer()));
		for (int v_I = 0; v_I < s_Queue.Count(); v_I++)
		{
			HoverMoveEntry v_Entry = s_Queue[v_I];
			// The item is gone, or no cell in the inventory shows it any more (card closed, moved by
			// something else, inventory rebuilt): drop it and its spinner.
			if (!v_Entry.m_Item || !v_Entry.Track())
			{
				s_Queue.RemoveOrdered(v_I);
				v_I--;
				continue;
			}

			v_Entry.m_Held += v_Dt;
			if (v_Entry.m_Held < HOLD_TIME)
			{
				v_Entry.ShowSpinner(v_Entry.m_Held / HOLD_TIME);
				continue;
			}
			v_Entry.ShowSpinner(1);
			if (!v_CanFire)
				continue;

			// Removed first, so the entry (and its spinner) is gone whatever the move does.
			s_Queue.RemoveOrdered(v_I);
			v_I--;
			v_CanFire = false;
			v_Entry.Fire();
		}
	}

	// Ctrl gates queueing; not while typing in the search box or dragging an item.
	protected static bool CanQueue()
	{
		if (!TransferMode.CtrlDown() || InputGuard.IsOn())
			return false;
		return !ItemManager.GetInstance().IsDragging();
	}

	protected static bool IsQueued(EntityAI v_Item)
	{
		foreach (HoverMoveEntry v_Entry : s_Queue)
		{
			if (v_Entry.m_Item == v_Item)
				return true;
		}
		return false;
	}
}

// One queued item, the cell that shows it and its spinner.
class HoverMoveEntry : Managed
{
	// Raw on purpose: entities and cells are Managed, so a destroyed one reads as null.
	EntityAI				m_Item;
	// A cargo or hands cell. Cargo cells stay put while other items move, so it is never looked for again.
	Icon					m_Icon;
	bool					m_LeftCtrl;
	// A vicinity slot, and its list: the list closes up when an item leaves, so the item is found again.
	SlotsIcon				m_Slot;
	VicinitySlotsContainer	m_List;
	float					m_Held;
	ref HoverSpinner		m_Spinner;

	void HoverMoveEntry(EntityAI v_Item)
	{
		m_Item = v_Item;
		m_Spinner = new HoverSpinner();
	}

	// True while a cell still shows the item; follows it to its new vicinity slot when the list shifts.
	bool Track()
	{
		if (m_Icon)
			return m_Icon.TransferHoverItem() == m_Item;
		if (m_Slot && m_Slot.TransferHoverItem() == m_Item)
			return true;
		m_Slot = null;
		if (m_List)
			m_Slot = m_List.TransferHoverFind(m_Item);
		return m_Slot != null;
	}

	void ShowSpinner(float v_Turn)
	{
		// The cell is read into a local first: passed straight in as an argument it arrived as null
		// (seen in the client log) although the same call worked inside TransferHoverCellSide.
		Widget v_Cell;
		float v_Side;
		if (m_Icon)
		{
			v_Cell = m_Icon.GetMainWidget();
			v_Side = m_Icon.TransferHoverCellSide();
		}
		else if (m_Slot)
		{
			v_Cell = m_Slot.GetMainWidget();
			v_Side = m_Slot.TransferHoverCellSide();
		}
		m_Spinner.Show(v_Cell, v_Side, v_Turn);
	}

	// Track() has just confirmed a cell still shows the item, so it is still in the player's gear or
	// in the vicinity list; each cell's Ctrl+click code checks the rest.
	void Fire()
	{
		if (m_Icon)
			m_Icon.TransferHoverFire(m_LeftCtrl);
		else if (m_Slot)
			m_Slot.TransferHoverFire();
	}
}
