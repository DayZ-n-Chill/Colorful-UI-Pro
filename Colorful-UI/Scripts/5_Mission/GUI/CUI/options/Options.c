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
		m_Options = new GameOptions();
		
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/options/cui.options_menu.layout", null);
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);
		
		m_Details  = layoutRoot.FindAnyWidget("settings_details");
		m_Version  = TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		
		m_GameTab     = new OptionsMenuGame(layoutRoot.FindAnyWidget("Tab_0"), m_Details, m_Options, this);
		m_SoundsTab   = new OptionsMenuSounds(layoutRoot.FindAnyWidget("Tab_1"), m_Details, m_Options, this);
		m_VideoTab    = new OptionsMenuVideo(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
		m_ControlsTab = new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_3"), m_Details, m_Options, this);

		m_Apply    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("apply"));
		m_Back     = ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		m_Reset    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("reset"));
		m_Defaults = ButtonWidget.Cast(layoutRoot.FindAnyWidget("defaults"));
		
		m_TopShader    = layoutRoot.FindAnyWidget("TopShader");
		m_BottomShader = layoutRoot.FindAnyWidget("BottomShader");
		m_MenuDivider  = layoutRoot.FindAnyWidget("MenuDivider");
		m_LoadingBar   = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
		
		if (m_LoadingBar) m_LoadingBar.SetColor(colorScheme.Loadingbar());
		
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Back), "Back", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Back");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Defaults), "Defaults", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "PerformSetToDefaults");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Apply), "Apply", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Apply");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Reset), "Reset", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Reset");
		
		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());
		m_MenuDivider.SetColor(colorScheme.Separator());

		// TutorialsTextWidget is a vanilla-string-id developer marker baked
		// purple in the layout — hide it.
		Widget tutorialsLabel = layoutRoot.FindAnyWidget("TutorialsTextWidget");
		if (tutorialsLabel) tutorialsLabel.Show(false);

		// Tab bar bg: layout's `color 0 0 0 0.549` drops the fractional alpha,
		// rendering fully opaque. Re-apply via ARGB so the bg stays translucent.
		Widget tabBarBg = layoutRoot.FindAnyWidget("Tab_Control_Container");
		if (tabBarBg) tabBarBg.SetColor(ARGB(140, 0, 0, 0));

		// Game-tab section headers — color via OptionHeaders so they follow
		// brand. All four tabs are constructed up-front so widgets exist here.
		TextWidget genHeader  = TextWidget.Cast(layoutRoot.FindAnyWidget("general_text"));
		TextWidget camHeader  = TextWidget.Cast(layoutRoot.FindAnyWidget("camera_text"));
		TextWidget hudHeader  = TextWidget.Cast(layoutRoot.FindAnyWidget("hud_text"));
		TextWidget chatHeader = TextWidget.Cast(layoutRoot.FindAnyWidget("chat_text"));
		if (genHeader)  genHeader.SetColor(colorScheme.OptionHeaders());
		if (camHeader)  camHeader.SetColor(colorScheme.OptionHeaders());
		if (hudHeader)  hudHeader.SetColor(colorScheme.OptionHeaders());
		if (chatHeader) chatHeader.SetColor(colorScheme.OptionHeaders());

		m_ModalLock = false;
		m_CanApplyOrReset = false;
		SetFocus(null);
			
		OnChanged();
		Class.CastTo(m_shader, layoutRoot.FindAnyWidget("Colorful_Shader"));
		m_Separator = layoutRoot.FindAnyWidget("colorful_separator");
		
		#ifndef WORKBENCH
		// Only show options video when opened from MAIN MENU, never from
		// in-game pause/options. Uses singleton CuiBackgroundVideo — Ensure()
		// is a no-op if already running (e.g. from MainMenu).
		if (EnableOptionsVideo && IsMainMenuContext())
		{
			if (!FileExist("$saves:" + m_OptionsMenuVideo))
				CopyFile("Colorful-UI/GUI/video/" + m_OptionsMenuVideo, "$saves:" + m_OptionsMenuVideo);
			CuiBackgroundVideo.Ensure("$saves:" + m_OptionsMenuVideo, true);
		}
		#endif

		return layoutRoot;
	}

	// // vanilla color helpers (unchanged)
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


	// Vanilla OptionsMenu.ColorHighlight tints option rows red — it's the
	// handler triggered when the chevron or row is hovered. Mirror vanilla's
	// structure (P:\scripts\5_mission\gui\newui\options\optionsmenu.c:759-811)
	// but swap the red ARGBs for our scheme hover color.
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

		// Vanilla fills the row background black on hover — preserved.
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

	// Vanilla OptionsMenu.Reset() and PerformSetToDefaults() revert immediately
	// with no confirmation. Wrap each with a CuiDialog so the user gets a
	// chance to back out — Confirm fires the vanilla revert via super.
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

	// ---- ShowDialog -> CuiDialog wiring ---------------------------------------
	// Vanilla optionsmenu.c uses g_Game.GetUIManager().ShowDialog(...) in
	// Apply() (restart-needed), Back() (discard changes), and OnAttemptTabSwitch
	// (discard changes on tab switch). Each method body is replicated verbatim
	// from vanilla, with the ShowDialog call replaced by CuiDialog.Show and
	// the OnModalResult Yes-branch logic moved into a Confirm callback.
	// Vanilla source: P:\scripts\5_mission\gui\newui\options\optionsmenu.c

	protected int m_PendingTabTarget;

	override void Apply()
	{
		if (m_ControlsTab.IsChanged()) m_ControlsTab.Apply();
		if (m_SoundsTab.IsChanged())   m_SoundsTab.Apply();
		if (m_GameTab.IsChanged())     m_GameTab.Apply();

		if (m_Options.IsChanged() || m_GameTab.IsChanged())
		{
			m_Options.Test();
			m_Options.Apply();
		}

		GetUApi().Export();

		if (g_Game.GetInput().IsEnabledMouseAndKeyboard())
		{
			m_Apply.SetFlags(WidgetFlags.IGNOREPOINTER);
			ColorDisable(m_Apply);
			m_Reset.SetFlags(WidgetFlags.IGNOREPOINTER);
			ColorDisable(m_Reset);
		}

		m_CanApplyOrReset = false;

		#ifdef PLATFORM_CONSOLE
		UpdateControlsElements();
		UpdateControlsElementVisibility();

		IngameHud hud;
		if (g_Game.GetMission() && Class.CastTo(hud, g_Game.GetMission().GetHud()))
		{
			hud.ShowQuickBar(g_Game.GetInput().IsEnabledMouseAndKeyboardEvenOnServer());
		}
		#endif

		if (m_Options.NeedRestart())
		{
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
		// Singleton bg video persists — do not touch CuiBackgroundVideo.s_Instance.
		cuiElmnt.CleanupForOwner(this);
	}

	// Vanilla Refresh (optionsmenu.c:676-692) calls m_Version.SetText
	// unconditionally. Our cui.options_menu.layout has no "version" widget,
	// so m_Version stays null after Init's FindAnyWidget — and vanilla's
	// Refresh NPEs the next time it fires (OnShow → Refresh, triggered by
	// MainMenu OpenSettings or by Cancel returning to OptionsMenu).
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
