// Quick assign: hover an item in the inventory and press a number key (1-0) to put it on that
// quick slot. Ported from DayZ Deluxe (DD_Core/scripts/4_World/DD_QuickAssign/DD_QuickAssign.c).
//
// The number keys already reach script while the inventory is open: PlayerBase.CommandHandler calls
// OnQuickBarSingleUse for hic.IsQuickBarSlot() without checking for an open menu
// (P:\scripts\4_world\entities\manbase\playerbase.c:3451-3456). Vanilla uses the slot; this takes
// the press and fills the slot instead when the cursor is over an item.
//
// The hovered item is pushed in from the inventory cells (5_Mission, QuickAssignHover.c) because
// ItemManager is 5_Mission and a modded PlayerBase must live in 4_World.
//
// No RPC is needed: SetQuickBarEntityShortcut sends the change to the server itself through
// ScriptInputUserData (P:\scripts\4_world\classes\quickbarbase.c:146-171).
class QuickAssign
{
	// The item under the cursor, or NULL. Set on mouse enter, cleared on mouse leave. Closing the
	// inventory hides the cells without a mouse leave, so TryAssign checks the menu itself.
	static EntityAI s_QuickAssignHovered;

	static void SetHovered(EntityAI v_Item)
	{
		s_QuickAssignHovered = v_Item;
	}

	// True if the press was spent on an assignment; the caller must then NOT also use the slot.
	static bool TryAssign(PlayerBase v_Player, int v_Slot)
	{
		// CommandHandler runs on the server too, where there is no cursor.
		if (!v_Player || g_Game.IsDedicatedServer() || !s_QuickAssignHovered)
			return false;

		// A hover left over from a closed inventory is not a hover.
		if (!g_Game.GetUIManager().IsMenuOpen(MENU_INVENTORY))
		{
			s_QuickAssignHovered = null;
			return false;
		}

		// Only what the player carries: a vicinity item can never be used from a quick slot.
		if (s_QuickAssignHovered.GetHierarchyRootPlayer() != v_Player)
			return false;

		// Same gate as vanilla's drag onto the quickbar.
		if (!s_QuickAssignHovered.GetInventory().CanRemoveEntity())
			return false;

		// Vanilla numbers its slots from 1 and stores them from 0.
		if (v_Slot < 1)
			return false;

		// force = true so an item already on another slot moves to this one.
		v_Player.SetQuickBarEntityShortcut(s_QuickAssignHovered, v_Slot - 1, true);
		return true;
	}
}

// Vanilla: P:\scripts\4_world\entities\manbase\playerbase.c:4938
modded class PlayerBase
{
	override void OnQuickBarSingleUse(int slotClicked)
	{
		// Digits typed into the inventory search box arrive here too; they must neither use nor
		// assign a slot (Deluxe QuickBarGuard.c).
		if (SearchFilter.IsTyping())
			return;

		// super is skipped only when the press was consumed by an assignment, so pressing 3 over a
		// bandage puts it on 3 instead of also using whatever was on 3.
		if (QuickAssign.TryAssign(this, slotClicked))
			return;

		super.OnQuickBarSingleUse(slotClicked);
	}
}
