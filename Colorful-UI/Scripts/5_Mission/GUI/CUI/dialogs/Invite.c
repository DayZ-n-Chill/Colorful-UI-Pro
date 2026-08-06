// InviteMenu override
// -----------------------------------------------------------------------------
// Vanilla `InviteMenu` (P:\scripts\5_mission\gui\invitemenu.c) shows a game
// invite popup with an auto-connect countdown. Our previous modded override
// referenced a non-existent layout (`cui.invite.dialog.layout`) and silently
// failed.
//
// This version delegates the visible UI to CuiDialog. Vanilla's 15s
// auto-connect countdown is intentionally NOT armed (its CallLater lives in
// vanilla Init(), which we replace) — instead the user chooses explicitly:
// Connect performs the join (same calls vanilla makes at countdown expiry),
// Cancel runs vanilla Cancel() to stay on the current session.

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
		// Vanilla's only connect path is the countdown expiry inside
		// Update() (invitemenu.c:78-85) — but the countdown CallLater is
		// armed in vanilla Init(), which we replaced, so m_iTime stays at 15
		// forever and that branch never fires. Perform the same join here.
		//
		// Vanilla deliberately does NOT close InviteMenu at this point —
		// ConnectFromJoin (dayzgame.c:2775) calls Connect(), which reads
		// Connect(GetUIManager().GetMenu(), ...) (dayzgame.c:2685) to hand
		// the still-open menu to the native connect/loading transition.
		// GetCallQueue(...).Call() only enqueues the call for the queue's
		// next Tick (P:\scripts\2_gamelib\tools.c:57, ticked every frame
		// from OnUpdate at dayzgame.c:2982) — it does not run synchronously.
		// Closing the menu here, before that Tick fires, would make
		// GetUIManager().GetMenu() return the wrong (or no) menu once
		// ConnectFromJoin actually executes. Queue Close() on the same
		// queue, after ConnectFromJoin, so it only runs once the native
		// call has already captured the menu.
		string ip;
		int port;
		OnlineServices.GetInviteServerInfo(ip, port);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.ConnectFromJoin, ip, port);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.Close);
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
