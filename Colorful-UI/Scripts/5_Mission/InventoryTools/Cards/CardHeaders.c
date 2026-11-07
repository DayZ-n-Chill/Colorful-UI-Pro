// The card headers: always Colorful UI's header layouts. With RoundedInventory (Settings.c) off,
// InventoryLook.Layout loads a copy whose background is the original game header instead of the
// rounded panel; text and buttons sit exactly where they do in the rounded one. These used to be
// WidgetLayoutName consts, which cannot follow a setting, so each header class that builds one
// (scripts/5_mission/gui/inventorynew/containeditems/headers) picks it in SetLayoutName. LayoutHolder
// builds from m_LayoutName right after SetLayoutName (layoutholder.c), so this is early enough.
// AttachmentsGroupContainer's layout is picked in CardCorners.c.
class CardHeaderLayouts
{
	// The 2 px HeaderRule along the bottom of the EQUIPMENT, VICINITY and HANDS bars, painted the brand
	// colour (Scheme.c) so it follows the theme; the colour in the layouts is only a Workbench preview.
	// The per-item bars (closable_header) have no rule, as on DayZ Deluxe.
	static void TintRule(Widget root)
	{
		if (!root)
			return;
		Widget rule = root.FindAnyWidget("HeaderRule");
		if (rule)
			rule.SetColor(colorScheme.BrandColor());
	}


	static const string Closable_Path		= "Colorful-UI/GUI/layouts/inventory/closable_header.layout";
	static const string Collapsible_Path	= "Colorful-UI/GUI/layouts/inventory/collapsible_header.layout";
	static const string Hands_Path			= "Colorful-UI/GUI/layouts/inventory/hands_header.layout";
}

modded class ClosableHeader
{
	override void SetLayoutName()
	{
		super.SetLayoutName();
		m_LayoutName = InventoryLook.Layout(CardHeaderLayouts.Closable_Path);
	}
}

modded class CollapsibleHeader
{
	override void SetLayoutName()
	{
		super.SetLayoutName();
		m_LayoutName = InventoryLook.Layout(CardHeaderLayouts.Collapsible_Path);
	}

	// VICINITY and EQUIPMENT both come through this class. Signature matches vanilla collapsibleheader.c.
	void CollapsibleHeader(LayoutHolder parent, string function_name)
	{
		CardHeaderLayouts.TintRule(GetMainWidget());
	}
}

modded class HandsHeader
{
	override void SetLayoutName()
	{
		super.SetLayoutName();
		m_LayoutName = InventoryLook.Layout(CardHeaderLayouts.Hands_Path);
	}

	// Signature matches vanilla handsheader.c.
	void HandsHeader(LayoutHolder parent, string function_name)
	{
		CardHeaderLayouts.TintRule(GetMainWidget());
	}
}
