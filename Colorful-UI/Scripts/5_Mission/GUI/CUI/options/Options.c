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
		// ---- Phase 1: let the real chain build the vanilla menu -----------
		// super.Init() walks the WHOLE modded-class chain below us (any other
		// mod's modded OptionsMenu.Init(), e.g. DayZ Expansion's, then vanilla
		// itself). That's what constructs vanilla's layout/tabs AND lets every
		// foreign mod add its own tab and build its own tab content — none of
		// that runs if we skip straight to our own layout the way this method
		// used to. TabberUI.AddTab records (index, name) for every tab added
		// while s_CuiRecordForeignTabs is set, which is how we find out what
		// to adopt below without knowing any mod's name up front.
		// Vanilla source verified: P Drive\scripts\5_Mission\gui\newui\options\optionsmenu.c:37-111
		// (Tab_0..Tab_3 are static layout children; foreign tabs only ever
		// arrive via AddTab, so the registry is foreign-only by construction.)
		TabberUI.s_CuiForeignTabIndices.Clear();
		TabberUI.s_CuiForeignTabNames.Clear();
		TabberUI.s_CuiRecordForeignTabs = true;

		Widget oldRoot = super.Init();

		TabberUI.s_CuiRecordForeignTabs = false;

		TabberUI oldTabber;
		oldRoot.FindAnyWidget("Tabber").GetScript(oldTabber);

		// ---- Phase 2: swap to the CUI shell (unchanged from before) -------
		// m_Options is NOT recreated here — super.Init() already built it,
		// and any foreign tab handler (e.g. Expansion's OptionsMenuExpansion)
		// captured that exact instance in its own constructor during phase 1.
		// Recreating it would leave foreign tabs pointed at an orphaned copy.
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/options/cui.options_menu.layout", null);

		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);

		m_Details  = layoutRoot.FindAnyWidget("settings_details");
		m_Version  = TextWidget.Cast(layoutRoot.FindAnyWidget("version"));

		m_GameTab     = new OptionsMenuGame(layoutRoot.FindAnyWidget("Tab_0"), m_Details, m_Options, this);
		m_SoundsTab   = new OptionsMenuSounds(layoutRoot.FindAnyWidget("Tab_1"), m_Details, m_Options, this);
		m_VideoTab    = new OptionsMenuVideo(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
		m_ControlsTab = new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_3"), m_Details, m_Options, this);

		// Vanilla registers these on the phase-1 tabber inside super.Init()
		// (optionsmenu.c:103-104); that instance is about to be destroyed, so
		// re-register on the tabber we're actually keeping.
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
		
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Back), "Back", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Back");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Defaults), "Defaults", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "PerformSetToDefaults");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Apply), "Apply", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Apply");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Reset), "Reset", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Reset");
		
		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());
		m_MenuDivider.SetColor(colorScheme.Separator());

		// Developer marker left in the layout; hidden here.
		// purple in the layout — hide it.
		Widget tutorialsLabel = layoutRoot.FindAnyWidget("TutorialsTextWidget");
		if (tutorialsLabel) tutorialsLabel.Show(false);

		// Tab bar bg: layout's `color 0 0 0 0.549` drops the fractional alpha,
		// rendering fully opaque. Re-apply via ARGB so the bg stays translucent.
		Widget tabBarBg = layoutRoot.FindAnyWidget("Tab_Control_Container");
		if (tabBarBg) tabBarBg.SetColor(ARGB(140, 0, 0, 0));

		// Section headers inside tabs stay uncolored (white, like every other
		// tab's) — one consistent look, nothing to restyle per foreign mod.

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

		// ---- Adopt every tab a foreign mod's chained Init() added ---------
		// (index 4+; the vanilla four never go through AddTab). The pane
		// widget already holds that mod's fully-built tab content — adoption
		// moves it, it doesn't rebuild it, so a foreign mod's own handler
		// object (e.g. Expansion's OptionsMenuExpansion, which stores a Widget
		// reference to this exact pane) keeps working untouched. No mod name
		// appears anywhere in this loop.
		for (int i = 0; i < TabberUI.s_CuiForeignTabIndices.Count(); i++)
		{
			int oldIndex = TabberUI.s_CuiForeignTabIndices.Get(i);
			string tabName = TabberUI.s_CuiForeignTabNames.Get(i);

			Widget pane = oldRoot.FindAnyWidget("Tab_" + oldIndex);
			if (!pane)
				continue; // recorded index didn't resolve to a real pane; skip rather than crash

			m_Tabber.CuiAdoptTab(tabName, pane);
		}

		// Destroy the phase-1 vanilla tree now that every foreign pane has
		// been reparented out of it. Cancel its tabber's pending realign
		// timer first (vanilla tabberui.c fires it a frame later; by then
		// this tree would be gone).
		if (oldTabber)
			oldTabber.CuiCancelInitTimer();
		oldRoot.Unlink();

		return layoutRoot;
	}

	// Row hover and disabled colouring.
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


	// Highlights an option row on hover, using the mod's colour scheme.
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

		// Row background darkens on hover.
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

	// Resetting normally happens instantly; ask first.
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
	// (discard changes on tab switch). Back() and OnAttemptTabSwitch() bodies
	// are replicated verbatim from vanilla, with the ShowDialog call replaced
	// by CuiDialog.Show and the OnModalResult Yes-branch logic moved into a
	// Confirm callback — vanilla doesn't expose a narrower hook for just the
	// dialog call, so a full copy is the only way to swap it.
	// Apply() is different (see below): it calls super.Apply() instead of
	// copying vanilla's body, so foreign mods' own chained Apply() overrides
	// (e.g. DayZ Expansion's, which applies its tab via a field only *it*
	// knows about) run for free with no per-mod code here.
	// Vanilla source: P Drive\scripts\5_Mission\gui\newui\options\optionsmenu.c

	protected int m_PendingTabTarget;

	// Chains to super.Apply() so every mod's own Apply() override in between
	// (vanilla tabs, plus any foreign mod's, e.g. Expansion's
	// `if (m_ExpansionTab && m_ExpansionTab.IsChanged()) m_ExpansionTab.Apply();`)
	// runs — that's the only way to reach a foreign mod's tab-apply logic
	// generically, since it's stored behind a field only that mod's own code
	// can name. Vanilla's Apply() (optionsmenu.c:232-274) ends by calling the
	// native g_Game.GetUIManager().ShowDialog(...) when a restart is needed;
	// nothing renders between that call and the two lines below (everything
	// here is synchronous within one Init/Apply call), so closing dialog id
	// 117 (vanilla's hardcoded restart-prompt id, optionsmenu.c:273) and
	// showing CuiDialog in its place produces no visible native-dialog flash.
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
		// Vanilla's IsDialogVisible guard doesn't know about CuiDialog, so
		// without this check Escape would stack a new confirm dialog on each
		// press. Treat Escape as Cancel on the open dialog instead.
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
		// Singleton bg video persists — do not touch CuiBackgroundVideo.s_Instance.
		cuiElmnt.CleanupForOwner(this);
	}

	// Vanilla Refresh (optionsmenu.c:676-692) calls m_Version.SetText
	// unconditionally. Our cui.options_menu.layout has no "version" widget,
	// so m_Version stays null after Init's FindAnyWidget — and vanilla's
	// Refresh NPEs the next time it fires (OnShow → Refresh, triggered by
	// MainMenu OpenSettings or by Cancel returning to OptionsMenu).
	// Deliberately does NOT call super.Refresh(): Expansion's own Refresh
	// override (`super.Refresh(); GetDayZGame().Expansion_SetGameVersionText(m_Version);`)
	// doesn't null-check m_Version either, so chaining into it here would
	// reintroduce the exact NPE this override exists to avoid. Net effect:
	// Expansion's version-text suffix on the options screen is lost — a
	// known, accepted gap, not a bug.
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
