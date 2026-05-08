modded class MainMenu extends UIScriptedMenu
{
	protected ref MainMenuStats m_Stats;
	protected TextWidget m_PlayerName;
	protected ButtonWidget m_PrevCharacter, m_NextCharacter;
	protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider0, m_MenuDivider, m_StatisticsBoxBG, m_SurvivorBox, m_Logo;
	protected ButtonWidget m_Play, m_Exit, m_SettingsBtn, m_TutorialBtn, m_MessageBtn, m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook, m_CharacterBtn;
	protected ButtonWidget m_TestBtn0, m_TestBtn1, m_TestBtn2, m_TestBtn3, m_TestBtn4, m_TestBtn5;
	protected Widget m_TopSpacer, m_BottomSpacer;
	protected ProgressBarWidget m_LoadingBar;
	protected VideoWidget m_MenuVid;

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/cui.mainMenu.layout");

		m_Play              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("PlayBtn"));
		m_Exit              = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ExitBtn"));
		m_SettingsBtn       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("SettingsBtn"));
		m_TutorialBtn       = ButtonWidget.Cast(layoutRoot.FindAnyWidget("TutorialBtn"));
		m_MessageBtn        = ButtonWidget.Cast(layoutRoot.FindAnyWidget("MessageBtn"));
		m_CharacterBtn      = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CharacterBtn"));

		m_PrioQ             = ButtonWidget.Cast(layoutRoot.FindAnyWidget("QueueBtn"));
		m_Website           = ButtonWidget.Cast(layoutRoot.FindAnyWidget("WebsiteBtn"));
		m_Discord           = ButtonWidget.Cast(layoutRoot.FindAnyWidget("DiscordBtn"));
		m_Twitter           = ButtonWidget.Cast(layoutRoot.FindAnyWidget("TwitterBtn"));
		m_Youtube           = ButtonWidget.Cast(layoutRoot.FindAnyWidget("YoutubeBtn"));
		m_Reddit            = ButtonWidget.Cast(layoutRoot.FindAnyWidget("RedditBtn"));
		m_Facebook          = ButtonWidget.Cast(layoutRoot.FindAnyWidget("FacebookBtn"));

		m_TopShader         = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
		m_BottomShader      = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
		m_StatisticsBoxBG = ImageWidget.Cast(layoutRoot.FindAnyWidget("StatisticsBoxBG"));
		m_SurvivorBox = ImageWidget.Cast(layoutRoot.FindAnyWidget("SurvivorBox"));
		m_TopSpacer         = layoutRoot.FindAnyWidget("TopSpacer");
		m_MenuDivider       = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider"));
		m_MenuDivider0      = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider0"));
		m_BottomSpacer      = layoutRoot.FindAnyWidget("BottomSpacer");

		m_LoadingBar        = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
		m_Logo              = ImageWidget.Cast(layoutRoot.FindAnyWidget("Logo"));
		m_PlayerName        = TextWidget.Cast(layoutRoot.FindAnyWidget("character_name_text"));
		m_PrevCharacter     = ButtonWidget.Cast(layoutRoot.FindAnyWidget("prev_character"));
		m_NextCharacter     = ButtonWidget.Cast(layoutRoot.FindAnyWidget("next_character"));

		m_TestBtn0          = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn0"));
		m_TestBtn1          = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn1"));
		m_TestBtn2          = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn2"));
		m_TestBtn3          = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn3"));
		m_TestBtn4          = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn4"));
		m_TestBtn5          = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn5"));
		
		Widget stats_root = layoutRoot.FindAnyWidget("StatsBox");
		if (stats_root)
			m_Stats = new MainMenuStats(stats_root);

		if (m_StatisticsBoxBG) m_StatisticsBoxBG.SetColor(UIColor.cuiDarkBlue());
		if (m_SurvivorBox) m_SurvivorBox.SetColor(UIColor.cuiDarkBlue());
		if (m_TopShader) m_TopShader.SetColor(colorScheme.TopShader());
		if (m_BottomShader) m_BottomShader.SetColor(colorScheme.BottomShader());
		if (m_MenuDivider) m_MenuDivider.SetColor(colorScheme.Separator());
		if (m_MenuDivider0) m_MenuDivider0.SetColor(colorScheme.Separator());
		if (m_LoadingBar) m_LoadingBar.SetColor(colorScheme.Loadingbar());

		cuiElmnt.proBtnDC(ButtonWidget.Cast(m_Play), "#main_menu_play", colorScheme.PrimaryText(), colorScheme.ButtonHover(), SERVER_IP, SERVER_PORT);

		cuiElmnt.proBtnCB(ButtonWidget.Cast(m_Exit), "#main_menu_exit", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Exit");
		cuiElmnt.proBtnCB(ButtonWidget.Cast(m_SettingsBtn), "Settings", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenSettings");
		cuiElmnt.proBtnCB(ButtonWidget.Cast(m_TutorialBtn), "Tutorial", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenTutorials");
		cuiElmnt.proBtnCB(ButtonWidget.Cast(m_MessageBtn), "Credits", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenCredits");
		cuiElmnt.proBtnCB(ButtonWidget.Cast(m_CharacterBtn), "", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenMenuCustomizeCharacter");
		
		cuiElmnt.proBtnCB(ButtonWidget.Cast(m_PrevCharacter), "", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "PreviousCharacter");
		cuiElmnt.proBtnCB(ButtonWidget.Cast(m_NextCharacter), "", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "NextCharacter");

		cuiElmnt.proBtnURL(ButtonWidget.Cast(m_PrioQ), "Priority Queue", colorScheme.PrimaryText(), colorScheme.ButtonHover(), CustomURL.PriorityQ);
		cuiElmnt.proBtnURL(ButtonWidget.Cast(m_Website), "Visit Website", colorScheme.PrimaryText(), colorScheme.ButtonHover(), CustomURL.Website);

		cuiElmnt.proBtnURL(ButtonWidget.Cast(m_Discord), "Discord", colorScheme.PrimaryText(), UIColor.Discord(), SocialURL.Discord);
		cuiElmnt.proBtnURL(ButtonWidget.Cast(m_Twitter), "Twitter", colorScheme.PrimaryText(), UIColor.Twitter(), SocialURL.Twitter);
		cuiElmnt.proBtnURL(ButtonWidget.Cast(m_Youtube), "Youtube", colorScheme.PrimaryText(), UIColor.YouTube(), SocialURL.Youtube);
		cuiElmnt.proBtnURL(ButtonWidget.Cast(m_Reddit), "Reddit", colorScheme.PrimaryText(), UIColor.Reddit(), SocialURL.Reddit);
		cuiElmnt.proBtnURL(ButtonWidget.Cast(m_Facebook), "Facebook", colorScheme.PrimaryText(), UIColor.Facebook(), SocialURL.Facebook);

		allInvalid = true; // reset shared state before this menu's checks
		CheckURL(m_PrioQ, CustomURL.PriorityQ);
		CheckURL(m_Website, CustomURL.Website);
		CheckSocials(m_Discord, SocialURL.Discord);
		CheckSocials(m_Twitter, SocialURL.Twitter);
		CheckSocials(m_Youtube, SocialURL.Youtube);
		CheckSocials(m_Reddit, SocialURL.Reddit);
		CheckSocials(m_Facebook, SocialURL.Facebook);
		
		if (allInvalid && m_MenuDivider)
		{
			if (m_TopSpacer) m_TopSpacer.Show(false);
			m_MenuDivider.Show(false);
			if (m_BottomSpacer) m_BottomSpacer.Show(false);
		}
		
		Branding.ApplyLogo(m_Logo);	

		#ifdef WORKBENCH
		CuiLogger.Log("Skipping video in Workbench mode");
		#else
		if (EnableMenuVideo)
		{
			Class.CastTo(m_MenuVid, layoutRoot.FindAnyWidget("MenuVideo"));
			if (m_MenuVid)
			{
				if (!FileExist("$saves:" + m_MainMenuVideo))
					CopyFile("Colorful-UI/GUI/video/" + m_MainMenuVideo, "$saves:" + m_MainMenuVideo);

				m_MenuVid.Load("$saves:" + m_MainMenuVideo, true);
				m_MenuVid.Play();
			}
		}
		#endif		
		return layoutRoot;
	}
	
	override void OnShow()
	{
		CuiLogger.Log("MainMenu.OnShow() - Validating UI elements");
		if (!m_LoadingBar) m_LoadingBar = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
		if (!m_Logo) m_Logo = ImageWidget.Cast(layoutRoot.FindAnyWidget("Logo"));
		if (!m_TopShader) m_TopShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
		if (!m_BottomShader) m_BottomShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
		
		if (m_Stats) m_Stats.UpdateStats();
		OnChangeCharacter(false);
	}
	
	override void Refresh()
	{
		super.Refresh();
		OnChangeCharacter(false);
	}
	
	override void OnChangeCharacter(bool create_character = true)
	{
		MissionMainMenu mission = MissionMainMenu.Cast(GetGame().GetMission());
		if (mission && mission.GetIntroScenePC() && mission.GetIntroScenePC().GetIntroCharacter())
		{
			int charID = mission.GetIntroScenePC().GetIntroCharacter().GetCharacterID();
			if (create_character)
			{
				mission.GetIntroScenePC().GetIntroCharacter().CreateNewCharacterById(charID);
			}
			
			string name = mission.GetIntroScenePC().GetIntroCharacter().GetCharacterNameById(charID);
			CuiLogger.Log("MainMenu.OnChangeCharacter() - CharID: " + charID + " Name: " + name);
			
			if (m_PlayerName)
				m_PlayerName.SetText(name);
			
			if (m_Stats) m_Stats.UpdateStats();
		}
		else
		{
			CuiLogger.Log("MainMenu.OnChangeCharacter() - FAILED: Mission or IntroScene is NULL");
		}
	}
	
	override void NextCharacter()
	{
		MissionMainMenu mission = MissionMainMenu.Cast(GetGame().GetMission());
		if (mission && mission.GetIntroScenePC() && mission.GetIntroScenePC().GetIntroCharacter())
		{
			int charID = mission.GetIntroScenePC().GetIntroCharacter().GetNextCharacterID();
			if (charID != mission.GetIntroScenePC().GetIntroCharacter().GetCharacterID())
			{
				mission.GetIntroScenePC().GetIntroCharacter().SetCharacterID(charID);
				OnChangeCharacter();
			}
		}
	}
	
	override void PreviousCharacter()
	{
		MissionMainMenu mission = MissionMainMenu.Cast(GetGame().GetMission());
		if (mission && mission.GetIntroScenePC() && mission.GetIntroScenePC().GetIntroCharacter())
		{
			int charID = mission.GetIntroScenePC().GetIntroCharacter().GetPrevCharacterID();
			if (charID != mission.GetIntroScenePC().GetIntroCharacter().GetCharacterID())
			{
				mission.GetIntroScenePC().GetIntroCharacter().SetCharacterID(charID);
				OnChangeCharacter();
			}
		}
	}
}
