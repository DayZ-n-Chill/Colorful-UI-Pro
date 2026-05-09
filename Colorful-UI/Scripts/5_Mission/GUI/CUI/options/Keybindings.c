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

		return layoutRoot;
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
			m_AlternativeBindButton.SetColor(ARGBF(1, 0, 0, 0));
			m_PrimaryClear.Show(false);
			m_AlternativeClear.Show(false);
		}
		return false;
	}
}

