// Reports what the cursor is over to QuickAssign (4_World). See QuickAssign.c for why
// the hover is pushed from here rather than read from ItemManager.

// Equipment and vicinity slots, and the hands.
// Vanilla: P:\scripts\5_mission\gui\inventorynew\containeditems\slotsicon.c:94,107
modded class SlotsIcon
{
	override bool MouseEnter(Widget w, int x, int y)
	{
		bool v_Result = super.MouseEnter(w, x, y);
		if (InventoryQuickSlots)
			QuickAssign.SetHovered(GetEntity());
		return v_Result;
	}

	override bool MouseLeave(Widget w, Widget s, int x, int y)
	{
		QuickAssign.SetHovered(null);
		return super.MouseLeave(w, s, x, y);
	}
}

// One cargo cell.
// Vanilla: P:\scripts\5_mission\gui\inventorynew\containeditems\icon.c:344,355
modded class Icon
{
	override bool MouseEnter(Widget w, int x, int y)
	{
		bool v_Result = super.MouseEnter(w, x, y);
		if (InventoryQuickSlots)
			QuickAssign.SetHovered(EntityAI.Cast(GetObject()));
		return v_Result;
	}

	override bool MouseLeave(Widget w, Widget s, int x, int y)
	{
		QuickAssign.SetHovered(null);
		return super.MouseLeave(w, s, x, y);
	}
}
