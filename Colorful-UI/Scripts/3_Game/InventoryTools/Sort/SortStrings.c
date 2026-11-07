// Every piece of text the inventory tools show, in one place so a translation pass only has to
// look here.
class SortStrings
{
	// Title of the SEARCH tab on the EQUIPMENT card. Upper case like the vanilla header titles.
	static const string CARD_TITLE = "SEARCH";
	static const string PLACEHOLDER = "Search inventory";
	// Hover tip on the magnifier in the EQUIPMENT bar that opens the search.
	static const string OPEN_SEARCH_TIP = "Search";
	static const string FOUND_SUFFIX = " found";
	static const string NO_MATCH = "No matches";
	// "12 found · 3 filters": appended to the count while any category tile is on.
	static const string FILTERS_SEP = " · ";
	static const string FILTER_ONE = " filter";
	static const string FILTER_MANY = " filters";
	// Text button beside the count: clears every tile and the text.
	static const string RESET = "Reset";

	static const string SORT_TIP = "Sort by...";
	// The sort button's drop-down (SortMenu).
	static const string SORTBY_TITLE = "SORT BY";
	static const string SORTBY_CATEGORY = "Category";
	static const string SORTBY_SIZE = "Size";
	static const string SORTBY_NAME = "Name";
	static const string SORTBY_TAB_ORDER = "Default order";
	static const string SORTBY_FIRST = " first";
	static const string SORTBY_SIZE_BIG = "Largest first";
	static const string SORTBY_SIZE_SMALL = "Smallest first";
	static const string SORTBY_NAME_AZ = "A to Z";
	static const string SORTBY_NAME_ZA = "Z to A";
	static const string SORTING = "Sorting";
	static const string SORTED = "Sorted";
	static const string ALREADY = "Already tidy";
	// The planned order could not be reached in a crowded container, so it was packed up into the top left instead.
	static const string PACKED = "Packed";
	static const string STOPPED = "Sort stopped";
	static const string CHANGED = "Container changed";

	// Stack button: merges split stacks, then runs the sort and reports it on the same chip.
	static const string STACK_TIP = "Auto sort (stack, then sort)";
	static const string STACKING = "Stacking";
	static const string STACKED = "Stacked";
	static const string STACKED_SORTED = ", sorted";
	static const string STACK_STOPPED = "Stack stopped";

	// Sort all, on the EQUIPMENT bar: stack and sort every container the player is wearing.
	static const string SORT_ALL_TIP = "Sort all";
	static const string SORTING_ALL = "Sorting all";
	static const string SORTED_ALL = "All sorted";
	// "Sorted 3 of 5" when some containers could not be sorted.
	static const string SORTED_OF = "Sorted %1 of %2";
	static const string NOTHING_TO_SORT = "Nothing to sort";

	// Grab all, on the VICINITY bar.
	static const string GRAB_ALL_TIP = "Grab all";
	static const string GRABBING = "Grabbing";
	static const string GRABBED = "Grabbed";
	static const string GRAB_LEFT = "left";
	static const string GRAB_NOTHING = "Nothing to grab";
	static const string GRAB_NO_ROOM = "No room";
	static const string GRAB_STOPPED = "Grab stopped";

	// The Transfer arrows (ItemTransfer.c), their progress chip, and the bar title while picking.
	static const string TRANSFER_MODE_TITLE = "TRANSFER MODE";
	static const string TRANSFER_TO_VICINITY_TIP = "Transfer to vicinity: click, pick items or set a target, click again";
	static const string TRANSFER_TO_GEAR_TIP = "Transfer to gear: click, pick items or set a target, click again";
	static const string MOVING = "Moving";
	static const string MOVED = "Moved";
	static const string DROPPED = "Dropped";
	static const string ON_GROUND = "on the ground";
	static const string TRANSFER_STOPPED = "Transfer stopped";

	// Quick transfer: what a target card flashes when the item does not fit, and the hover tips on a
	// card's target toggle (what a click will do).
	static const string TARGET_NO_ROOM = "No room";
	static const string TARGET_SET_TIP = "Set as transfer target";
	static const string TARGET_CLEAR_TIP = "Clear transfer target";

	// Category tiles under the search box, in row order (shown as the tiles' hover names), and the
	// category sort's "... first" rows.
	static const string CAT_WEAPONS = "Weapons";
	static const string CAT_AMMO = "Ammo";
	static const string CAT_MAGAZINES = "Magazines";
	static const string CAT_MEDICAL = "Medical";
	static const string CAT_FOOD = "Food";
	static const string CAT_CLOTHING = "Clothing";
	static const string CAT_TOOLS = "Tools";
	static const string CAT_SUPPLIES = "Supplies";
}
