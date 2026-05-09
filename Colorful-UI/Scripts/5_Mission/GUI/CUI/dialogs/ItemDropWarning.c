// WarningMenuBase override
// -----------------------------------------------------------------------------
// Vanilla `WarningMenuBase` (and its `PlayerRepositionWarningMenu` subclass) is
// info-only: a single OK button calls `Close()` on the menu. We delegate the
// actual UI to our CuiDialog and use a hidden stub layoutRoot so the
// UIScriptedMenu lifecycle still works.
//
// Path:
//   vanilla code calls EnterScriptedMenu(MENU_WARNING_ITEMDROP / _TELEPORT)
//   -> our Init() runs, builds a hidden stub root, spawns CuiDialog
//   -> user clicks Confirm or Cancel on CuiDialog
//   -> our callback fires Close() on the UIScriptedMenu, which lets vanilla
//      run its post-close logic correctly.

modded class WarningMenuBase extends UIScriptedMenu
{
	override Widget Init()
	{
		// Hidden 1x1 placeholder so the engine has a layoutRoot to attach.
		// The visible UI is owned by CuiDialog below.
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.dialog_stub.layout");

		string body = GetText();
		if (body == "") body = "An action will drop items from your character.";

		// Info-only dialog — both Confirm and Cancel route through DoClose so
		// the wrapping vanilla menu unwinds cleanly after the user dismisses.
		CuiDialog.Show("Warning", body, true, this, "DoClose", "DoClose");

		return layoutRoot;
	}

	void DoClose()
	{
		Close();   // close the wrapping UIScriptedMenu
	}

	void ~WarningMenuBase()
	{
		cuiElmnt.CleanupForOwner(this);
	}
}
