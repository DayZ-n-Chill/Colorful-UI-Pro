// Picks the rounded or original-style copy of a Colorful UI card header from RoundedInventory
// (Settings.c). Each header in layouts/inventory has a twin under layouts/inventory/square/ that draws
// the original game header background instead of the rounded panel; text, buttons and sizes are the
// same. The inventory tools' own buttons, tips and menus stay rounded whatever the setting.
class InventoryLook
{
	static const string Cards_Root = "Colorful-UI/GUI/layouts/inventory/";

	static string Layout(string path)
	{
		if (RoundedInventory || path.IndexOf(Cards_Root) != 0)
			return path;
		return Cards_Root + "square/" + path.Substring(Cards_Root.Length(), path.Length() - Cards_Root.Length());
	}
}
