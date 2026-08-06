// The "your items will be dropped" and "you will be teleported" warnings,
// shown in the Colorful-UI dialog. Information only — either button just
// dismisses it.

modded class WarningMenuBase extends UIScriptedMenu
{
	override Widget Init()
	{
		// Invisible placeholder: the engine requires a layout here, but the
		// visible popup is the CuiDialog created below.
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.dialog_stub.layout");
		if (!layoutRoot) return null;

		string body = GetText();
		if (body == "") body = "An action will drop items from your character.";

		// Both buttons dismiss: there is nothing to confirm or cancel.
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
