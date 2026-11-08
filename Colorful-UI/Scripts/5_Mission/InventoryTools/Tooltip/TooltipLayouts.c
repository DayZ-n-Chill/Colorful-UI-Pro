// The item tooltip and the slot name tooltip, Deluxe's rounded re-skins of vanilla's pair.
//
// Neither tooltip is a LayoutHolder, so there is no SetLayoutName to override: ItemManager builds both
// in its own constructor from two literal CreateWidgets calls (itemmanager.c:53-61). A modded
// constructor runs after vanilla's (Enforce chains constructors itself; there is no super call to
// write), so vanilla's pair already exists here. It is deleted and ours built in its place, then
// hidden the way vanilla hides its own.
//
// Desktop only: console keeps vanilla's xbox tooltip layouts. With InventoryTooltips (Settings.c) off,
// vanilla's pair is left as built.
class TooltipPaths
{
	static const string Item_Path = "Colorful-UI/GUI/layouts/inventory_tools/tooltip/TooltipItem.layout";
	static const string Slot_Path = "Colorful-UI/GUI/layouts/inventory_tools/tooltip/TooltipSlot.layout";
}

modded class ItemManager
{
	void ItemManager(Widget root)
	{
		if (!InventoryTooltips)
			return;
		#ifndef PLATFORM_CONSOLE
		delete m_TooltipWidget;
		delete m_TooltipSlotWidget;

		m_TooltipWidget		= g_Game.GetWorkspace().CreateWidgets(TooltipPaths.Item_Path, root);
		m_TooltipSlotWidget	= g_Game.GetWorkspace().CreateWidgets(TooltipPaths.Slot_Path, root);

		m_TooltipWidget.Show(false);
		m_TooltipSlotWidget.Show(false);
		#endif
	}
}
