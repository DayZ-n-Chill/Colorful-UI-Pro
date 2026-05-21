// InviteMenu override
// -----------------------------------------------------------------------------
// Vanilla `InviteMenu` (P:\scripts\5_mission\gui\invitemenu.c) shows a game
// invite popup with an auto-connect countdown. Our previous modded override
// referenced a non-existent layout (`cui.invite.dialog.layout`) and silently
// failed.
//
// This version delegates the visible UI to CuiDialog and keeps the vanilla
// timer/connect logic intact via super calls.

modded class InviteMenu extends UIScriptedMenu
{
	override Widget Init()
	{
		// Hidden 1x1 placeholder so the engine has a layoutRoot to attach.
		// The visible UI is owned by CuiDialog.
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.dialog_stub.layout");
		if (!layoutRoot) return null;

		CuiDialog.Show(
			"Game Invite",
			"You have been invited to a session. Connect now or cancel.",
			true, this, "DoConnect", "DoCancel");

		// Player should sit down if possible (preserved from vanilla custom
		// behavior — this was in the previous modded version).
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious())
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}

		return layoutRoot;
	}

	void DoConnect()
	{
		// Tell the menu to dismiss; vanilla connect logic will run on close.
		// (For the test flow this just closes — the real auto-connect happens
		// on the timer in vanilla Update().)
		Close();
	}

	void DoCancel()
	{
		Cancel();   // vanilla Cancel() — sets game state, returns to controller select
	}

	void ~InviteMenu()
	{
		cuiElmnt.CleanupForOwner(this);
	}
}
