modded class LogoutMenu extends UIScriptedMenu
{	
	protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider;
	protected ButtonWidget m_LogoutNow, m_Cancel, m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook;
	private	Widget m_timerText

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-ui/gui/layouts/menus/inGame/cui.logout.layout");
	
		m_LogoutTimeText 	= TextWidget.Cast(layoutRoot.FindAnyWidget("txtLogoutTime"));

		m_LogoutNow			= layoutRoot.FindAnyWidget("ExitBtn");
		m_Cancel			= layoutRoot.FindAnyWidget("CancelBtn");
		m_PrioQ				= layoutRoot.FindAnyWidget("QueueBtn");
		m_Website			= layoutRoot.FindAnyWidget("WebsiteBtn");
		m_Discord 			= layoutRoot.FindAnyWidget("DiscordBtn");
		m_Twitter 			= layoutRoot.FindAnyWidget("TwitterBtn");
		m_Youtube 			= layoutRoot.FindAnyWidget("YoutubeBtn");
		m_Reddit 			= layoutRoot.FindAnyWidget("RedditBtn");
		m_Facebook 			= layoutRoot.FindAnyWidget("FacebookBtn");
		m_TopShader 		= layoutRoot.FindAnyWidget("TopShader");
		m_BottomShader 		= layoutRoot.FindAnyWidget("BottomShader");
		m_MenuDivider		= layoutRoot.FindAnyWidget("MenuDivider");
		
		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());
		m_MenuDivider.SetColor(colorScheme.Separator());
		m_LogoutTimeText.SetColor(colorScheme.LogOutTimer());

        cuiElmnt.proBtn(m_LogoutNow,"#main_menu_exit",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"abortMission");
		cuiElmnt.proBtn(m_Cancel,"Cancel",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"canelExit");

        cuiElmnt.proBtn(m_PrioQ,"Priority Queue",colorScheme.PrimaryText(),colorScheme.ButtonHover(),MenuURLS.urlPriorityQ);           
        cuiElmnt.proBtn(m_Website,"Visit Website",colorScheme.PrimaryText(),colorScheme.ButtonHover(),MenuURLS.urlWebsite);
        cuiElmnt.proBtn(m_Discord,"Discord",colorScheme.PrimaryText(),UIColor.Discord(),MenuURLS.urlDiscord);
        cuiElmnt.proBtn(m_Twitter,"Twitter",colorScheme.PrimaryText(),UIColor.Twitter(),MenuURLS.urlTwitter);  
        cuiElmnt.proBtn(m_Youtube,"Youtube",colorScheme.PrimaryText(),UIColor.YouTube(),MenuURLS.urlYoutube);
        cuiElmnt.proBtn(m_Reddit,"Reddit",colorScheme.PrimaryText(),UIColor.Reddit(),MenuURLS.urlReddit);
        cuiElmnt.proBtn(m_Facebook,"Facebook",colorScheme.PrimaryText(),UIColor.Facebook(),MenuURLS.urlFacebook);

        CheckURL(m_PrioQ,    MenuURLS.urlPriorityQ);
        CheckURL(m_Website,  MenuURLS.urlWebsite);
        CheckURL(m_Discord,  MenuURLS.urlDiscord);
        CheckURL(m_Twitter,  MenuURLS.urlTwitter);
        CheckURL(m_Youtube,  MenuURLS.urlYoutube);
        CheckURL(m_Reddit,   MenuURLS.urlReddit);
        CheckURL(m_Facebook, MenuURLS.urlFacebook);

        if (allInvalid && m_MenuDivider)
        {
            m_MenuDivider.Show(false);
        }

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player.GetEmoteManager() && !player.IsRestrained() && !player.IsUnconscious()) 
		{
			player.GetEmoteManager().CreateEmoteCBFromMenu(EmoteConstants.ID_EMOTE_LYINGDOWN);
			player.GetEmoteManager().GetEmoteLauncher().SetForced(EmoteLauncher.FORCE_DIFFERENT);
		}

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
}
