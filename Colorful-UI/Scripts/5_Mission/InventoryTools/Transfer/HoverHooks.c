// Queues an item for HoverMove when the cursor enters its cell with Ctrl held, and runs the cell's
// Ctrl+click when the item's turn comes. Only the cells where Ctrl+click already transfers take part:
// the cargo grid and hands Icons (cargocontainer.c:84, handspreview.c:103; QuickMove.c, modded Icon)
// and the vicinity list's slots (QuickMove.c, modded VicinitySlotsContainer).
// Vanilla: P:\scripts\5_mission\gui\inventorynew\containeditems\icon.c:344,822
//          P:\scripts\5_mission\gui\inventorynew\containeditems\slotsicon.c:94

modded class Icon
{
	override bool MouseEnter(Widget w, int x, int y)
	{
		bool v_Result = super.MouseEnter(w, x, y);
		HoverMove.EnterIcon(this);
		return v_Result;
	}

	// The item a hover transfer would move, or null for a reserved placeholder.
	EntityAI TransferHoverItem()
	{
		if (m_Lock)
			return null;
		return m_Item;
	}

	// One grid cell's side in screen pixels: the icon's size over its size in cells.
	float TransferHoverCellSide()
	{
		float v_W, v_H;
		GetMainWidget().GetScreenSize(v_W, v_H);
		if (m_SizeX > 0 && m_SizeY > 0)
			return Math.Min(v_W / m_SizeX, v_H / m_SizeY);
		return Math.Min(v_W, v_H);
	}

	// What a Ctrl+left-click on this cell does (modded Icon.MouseClick in QuickMove.c): the quick
	// transfer, and when that declines, vanilla's Ctrl+click drop (icon.c:856-866), which vanilla only
	// runs for left Ctrl (v_LeftCtrl: it was held when the item was queued).
	void TransferHoverFire(bool v_LeftCtrl)
	{
		if (m_Lock || !m_Item)
			return;
		if (QuickMove.OnItemCtrlClick(m_Item))
			return;

		PlayerBase v_Player = PlayerBase.Cast(g_Game.GetPlayer());
		if (!v_Player || !v_LeftCtrl || !v_Player.CanDropEntity(m_Item))
			return;
		if (m_Item.GetTargetQuantityMax() < m_Item.GetQuantity())
			m_Item.SplitIntoStackMaxClient(null, -1);
		else
			v_Player.PhysicalPredictiveDropItem(m_Item);
	}
}

modded class SlotsIcon
{
	// The vicinity list this slot sits in, or null; m_TransferHoverLooked once it has been looked for.
	// Raw on purpose: the list is Managed.
	protected VicinitySlotsContainer	m_TransferHoverList;
	protected bool						m_TransferHoverLooked;

	override bool MouseEnter(Widget w, int x, int y)
	{
		bool v_Result = super.MouseEnter(w, x, y);
		VicinitySlotsContainer v_List = TransferHoverList();
		if (v_List)
			HoverMove.EnterSlot(this, v_List);
		return v_Result;
	}

	// The vicinity list builds its slots as VicinitySlotsContainer > AttachmentsGroupContainer >
	// SlotsContainer > SlotsIcon (vicinityslotscontainer.c:10-16). Looked up once.
	protected VicinitySlotsContainer TransferHoverList()
	{
		if (!m_TransferHoverLooked)
		{
			m_TransferHoverLooked = true;
			LayoutHolder v_P = GetParent();
			while (v_P && !m_TransferHoverList)
			{
				m_TransferHoverList = VicinitySlotsContainer.Cast(v_P);
				v_P = v_P.GetParent();
			}
		}
		return m_TransferHoverList;
	}

	EntityAI TransferHoverItem()
	{
		if (m_Reserved)
			return null;
		return GetEntity();
	}

	float TransferHoverCellSide()
	{
		float v_W, v_H;
		GetMainWidget().GetScreenSize(v_W, v_H);
		return Math.Min(v_W, v_H);
	}

	// What a Ctrl+left-click on a vicinity slot does (modded VicinitySlotsContainer.MouseClick in
	// QuickMove.c). When the quick transfer declines, that click falls through to vanilla's plain left
	// click, which opens the item's card and moves nothing, so a hover does nothing more.
	void TransferHoverFire()
	{
		EntityAI v_Item = TransferHoverItem();
		if (v_Item && QuickMove.OnItemCtrlClick(v_Item))
			HideOwnedTooltip();
	}
}

// Finds the slot now showing an item, after the list closed up under a queued one. Rows and columns as
// vanilla fills them (vicinityslotscontainer.c:259-266). Only called when a queued item's slot changed.
modded class VicinitySlotsContainer
{
	SlotsIcon TransferHoverFind(EntityAI v_Item)
	{
		if (!v_Item || !m_Container)
			return null;
		int v_Rows = m_Container.Count();
		for (int v_R = 0; v_R < v_Rows; v_R++)
		{
			SlotsContainer v_Row = SlotsContainer.Cast(m_Container.Get(v_R));
			if (!v_Row)
				continue;
			for (int v_C = 0; v_C < ITEMS_IN_ROW; v_C++)
			{
				SlotsIcon v_Slot = v_Row.GetSlotIcon(v_C);
				if (v_Slot && v_Slot.GetMainWidget().IsVisible() && v_Slot.TransferHoverItem() == v_Item)
					return v_Slot;
			}
		}
		return null;
	}
}
