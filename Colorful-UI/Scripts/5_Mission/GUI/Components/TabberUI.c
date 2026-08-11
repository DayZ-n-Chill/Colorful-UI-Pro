modded class TabberUI
{
	// Universal mod-tab adoption (see Options.c). While s_CuiRecordForeignTabs
	// is true, every AddTab(name) call — including ones made by other mods'
	// chained OptionsMenu.Init() overrides, e.g. DayZ Expansion's
	// `m_Tabber.AddTab("EXPANSION")` — gets its (index, name) recorded here.
	// Options.c flips the flag on immediately before calling super.Init() and
	// off immediately after, so only foreign tabs added during that window
	// are captured; vanilla's own four tabs never pass through AddTab (they're
	// discovered by scanning existing layout children in Init(), see vanilla
	// tabberui.c OnWidgetScriptInit -> Init()).
	static bool s_CuiRecordForeignTabs = false;
	static ref array<int>    s_CuiForeignTabIndices = new array<int>();
	static ref array<string> s_CuiForeignTabNames    = new array<string>();

	// Vanilla Init() ends with `m_InitTimer.Run(0.01, this, "AlignTabbers")`,
	// which fires a frame later. If this tabber's widget tree has since been
	// Unlink()'d (our phase-1 -> phase-2 swap in Options.c), that callback
	// dereferences dead widgets. Call this on the phase-1 tabber right before
	// destroying its tree.
	void CuiCancelInitTimer()
	{
		if (m_InitTimer)
			m_InitTimer.Stop();
	}

	// Adopts an already-built tab content pane from another (soon to be
	// destroyed) tabber's tree, instead of AddTab()'s normal "create an empty
	// pane" behaviour. The pane keeps its identity — Widget.AddChild moves it
	// in the tree without recreating it — so any handler object a foreign mod
	// stored a reference to (e.g. Expansion's OptionsMenuExpansion.m_Root)
	// stays valid. Only the tab control (the clickable header) is created
	// fresh here, using our own styled prefab, same as AddTab().
	int CuiAdoptTab(string name, Widget pane)
	{
		int new_index = m_Tabs.Count();

		Widget control = GetGame().GetWorkspace().CreateWidgets( "Colorful-UI/GUI/layouts/components/tabber_prefab/cui.tab_control.layout", m_Root.FindAnyWidget( "Tab_Control_Container" ) );
		TextWidget control_text = TextWidget.Cast( control.FindAnyWidget( "Tab_Control_x_Title" ) );

		pane.SetName( "Tab_" + new_index );
		m_Root.AddChild( pane ); // reparents; auto-detaches from its old parent

		control.SetName( "Tab_Control_" + new_index );
		control_text.SetName( "Tab_Control_" + new_index + "_Title" );
		control.FindAnyWidget( "Tab_Control_x_Background" ).SetName( "Tab_Control_" + new_index + "_Background" );

		control_text.SetText( name );

		control.SetHandler( this );
		m_TabControls.Insert( new_index, control );
		m_Tabs.Insert( new_index, pane );

		AlignTabbers();

		return new_index;
	}

	override void AlignTabbers()
	{
		float total_size;
		float x, y;

		Widget tab_controls_container = m_TabControlsRoot.FindAnyWidget( "Tab_Control_Container" );
		Widget tab_controls_scroller  = m_TabControlsRoot.FindAnyWidget( "Tab_Control_Scroller" );

		m_TabControlsRoot.Update();
		tab_controls_container.Update();

		Widget tab_child = tab_controls_container.GetChildren();
		while ( tab_child )
		{
			if ( tab_child.IsVisible() )
			{
				TextWidget tab_text = TextWidget.Cast( tab_child.FindAnyWidget( tab_child.GetName() + "_Title" ) );
				int t_x, t_y;
				tab_text.Update();
				tab_text.GetTextSize( t_x, t_y );

				float tab_width = t_x + 20;
				tab_child.SetSize( tab_width, 1 );

				tab_controls_container.Update();

				total_size += tab_width;
			}

			tab_child = tab_child.GetSibling();
		}

		tab_controls_container.SetSize(total_size, 1);
		tab_child = tab_controls_container.GetChildren();

		float x_f_c, y_f_c;
		tab_controls_container.GetScreenPos( x_f_c, y_f_c );

		while ( tab_child )
		{
			Widget tab_bg = tab_child.FindAnyWidget( tab_child.GetName() + "_Background" );
			tab_child.GetScreenPos( x, y );
			tab_bg.SetPos( ( x_f_c - x ), 0 );

			tab_bg.SetSize( total_size, 1 );

			tab_child = tab_child.GetSibling();
		}

		// The root keeps its layout width on purpose: both the options and
		// keybindings layouts wrap Tab_Control_Container in Tab_Control_Scroller,
		// so an over-wide container overflows INSIDE the scroller and scrolls.
		// Growing the root to total_size (vanilla's tail) would let the bar
		// stretch past the screen and the scroller would never engage.

		tab_controls_container.Update();
		if ( tab_controls_scroller )
			tab_controls_scroller.Update();
		m_TabControlsRoot.Update();
	}

	override void SelectTabControl( int index )
	{
		Widget tab_control = m_TabControls.Get( index );
		if( tab_control )
		{
			Widget tab_title = TextWidget.Cast(tab_control.FindAnyWidget( tab_control.GetName() + "_Title" ));
			
			int color_title = colorScheme.TabHoverColor();
			int color_backg = UIColor.Black();
			
			tab_title.SetColor( color_title );
			tab_control.SetColor( color_backg );
		}
	}

	override bool OnMouseEnter( Widget w, int x, int y )
	{
		int index = m_TabControls.GetKeyByValue( w );
		if( m_SelectedIndex == index )
		{
			return false;
		}
		
		Widget tab_control = m_TabControls.Get( index );
		if( tab_control )
		{			
			Widget tab_title = TextWidget.Cast(tab_control.FindAnyWidget( tab_control.GetName() + "_Title" ));
			tab_title.SetColor(colorScheme.TabHoverColor());
			tab_control.SetColor(UIColor.Black());
		}
		
		return false;
	}

	// Rebuild an existing tab control from our layout. Other mods (DayZ
	// Expansion among them) replace AddTab with their own tab control layout
	// and can compile after us, so their control wins at creation time. Menus
	// we own call this afterwards to reclaim each control. TextWidget has no
	// GetText, so the caller supplies the title.
	void CuiReskinTabControl( int index, string name )
	{
		Widget old_control = m_TabControls.Get( index );
		if ( !old_control )
			return;

		Widget container = m_TabControlsRoot.FindAnyWidget( "Tab_Control_Container" );
		if ( !container )
			return;

		string cname = old_control.GetName();

		Widget control = GetGame().GetWorkspace().CreateWidgets( "Colorful-UI/GUI/layouts/components/tabber_prefab/cui.tab_control.layout", container );
		TextWidget control_text = TextWidget.Cast( control.FindAnyWidget( "Tab_Control_x_Title" ) );

		control.SetName( cname );
		control_text.SetName( cname + "_Title" );
		control.FindAnyWidget( "Tab_Control_x_Background" ).SetName( cname + "_Background" );
		control_text.SetText( name );
		control.SetHandler( this );

		m_TabControls.Set( index, control );
		old_control.Unlink();

		if ( index == m_SelectedIndex )
			SelectTabControl( index );

		AlignTabbers();
	}

	override int AddTab( string name )
	{
		int new_index = m_Tabs.Count();
		Widget tab = GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/tabber_prefab/tab.layout", m_Root );
		Widget control = GetGame().GetWorkspace().CreateWidgets( "Colorful-UI/GUI/layouts/components/tabber_prefab/cui.tab_control.layout", m_Root.FindAnyWidget( "Tab_Control_Container" ) );
		TextWidget control_text = TextWidget.Cast( control.FindAnyWidget( "Tab_Control_x_Title" ) );
		
		tab.SetName( "Tab_" + new_index );
		control.SetName( "Tab_Control_" + new_index );
		control_text.SetName( "Tab_Control_" + new_index + "_Title" );
		control.FindAnyWidget( "Tab_Control_x_Background" ).SetName( "Tab_Control_" + new_index + "_Background" );
		
		control_text.SetText( name );
		
		control.SetHandler( this );
		m_TabControls.Insert( new_index, control );
		m_Tabs.Insert( new_index, tab );

		AlignTabbers();

		if ( s_CuiRecordForeignTabs )
		{
			s_CuiForeignTabIndices.Insert( new_index );
			s_CuiForeignTabNames.Insert( name );
		}

		return new_index;
	}
}