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

		// The stub can't be clicked and vanilla blocks ESC, so if the dialog failed to draw
		// fall back to vanilla's layout, which has a working OK button.
		CuiDialog dlg = CuiDialog.Show("Warning", body, true, this, "DoClose", "DoClose");
		if (!dlg)
		{
			layoutRoot.Unlink();
			return super.Init();
		}

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
