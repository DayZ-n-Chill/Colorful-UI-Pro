// Repaints the quick slot bars after every real change to the hands. Ported from DayZ Deluxe's
// DD_QuickSlotsRefresh.c.
//
// Vanilla asks the inventory's bar to repaint right after REQUESTING a move, before the hands slot has
// changed, so putting an item away left its slot painted "in hands". Vanilla's in-hands tint is a
// faint wash so the gap barely shows; ours is a solid cell (QuickSlotSlots.Fill), so it does. The HUD
// bar's own timer cannot cover it either, since it skips refreshing while the inventory is open.
//
// One GUI frame later, because the hands slot may not be settled when these fire.
// IngameHud.RefreshQuickbar(true) repaints the inventory's bar as well as its own.
modded class PlayerBase
{
	override void EEItemIntoHands( EntityAI item )
	{
		super.EEItemIntoHands( item );
		QuickSlotQueueRepaint();
	}

	override void EEItemOutOfHands( EntityAI item )
	{
		super.EEItemOutOfHands( item );
		QuickSlotQueueRepaint();
	}

	protected void QuickSlotQueueRepaint()
	{
		// Only the rounded cells need it: vanilla's own look (RoundedQuickSlots off) keeps vanilla's timing.
		if ( !RoundedQuickSlots || !g_Game.IsClient() || GetInstanceType() != DayZPlayerInstanceType.INSTANCETYPE_CLIENT )
			return;

		g_Game.GetCallQueue( CALL_CATEGORY_GUI ).Remove( QuickSlotRepaint );
		g_Game.GetCallQueue( CALL_CATEGORY_GUI ).Call( QuickSlotRepaint );
	}

	protected void QuickSlotRepaint()
	{
		if ( m_Hud )
			m_Hud.RefreshQuickbar( true );
	}
}
