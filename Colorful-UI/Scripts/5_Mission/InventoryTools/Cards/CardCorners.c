// Rounded card corners on the inventory screen, Deluxe's rule: an OPEN card's
// header is rounded on the top corners only and its body on the bottom corners only, so the two read
// as one rounded card; a FOLDED card is only its header, rounded on all four corners (FoldBack,
// painted by FoldHooks.c). Colours are unchanged.
//
// The headers carry their own top rounding in Colorful UI's header layouts. The bodies are vanilla
// layouts, so the pieces that END a card are pointed at rounded copies here. LayoutHolder's
// constructor calls SetLayoutName() and then builds from m_LayoutName (layoutholder.c), and it sets
// m_Parent first, so a piece can pick its copy by who builds it. A style cannot be swapped once a
// widget exists (Widget has GetStyleName but no setter), which is why these are layout copies.
//
// Only a piece that is the bottom of its card is rounded: a rounded bottom in the middle of a stack
// cuts the card in two.
//
// The rounded body copies are only used while RoundedInventory (Settings.c) is on; with it off every
// body keeps its original layout. The headers pick their own copy in CardHeaders.c.
class CardLayouts
{
	static const string AttachmentsGroup_Path	= "Colorful-UI/GUI/layouts/inventory/attachments_group_container.layout";
	static const string ContainerBottom_Path	= "Colorful-UI/GUI/layouts/inventory/container_bottom.layout";
	static const string AttachmentsBottom_Path	= "Colorful-UI/GUI/layouts/inventory/attachments_group_container_bottom.layout";
	static const string HandsPreview_Path		= "Colorful-UI/GUI/layouts/inventory/hands_preview_card.layout";
	static const string CargoNarrow_Path		= "Colorful-UI/GUI/layouts/inventory/narrow/cargo_container_card.layout";
	static const string CargoMedium_Path		= "Colorful-UI/GUI/layouts/inventory/medium/cargo_container_card.layout";
	static const string CargoWide_Path			= "Colorful-UI/GUI/layouts/inventory/wide/cargo_container_card.layout";
}

// The VICINITY card's body: the ground slot band, the bottom of its card.
modded class VicinitySlotsContainer
{
	override void SetLayoutName()
	{
		super.SetLayoutName();
		#ifndef PLATFORM_CONSOLE
		if (RoundedInventory)
			m_LayoutName = CardLayouts.ContainerBottom_Path;
		#endif
	}
}

// The slot band is the bottom of its card under EQUIPMENT, under VICINITY and on a corpse. Everywhere
// else (an attachment category's row, an item card's attachment strip) it has more stacked under it
// and keeps the square layout; AttachmentsWrapper below decides for item cards' sections.
// Colorful UI's own copy used to come in through a WidgetLayoutName const; a const cannot follow
// RoundedInventory, so it is picked here (CardHeaders.c does the same for the headers).
modded class AttachmentsGroupContainer
{
	override void SetLayoutName()
	{
		super.SetLayoutName();
		m_LayoutName = CardLayouts.AttachmentsGroup_Path;
		#ifndef PLATFORM_CONSOLE
		if (RoundedInventory && (PlayerContainer.Cast(m_Parent) || VicinitySlotsContainer.Cast(m_Parent) || ZombieContainer.Cast(m_Parent)))
			m_LayoutName = CardLayouts.AttachmentsBottom_Path;
		#endif
	}
}

// The cargo grid: the bottom of every cargo card (cargo always comes after the attachment sections).
modded class CargoContainer
{
	override void SetLayoutName()
	{
		super.SetLayoutName();
		#ifndef PLATFORM_CONSOLE
		if (!RoundedInventory)
			return;
		switch (InventoryMenu.GetWidthType())
		{
			case ScreenWidthType.NARROW:
				m_LayoutName = CardLayouts.CargoNarrow_Path;
				break;
			case ScreenWidthType.MEDIUM:
				m_LayoutName = CardLayouts.CargoMedium_Path;
				break;
			case ScreenWidthType.WIDE:
				m_LayoutName = CardLayouts.CargoWide_Path;
				break;
		}
		#endif
	}
}

// The HANDS card's slot.
modded class HandsPreview
{
	override void SetLayoutName()
	{
		super.SetLayoutName();
		#ifndef PLATFORM_CONSOLE
		if (RoundedInventory)
			m_LayoutName = CardLayouts.HandsPreview_Path;
		#endif
	}
}

// ---- item cards' attachment sections --------------------------------------------------------------
// A weapon card or the item in hands draws its attachment slots in AttachmentsWrapper sections. A
// section is rounded only when it ends the card:
// - the item's own section is square when a cargo grid or an attachment's own section follows it;
// - an attachment's section (a handguard's lights) is rounded only when it is the last such section.
// Vanilla builds the section inside Attachments.InitAttachmentGrid, before the wrapper knows its item,
// so the item is handed over through a static for the length of that call.
class CardSections
{
	static EntityAI s_CardBuilding;

	// An attachment that gets a section of its own (containerwithcargoandattachments.c).
	static bool HasSection(EntityAI v_Item)
	{
		return v_Item && v_Item.GetInventory().GetAttachmentSlotsCount() > 0 && v_Item.CanDisplayAnyAttachmentSlot();
	}

	static bool EndsCard(EntityAI v_Owner, EntityAI v_Section)
	{
		if (!v_Owner || !v_Section)
			return true;
		int v_Count = v_Owner.GetInventory().AttachmentCount();
		EntityAI v_Last = null;
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			EntityAI v_Att = v_Owner.GetInventory().GetAttachmentFromIndex(v_I);
			if (HasSection(v_Att))
				v_Last = v_Att;
		}
		if (v_Section == v_Owner)
			return !v_Last && !v_Owner.GetInventory().GetCargo();
		return v_Section == v_Last;
	}
}

modded class Attachments
{
	override void InitAttachmentGrid(int att_row_index)
	{
		CardSections.s_CardBuilding = m_Entity;
		super.InitAttachmentGrid(att_row_index);
		CardSections.s_CardBuilding = null;
	}
}

modded class AttachmentsWrapper
{
	protected bool m_CardRounded = true;

	override void SetLayoutName()
	{
		super.SetLayoutName();
		#ifndef PLATFORM_CONSOLE
		if (!RoundedInventory)
			return;
		EntityAI v_Owner = null;
		ClosableContainer v_Card = ClosableContainer.Cast(m_Parent);
		if (v_Card)
			v_Owner = v_Card.FoldGetEntity();
		else if (HandsContainer.Cast(m_Parent) && GetGame().GetPlayer())
			v_Owner = GetGame().GetPlayer().GetHumanInventory().GetEntityInHands();
		if (!v_Owner)
			return;
		m_CardRounded = CardSections.EndsCard(v_Owner, CardSections.s_CardBuilding);
		if (m_CardRounded)
			m_LayoutName = CardLayouts.AttachmentsBottom_Path;
		#endif
	}

	bool CardIsRounded()
	{
		return m_CardRounded;
	}
}

// A card is usually built the moment its item lands somewhere (the belt goes on before the sheath is
// put on it), so the item's own section can be built as the card's last section (rounded) and then
// have an attachment's section added under it. When an attachment comes or goes and the item's own
// section now has the wrong shape, it is rebuilt with the right one, as HandsContainer does when the
// item in hand changes.
modded class ContainerWithCargoAndAttachments
{
	override void AttachmentAddedEx(EntityAI item, string slot, EntityAI parent, bool immedUpdate = true)
	{
		super.AttachmentAddedEx(item, slot, parent, immedUpdate);
		CardQueueSectionShape();
	}

	override void AttachmentRemoved(EntityAI item, string slot, EntityAI parent)
	{
		super.AttachmentRemoved(item, slot, parent);
		CardQueueSectionShape();
	}

	void ~ContainerWithCargoAndAttachments()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(CardFixSectionShape);
	}

	// Next GUI tick: this runs inside the item's attach or detach event, which the old Attachments is
	// also listening to, so it must not be replaced while that event is still being delivered.
	protected void CardQueueSectionShape()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(CardFixSectionShape);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(CardFixSectionShape);
	}

	protected void CardFixSectionShape()
	{
		#ifndef PLATFORM_CONSOLE
		if (!RoundedInventory || !m_Atts || !m_Entity)
			return;
		AttachmentsWrapper v_Wrapper = m_Atts.GetWrapper();
		if (!v_Wrapper)
			return;
		bool v_Want = CardSections.EndsCard(m_Entity, m_Entity);
		if (v_Wrapper.CardIsRounded() == v_Want)
			return;

		m_Body.RemoveItem(v_Wrapper);
		m_Atts = new Attachments(this, m_Entity);
		m_Atts.InitAttachmentGrid(SORT_ATTACHMENTS_OWN);
		m_AttachmentSlotsSorted = m_Atts.GetSlotsSorted();
		m_Atts.UpdateInterval();
		RecomputeContainers();
		RecomputeOpenedContainers();
		if (m_Parent)
			m_Parent.Refresh();
		#endif
	}
}
