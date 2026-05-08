modded class InviteMenu extends UIScriptedMenu
{
	private TextWidget		m_LogoutTimetext;
	private TextWidget		m_Info;
	private int m_iTime;
    private Widget m_Separator1;
	private	Widget m_shader;

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.invite.dialog.layout");
		if (!layoutRoot) return null;

		m_LogoutTimetext = TextWidget.Cast( layoutRoot.FindAnyWidget("logoutTimeText") );
		m_Info = TextWidget.Cast( layoutRoot.FindAnyWidget("txtInfo") );
		if (m_LogoutTimetext) m_LogoutTimetext.SetText(m_iTime.ToString());

		// Class.CastTo(m_shader, layoutRoot.FindAnyWidget("Colorful_Shader"));
		// m_shader.SetColor(colorScheme.ShaderColor());

		Widget toolbar_bg = layoutRoot.FindAnyWidget("toolbar_bg");
		if (toolbar_bg) toolbar_bg.Show(true);
		RichTextWidget toolbar_b = RichTextWidget.Cast(layoutRoot.FindAnyWidget("BackIcon"));
		if (toolbar_b) toolbar_b.SetText(InputUtils.GetRichtextButtonIconFromInputAction("UAUIBack", "", EUAINPUT_DEVICE_CONTROLLER, InputUtils.ICON_SCALE_TOOLBAR));
		
		// player should sit down if possible
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_SITA);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}
		
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( UpdateTime, 1000, true );
        // m_Separator1 = layoutRoot.FindAnyWidget( "SeparatorPanel" );
        // m_Separator1.SetColor(colorScheme.SeparatorColor());
		return layoutRoot;
	}
	

	void ~InviteMenu()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(UpdateTime);
		cuiElmnt.CleanupForOwner(this);
	}
}