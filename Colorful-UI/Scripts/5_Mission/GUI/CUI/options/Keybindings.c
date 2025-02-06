modded class KeybindingsMenu extends UIScriptedMenu
{
	private Widget m_TopShader;
	private Widget m_BottomShader;
	private Widget m_MenuDivider;
    private TextWidget m_ApplyBtn_Label;
		
	override Widget Init()
	{
		Input input = GetGame().GetInput();
		layoutRoot			= GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/settings/cui.keybindings.layout", null);
		
		m_Apply				= ButtonWidget.Cast(layoutRoot.FindAnyWidget("ApplyBtn"));
        m_ApplyBtn_Label    = TextWidget.Cast(layoutRoot.FindAnyWidget("ApplyBtn_Label"));
		m_Back				= ButtonWidget.Cast(layoutRoot.FindAnyWidget("BackBtn"));
		m_Undo				= ButtonWidget.Cast(layoutRoot.FindAnyWidget("UndoBtn"));
		m_Defaults			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("ResetBtn"));
		m_HardReset			= ButtonWidget.Cast(layoutRoot.FindAnyWidget("ResetAllBtn"));

        cuiElmnt.proBtn(m_Apply,"#main_menu_exit",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"", this,"OnApplyPressed");

		m_TopShader 			    = layoutRoot.FindAnyWidget( "TopShader" );
		m_BottomShader 			    = layoutRoot.FindAnyWidget( "BottomShader" );
		m_MenuDivider				= layoutRoot.FindAnyWidget( "MenuDivider" );

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
		
		// ColorDisabled(m_ApplyBtn_Label);
		// m_ApplyBtn_Label.SetFlags(WidgetFlags.IGNOREPOINTER);
		// ColorDisabled(m_Undo);
		// m_Undo.SetFlags(WidgetFlags.IGNOREPOINTER);
		// ColorWhite(m_Defaults, null);
		// m_Defaults.ClearFlags(WidgetFlags.IGNOREPOINTER);
		
		return layoutRoot;
	}
	
    static void OnApplyPressed()
    {
        // Logic for applying changes
        Print("Apply button clicked");
    }

    // void OnBackPressed()
    // {
    //     // Logic for returning to the previous menu
    //     Print("Back button clicked");
    //     CloseMenu();
    // }

    // void OnUndoPressed()
    // {
    //     // Logic for undoing changes
    //     Print("Undo button clicked");
    //     UndoChanges();
    // }

    // void OnResetPressed()
    // {
    //     // Logic for resetting to defaults
    //     Print("Reset button clicked");
    //     ResetToDefaults();
    // }

    // void OnHardResetPressed()
    // {
    //     // Logic for performing a hard reset
    //     Print("Hard Reset button clicked");
    //     PerformHardReset();
    // }

	// override void ClearKeybind(int key_index)
	// {
	// 	ColorWhite(m_ApplyBtn_Label, null);
	// 	m_ApplyBtn_Label.ClearFlags(WidgetFlags.IGNOREPOINTER);
	// 	ColorWhite(m_Undo, null);
	// 	m_Undo.ClearFlags(WidgetFlags.IGNOREPOINTER);
	// }
	
	// override void ClearAlternativeKeybind(int key_index)
	// {
	// 	ColorWhite(m_ApplyBtn_Label, null);
	// 	m_ApplyBtn_Label.ClearFlags(WidgetFlags.IGNOREPOINTER);
	// 	ColorWhite(m_Undo, null);
	// 	m_Undo.ClearFlags(WidgetFlags.IGNOREPOINTER);
	// }
	
	// //Coloring functions (Until WidgetStyles are useful)
	// override void ColorRed(Widget w)
	// {
	// 	SetFocus(w);
		
	// 	ButtonWidget button = ButtonWidget.Cast(w);
	// 	if (m_Apply)
	// 	{
	// 		m_ApplyBtn_Label.SetColor(colorScheme.ButtonHover());
	// 	}
	// }
	
	// override void ColorWhite(Widget w, Widget enterW)
	// {
	// 	#ifdef PLATFORM_WINDOWS
	// 	SetFocus(null);
	// 	#endif
		
	// 	ButtonWidget button = ButtonWidget.Cast(w);
	// 	if (button)
	// 	{
	// 		if (button.GetFlags() & WidgetFlags.DISABLED)
	// 		{
	// 			button.SetTextColor(ColorManager.COLOR_DISABLED_TEXT);
	// 		}
	// 		else
	// 		{
	// 			button.SetTextColor(ColorManager.COLOR_NORMAL_TEXT);
	// 		}
	// 	}
	// }
	
	// override void ColorDisabled(Widget w)
	// {
	// 	#ifdef PLATFORM_WINDOWS
	// 	SetFocus(null);
	// 	#endif
		
	// 	ButtonWidget button = ButtonWidget.Cast(w);
	// 	if (button)
	// 	{
	// 		// These will be reset in the next update with the new theme manager.
	// 		button.SetTextColor(ColorManager.COLOR_DISABLED_TEXT);
	// 	}
	// }
}