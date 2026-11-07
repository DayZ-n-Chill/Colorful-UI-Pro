// The quick slots (the item bar along the bottom centre of the screen, and the same strip inside the
// inventory screen), restyled as DayZ Deluxe has them: dark rounded cells instead of vanilla's flat
// grey squares. Ported from Deluxe's DD_QuickSlots.c.
//
// How the bar WORKS is not touched. Both bars are vanilla's InventoryGrid (the scriptclass on the
// QuickbarGrid widget in day_z_hud.layout and in all three inventory widths), and it spawns its
// tiles and items from two layout files and addresses them by name. So the restyle is a layout swap
// plus a re-render of the state colour vanilla already classifies in
// InventoryQuickbar.GetQuickbarItemColor.
//
// Every override calls super first and then replaces what vanilla built, so another mod's chained
// override still runs. The tiles are created once per grid, so building them twice costs nothing.
//
// With RoundedQuickSlots (Settings.c) off, every override returns right after super, so vanilla's own
// inventoryGridBackground/inventoryGridItem layouts and colours stay and nothing here looks for the
// Label1/LabelTR names that only our tiles have.
class QuickSlotSlots
{
	static const string TileLayout = "Colorful-UI/GUI/layouts/inventory_tools/quickslots/QuickSlotTile.layout";
	static const string ItemLayout = "Colorful-UI/GUI/layouts/inventory_tools/quickslots/QuickSlotItem.layout";

	// The state colours, inlined from Deluxe's tablet theme.
	static const int FILL_IN_HANDS		= 0xFF2A2A2A;	// #2a2a2a opaque: the item in the player's hands
	static const int FILL_NO_ROOM		= 0xFF8E3A31;	// #8e3a31 opaque: in hands, nowhere to put it
	static const int FILL_WONT_FIT		= 0x8C7A5A2A;	// #7a5a2a at 140: what is in hands cannot go here
	static const int FILL_RESTING		= 0x00000000;	// clear: the cell's own dark ground shows

	// Maps vanilla's state answer onto the palette. The active cell is the one OPAQUE cell in a row of
	// translucent ones, which is the only marker by design. A fifth value from another mod falls
	// through to the resting look.
	static int Fill( int v_State )
	{
		if ( v_State == InventoryGrid.ITEM_COLOR_QUICKBAR_H_GOOD )
			return FILL_IN_HANDS;

		if ( v_State == InventoryGrid.ITEM_COLOR_QUICKBAR_H_BAD )
			return FILL_NO_ROOM;

		if ( v_State == InventoryGrid.ITEM_COLOR_QUICKBAR_I_BAD )
			return FILL_WONT_FIT;

		return FILL_RESTING;
	}
}

modded class InventoryGrid
{
	// Called once, with 10, from InventoryQuickbar's constructor. Vanilla's tiles are swapped for ours
	// in index order, so the WrapSpacer ends up holding ours in slot order.
	override void GenerateQuickBarBackgroundTiles( int count )
	{
		super.GenerateQuickBarBackgroundTiles( count );

		if ( !RoundedQuickSlots )
			return;

		int			v_I			= 0;
		Widget		v_Old		= NULL;
		Widget		v_Root		= NULL;
		TextWidget	v_Corner	= NULL;
		TextWidget	v_Centre	= NULL;

		for ( v_I = 0; v_I < count; v_I++ )
		{
			v_Old = m_BackgroundWidgets.Get( v_I );
			if ( v_Old )
				v_Old.Unlink();

			v_Root = g_Game.GetWorkspace().CreateWidgets( QuickSlotSlots.TileLayout, m_Root );
			v_Corner = TextWidget.Cast( v_Root.FindAnyWidget( "Label1" ) );
			v_Centre = TextWidget.Cast( v_Root.FindAnyWidget( "Label2" ) );
			v_Corner.SetText( ( v_I + 1 ).ToString() );
			v_Centre.SetText( ( v_I + 1 ).ToString() );
			m_BackgroundWidgets.Set( v_I, v_Root );
		}
	}

	// Vanilla's AddItem spawns its own item cell; that cell is removed and ours spawned in its place,
	// then the rest of vanilla's body runs again against ours.
	override void AddItem( InventoryItem item, vector data, vector rotation )
	{
		super.AddItem( item, data, rotation );

		if ( !RoundedQuickSlots )
			return;

		int		v_Index		= Math.Round( data[ 0 ] );
		int		v_Width		= Math.Round( data[ 1 ] );
		int		v_Height	= Math.Round( data[ 2 ] );
		Widget	v_Bck		= GetItemBackground( v_Index );

		if ( !v_Bck )
			return;

		QuickSlotRemoveCell( v_Bck, m_ItemWidgets.Get( v_Index ) );

		Widget	v_CellRoot	= g_Game.GetWorkspace().CreateWidgets( QuickSlotSlots.ItemLayout, v_Bck );
		Widget	v_ItemW		= v_CellRoot.FindAnyWidget( "GridItem" );

		v_Bck.FindAnyWidget( "LabelTR" ).Show( true );
		v_Bck.FindAnyWidget( "LabelCC" ).Show( false );

		m_ItemWidgets.Set( v_Index, v_ItemW );

		ResetItemWidget( v_ItemW, v_Width, v_Height );

		if ( item )
		{
			ItemPreviewWidget v_Preview = ItemPreviewWidget.Cast( v_ItemW.FindAnyWidget( "Preview" ) );
			v_Preview.SetItem( item );
			v_Preview.SetModelOrientation( rotation );
			v_Preview.SetView( v_Preview.GetItem().GetViewIndex() );
			RefreshItemVariables( item, data, true, true );
		}
	}

	// Walks up from the item widget vanilla just recorded to the GridCell root it was spawned as, and
	// deletes that cell. Walking from the recorded widget, not FindAnyWidget, picks the right cell.
	protected void QuickSlotRemoveCell( Widget v_Bck, Widget v_ItemW )
	{
		Widget v_Cell = v_ItemW;

		while ( v_Cell && v_Cell.GetParent() != v_Bck )
			v_Cell = v_Cell.GetParent();

		if ( v_Cell )
			v_Cell.Unlink();
	}

	// Vanilla asks the controller for the state and paints it; super does exactly that and then the
	// answer is re-painted through the Deluxe palette. RefreshItemVariables inside super runs with
	// show_temperature false, so nothing paints GridItem after this.
	override void RefreshQuickbarItemVariables( InventoryItem item, vector data )
	{
		super.RefreshQuickbarItemVariables( item, data );

		if ( !RoundedQuickSlots )
			return;

		int		v_Index	= Math.Round( data[ 0 ] );
		Widget	v_Bck	= GetItemBackground( v_Index );

		if ( !v_Bck )
			return;

		Widget v_ItemW = v_Bck.FindAnyWidget( "GridItem" );
		if ( v_ItemW )
			v_ItemW.SetColor( QuickSlotSlots.Fill( m_Controller.GetQuickbarItemColor( this, item ) ) );
	}

	// Vanilla resets the item panel to a 4% white wash, a haze over the dark cell. The panel is the
	// state surface and only state should paint it, so it starts clear. Every caller is followed by a
	// state pass, so nothing is left unpainted.
	override protected void ResetItemWidget( Widget item_w, int width, int height )
	{
		super.ResetItemWidget( item_w, width, height );

		if ( RoundedQuickSlots && item_w )
			item_w.SetColor( QuickSlotSlots.FILL_RESTING );
	}
}
