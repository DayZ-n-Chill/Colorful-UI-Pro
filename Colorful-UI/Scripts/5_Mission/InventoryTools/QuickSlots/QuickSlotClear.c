// Right-clicking an item on the inventory's quick slot bar takes it off the bar, the same as dragging
// it off (InventoryQuickbar.OnItemDrop then Remove). The item itself stays where it is. Ported from
// DayZ Deluxe's DD_QuickSlotClear.c. InventoryGrid already routes a right mouse-down on a cell here
// (inventorygrid.c:129); vanilla's InventoryQuickbar just never answered it.
modded class InventoryQuickbar
{
	override void OnItemRightClick( InventoryGrid grid, Widget w, int row, int col )
	{
		super.OnItemRightClick( grid, w, row, col );

		PlayerBase v_Player = PlayerBase.Cast( g_Game.GetPlayer() );
		if ( !v_Player )
			return;

		InventoryItem v_Item = InventoryItem.Cast( v_Player.GetQuickBarEntity( col ) );
		if ( !v_Item )
			return;

		Remove( v_Item );

		InventoryMenu v_Menu = InventoryMenu.Cast( g_Game.GetUIManager().FindMenu( MENU_INVENTORY ) );
		if ( v_Menu )
			v_Menu.RefreshQuickbar();
	}
}
