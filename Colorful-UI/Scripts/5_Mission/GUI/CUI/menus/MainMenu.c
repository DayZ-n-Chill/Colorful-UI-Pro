modded class MainMenu extends UIScriptedMenu
{
    protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider;
    protected ButtonWidget m_Play, m_Exit, m_SettingsBtn, m_TutorialBtn, m_MessageBtn, m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook;
	protected Widget m_TopSpacer, m_BottomSpacer;

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets( "Colorful-UI/GUI/layouts/menus/cui.mainMenu.layout" );
	
    	m_Play              = layoutRoot.FindAnyWidget("PlayBtn");
        m_Exit              = layoutRoot.FindAnyWidget("ExitBtn");
        m_SettingsBtn       = layoutRoot.FindAnyWidget("SettingsBtn");
        m_TutorialBtn       = layoutRoot.FindAnyWidget("TutorialBtn");
        m_MessageBtn        = layoutRoot.FindAnyWidget("MessageBtn");

        m_PrioQ             = layoutRoot.FindAnyWidget("QueueBtn");
        m_Website           = layoutRoot.FindAnyWidget("WebsiteBtn");
        m_Discord           = layoutRoot.FindAnyWidget("DiscordBtn");
        m_Twitter           = layoutRoot.FindAnyWidget("TwitterBtn");
        m_Youtube           = layoutRoot.FindAnyWidget("YoutubeBtn");
        m_Reddit            = layoutRoot.FindAnyWidget("RedditBtn");
        m_Facebook          = layoutRoot.FindAnyWidget("FacebookBtn");

        m_TopShader         = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_BottomShader      = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
        m_TopSpacer         = layoutRoot.FindAnyWidget("TopSpacer");
        m_MenuDivider       = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider"));
        m_BottomSpacer      = layoutRoot.FindAnyWidget("BottomSpacer");

        if (m_TopShader)    m_TopShader.SetColor(colorScheme.TopShader());
        if (m_BottomShader) m_BottomShader.SetColor(colorScheme.BottomShader());
        if (m_MenuDivider)  m_MenuDivider.SetColor(colorScheme.Separator());

        cuiElmnt.proBtn(m_Play,"#main_menu_play",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"Play");
        cuiElmnt.proBtn(m_Exit,"#main_menu_exit",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"Exit");
        cuiElmnt.proBtn(m_SettingsBtn,"Settings",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"OpenSettings");
        cuiElmnt.proBtn(m_TutorialBtn,"Tutorial",colorScheme.PrimaryText(),colorScheme.ButtonHover(),MenuURLS.urlPriorityQ);
        cuiElmnt.proBtn(m_MessageBtn,"Credits",colorScheme.PrimaryText(),colorScheme.ButtonHover(),MenuURLS.urlPriorityQ);

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
            m_TopSpacer.Show(false);
            m_MenuDivider.Show(false);
            m_BottomSpacer.Show(false);
        }
       
		return layoutRoot;
	}	    
    
	override void Exit() {super.Exit();}
    override void OpenSettings(){super.OpenSettings();}
    override void OpenTutorials(){super.OpenTutorials();}
    override void OpenCredits(){super.OpenCredits();}
    override void PreviousCharacter(){super.PreviousCharacter();}
    override void NextCharacter(){super.NextCharacter();}	
}
