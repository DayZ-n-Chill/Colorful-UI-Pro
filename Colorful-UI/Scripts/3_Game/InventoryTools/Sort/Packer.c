// The sort's arithmetic, kept free of engine calls so it can be reasoned about (and SelfTest run)
// without an inventory. SortRunner (5_Mission) fills the items from a real cargo, asks Plan for
// the target layout, then asks NextMove for one move at a time against the live positions.
//
// Footprints are stored unflipped: m_W is columns and m_H is rows, the same order
// g_Game.GetInventoryItemSize returns them in. A flipped item swaps the two.
class PackItem
{
	int m_Key = -1;		// index into the runner's entity array; -1 for an anonymous reserved block
	int m_W;
	int m_H;
	bool m_CanFlip;		// vanilla only lets non-square items rotate
	bool m_Pinned;		// locked, reserved or not removable: planned around, never moved
	int m_Cat;			// Packer.CAT_* bit; lower packs first, so categories come out in the default order
	string m_Name;		// display name, lower case, for the name order
	string m_Group;		// type name, so stacks of the same item end up side by side
	int m_Row;
	int m_Col;
	bool m_Flip;
	int m_TRow;
	int m_TCol;
	bool m_TFlip;

	bool AtTarget()
	{
		return m_Row == m_TRow && m_Col == m_TCol && m_Flip == m_TFlip;
	}

	int Cols(bool v_Flip)
	{
		if (v_Flip)
			return m_H;
		return m_W;
	}

	int Rows(bool v_Flip)
	{
		if (v_Flip)
			return m_W;
		return m_H;
	}
}

class Packer
{
	static const int MOVE_DONE = -1;
	static const int MOVE_STUCK = -2;

	// Category bits, in the default packing order. Each item's category is decided in 5_Mission
	// (SortCategory), because the item classes it tests live in 4_World.
	static const int CAT_WEAPONS	= 1;
	static const int CAT_AMMO		= 2;
	static const int CAT_MAGAZINES	= 4;
	static const int CAT_MEDICAL	= 8;
	static const int CAT_FOOD		= 16;
	static const int CAT_CLOTHING	= 32;
	static const int CAT_TOOLS		= 64;
	static const int CAT_SUPPLIES	= 128;

	// ---- the order the player picked from the sort button's menu (SortMenu) ----
	static const int MODE_CATEGORY = 0;
	static const int MODE_SIZE = 2;
	static const int MODE_NAME = 3;
	// Which order, whether it runs the other way (size smallest first, name Z to A), and for the
	// category order which category goes first (a CAT_* bit, 0 for the default order).
	// Client-side statics: the choice lasts for the session.
	static int s_Mode = MODE_CATEGORY;
	static bool s_Reverse;
	static int s_CatFirst;

	// ---- ordering ----

	// Manual insertion sort: Enforce Script has no comparator sort for arrays of objects. Built
	// into a second array with InsertAt rather than swapping in place, so no element is ever
	// overwritten while a ref to it is in flight.
	static void Order(array<ref PackItem> v_Items)
	{
		array<ref PackItem> v_Sorted = new array<ref PackItem>;
		int v_Count = v_Items.Count();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			PackItem v_Item = v_Items[v_I];
			int v_At = v_Sorted.Count();
			for (int v_J = 0; v_J < v_Sorted.Count(); v_J++)
			{
				if (Before(v_Item, v_Sorted[v_J]))
				{
					v_At = v_J;
					break;
				}
			}
			v_Sorted.InsertAt(v_Item, v_At);
		}

		v_Items.Clear();
		for (int v_K = 0; v_K < v_Sorted.Count(); v_K++)
		{
			v_Items.Insert(v_Sorted[v_K]);
		}
	}

	// Largest first is what makes a greedy row-major pack come out gap-free in practice: the big
	// pieces claim the top-left and the 1x1s fill whatever is left, so size is always a late key.
	// First the player's chosen order (s_Mode): category (tab order, or one category pulled to the
	// front), size or name. The last keys keep the order stable, so sorting an already sorted
	// container plans no moves.
	protected static bool Before(PackItem v_A, PackItem v_B)
	{
		int v_Cmp;
		if (s_Mode == MODE_NAME)
		{
			v_Cmp = CompareText(v_A.m_Name, v_B.m_Name);
			if (v_Cmp != 0)
			{
				if (s_Reverse)
					return v_Cmp > 0;
				return v_Cmp < 0;
			}
		}
		else if (s_Mode == MODE_SIZE)
		{
			int v_SA = v_A.m_W * v_A.m_H;
			int v_SB = v_B.m_W * v_B.m_H;
			if (v_SA != v_SB)
			{
				if (s_Reverse)
					return v_SA < v_SB;
				return v_SA > v_SB;
			}
		}

		// Grouped by category, in the default order (weapons, ammo, magazines, medical, food,
		// clothing, tools, supplies) - the main key for the category order and the tie-break for
		// the others, so like items still sit together.
		int v_CA = CatKey(v_A.m_Cat);
		int v_CB = CatKey(v_B.m_Cat);
		if (v_CA != v_CB)
			return v_CA < v_CB;

		int v_AreaA = v_A.m_W * v_A.m_H;
		int v_AreaB = v_B.m_W * v_B.m_H;
		if (v_AreaA != v_AreaB)
			return v_AreaA > v_AreaB;

		int v_MaxA = Math.Max(v_A.m_W, v_A.m_H);
		int v_MaxB = Math.Max(v_B.m_W, v_B.m_H);
		if (v_MaxA != v_MaxB)
			return v_MaxA > v_MaxB;

		v_Cmp = CompareText(v_A.m_Group, v_B.m_Group);
		if (v_Cmp != 0)
			return v_Cmp < 0;

		if (v_A.m_Row != v_B.m_Row)
			return v_A.m_Row < v_B.m_Row;
		return v_A.m_Col < v_B.m_Col;
	}

	// The category's place in the order: the picked "first" category ahead of all others.
	protected static int CatKey(int v_Cat)
	{
		if (s_Mode == MODE_CATEGORY && s_CatFirst != 0 && v_Cat == s_CatFirst)
			return 0;
		return v_Cat;
	}

	// Character-code compare, because relational operators on strings are not something to rely on.
	// Returns <0, 0 or >0 like strcmp.
	protected static int CompareText(string v_A, string v_B)
	{
		int v_LenA = v_A.Length();
		int v_LenB = v_B.Length();
		int v_Len = Math.Min(v_LenA, v_LenB);
		for (int v_I = 0; v_I < v_Len; v_I++)
		{
			int v_CA = v_A.Get(v_I).ToAscii();
			int v_CB = v_B.Get(v_I).ToAscii();
			if (v_CA != v_CB)
				return v_CA - v_CB;
		}
		return v_LenA - v_LenB;
	}

	// ---- grid helpers ----

	// Occupancy holds item index + 1 per cell, 0 for free, so "free or held by me" is one compare.
	protected static void NewGrid(int v_Cols, int v_Rows, array<int> v_Occ)
	{
		v_Occ.Clear();
		int v_Size = v_Cols * v_Rows;
		for (int v_I = 0; v_I < v_Size; v_I++)
		{
			v_Occ.Insert(0);
		}
	}

	protected static void Stamp(int v_Cols, array<int> v_Occ, int v_Row, int v_Col, int v_W, int v_H, int v_Value)
	{
		for (int v_R = v_Row; v_R < v_Row + v_H; v_R++)
		{
			for (int v_C = v_Col; v_C < v_Col + v_W; v_C++)
			{
				v_Occ[v_R * v_Cols + v_C] = v_Value;
			}
		}
	}

	// Inside the grid, and every cell free or already held by v_Self (an index, or -1 for none).
	protected static bool Fits(int v_Cols, int v_Rows, array<int> v_Occ, int v_Row, int v_Col, int v_W, int v_H, int v_Self)
	{
		if (v_Row < 0 || v_Col < 0 || v_Row + v_H > v_Rows || v_Col + v_W > v_Cols)
			return false;

		for (int v_R = v_Row; v_R < v_Row + v_H; v_R++)
		{
			for (int v_C = v_Col; v_C < v_Col + v_W; v_C++)
			{
				int v_Cell = v_Occ[v_R * v_Cols + v_C];
				if (v_Cell != 0 && v_Cell != v_Self + 1)
					return false;
			}
		}
		return true;
	}

	protected static bool Overlap(int v_R1, int v_C1, int v_W1, int v_H1, int v_R2, int v_C2, int v_W2, int v_H2)
	{
		if (v_C1 + v_W1 <= v_C2 || v_C2 + v_W2 <= v_C1)
			return false;
		if (v_R1 + v_H1 <= v_R2 || v_R2 + v_H2 <= v_R1)
			return false;
		return true;
	}

	// True when the rectangle touches the target of any item that still has to move (other than
	// v_Skip). Staging a blocker outside all of them avoids having to move it twice.
	protected static bool HitsPendingTarget(array<ref PackItem> v_Items, int v_Skip, int v_Row, int v_Col, int v_W, int v_H)
	{
		for (int v_I = 0; v_I < v_Items.Count(); v_I++)
		{
			if (v_I == v_Skip)
				continue;
			PackItem v_P = v_Items[v_I];
			if (v_P.m_Pinned || v_P.AtTarget())
				continue;
			if (Overlap(v_Row, v_Col, v_W, v_H, v_P.m_TRow, v_P.m_TCol, v_P.Cols(v_P.m_TFlip), v_P.Rows(v_P.m_TFlip)))
				return true;
		}
		return false;
	}

	// ---- planning ----

	// Writes every item's m_T* target. Items must already be in Order. Pinned items keep their
	// current spot and everything else is packed row by row around them. Returns false if any item
	// cannot be placed, in which case the caller makes no moves at all.
	static bool Plan(int v_Cols, int v_Rows, array<ref PackItem> v_Items)
	{
		array<int> v_Occ = new array<int>;
		NewGrid(v_Cols, v_Rows, v_Occ);

		int v_Count = v_Items.Count();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			PackItem v_Pin = v_Items[v_I];
			if (!v_Pin.m_Pinned)
				continue;
			v_Pin.m_TRow = v_Pin.m_Row;
			v_Pin.m_TCol = v_Pin.m_Col;
			v_Pin.m_TFlip = v_Pin.m_Flip;
			Stamp(v_Cols, v_Occ, v_Pin.m_Row, v_Pin.m_Col, v_Pin.Cols(v_Pin.m_Flip), v_Pin.Rows(v_Pin.m_Flip), v_I + 1);
		}

		for (int v_J = 0; v_J < v_Count; v_J++)
		{
			PackItem v_Item = v_Items[v_J];
			if (v_Item.m_Pinned)
				continue;

			// Landscape first: a wide piece lying flat leaves full-width rows below it.
			bool v_First = v_Item.m_CanFlip && v_Item.m_H > v_Item.m_W;
			bool v_Placed = false;
			for (int v_R = 0; v_R < v_Rows && !v_Placed; v_R++)
			{
				for (int v_C = 0; v_C < v_Cols && !v_Placed; v_C++)
				{
					if (Fits(v_Cols, v_Rows, v_Occ, v_R, v_C, v_Item.Cols(v_First), v_Item.Rows(v_First), -1))
					{
						v_Item.m_TFlip = v_First;
						v_Placed = true;
					}
					else if (v_Item.m_CanFlip && Fits(v_Cols, v_Rows, v_Occ, v_R, v_C, v_Item.Cols(!v_First), v_Item.Rows(!v_First), -1))
					{
						v_Item.m_TFlip = !v_First;
						v_Placed = true;
					}

					if (v_Placed)
					{
						v_Item.m_TRow = v_R;
						v_Item.m_TCol = v_C;
						Stamp(v_Cols, v_Occ, v_R, v_C, v_Item.Cols(v_Item.m_TFlip), v_Item.Rows(v_Item.m_TFlip), v_J + 1);
					}
				}
			}

			if (!v_Placed)
				return false;
		}
		return true;
	}

	static bool IsSorted(array<ref PackItem> v_Items)
	{
		for (int v_I = 0; v_I < v_Items.Count(); v_I++)
		{
			if (!v_Items[v_I].AtTarget())
				return false;
		}
		return true;
	}

	// The next single move, worked out from the items' live m_Row/m_Col/m_Flip (the runner copies
	// them in from the real cargo before every call, so a move the engine adjusted is seen as it is).
	// Returns the item's index with its destination, MOVE_DONE when everything is at its target, or
	// MOVE_STUCK when nothing can move. v_SelfOk: whether a destination may overlap the item's own
	// current cells (false once the engine has refused such a move).
	static int NextMove(int v_Cols, int v_Rows, array<ref PackItem> v_Items, out int v_Row, out int v_Col, out bool v_Flip, bool v_SelfOk = true)
	{
		array<int> v_Occ = new array<int>;
		LiveGrid(v_Cols, v_Rows, v_Items, v_Occ);

		// 1. Anything whose target is already clear (or only covered by itself) goes straight there.
		bool v_Any = false;
		int v_Count = v_Items.Count();
		for (int v_J = 0; v_J < v_Count; v_J++)
		{
			PackItem v_Item = v_Items[v_J];
			if (v_Item.m_Pinned || v_Item.AtTarget())
				continue;
			v_Any = true;
			if (Fits(v_Cols, v_Rows, v_Occ, v_Item.m_TRow, v_Item.m_TCol, v_Item.Cols(v_Item.m_TFlip), v_Item.Rows(v_Item.m_TFlip), SelfFor(v_J, v_SelfOk)))
			{
				v_Row = v_Item.m_TRow;
				v_Col = v_Item.m_TCol;
				v_Flip = v_Item.m_TFlip;
				return v_J;
			}
		}
		if (!v_Any)
			return MOVE_DONE;

		// 2. Every pending target is blocked. Try every pending item's every blocker until one can be
		// staged out of the way, from the bottom row up (the end of the packed layout is where the
		// free space ends up). Pass 0 keeps clear of every pending target; pass 1 settles for keeping
		// clear of the one being freed.
		for (int v_Pass = 0; v_Pass < 2; v_Pass++)
		{
			for (int v_P = 0; v_P < v_Count; v_P++)
			{
				PackItem v_Pend = v_Items[v_P];
				if (v_Pend.m_Pinned || v_Pend.AtTarget())
					continue;
				int v_PW = v_Pend.Cols(v_Pend.m_TFlip);
				int v_PH = v_Pend.Rows(v_Pend.m_TFlip);
				for (int v_R = v_Pend.m_TRow; v_R < v_Pend.m_TRow + v_PH; v_R++)
				{
					for (int v_C = v_Pend.m_TCol; v_C < v_Pend.m_TCol + v_PW; v_C++)
					{
						int v_Cell = v_Occ[v_R * v_Cols + v_C] - 1;
						if (v_Cell < 0 || v_Items[v_Cell].m_Pinned)
							continue;
						if (v_Cell == v_P && v_SelfOk)
							continue;
						if (Stage(v_Cols, v_Rows, v_Items, v_Occ, v_Cell, v_Pend, v_Pass, v_SelfOk, v_Row, v_Col, v_Flip))
							return v_Cell;
					}
				}
			}
		}

		// 3. Nowhere to put any blocker.
		return MOVE_STUCK;
	}

	// A spot for item v_B that is off v_Pend's target (and, on pass 0, off every pending target).
	protected static bool Stage(int v_Cols, int v_Rows, array<ref PackItem> v_Items, array<int> v_Occ, int v_B, PackItem v_Pend, int v_Pass, bool v_SelfOk, out int v_Row, out int v_Col, out bool v_Flip)
	{
		PackItem v_Blk = v_Items[v_B];
		int v_PW = v_Pend.Cols(v_Pend.m_TFlip);
		int v_PH = v_Pend.Rows(v_Pend.m_TFlip);
		for (int v_SR = v_Rows - 1; v_SR >= 0; v_SR--)
		{
			for (int v_SC = 0; v_SC < v_Cols; v_SC++)
			{
				for (int v_O = 0; v_O < 2; v_O++)
				{
					bool v_SFlip = v_Blk.m_Flip;
					if (v_O == 1)
					{
						if (!v_Blk.m_CanFlip)
							continue;
						v_SFlip = !v_Blk.m_Flip;
					}
					int v_SW = v_Blk.Cols(v_SFlip);
					int v_SH = v_Blk.Rows(v_SFlip);
					if (v_SR == v_Blk.m_Row && v_SC == v_Blk.m_Col && v_SFlip == v_Blk.m_Flip)
						continue;
					if (!Fits(v_Cols, v_Rows, v_Occ, v_SR, v_SC, v_SW, v_SH, SelfFor(v_B, v_SelfOk)))
						continue;
					if (Overlap(v_SR, v_SC, v_SW, v_SH, v_Pend.m_TRow, v_Pend.m_TCol, v_PW, v_PH))
						continue;
					if (v_Pass == 0 && HitsPendingTarget(v_Items, v_B, v_SR, v_SC, v_SW, v_SH))
						continue;
					v_Row = v_SR;
					v_Col = v_SC;
					v_Flip = v_SFlip;
					return true;
				}
			}
		}
		return false;
	}

	// The fallback when the planned layout cannot be reached in a crowded cargo: one move of the
	// first item, in reading order, that fits somewhere earlier in reading order (top-left first,
	// either way round). Repeated until nothing can move, it packs everything up into the top left
	// without needing any free space to shuffle through. Returns the item or MOVE_DONE.
	static int NextGravity(int v_Cols, int v_Rows, array<ref PackItem> v_Items, out int v_Row, out int v_Col, out bool v_Flip, bool v_SelfOk = true)
	{
		array<int> v_Occ = new array<int>;
		LiveGrid(v_Cols, v_Rows, v_Items, v_Occ);

		// Reading order of the items' current spots, walked straight off the grid.
		array<int> v_Seen = new array<int>;
		int v_Cells = v_Cols * v_Rows;
		for (int v_I = 0; v_I < v_Cells; v_I++)
		{
			int v_K = v_Occ[v_I] - 1;
			if (v_K < 0 || v_Seen.Find(v_K) >= 0)
				continue;
			v_Seen.Insert(v_K);
			PackItem v_It = v_Items[v_K];
			if (v_It.m_Pinned)
				continue;
			int v_Here = v_It.m_Row * v_Cols + v_It.m_Col;
			for (int v_At = 0; v_At < v_Here; v_At++)
			{
				int v_R = v_At / v_Cols;
				int v_C = v_At % v_Cols;
				for (int v_O = 0; v_O < 2; v_O++)
				{
					bool v_F = v_It.m_Flip;
					if (v_O == 1)
					{
						if (!v_It.m_CanFlip)
							continue;
						v_F = !v_It.m_Flip;
					}
					if (Fits(v_Cols, v_Rows, v_Occ, v_R, v_C, v_It.Cols(v_F), v_It.Rows(v_F), SelfFor(v_K, v_SelfOk)))
					{
						v_Row = v_R;
						v_Col = v_C;
						v_Flip = v_F;
						return v_K;
					}
				}
			}
		}
		return MOVE_DONE;
	}

	protected static void LiveGrid(int v_Cols, int v_Rows, array<ref PackItem> v_Items, array<int> v_Occ)
	{
		NewGrid(v_Cols, v_Rows, v_Occ);
		for (int v_I = 0; v_I < v_Items.Count(); v_I++)
		{
			PackItem v_Live = v_Items[v_I];
			Stamp(v_Cols, v_Occ, v_Live.m_Row, v_Live.m_Col, v_Live.Cols(v_Live.m_Flip), v_Live.Rows(v_Live.m_Flip), v_I + 1);
		}
	}

	protected static int SelfFor(int v_Index, bool v_SelfOk)
	{
		if (v_SelfOk)
			return v_Index;
		return -1;
	}

	// Plays NextMove out on a copy of the items, without touching the inventory. True when the
	// planned layout is reached inside v_Cap moves. The runner asks this before making any real move,
	// so a crowded cargo where staging would only shuffle items back and forth goes straight to the
	// top-left pack instead of visibly trying first.
	static bool Reaches(int v_Cols, int v_Rows, array<ref PackItem> v_Items, int v_Cap)
	{
		array<ref PackItem> v_Sim = new array<ref PackItem>;
		for (int v_I = 0; v_I < v_Items.Count(); v_I++)
		{
			PackItem v_Src = v_Items[v_I];
			PackItem v_Copy = new PackItem;
			v_Copy.m_Key = v_Src.m_Key;
			v_Copy.m_W = v_Src.m_W;
			v_Copy.m_H = v_Src.m_H;
			v_Copy.m_CanFlip = v_Src.m_CanFlip;
			v_Copy.m_Pinned = v_Src.m_Pinned;
			v_Copy.m_Cat = v_Src.m_Cat;
			v_Copy.m_Name = v_Src.m_Name;
			v_Copy.m_Group = v_Src.m_Group;
			v_Copy.m_Row = v_Src.m_Row;
			v_Copy.m_Col = v_Src.m_Col;
			v_Copy.m_Flip = v_Src.m_Flip;
			v_Copy.m_TRow = v_Src.m_TRow;
			v_Copy.m_TCol = v_Src.m_TCol;
			v_Copy.m_TFlip = v_Src.m_TFlip;
			v_Sim.Insert(v_Copy);
		}
		for (int v_M = 0; v_M < v_Cap; v_M++)
		{
			int v_Row, v_Col;
			bool v_Flip;
			int v_K = NextMove(v_Cols, v_Rows, v_Sim, v_Row, v_Col, v_Flip);
			if (v_K == MOVE_DONE)
				return true;
			if (v_K == MOVE_STUCK)
				return false;
			v_Sim[v_K].m_Row = v_Row;
			v_Sim[v_K].m_Col = v_Col;
			v_Sim[v_K].m_Flip = v_Flip;
		}
		return false;
	}

	// ---- test ----

	// Not called by anything. Run from the script console (Packer.SelfTest();) after changing
	// the packer: packs a scrambled 6x4 grid, replays NextMove to the end and checks the result.
	static void SelfTest()
	{
		array<ref PackItem> v_Items = new array<ref PackItem>;
		// key, w, h, row, col, flip, pinned, group
		v_Items.Insert(TestItem(0, 1, 1, 0, 0, false, false, "Apple"));
		v_Items.Insert(TestItem(1, 2, 3, 0, 4, false, false, "Rifle"));
		v_Items.Insert(TestItem(2, 1, 1, 3, 0, false, false, "Apple"));
		v_Items.Insert(TestItem(3, 2, 1, 2, 1, false, false, "Mag"));
		v_Items.Insert(TestItem(4, 1, 2, 0, 2, false, false, "Can"));
		v_Items.Insert(TestItem(5, 2, 2, 2, 2, false, false, "Box"));
		v_Items.Insert(TestItem(6, 1, 1, 3, 5, false, true, "Locked"));

		int v_Cols = 6;
		int v_Rows = 4;
		Order(v_Items);
		if (!Plan(v_Cols, v_Rows, v_Items))
		{
			Print("[Packer] SelfTest FAIL: Plan found no layout");
			return;
		}

		int v_Moves = 0;
		int v_Cap = 4 * v_Items.Count();
		while (v_Moves < v_Cap)
		{
			int v_Row;
			int v_Col;
			bool v_Flip;
			int v_K = NextMove(v_Cols, v_Rows, v_Items, v_Row, v_Col, v_Flip);
			if (v_K == MOVE_DONE)
				break;
			if (v_K == MOVE_STUCK)
			{
				Print("[Packer] SelfTest FAIL: stuck after " + v_Moves + " moves");
				return;
			}
			PackItem v_M = v_Items[v_K];
			if (v_M.m_Pinned)
			{
				Print("[Packer] SelfTest FAIL: tried to move a pinned item");
				return;
			}
			v_M.m_Row = v_Row;
			v_M.m_Col = v_Col;
			v_M.m_Flip = v_Flip;
			v_Moves++;
		}

		if (!IsSorted(v_Items))
		{
			Print("[Packer] SelfTest FAIL: not sorted after " + v_Moves + " moves");
			return;
		}

		// Final layout must not overlap anywhere.
		array<int> v_Occ = new array<int>;
		NewGrid(v_Cols, v_Rows, v_Occ);
		for (int v_I = 0; v_I < v_Items.Count(); v_I++)
		{
			PackItem v_F = v_Items[v_I];
			if (!Fits(v_Cols, v_Rows, v_Occ, v_F.m_Row, v_F.m_Col, v_F.Cols(v_F.m_Flip), v_F.Rows(v_F.m_Flip), -1))
			{
				Print("[Packer] SelfTest FAIL: overlap in final layout");
				return;
			}
			Stamp(v_Cols, v_Occ, v_F.m_Row, v_F.m_Col, v_F.Cols(v_F.m_Flip), v_F.Rows(v_F.m_Flip), v_I + 1);
		}

		Print("[Packer] SelfTest PASS in " + v_Moves + " moves");
	}

	protected static PackItem TestItem(int v_Key, int v_W, int v_H, int v_Row, int v_Col, bool v_Flip, bool v_Pinned, string v_Group)
	{
		PackItem v_Item = new PackItem;
		v_Item.m_Key = v_Key;
		v_Item.m_W = v_W;
		v_Item.m_H = v_H;
		v_Item.m_CanFlip = v_W != v_H;
		v_Item.m_Pinned = v_Pinned;
		v_Item.m_Group = v_Group;
		v_Item.m_Row = v_Row;
		v_Item.m_Col = v_Col;
		v_Item.m_Flip = v_Flip;
		return v_Item;
	}
}
