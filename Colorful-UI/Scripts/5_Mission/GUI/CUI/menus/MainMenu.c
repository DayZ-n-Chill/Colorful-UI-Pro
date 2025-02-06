modded class MainMenu extends UIScriptedMenu
{
    protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider, m_StatisticsBoxBG, m_SurvivorBox;
    protected ButtonWidget m_Play, m_Exit, m_SettingsBtn, m_TutorialBtn, m_MessageBtn, m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook, m_CharacterBtn;
	protected Widget m_TopSpacer, m_BottomSpacer;
    protected ProgressBarWidget m_LoadingBar;

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets( "Colorful-UI/GUI/layouts/menus/cui.mainMenu.layout" );
	
    	m_Play              = layoutRoot.FindAnyWidget("PlayBtn");
        m_Exit              = layoutRoot.FindAnyWidget("ExitBtn");
        m_SettingsBtn       = layoutRoot.FindAnyWidget("SettingsBtn");
        m_TutorialBtn       = layoutRoot.FindAnyWidget("TutorialBtn");
        m_MessageBtn        = layoutRoot.FindAnyWidget("MessageBtn");
        m_CharacterBtn      = layoutRoot.FindAnyWidget("CharacterBtn");

        m_PrioQ             = layoutRoot.FindAnyWidget("QueueBtn");
        m_Website           = layoutRoot.FindAnyWidget("WebsiteBtn");
        m_Discord           = layoutRoot.FindAnyWidget("DiscordBtn");
        m_Twitter           = layoutRoot.FindAnyWidget("TwitterBtn");
        m_Youtube           = layoutRoot.FindAnyWidget("YoutubeBtn");
        m_Reddit            = layoutRoot.FindAnyWidget("RedditBtn");
        m_Facebook          = layoutRoot.FindAnyWidget("FacebookBtn");

        m_TopShader         = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_BottomShader      = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
        m_StatisticsBoxBG   = layoutRoot.FindAnyWidget("StatisticsBoxBG");
        m_SurvivorBox       = layoutRoot.FindAnyWidget("SurvivorBox");
        m_TopSpacer         = layoutRoot.FindAnyWidget("TopSpacer");
        m_MenuDivider       = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider"));
        m_BottomSpacer      = layoutRoot.FindAnyWidget("BottomSpacer");

        m_LoadingBar        = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));

        // Set the colors of UI Elements
        m_StatisticsBoxBG.SetColor(UIColor.cuiDarkBlue());
        m_SurvivorBox.SetColor(UIColor.cuiDarkBlue());
        m_TopShader.SetColor(colorScheme.TopShader());
        m_BottomShader.SetColor(colorScheme.BottomShader());
        m_MenuDivider.SetColor(colorScheme.Separator());
        m_LoadingBar.SetColor(colorScheme.Loadingbar());

        // Example of a button that directly connects to a server
        cuiElmnt.proBtnDC(m_Play,"#main_menu_play",colorScheme.PrimaryText(),colorScheme.ButtonHover(),SERVER_IP,SERVER_PORT);

        // Example of a button with a callbacks
        cuiElmnt.proBtnCB(m_Exit,"#main_menu_exit",colorScheme.PrimaryText(),colorScheme.ButtonHover(),this,"Exit");
        cuiElmnt.proBtnCB(m_SettingsBtn,"Settings",colorScheme.PrimaryText(),colorScheme.ButtonHover(),this,"OpenSettings");
        cuiElmnt.proBtnCB(m_TutorialBtn,"Tutorial",colorScheme.PrimaryText(),colorScheme.ButtonHover(),this,"OpenTutorials");
        cuiElmnt.proBtnCB(m_MessageBtn,"Credits",colorScheme.PrimaryText(),colorScheme.ButtonHover(),this,"OpenCredits");
        
        // TODO:  I need to add a variant to the Pro Button function for just icons that are not default along side a buttons text
        //        Gotta add the left and right button icons
        cuiElmnt.proBtnCB(m_CharacterBtn,"Credits",colorScheme.PrimaryText(),colorScheme.ButtonHover(),this,"OpenMenuCustomizeCharacter"); 

		// Custom Links
        cuiElmnt.proBtn(m_PrioQ,"Priority Queue",colorScheme.PrimaryText(),colorScheme.ButtonHover(),CustomURL.PriorityQ);           
        cuiElmnt.proBtn(m_Website,"Visit Website",colorScheme.PrimaryText(),colorScheme.ButtonHover(),CustomURL.Website);

		// Social Links
        cuiElmnt.proBtn(m_Discord,"Discord",colorScheme.PrimaryText(),UIColor.Discord(),SocialURL.Discord);
        cuiElmnt.proBtn(m_Twitter,"Twitter",colorScheme.PrimaryText(),UIColor.Twitter(),SocialURL.Twitter);  
        cuiElmnt.proBtn(m_Youtube,"Youtube",colorScheme.PrimaryText(),UIColor.YouTube(),SocialURL.Youtube);
        cuiElmnt.proBtn(m_Reddit,"Reddit",colorScheme.PrimaryText(),UIColor.Reddit(),SocialURL.Reddit);
        cuiElmnt.proBtn(m_Facebook,"Facebook",colorScheme.PrimaryText(),UIColor.Facebook(),SocialURL.Facebook);

		// These checks show/hide what buttons that are invalid or null
        CheckURL(m_PrioQ,    	 CustomURL.PriorityQ);
        CheckURL(m_Website,  	 CustomURL.Website);
        CheckSocials(m_Discord,  SocialURL.Discord);
        CheckSocials(m_Twitter,  SocialURL.Twitter);
        CheckSocials(m_Youtube,  SocialURL.Youtube);
        CheckSocials(m_Reddit,   SocialURL.Reddit);
        CheckSocials(m_Facebook, SocialURL.Facebook);

        // Hide the spacer specific buttons are invalid
        if (allInvalid && m_MenuDivider)
        {
            m_TopSpacer.Show(false);
            m_MenuDivider.Show(false);
            m_BottomSpacer.Show(false);
        }
       
		return layoutRoot;
	}	    
}
