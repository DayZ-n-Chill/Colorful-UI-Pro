class colorfulConfigureDialog extends UIScriptedMenu
{
	protected Widget				m_Separator;
	protected Widget		 		m_bYes;
	protected Widget				m_bNo;
	protected Widget 				m_LastFocusedButton;
	protected Widget 				m_Text;
	protected Widget 				m_Caption;
	protected ButtonWidget			m_Reset; //undo
	protected ref GameOptions		m_Options;
	private Widget m_TopShader;
	private Widget m_BottomShader;
	

	override Widget Init()
	{
		m_Options		= new GameOptions();

		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.configure.dialog.layout");
		
		m_Separator					= layoutRoot.FindAnyWidget("SeparatorPanel");
		m_bYes						= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bYes"));
		m_bNo						= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bNo"));
		m_Reset						= ButtonWidget.Cast(layoutRoot.FindAnyWidget("m_Reset"));
		m_Text						= layoutRoot.FindAnyWidget("Text");
		m_Caption					= layoutRoot.FindAnyWidget("Caption");
		
		m_TopShader 			    = layoutRoot.FindAnyWidget("TopShader");
		m_BottomShader 			    = layoutRoot.FindAnyWidget("BottomShader");

		if (m_Separator) m_Separator.SetColor(colorScheme.Separator());
		if (m_TopShader) m_TopShader.SetColor(colorScheme.TopShader());
		if (m_BottomShader) m_BottomShader.SetColor(colorScheme.BottomShader());
		m_LastFocusedButton = m_bYes;
		
		cuiElmnt.proBtnCB(m_bYes, "#message_dialog_yes", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "ResetCurrentTab");
		cuiElmnt.proBtnCB(m_bNo, "BACK", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Cancel");
		cuiElmnt.proBtnCB(m_Reset, "#layout_xbox_reset_options_defaults", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "ResetCurrentTab");
		
		return layoutRoot;
	}

	void ResetCurrentTab()
	{
		if (m_Options && m_Options.IsChanged())
		{
			m_Options.Revert();
		}
	}

	void Cancel()
	{
		GetGame().GetUIManager().Back();
	}

	void ColorNormal( Widget w )
	{
		if( !w )
			return;

		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.PrimaryText();

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColor(w, color_lbl);
	}

	void ButtonSetColor( Widget w, int color )
	{
		if( !w )
			return;

		Widget panel = w.FindWidget( w.GetName() + "_panel" );

		if( panel )
		{
			panel.SetColor( color );
		}
	}

	void ButtonSetTextColor( Widget w, int color )
	{
		if( !w )
			return;

		TextWidget label	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_label" ) );
		TextWidget text		= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text" ) );
		TextWidget text2	= TextWidget.Cast(w.FindAnyWidget( w.GetName() + "_text_1" ) );

		if( label )
		{
			label.SetColor( color );
		}

		if( text )
		{
			text.SetColor( color );
		}

		if( text2 )
		{
			text2.SetColor( color );
		}
	}

	//resets it all
	// void Reset()
	// {
	// 	m_Options.Revert();
	// 	m_GameTab.Revert();
	// 	m_SoundsTab.Revert();
	// 	m_ControlsTab.Revert();
	// 	#ifndef PLATFORM_XBOX
	// 	m_VideoTab.Revert();
	// 	#endif
		
	// 	if ( m_Options.IsChanged() )
	// 		m_Options.Revert();
		
	// 	if (GetGame().GetInput().IsEnabledMouseAndKeyboard())
	// 	{
	// 		m_Apply.SetFlags( WidgetFlags.IGNOREPOINTER );
	// 		ColorDisable( m_Apply );
	// 		m_Reset.SetFlags( WidgetFlags.IGNOREPOINTER );
	// 		ColorDisable( m_Reset );
	// 	}
	// 	else
	// 	{
	// 		layoutRoot.FindAnyWidget( "Apply" ).Show( false );
	// 		layoutRoot.FindAnyWidget( "Reset" ).Show( false );
	// 	}
		
	// 	m_CanApplyOrReset = false;
	// }
};
