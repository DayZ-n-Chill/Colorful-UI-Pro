// MainMenu, stats and credits — CUI main menu.
// Vanilla source: P:\scripts\5_mission\gui\newui\mainmenu\mainmenu.c

modded class MainMenu extends UIScriptedMenu
{
	protected ref MainMenuStats m_Stats;
	protected TextWidget m_PlayerName;
	protected TextWidget m_WelcomeBack;
	protected ButtonWidget m_PrevCharacter, m_NextCharacter;
	protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider, m_StatisticsBoxBG, m_SurvivorBox, m_Logo, m_Background;
	protected Widget m_MenuDivider0;
	protected ButtonWidget m_Play, m_Exit, m_SettingsBtn, m_TutorialBtn, m_MessageBtn, m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook, m_CharacterBtn;
	protected ButtonWidget m_TestBtn0, m_TestBtn1, m_TestBtn2, m_TestBtn3, m_TestBtn4, m_TestBtn5;
	protected Widget m_TopSpacer, m_BottomSpacer;
	protected ProgressBarWidget m_LoadingBar;

	protected ref CUI_ErrorTestScreen m_ErrorTestScreen;

	static const float ICON_TEXT_GAP = 8;

	protected int m_FitRetries;

	protected void SetNavLabel(ButtonWidget btn, string key)
	{
		if (!btn) return;

		TextWidget label = TextWidget.Cast(btn.FindAnyWidget(btn.GetName() + "_label"));
		if (label) label.SetText(key);
	}

	protected void RefreshNavLabels()
	{
		SetNavLabel(ButtonWidget.Cast(m_MessageBtn),  "#menu_credits");
		SetNavLabel(ButtonWidget.Cast(m_TutorialBtn), "#menu_tutorials");
		SetNavLabel(ButtonWidget.Cast(m_SettingsBtn), "#layout_xbox_ingame_menu_options");
		SetNavLabel(ButtonWidget.Cast(m_Exit),        "#main_menu_exit");
	}

	protected bool FitIconButton(ButtonWidget btn)
	{
		if (!btn) return true;

		TextWidget label = TextWidget.Cast(btn.FindAnyWidget(btn.GetName() + "_label"));
		if (!label) return true;

		label.Update();

		int textW, textH;
		label.GetTextSize(textW, textH);
		if (textW <= 0) return false;

		ImageWidget icon = ImageWidget.Cast(btn.FindAnyWidget(btn.GetName() + "_img"));
		if (!icon) icon = ImageWidget.Cast(btn.FindAnyWidget(btn.GetName() + "_image"));

		float iconW, iconH;
		if (icon) icon.GetSize(iconW, iconH);

		float btnW, btnH;
		btn.GetSize(btnW, btnH);
		btn.SetSize(textW + ICON_TEXT_GAP + iconW, btnH);

		Widget container = btn.GetParent();
		if (container) container.Update();

		return true;
	}

	protected void FitTopNav()
	{
		if (!layoutRoot) return;

		bool ok = true;
		ok = FitIconButton(ButtonWidget.Cast(m_MessageBtn))  && ok;
		ok = FitIconButton(ButtonWidget.Cast(m_TutorialBtn)) && ok;
		ok = FitIconButton(ButtonWidget.Cast(m_SettingsBtn)) && ok;
		ok = FitIconButton(ButtonWidget.Cast(m_Exit))        && ok;

		Widget topNav = layoutRoot.FindAnyWidget("TopNavigation");
		if (topNav) topNav.Update();

		if (!ok && m_FitRetries < 20)
		{
			m_FitRetries++;
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.FitTopNav, 50, false);
		}
	}

	protected void ScheduleTopNavFit()
	{
		m_FitRetries = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.FitTopNav);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.FitTopNav, 0, false);
	}

	override Widget Init()
	{
		if (ErrorTestScreen)
		{
			m_ErrorTestScreen = new CUI_ErrorTestScreen();
			layoutRoot = m_ErrorTestScreen.Build(this);
			return layoutRoot;
		}

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

		m_Background        = ImageWidget.Cast(layoutRoot.FindAnyWidget("ImageBackground"));
		if (m_Background)
		{
			if (EnableMenuVideo) m_Background.Show(false);
			else                 m_Background.LoadImageFile(0, GetMainMenuBackground());
		}

		m_TopShader         = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
		m_BottomShader      = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
		m_StatisticsBoxBG 	= ImageWidget.Cast(layoutRoot.FindAnyWidget("StatisticsBoxBG"));
		m_SurvivorBox 		= ImageWidget.Cast(layoutRoot.FindAnyWidget("SurvivorBox"));
		m_TopSpacer         = layoutRoot.FindAnyWidget("TopSpacer");
		m_MenuDivider       = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider"));
		m_MenuDivider0      = layoutRoot.FindAnyWidget("MenuDivider0");
		m_BottomSpacer      = layoutRoot.FindAnyWidget("BottomSpacer");

		m_LoadingBar        = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
		m_Logo              = ImageWidget.Cast(layoutRoot.FindAnyWidget("Logo"));
		m_PlayerName        = TextWidget.Cast(layoutRoot.FindAnyWidget("character_name_text"));
		m_WelcomeBack       = TextWidget.Cast(layoutRoot.FindAnyWidget("WelcomeBack"));
		if (m_WelcomeBack) m_WelcomeBack.SetColor(colorScheme.BrandColor());
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

		TextWidget statsHeader = TextWidget.Cast(layoutRoot.FindAnyWidget("character_stats_textImg"));
		if (statsHeader) statsHeader.SetColor(colorScheme.BrandColor());

		cuiElmnt.proBtnDC(this, ButtonWidget.Cast(m_Play), "#main_menu_play", colorScheme.PrimaryText(), colorScheme.ButtonHover(), SERVER_IP, SERVER_PORT);

		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Exit), "#main_menu_exit", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenExitDialog");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_SettingsBtn), "#layout_xbox_ingame_menu_options", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenSettings");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_TutorialBtn), "#menu_tutorials", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenTutorials");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_MessageBtn), "#menu_credits", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenCredits");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_CharacterBtn), "", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OpenMenuCustomizeCharacter");

		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_PrevCharacter), "", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "PreviousCharacter");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_NextCharacter), "", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "NextCharacter");

		cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_PrioQ), "#CUI_priority_queue", colorScheme.PrimaryText(), colorScheme.ButtonHover(), CustomURL.PriorityQ);
		cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Website), "#mod_detail_info_website", colorScheme.PrimaryText(), colorScheme.ButtonHover(), CustomURL.Website);

		cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Discord), "Discord", colorScheme.PrimaryText(), UIColor.Discord(), SocialURL.Discord);
		cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Twitter), "Twitter", colorScheme.PrimaryText(), UIColor.Twitter(), SocialURL.Twitter);
		cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Youtube), "Youtube", colorScheme.PrimaryText(), UIColor.YouTube(), SocialURL.Youtube);
		cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Reddit), "Reddit", colorScheme.PrimaryText(), UIColor.Reddit(), SocialURL.Reddit);
		cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Facebook), "Facebook", colorScheme.PrimaryText(), UIColor.Facebook(), SocialURL.Facebook);

		allInvalid = true;
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

		#ifndef WORKBENCH
		if (EnableMenuVideo)
		{
			if (!FileExist("$saves:" + m_MainMenuVideo))
				CopyFile("Colorful-UI/GUI/video/" + m_MainMenuVideo, "$saves:" + m_MainMenuVideo);
			CuiBackgroundVideo.Ensure("$saves:" + m_MainMenuVideo, true);
		}
		#endif

		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.CheckPendingCuiError, 0, false);
		ScheduleTopNavFit();

		return layoutRoot;
	}

	protected void CheckPendingCuiError()
	{
		MissionBase mission = MissionBase.Cast(GetGame().GetMission());
		if (mission) mission.CuiFlushPendingError();
	}
	
	override void OnShow()
	{
		if (layoutRoot)
		{
			if (!m_LoadingBar) m_LoadingBar = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
			if (!m_Logo) m_Logo = ImageWidget.Cast(layoutRoot.FindAnyWidget("Logo"));
			if (!m_TopShader) m_TopShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
			if (!m_BottomShader) m_BottomShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
		}

		if (m_Stats) m_Stats.UpdateStats();
		OnChangeCharacter(false);

		RefreshNavLabels();
		ScheduleTopNavFit();
	}
	
	override void Refresh()
	{
		OnChangeCharacter(false);

		if (m_Version)
		{
			string version;
			g_Game.GetVersion(version);
			m_Version.SetText("#main_menu_version" + " " + version);
		}

		if (m_DisplayedDlcHandler)
			m_DisplayedDlcHandler.UpdateAllPromotionInfo();
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

			if (m_PlayerName)
				m_PlayerName.SetText(name);

			if (m_Stats) m_Stats.UpdateStats();
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

	void OpenExitDialog()
	{
		CuiDialog.Show("#main_menu_exit", "#main_menu_exit_desc", true, this, "DoExit", "");
	}

	override void Exit()
	{
		if (CuiDialog.CancelTop())
			return;
		OpenExitDialog();
	}

	void DoExit()
	{
		MissionMainMenu.s_CuiQuitRequested = true;
	}

	void CUI_ErrorTest_FlushPendingError()
	{
		CheckPendingCuiError();
	}

	void CUI_BackToMainMenu()
	{
		ErrorTestScreen = false;
		Close();
		GetGame().GetUIManager().EnterScriptedMenu(MENU_MAIN, null);
	}

	void ~MainMenu()
	{
		if (GetGame())
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.CheckPendingCuiError);
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.FitTopNav);

		cuiElmnt.CleanupForOwner(this);
		if (m_ErrorTestScreen) m_ErrorTestScreen.Cleanup();
	}
}

modded class MissionMainMenu
{
	static bool s_CuiQuitRequested;

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (s_CuiQuitRequested)
		{
			s_CuiQuitRequested = false;
			g_Game.RequestExit(IDC_MAIN_QUIT);
		}
	}
}

modded class MissionBase
{
	void CuiFlushPendingError()
	{
		if (!CuiPendingError.s_Pending) return;

		string caption   = CuiPendingError.s_Caption;
		string message   = CuiPendingError.s_Message;
		int    errorCode = CuiPendingError.s_ErrorCode;
		CuiPendingError.Clear();

		Print(string.Format("[CUI ErrorRouter] MissionBase.CuiFlushPendingError showing caption='%1' message='%2'", caption, message));

		CuiDialog dlg = CuiDialog.Show(caption, message);
		if (!dlg)
		{
			Print("[CUI ErrorRouter] CuiDialog.Show returned null - falling back to native ShowDialog");

			UIManager ui = g_Game.GetUIManager();
			if (ui)
				ui.ShowDialog(caption, message, errorCode, DBT_OK, DBB_OK, DMT_EXCLAMATION, null);
		}
	}
}

modded class MainMenuStats
{
	protected RichTextWidget m_RichTime;
	protected RichTextWidget m_RichDistance;
	protected RichTextWidget m_RichLongRange;

	protected string BrandRgba()
	{
		int c = colorScheme.BrandColor();
		int a = (c >> 24) & 0xFF;
		int r = (c >> 16) & 0xFF;
		int g = (c >> 8)  & 0xFF;
		int b =  c        & 0xFF;
		return string.Format("%1, %2, %3, %4", r, g, b, a);
	}

	void MainMenuStats(Widget root)
	{
		m_RichTime      = RichTextWidget.Cast(root.FindAnyWidget("TimeSurvivedValue"));
		m_RichDistance  = RichTextWidget.Cast(root.FindAnyWidget("DistanceTraveledValue"));
		m_RichLongRange = RichTextWidget.Cast(root.FindAnyWidget("LongRangeShotValue"));
	}

	override void UpdateStats()
	{
		super.UpdateStats();

		PlayerBase player;
		MissionMainMenu missionMainMenu = MissionMainMenu.Cast(g_Game.GetMission());
		if (!missionMainMenu || !missionMainMenu.GetIntroScenePC()) return;

		player = missionMainMenu.GetIntroScenePC().GetIntroCharacter().GetCharacterObj();
		if (!player) return;

		FullTimeData ft = new FullTimeData();
		TimeConversions.ConvertSecondsToFullTime(player.StatGet(AnalyticsManagerServer.STAT_PLAYTIME), ft);
		if (m_RichTime) m_RichTime.SetText(BuildColoredTime(ft));

		if (m_RichDistance)  m_RichDistance.SetText(BuildColoredDistance(player.StatGet(AnalyticsManagerServer.STAT_DISTANCE), false));
		if (m_RichLongRange) m_RichLongRange.SetText(BuildColoredDistance(player.StatGet(AnalyticsManagerServer.STAT_LONGEST_SURVIVOR_HIT), true));
	}

	protected string ColorTag(string locKey)
	{
		return string.Format("<color rgba=\"%1\">%2</color>", BrandRgba(), locKey);
	}

	protected string BuildColoredTime(FullTimeData ft)
	{
		string msg;
		if (ft.m_Days > 0)    msg += string.Format("%1%2 ", ft.m_Days,    ColorTag("#STR_time_unit_abbrev_day_0"));
		if (ft.m_Hours > 0)   msg += string.Format("%1%2 ", ft.m_Hours,   ColorTag("#STR_time_unit_abbrev_hour_0"));
		if (ft.m_Minutes > 0) msg += string.Format("%1%2 ", ft.m_Minutes, ColorTag("#STR_time_unit_abbrev_minute_0"));
		msg += string.Format("%1%2", ft.m_Seconds, ColorTag("#STR_time_unit_abbrev_second_0"));
		return msg;
	}

	protected string BuildColoredDistance(float total_distance, bool meters_only)
	{
		if (total_distance > 0)
		{
			float kilometers = Math.Round(total_distance * 0.001);
			if (kilometers >= 10 && !meters_only)
				return GetValueString(kilometers, true) + " " + ColorTag("#STR_distance_unit_abbrev_kilometer_0");
			return GetValueString(total_distance) + " " + ColorTag("#STR_distance_unit_abbrev_meter_0");
		}
		return "0 " + ColorTag("#STR_distance_unit_abbrev_meter_0");
	}
}

modded class CreditsDepartmentElement
{
	void CreditsDepartmentElement(int index, Widget parent, JsonDataCreditsDepartment department_data)
	{
		if (m_Root)
		{
			Widget sep = m_Root.FindAnyWidget("SeparatorPanel");
			if (sep) sep.SetColor(colorScheme.Separator());

			if (department_data && department_data.DepartmentName == CUI_CREDITS_DEPARTMENT)
				m_Root.SetSort(0);
		}
	}
}

modded class CreditsDepartmentSection
{
	void CreditsDepartmentSection(int index, Widget parent, JsonDataCreditsSection section_data)
	{
		if (m_Root)
		{
			Widget sep = m_Root.FindAnyWidget("SeparatorPanel");
			if (sep) sep.SetColor(colorScheme.Separator());
		}
	}
}

modded class CreditsMenu
{
	protected ImageWidget m_CuiBackground;
	protected ImageWidget m_CuiTopShader;
	protected ImageWidget m_CuiBottomShader;

	override Widget Init()
	{
		Widget root = super.Init();
		if (!root) return root;

		Widget panel = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/cui.credits_bg.layout", root);
		if (panel)
		{
			m_CuiBackground   = ImageWidget.Cast(panel.FindAnyWidget("ImageBackground"));
			m_CuiTopShader    = ImageWidget.Cast(panel.FindAnyWidget("TopShader"));
			m_CuiBottomShader = ImageWidget.Cast(panel.FindAnyWidget("BottomShader"));

			if (m_CuiBackground)
			{
				if (EnableMenuVideo) m_CuiBackground.Show(false);
				else                 m_CuiBackground.LoadImageFile(0, GetMainMenuBackground());
			}
			if (m_CuiTopShader)    m_CuiTopShader.SetColor(colorScheme.TopShader());
			if (m_CuiBottomShader) m_CuiBottomShader.SetColor(colorScheme.BottomShader());
		}

		#ifndef WORKBENCH
		if (EnableMenuVideo)
		{
			if (!FileExist("$saves:" + m_MainMenuVideo))
				CopyFile("Colorful-UI/GUI/video/" + m_MainMenuVideo, "$saves:" + m_MainMenuVideo);
			CuiBackgroundVideo.Ensure("$saves:" + m_MainMenuVideo, true);
		}
		#endif

		return root;
	}

	void ~CreditsMenu()
	{
	}
}
