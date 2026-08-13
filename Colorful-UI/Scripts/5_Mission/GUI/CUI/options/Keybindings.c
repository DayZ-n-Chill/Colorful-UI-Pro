// KeybindingsMenu — CUI keybindings screen.
// Vanilla source: P:\scripts\5_mission\gui\newui\keybindings\keybindingsmenu.c

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

		m_LoadingBar      = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
		if (m_LoadingBar) m_LoadingBar.SetColor(colorScheme.Loadingbar());

		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());
		m_MenuDivider.SetColor(colorScheme.Separator());
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);

		InitInputSortingMap();
		CreateTabs();
		CuiReskinTabs();
		CreateGroupContainer();

		InitPresets(-1, layoutRoot.FindAnyWidget("group_header"), input);
		m_Tabber.m_OnTabSwitch.Insert(UpdateTabContent);
		m_Tabber.SelectTabControl(0);
		m_Tabber.SelectTabPanel(0);
		g_Game.SetKeyboardHandle(this);
		m_Tabber.RefreshTab(true);

		ColorDisabled(m_Apply);
		m_Apply.SetFlags(WidgetFlags.IGNOREPOINTER);
		ColorDisabled(m_Undo);
		m_Undo.SetFlags(WidgetFlags.IGNOREPOINTER);

		return layoutRoot;
	}

	protected void CuiReskinTabs()
	{
		int tab_index = 0;
		int sort_count = InputUtils.GetInputActionSortingMap().Count();
		for (int i = 0; i < sort_count; i++)
		{
			if (InputUtils.GetInputActionSortingMap().GetElement(i) && InputUtils.GetInputActionSortingMap().GetElement(i).Count() > 0)
			{
				string group_name = GetUApi().SortingLocalization(InputUtils.GetInputActionSortingMap().GetKey(i));
				m_Tabber.CuiReskinTabControl(tab_index, Widget.TranslateString("#" + group_name));
				tab_index++;
			}
		}

		if (InputUtils.GetUnsortedInputActions() && InputUtils.GetUnsortedInputActions().Count() > 0)
		{
			m_Tabber.CuiReskinTabControl(tab_index, Widget.TranslateString("#layout_pc_keybinding_unsorted"));
		}
	}

	override void ColorDisabled(Widget w)
	{
		if (!w) return;

		ButtonWidget button = ButtonWidget.Cast(w);
		if (button) button.SetTextColor(colorScheme.DisabledText());

		TextWidget label = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		if (label) label.SetColor(colorScheme.DisabledText());
	}

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

	override void Back()
	{
		if (CuiDialog.CancelTop())
			return;

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
