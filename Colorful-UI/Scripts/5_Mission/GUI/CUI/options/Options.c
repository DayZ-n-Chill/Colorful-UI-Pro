// OptionsMenu — CUI options screen with universal mod-tab adoption.
// Vanilla source: P:\scripts\5_mission\gui\newui\options\optionsmenu.c

modded class OptionsMenu extends UIScriptedMenu
{
	private Widget m_Separator, m_shader, m_TopShader, m_BottomShader, m_MenuDivider, m_LoadingBar;

	private bool IsMainMenuContext()
	{
		Mission m = GetGame().GetMission();
		return m && m.IsInherited(MissionMainMenu);
	}
		
	override Widget Init()
	{
		TabberUI.s_CuiForeignTabIndices.Clear();
		TabberUI.s_CuiForeignTabNames.Clear();
		TabberUI.s_CuiRecordForeignTabs = true;

		Widget oldRoot = super.Init();

		TabberUI.s_CuiRecordForeignTabs = false;

		TabberUI oldTabber;
		oldRoot.FindAnyWidget("Tabber").GetScript(oldTabber);

		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/options/cui.options_menu.layout", null);

		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);

		m_Details  = layoutRoot.FindAnyWidget("settings_details");
		m_Version  = TextWidget.Cast(layoutRoot.FindAnyWidget("version"));

		m_GameTab     = new OptionsMenuGame(layoutRoot.FindAnyWidget("Tab_0"), m_Details, m_Options, this);
		m_SoundsTab   = new OptionsMenuSounds(layoutRoot.FindAnyWidget("Tab_1"), m_Details, m_Options, this);
		m_VideoTab    = new OptionsMenuVideo(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
		m_ControlsTab = new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_3"), m_Details, m_Options, this);

		m_Tabber.m_OnTabSwitch.Insert(OnTabSwitch);
		m_Tabber.m_OnAttemptTabSwitch.Insert(OnAttemptTabSwitch);

		m_Apply    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("apply"));
		m_Back     = ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		m_Reset    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("reset"));
		m_Defaults = ButtonWidget.Cast(layoutRoot.FindAnyWidget("defaults"));
		
		m_TopShader    = layoutRoot.FindAnyWidget("TopShader");
		m_BottomShader = layoutRoot.FindAnyWidget("BottomShader");
		m_MenuDivider  = layoutRoot.FindAnyWidget("MenuDivider");
		m_LoadingBar   = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
		
		if (m_LoadingBar) m_LoadingBar.SetColor(colorScheme.Loadingbar());
		
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Back), "#STR_settings_menu_root_play_panel_root_top_back0", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Back");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Defaults), "#menu_default_cap", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "PerformSetToDefaults");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Apply), "#STR_settings_menu_root_play_panel_root_bottom_apply0", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Apply");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Reset), "#menu_undo_cap", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Reset");
		
		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());
		m_MenuDivider.SetColor(colorScheme.Separator());

		Widget tutorialsLabel = layoutRoot.FindAnyWidget("TutorialsTextWidget");
		if (tutorialsLabel) tutorialsLabel.Show(false);

		Widget tabBarBg = layoutRoot.FindAnyWidget("Tab_Control_Container");
		if (tabBarBg) tabBarBg.SetColor(ARGB(140, 0, 0, 0));

		m_ModalLock = false;
		m_CanApplyOrReset = false;
		SetFocus(null);
			
		OnChanged();
		Class.CastTo(m_shader, layoutRoot.FindAnyWidget("Colorful_Shader"));
		m_Separator = layoutRoot.FindAnyWidget("colorful_separator");
		
		#ifndef WORKBENCH
		if (EnableOptionsVideo && IsMainMenuContext())
		{
			if (!FileExist("$saves:" + m_OptionsMenuVideo))
				CopyFile("Colorful-UI/GUI/video/" + m_OptionsMenuVideo, "$saves:" + m_OptionsMenuVideo);
			CuiBackgroundVideo.Ensure("$saves:" + m_OptionsMenuVideo, true);
		}
		#endif

		for (int i = 0; i < TabberUI.s_CuiForeignTabIndices.Count(); i++)
		{
			int oldIndex = TabberUI.s_CuiForeignTabIndices.Get(i);
			string tabName = TabberUI.s_CuiForeignTabNames.Get(i);

			Widget pane = oldRoot.FindAnyWidget("Tab_" + oldIndex);
			if (!pane)
				continue;

			m_Tabber.CuiAdoptTab(tabName, pane);
		}

		if (oldTabber)
			oldTabber.CuiCancelInitTimer();
		oldRoot.Unlink();

		return layoutRoot;
	}

	override void ColorDisable(Widget w)
	{
		SetFocus(null);
		w.SetAlpha(0.5);
		if (w)
		{
			Widget label = w.FindAnyWidget(w.GetName() + "_label");
			if (label && label.IsInherited(TextWidget))
			{
				TextWidget.Cast(label).SetColor(colorScheme.DisabledText());
			}
		}
	}

	override void ColorHighlight(Widget w)
	{
		if ((w.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER)
			return;

		int hover = colorScheme.TextHover();

		if (w.IsInherited(ButtonWidget))
		{
			ButtonWidget button = ButtonWidget.Cast(w);
			button.SetTextColor(hover);
		}

		w.SetColor(UIColor.Black());

		TextWidget  text1        = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget  text2        = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		TextWidget  text3        = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));
		ImageWidget image        = ImageWidget.Cast(w.FindAnyWidget(w.GetName() + "_image"));
		Widget      option       = Widget.Cast(w.FindAnyWidget(w.GetName() + "_option_wrapper"));
		Widget      option_label = w.FindAnyWidget("option_label");

		if (text1)        text1.SetColor(hover);
		if (text2)        text2.SetColor(hover);
		if (text3)        { text3.SetColor(hover); w.SetAlpha(1); }
		if (image)        image.SetColor(hover);
		if (option)       option.SetColor(hover);
		if (option_label) option_label.SetColor(hover);
	}

	override void Reset()
	{
		CuiDialog.Show(
			"Reset Settings",
			"Revert all settings to their saved values? Any unsaved changes will be lost.",
			true, this, "DoReset", "");
	}

	void DoReset()
	{
		super.Reset();
	}

	override void PerformSetToDefaults()
	{
		CuiDialog.Show(
			"Reset to Defaults",
			"Reset all settings to factory defaults? This will overwrite your saved configuration.",
			true, this, "DoSetToDefaults", "");
	}

	void DoSetToDefaults()
	{
		super.PerformSetToDefaults();
	}

	protected int m_PendingTabTarget;

	override void Apply()
	{
		super.Apply();

		if (m_Options.NeedRestart())
		{
			g_Game.GetUIManager().CloseSpecificDialog(117);
			CuiDialog.Show(
				"#main_menu_configure", "#menu_restart_needed",
				true, this, "DoRequestRestart", "");
		}
	}

	void DoRequestRestart()
	{
		g_Game.RequestRestart(IDC_MAIN_QUIT);
	}

	override void Back()
	{
		if (CuiDialog.CancelTop())
			return;

		if (g_Game.GetUIManager().IsDialogVisible() || g_Game.GetUIManager().IsModalVisible())
			return;

		if (IsAnyTabChanged())
		{
			CuiDialog.Show(
				"#main_menu_configure", "#main_menu_configure_desc",
				true, this, "DoConfirmBack", "");
		}
		else
		{
			m_Options.Revert();
			g_Game.EndOptionsVideo();
			g_Game.GetUIManager().Back();
		}
	}

	void DoConfirmBack()
	{
		m_Options.Revert();
		g_Game.EndOptionsVideo();
		g_Game.GetUIManager().Back();
	}

	override void OnAttemptTabSwitch(int source, int target)
	{
		bool changed = IsAnyTabChanged();
		if (changed)
		{
			if (!g_Game.GetUIManager().IsDialogVisible() && !g_Game.GetUIManager().IsModalVisible())
			{
				m_PendingTabTarget = target;
				CuiDialog.Show(
					"#main_menu_configure", "#main_menu_configure_desc",
					true, this, "DoConfirmTabSwitch", "");
			}
		}
		else
		{
			ResetCurrentTab();
		}

		m_Tabber.SetCanSwitch(!changed);
	}

	void DoConfirmTabSwitch()
	{
		ResetCurrentTab();
		m_Tabber.PerformSwitchTab(m_PendingTabTarget);
	}

	void ~OptionsMenu()
	{
		cuiElmnt.CleanupForOwner(this);
	}

	override void Refresh()
	{
		if (m_Version)
		{
			string version;
			g_Game.GetVersion(version);
			m_Version.SetText("#main_menu_version" + " " + version);
		}
	}
}
