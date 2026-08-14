// WarningMenuBase — item-drop and teleport warnings shown in CuiDialog.
// Vanilla source: P:\scripts\5_mission\gui\itemdropwarningmenu.c

modded class WarningMenuBase extends UIScriptedMenu
{
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.dialog_stub.layout");
		if (!layoutRoot) return null;

		string body = GetText();
		if (body == "") body = "An action will drop items from your character.";

		CuiDialog.Show("Warning", body, true, this, "DoClose", "DoClose");

		return layoutRoot;
	}

	void DoClose()
	{
		Close();
	}

	void ~WarningMenuBase()
	{
		cuiElmnt.CleanupForOwner(this);
	}
}
