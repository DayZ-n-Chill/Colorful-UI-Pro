// InviteMenu — game-invite prompt shown in CuiDialog.
// Vanilla source: P:\scripts\5_mission\gui\invitemenu.c

modded class InviteMenu extends UIScriptedMenu
{
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.dialog_stub.layout");
		if (!layoutRoot) return null;

		// The stub can't be clicked and vanilla blocks ESC, so if the dialog failed to draw
		// fall back to vanilla's layout, which has a working Cancel button.
		CuiDialog dlg = CuiDialog.Show(
			"Game Invite",
			"You have been invited to a session. Connect now or cancel.",
			true, this, "DoConnect", "DoCancel");
		if (!dlg)
		{
			layoutRoot.Unlink();
			return super.Init();
		}

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
		string ip;
		int port;
		OnlineServices.GetInviteServerInfo(ip, port);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.ConnectFromJoin, ip, port);
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.Close);
	}

	void DoCancel()
	{
		Cancel();
	}

	void ~InviteMenu()
	{
		cuiElmnt.CleanupForOwner(this);
	}
}
