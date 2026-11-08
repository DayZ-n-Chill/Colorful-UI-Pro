// Where each tool button sits on the EQUIPMENT and VICINITY bars and in a card's header, counted from
// the right edge (slot 0 is the rightmost place, SortButton.SLOT_W apart), in Deluxe's order. A button
// whose feature is switched off in Settings.c takes no slot, so every button left of it closes up and
// the row never shows a gap. Vanilla's own +/- is hidden on both bars (collapsible_header.layout), so
// it takes no place.
// EQUIPMENT, right to left: Search, Sort all, Show/close all, Collapse/expand all, Transfer.
// VICINITY, right to left: Show/close all, Collapse/expand all, Grab all, Transfer.
// Card header, right to left after vanilla's X: fold chevron, sort, stack.
class BarSlots
{
	static const int EQ_SEARCH		= 0;
	static const int EQ_SORT_ALL	= 1;
	static const int EQ_FOLD_EYE	= 2;
	static const int EQ_FOLD_ALL	= 3;
	static const int EQ_TRANSFER	= 4;

	static const int VIC_FOLD_EYE	= 0;
	static const int VIC_FOLD_ALL	= 1;
	static const int VIC_GRAB		= 2;
	static const int VIC_TRANSFER	= 3;

	static const int CARD_FOLD		= 0;
	static const int CARD_SORT		= 1;
	static const int CARD_STACK		= 2;

	static int Equipment(int v_Button)
	{
		int v_Slot = 0;
		for (int v_I = 0; v_I < v_Button; v_I++)
		{
			if (EquipmentOn(v_I))
				v_Slot++;
		}
		return v_Slot;
	}

	static int Vicinity(int v_Button)
	{
		int v_Slot = 0;
		for (int v_I = 0; v_I < v_Button; v_I++)
		{
			if (VicinityOn(v_I))
				v_Slot++;
		}
		return v_Slot;
	}

	static int Card(int v_Button)
	{
		int v_Slot = 0;
		for (int v_I = 0; v_I < v_Button; v_I++)
		{
			if (CardOn(v_I))
				v_Slot++;
		}
		return v_Slot;
	}

	protected static bool EquipmentOn(int v_Button)
	{
		if (v_Button == EQ_SEARCH)
			return InventorySearch;
		if (v_Button == EQ_SORT_ALL)
			return InventorySort;
		if (v_Button == EQ_TRANSFER)
			return InventoryTransfer;
		return InventoryFold;
	}

	protected static bool VicinityOn(int v_Button)
	{
		if (v_Button == VIC_GRAB || v_Button == VIC_TRANSFER)
			return InventoryTransfer;
		return InventoryFold;
	}

	protected static bool CardOn(int v_Button)
	{
		if (v_Button == CARD_FOLD)
			return InventoryFold;
		return InventorySort;
	}
}
