// The search state shared by every inventory cell. Lives in 3_Game so a lower layer (4_World) could
// read IsTyping() too; nothing above 3_Game is referenced here.
//
// Cells repaint only when Gen() moves on, so typing costs one repaint per cell per keystroke and an
// unchanged filter costs each cell a single int compare per frame.
//
// Search prefixed (not Filter) because the sort feature has its own category bits; the
// two copies must never collide.
class CUISearchFilter
{
	protected static string s_Raw;
	protected static ref array<string> s_Tokens = new array<string>;
	protected static int s_Gen;
	// Matches seen during the current InventoryMenu.Update. A map, not a counter, so an item drawn
	// twice (a worn backpack has both its equipment slot icon and its container) counts once.
	protected static ref map<EntityAI, bool> s_FrameHits = new map<EntityAI, bool>;
	protected static int s_Count;
	// True while the search box owns the keyboard.
	protected static bool s_Typing;

	// Category bits for the tile row under the search box. Each item's category is worked out in
	// SearchItemWords (5_Mission), because the item classes it tests live in 4_World.
	static const int CAT_WEAPONS	= 1;
	static const int CAT_AMMO		= 2;
	static const int CAT_MAGAZINES	= 4;
	static const int CAT_MEDICAL	= 8;
	static const int CAT_FOOD		= 16;
	static const int CAT_CLOTHING	= 32;
	static const int CAT_TOOLS		= 64;
	static const int CAT_SUPPLIES	= 128;
	static const int CAT_COUNT		= 8;

	// The selected tiles. Kept for the game session (closing the inventory does not clear them).
	protected static int s_Cats;

	// Lower-cased and split on spaces, so "9x19 ammo" matches an item whose words contain both.
	static void Set(string v_Raw)
	{
		string v_Clean = v_Raw.Trim();
		v_Clean.ToLower();
		if (v_Clean == s_Raw)
			return;

		s_Raw = v_Clean;
		s_Tokens.Clear();

		string v_Word = "";
		int v_Len = v_Clean.Length();
		for (int v_I = 0; v_I < v_Len; v_I++)
		{
			string v_Ch = v_Clean.Get(v_I);
			if (v_Ch == " ")
			{
				if (v_Word != "")
					s_Tokens.Insert(v_Word);
				v_Word = "";
			}
			else
			{
				v_Word += v_Ch;
			}
		}
		if (v_Word != "")
			s_Tokens.Insert(v_Word);

		s_Gen++;
	}

	// Active when there is text OR a tile is selected.
	static bool IsActive()
	{
		return s_Tokens.Count() > 0 || s_Cats != 0;
	}

	static bool HasText()
	{
		return s_Tokens.Count() > 0;
	}

	static int Cats()
	{
		return s_Cats;
	}

	static void SetCats(int v_Mask)
	{
		if (v_Mask == s_Cats)
			return;
		s_Cats = v_Mask;
		s_Gen++;
	}

	static void ToggleCat(int v_Bit)
	{
		SetCats(s_Cats ^ v_Bit);
	}

	static bool HasCat(int v_Bit)
	{
		return (s_Cats & v_Bit) != 0;
	}

	// OR across the tiles; ANDed with the text by SearchItemWords.Matches. None selected = all pass.
	static bool MatchesCat(int v_ItemCat)
	{
		if (s_Cats == 0)
			return true;
		return (s_Cats & v_ItemCat) != 0;
	}

	static int Gen()
	{
		return s_Gen;
	}

	static string Raw()
	{
		return s_Raw;
	}

	// v_Hay must already be lower case (SearchItemWords caches it that way).
	static bool MatchesHay(string v_Hay)
	{
		int v_Count = s_Tokens.Count();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			if (!v_Hay.Contains(s_Tokens[v_I]))
				return false;
		}
		return true;
	}

	static void BeginFrame()
	{
		s_FrameHits.Clear();
	}

	static void CountHit(EntityAI v_E)
	{
		if (v_E)
			s_FrameHits.Set(v_E, true);
	}

	// Always written, even with no hits, so a filter that matches nothing shows 0 and not the old count.
	static void EndFrame()
	{
		s_Count = s_FrameHits.Count();
	}

	static int Count()
	{
		return s_Count;
	}

	static void SetTyping(bool v_On)
	{
		s_Typing = v_On;
	}

	static bool IsTyping()
	{
		return s_Typing;
	}
}
