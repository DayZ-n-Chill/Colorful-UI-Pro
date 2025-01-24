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

        cuiElmnt.proBtn(m_PrioQ,"Priority Queue",colorScheme.PrimaryText(),colorScheme.ButtonHover(),CustomURL.PriorityQ);           
        cuiElmnt.proBtn(m_Website,"Visit Website",colorScheme.PrimaryText(),colorScheme.ButtonHover(),CustomURL.Website);
        cuiElmnt.proBtn(m_Discord,"Discord",colorScheme.PrimaryText(),UIColor.Discord(),SocialURL.Discord);
        cuiElmnt.proBtn(m_Twitter,"Twitter",colorScheme.PrimaryText(),UIColor.Twitter(),SocialURL.Twitter);  
        cuiElmnt.proBtn(m_Youtube,"Youtube",colorScheme.PrimaryText(),UIColor.YouTube(),SocialURL.Youtube);
        cuiElmnt.proBtn(m_Reddit,"Reddit",colorScheme.PrimaryText(),UIColor.Reddit(),SocialURL.Reddit);
        cuiElmnt.proBtn(m_Facebook,"Facebook",colorScheme.PrimaryText(),UIColor.Facebook(),SocialURL.Facebook);

        CheckURL(m_PrioQ,    CustomURL.PriorityQ);
        CheckURL(m_Website,  CustomURL.Website);
        CheckURL(m_Discord,  SocialURL.Discord);
        CheckURL(m_Twitter,  SocialURL.Twitter);
        CheckURL(m_Youtube,  SocialURL.Youtube);
        CheckURL(m_Reddit,   SocialURL.Reddit);
        CheckURL(m_Facebook, SocialURL.Facebook);

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
