// Every piece of text the inventory search shows. Its own class (not SortStrings, which the sort
// feature owns) so the features never collide.
class SearchStrings
{
	// The EQUIPMENT bar's title while the SEARCH tab is showing.
	static const string CARD_TITLE = "SEARCH";
	static const string PLACEHOLDER = "Search inventory";
	// Hover tip on the magnifier in the EQUIPMENT bar.
	static const string OPEN_SEARCH_TIP = "Search";
	static const string CLOSE_SEARCH_TIP = "Close search";
	static const string FOUND_SUFFIX = " found";
	static const string NO_MATCH = "No matches";
	// "12 found, 3 filters": appended to the count while any category tile is on.
	static const string FILTERS_SEP = ", ";
	static const string FILTER_ONE = " filter";
	static const string FILTER_MANY = " filters";
	// Text button beside the count: clears every tile and the text.
	static const string RESET = "Reset";

	// Category tiles, in row order (shown as the tiles' hover names).
	static const string CAT_WEAPONS = "Weapons";
	static const string CAT_AMMO = "Ammo";
	static const string CAT_MAGAZINES = "Magazines";
	static const string CAT_MEDICAL = "Medical";
	static const string CAT_FOOD = "Food";
	static const string CAT_CLOTHING = "Clothing";
	static const string CAT_TOOLS = "Tools";
	static const string CAT_SUPPLIES = "Supplies";
}
