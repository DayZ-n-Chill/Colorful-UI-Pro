modded class KeybindingsMenu extends UIScriptedMenu
{
	private Widget m_TopShader, m_BottomShader, m_MenuDivider;
    private TextWidget m_ApplyBtn_Label;
	protected ProgressBarWidget m_LoadingBar;

	override Widget Init()
	{
		Input input = GetGame().GetInput();
		layoutRoot			= GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/options/cui.keybindings.layout", null);
		
		m_Apply     = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ApplyBtn"));
		m_Back      = ButtonWidget.Cast(layoutRoot.FindAnyWidget("BackBtn"));
		m_Undo      = ButtonWidget.Cast(layoutRoot.FindAnyWidget("UndoBtn"));
		m_Defaults  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ResetBtn"));
		m_HardReset = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ResetAllBtn"));

		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Apply),     "Apply",      colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Apply");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Back),      "Back",       colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Back");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Undo),      "Undo",       colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Reset");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Defaults),  "Defaults",   colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "SetToDefaults");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_HardReset), "Hard Reset", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "HardReset");

		m_TopShader    = layoutRoot.FindAnyWidget( "TopShader" );
		m_BottomShader = layoutRoot.FindAnyWidget( "BottomShader" );
		m_MenuDivider  = layoutRoot.FindAnyWidget( "MenuDivider" );

		// Note that this is just used as a visual trim, not a real loading bar.
		m_LoadingBar      = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
		if (m_LoadingBar) m_LoadingBar.SetColor(colorScheme.Loadingbar());

		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());
		m_MenuDivider.SetColor(colorScheme.Separator());
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);

		InitInputSortingMap();
		CreateTabs();
		CreateGroupContainer();

		InitPresets(-1, layoutRoot.FindAnyWidget("group_header"), input);
		m_Tabber.m_OnTabSwitch.Insert(UpdateTabContent);
		m_Tabber.SelectTabControl(0);
		m_Tabber.SelectTabPanel(0);
		g_Game.SetKeyboardHandle(this);
		m_Tabber.RefreshTab(true);

		// Match vanilla initial state — Apply/Undo are disabled until the user
		// edits a bind. Our `override Init()` replaces vanilla's, so vanilla's
		// own ColorDisabled() init lines (keybindingsmenu.c:76-81) never ran;
		// reapply them here. Vanilla's bind-edit handlers (ConfirmKeybindEntry
		// etc.) call ColorWhite() and ClearFlags() at the right time so these
		// re-enable themselves when the user actually changes a binding.
		ColorDisabled(m_Apply);
		m_Apply.SetFlags(WidgetFlags.IGNOREPOINTER);
		ColorDisabled(m_Undo);
		m_Undo.SetFlags(WidgetFlags.IGNOREPOINTER);

		return layoutRoot;
	}

	// Vanilla ColorDisabled / ColorWhite only call button.SetTextColor — but
	// our cuiElmnt.proBtnCB puts the visible text on a `_label` child, so the
	// button's own text color is invisible. Replicate vanilla's body inline
	// (no super — vanilla's methods aren't overrides on UIScriptedMenu, so a
	// super call would fail to compile) and also color the `_label` child.
	// Vanilla source: keybindingsmenu.c:446-477.

	override void ColorDisabled(Widget w)
	{
		if (!w) return;

		ButtonWidget button = ButtonWidget.Cast(w);
		if (button) button.SetTextColor(colorScheme.DisabledText());

		TextWidget label = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		if (label) label.SetColor(colorScheme.DisabledText());
	}

	// Vanilla SetToDefaults() and HardReset() open the engine's yes/no dialog
	// via g_Game.GetUIManager().ShowDialog(...). Override to use our CuiDialog
	// and skip straight to PerformSetToDefaultsExt() with the right parameter
	// on confirm — same call vanilla's OnModalResult would make.
	// Vanilla source: keybindingsmenu.c:284-292.

	override void SetToDefaults()
	{
		CuiDialog.Show(
			"#menu_default_cap", "#menu_default_desc",
			true, this, "DoSetToDefaultsCurrent", "");
	}

	void DoSetToDefaultsCurrent()
	{
		PerformSetToDefaultsExt(MODAL_RESULT_DEFAULT_CURRENT);
	}

	override void HardReset()
	{
		CuiDialog.Show(
			"#menu_default_cap", "#menu_default_all_desc",
			true, this, "DoSetToDefaultsAll", "");
	}

	void DoSetToDefaultsAll()
	{
		PerformSetToDefaultsExt(MODAL_RESULT_DEFAULT_ALL);
	}

	override void ColorWhite(Widget w, Widget enterW)
	{
		if (!w) return;

		ButtonWidget button = ButtonWidget.Cast(w);
		bool disabled = button && ((button.GetFlags() & WidgetFlags.DISABLED) == WidgetFlags.DISABLED);

		if (button)
		{
			if (disabled) button.SetTextColor(colorScheme.DisabledText());
			else          button.SetTextColor(colorScheme.PrimaryText());
		}

		TextWidget label = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		if (label)
		{
			if (disabled) label.SetColor(colorScheme.DisabledText());
			else          label.SetColor(colorScheme.PrimaryText());
		}
	}

	// Vanilla Back() shows ShowDialog when there are unsaved bind changes.
	// Replicated verbatim, ShowDialog -> CuiDialog. Vanilla source: line 247-271.
	override void Back()
	{
		if (m_CurrentSettingKeyIndex != -1)
		{
			CancelEnteringKeybind();
			return;
		}

		if (m_CurrentSettingAlternateKeyIndex != -1)
		{
			CancelEnteringAlternateKeybind();
			return;
		}

		bool changed = m_GroupsContainer.IsChanged();

		if (changed)
		{
			CuiDialog.Show(
				"#main_menu_configure", "#main_menu_configure_desc",
				true, this, "DoConfirmKBBack", "");
		}
		else
		{
			g_Game.GetUIManager().Back();
		}
	}

	void DoConfirmKBBack()
	{
		Reset();
		g_Game.GetUIManager().Back();
	}

	// Vanilla OnAttemptSelectPreset shows ShowDialog when changing presets
	// would discard unsaved bind changes. Replicated verbatim, ShowDialog ->
	// CuiDialog. m_TargetPresetIndex is set unconditionally so the confirm
	// callback knows which preset to switch to. Vanilla source: line 508-519.
	override void OnAttemptSelectPreset(int index)
	{
		bool changed = m_GroupsContainer.IsChanged() && m_OriginalPresetIndex != index;
		m_TargetPresetIndex = index;

		if (changed)
		{
			CuiDialog.Show(
				"#main_menu_configure", "#main_menu_configure_desc",
				true, this, "DoConfirmPresetChange", "");
		}

		m_PresetSelector.SetCanSwitch(!changed);
	}

	void DoConfirmPresetChange()
	{
		Reset();
		m_PresetSelector.PerformSetOption(m_TargetPresetIndex);
	}

	void ~KeybindingsMenu()
	{
		cuiElmnt.CleanupForOwner(this);
	}
}

// Vanilla KeybindingElementNew.OnMouseEnter / OnMouseLeave paint the bind
// buttons red ARGBF(1,1,0,0) on hover. We mirror vanilla's structure
// (clear-X show/hide is preserved) and swap red for our scheme color.
//
// Vanilla source: P:\scripts\5_mission\gui\newui\keybindings\keybindingelementnew.c:263-310

modded class KeybindingElementNew
{
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int hover = colorScheme.ButtonHover();

		if (w == m_PrimaryBindButton || w == m_PrimaryClear)
		{
			m_PrimaryBindButton.SetColor(hover);
			m_PrimaryClear.Show(true);
			m_PrimaryClear.Update();
			m_AlternativeClear.Show(false);
			return true;
		}
		else if (w == m_AlternativeBindButton || w == m_AlternativeClear)
		{
			m_AlternativeBindButton.SetColor(hover);
			m_PrimaryClear.Show(false);
			m_AlternativeClear.Show(true);
			m_AlternativeClear.Update();
			return true;
		}
		else
		{
			m_PrimaryBindButton.SetColor(ARGBF(0, 0, 0, 0));
			m_AlternativeBindButton.SetColor(ARGBF(0, 0, 0, 0));
			m_PrimaryClear.Show(false);
			m_AlternativeClear.Show(false);
		}
		return false;
	}
}

