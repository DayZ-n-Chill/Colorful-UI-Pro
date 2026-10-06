// LogoutMenu — CUI logout screen.
// Vanilla source: P:\scripts\5_mission\gui\logoutmenu.c

modded class LogoutMenu extends UIScriptedMenu
{	
	protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider, m_Logo;
	protected ButtonWidget m_LogoutNow, m_Cancel, m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook;
	protected Widget m_TopSpacer, m_BottomSpacer;
	private	Widget m_timerText;
	protected TextWidget m_LogoutTimeText;
	protected int m_CuiLogoutTime;

        override Widget Init()
        {
                layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/inGame/cui.logout.layout");

		m_Logo 				= ImageWidget.Cast(layoutRoot.FindAnyWidget("Logo"));
		m_LogoutTimeText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLogoutTime"));

		m_LogoutNow = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ExitBtn"));
		m_Cancel = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CancelBtn"));

		m_PrioQ = ButtonWidget.Cast(layoutRoot.FindAnyWidget("QueueBtn"));
		m_Website = ButtonWidget.Cast(layoutRoot.FindAnyWidget("WebsiteBtn"));
		m_Discord = ButtonWidget.Cast(layoutRoot.FindAnyWidget("DiscordBtn"));
		m_Twitter = ButtonWidget.Cast(layoutRoot.FindAnyWidget("TwitterBtn"));
		m_Youtube = ButtonWidget.Cast(layoutRoot.FindAnyWidget("YoutubeBtn"));
		m_Reddit = ButtonWidget.Cast(layoutRoot.FindAnyWidget("RedditBtn"));
		m_Facebook = ButtonWidget.Cast(layoutRoot.FindAnyWidget("FacebookBtn"));

		m_TopShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
		m_BottomShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
		
		m_TopSpacer         = layoutRoot.FindAnyWidget("TopSpacer");
        m_MenuDivider       = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider"));
        m_BottomSpacer      = layoutRoot.FindAnyWidget("BottomSpacer");
		
        m_TopShader.SetColor(colorScheme.TopShader());
        m_BottomShader.SetColor(colorScheme.BottomShader());
        m_MenuDivider.SetColor(colorScheme.Separator());
		m_LogoutTimeText.SetColor(colorScheme.LogOutTimer());

        cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_LogoutNow),"#main_menu_exit",colorScheme.PrimaryText(),colorScheme.ButtonHover(),this,"abortMission");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Cancel),"#dialog_cancel",colorScheme.PrimaryText(),colorScheme.ButtonHover(),this,"canelExit");

        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_PrioQ),CuiLoc.Get("CUI_priority_queue"),colorScheme.PrimaryText(),colorScheme.ButtonHover(),CustomURL.PriorityQ);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Website),"#mod_detail_info_website",colorScheme.PrimaryText(),colorScheme.ButtonHover(),CustomURL.Website);

        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Discord),"Discord",colorScheme.PrimaryText(),UIColor.Discord(),SocialURL.Discord);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Twitter),"Twitter",colorScheme.PrimaryText(),UIColor.Twitter(),SocialURL.Twitter);  
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Youtube),"Youtube",colorScheme.PrimaryText(),UIColor.YouTube(),SocialURL.Youtube);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Reddit),"Reddit",colorScheme.PrimaryText(),UIColor.Reddit(),SocialURL.Reddit);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Facebook),"Facebook",colorScheme.PrimaryText(),UIColor.Facebook(),SocialURL.Facebook);

        allInvalid = true;
        CheckURL(m_PrioQ,    	 CustomURL.PriorityQ);
        CheckURL(m_Website,  	 CustomURL.Website);
        CheckSocials(m_Discord,  SocialURL.Discord);
        CheckSocials(m_Twitter,  SocialURL.Twitter);
        CheckSocials(m_Youtube,  SocialURL.Youtube);
        CheckSocials(m_Reddit,   SocialURL.Reddit);
        CheckSocials(m_Facebook, SocialURL.Facebook);

        if (allInvalid && m_MenuDivider)
        {
            m_TopSpacer.Show(false);
            m_MenuDivider.Show(false);
            m_BottomSpacer.Show(false);
        }

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious())
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_LYINGDOWN);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}
		
		Branding.ApplyLogo(m_Logo);
		return layoutRoot;
	}

        void abortMission()
        {
                GetGame().GetMission().AbortMission();
        }

        void canelExit()
        {
                Hide();
                Cancel();
        }

	// Vanilla's m_LogoutTimeText, m_DescriptionText and m_iTime are private and never set
	// by this Init, so the vanilla countdown methods null-deref. These drive our widgets.
	override void SetTime(int time)
	{
		m_CuiLogoutTime = time;
		if (!m_LogoutTimeText) return;

		FullTimeData ft = new FullTimeData();
		TimeConversions.ConvertSecondsToFullTime(time, ft);

		string text = "#layout_logout_dialog_until_logout_";
		if (ft.m_Days > 0)         text += "dhms";
		else if (ft.m_Hours > 0)   text += "hms";
		else if (ft.m_Minutes > 0) text += "ms";
		else                       text += "s";

		text = Widget.TranslateString(text);
		text = string.Format(text, ft.m_Seconds, ft.m_Minutes, ft.m_Hours, ft.m_Days);
		m_LogoutTimeText.SetText(text);
	}

	override void UpdateTime()
	{
		if (m_CuiLogoutTime > 0)
			SetTime(--m_CuiLogoutTime);
		else
			Exit();
	}

	override void SetLogoutTime()
	{
		if (m_LogoutTimeText) m_LogoutTimeText.SetText(" ");
	}

	override void UpdateInfo()
	{
	}

	void ~LogoutMenu()
	{
		cuiElmnt.CleanupForOwner(this);
	}
}
