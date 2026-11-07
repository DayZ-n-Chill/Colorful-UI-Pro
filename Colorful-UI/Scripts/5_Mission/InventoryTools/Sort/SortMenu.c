// The sort button's drop-down. A click on a container's sort button opens "SORT BY" under it with
// three orders - Category, Size, Name - and hovering one opens its second level beside it
// (which category first; largest or smallest; A to Z or Z to A).
// Picking a second-level row sets Packer's order and sorts that container at once. The order
// in use is ticked, and stays picked for the session, so Sort all and later sorts use it too.
//
// The stack button next to it is the one-click "auto sort": it stacks, then sorts in this order.
//
// Both levels are SortMenu.layout panels under the inventory menu's root, so they draw above
// every card; rows are SortMenuRow.layout, placed by script. Ticked every frame from
// InventoryMenu.Update (SortHooks.c): a mouse press anywhere outside the menu, Escape, or the
// inventory closing shuts it.
class SmEntry
{
	string	m_Label;
	int		m_Mode;
	bool	m_Reverse;
	int		m_CatFirst;
	// Index into the first level's entries for a second-level row; -1 for a first-level row.
	int		m_Parent = -1;
}

class SortMenu : ScriptedWidgetEventHandler
{
	static const string Menu_Path = "Colorful-UI/GUI/layouts/inventory_tools/sort/SortMenu.layout";
	static const string Row_Path = "Colorful-UI/GUI/layouts/inventory_tools/sort/SortMenuRow.layout";

	// Design px: the row's authored height, the panel's padding top and bottom, the gap between
	// the button and the menu, and between the two levels.
	static const float ROW_H = 26;
	static const float PAD = 5;
	static const float GAP = 4;

	protected static ref SortMenu s_Menu;

	protected EntityAI			m_SmOwner;
	protected int				m_SmIdx;
	// Raw on purpose: SortButton is Managed, so this goes null if the header is destroyed.
	protected SortButton	m_SmBtn;
	protected Widget			m_SmAnchor;

	protected Widget			m_SmLevel1;
	protected Widget			m_SmLevel2;
	protected ref array<Widget>			m_SmRows1 = new array<Widget>;
	protected ref array<Widget>			m_SmRows2 = new array<Widget>;
	protected ref array<ref SmEntry>	m_SmTop = new array<ref SmEntry>;
	protected ref array<ref SmEntry>	m_SmSub = new array<ref SmEntry>;
	protected int				m_SmOpenTop = -1;
	protected float				m_SmScale = 1;
	protected bool				m_SmMouseWasDown = true;

	// ---- statics ----------------------------------------------------------------------------

	static void Open(EntityAI v_Owner, int v_Idx, SortButton v_Btn)
	{
		Close();
		if (!v_Owner || !v_Btn || !v_Btn.GetButton())
			return;
		s_Menu = new SortMenu;
		if (!s_Menu.Build(v_Owner, v_Idx, v_Btn))
			Close();
	}

	static void Close()
	{
		s_Menu = null;
	}

	// From inside the menu's own handlers: the menu must not be destroyed while one of its methods
	// is still running, so the close waits for the next GUI tick.
	protected static void CloseLater()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(SortMenu.Close);
	}

	static bool IsOpen()
	{
		return s_Menu != null;
	}

	static void Tick()
	{
		if (s_Menu)
			s_Menu.Update();
	}

	// ---- instance ---------------------------------------------------------------------------

	void ~SortMenu()
	{
		if (m_SmLevel1)
			m_SmLevel1.Unlink();
		if (m_SmLevel2)
			m_SmLevel2.Unlink();
	}

	protected bool Build(EntityAI v_Owner, int v_Idx, SortButton v_Btn)
	{
		m_SmOwner = v_Owner;
		m_SmIdx = v_Idx;
		m_SmBtn = v_Btn;
		m_SmAnchor = v_Btn.GetButton();

		UIScriptedMenu v_Inv = GetGame().GetUIManager().FindMenu(MENU_INVENTORY);
		if (!v_Inv || !v_Inv.GetLayoutRoot())
			return false;
		Widget v_Root = v_Inv.GetLayoutRoot();
		m_SmLevel1 = GetGame().GetWorkspace().CreateWidgets(Menu_Path, v_Root);
		m_SmLevel2 = GetGame().GetWorkspace().CreateWidgets(Menu_Path, v_Root);
		if (!m_SmLevel1 || !m_SmLevel2)
			return false;

		AddTop(SortStrings.SORTBY_CATEGORY, Packer.MODE_CATEGORY);
		AddTop(SortStrings.SORTBY_SIZE, Packer.MODE_SIZE);
		AddTop(SortStrings.SORTBY_NAME, Packer.MODE_NAME);

		// The heading, then one row per order.
		Widget v_Head = AddRow(m_SmLevel1, m_SmRows1, SortStrings.SORTBY_TITLE, false);
		if (!v_Head)
			return false;
		// The scale the engine gave the authored row: everything else is placed in the same units.
		float v_RW, v_RH;
		v_Head.GetSize(v_RW, v_RH);
		if (v_RH > 0)
			m_SmScale = v_RH / ROW_H;
		TextWidget v_HeadText = TextWidget.Cast(v_Head.FindAnyWidget("SmText"));
		if (v_HeadText)
			v_HeadText.SetColor(ARGB(255, 169, 168, 160));

		for (int v_I = 0; v_I < m_SmTop.Count(); v_I++)
		{
			Widget v_Row = AddRow(m_SmLevel1, m_SmRows1, m_SmTop[v_I].m_Label, true);
			Mark(v_Row, true, m_SmTop[v_I].m_Mode == Packer.s_Mode);
		}
		Fit(m_SmLevel1, m_SmRows1);

		// Under the button, its right edge on the button's right edge; above it if the screen ends.
		float v_BX, v_BY, v_BW, v_BH;
		m_SmAnchor.GetScreenPos(v_BX, v_BY);
		m_SmAnchor.GetScreenSize(v_BW, v_BH);
		float v_MW, v_MH;
		m_SmLevel1.GetScreenSize(v_MW, v_MH);
		int v_SW, v_SH;
		GetScreenSize(v_SW, v_SH);
		float v_X = v_BX + v_BW - v_MW;
		float v_Y = v_BY + v_BH + GAP * m_SmScale;
		if (v_Y + v_MH > v_SH - 8)
			v_Y = v_BY - GAP * m_SmScale - v_MH;
		if (v_X < 8)
			v_X = 8;
		PlaceOnScreen(m_SmLevel1, v_X, v_Y);
		m_SmLevel1.Show(true);
		m_SmLevel2.Show(false);
		return true;
	}

	protected void AddTop(string v_Label, int v_Mode)
	{
		SmEntry v_E = new SmEntry;
		v_E.m_Label = v_Label;
		v_E.m_Mode = v_Mode;
		m_SmTop.Insert(v_E);
	}

	protected void AddSub(int v_Parent, string v_Label, bool v_Reverse, int v_CatFirst)
	{
		SmEntry v_E = new SmEntry;
		v_E.m_Label = v_Label;
		v_E.m_Mode = m_SmTop[v_Parent].m_Mode;
		v_E.m_Reverse = v_Reverse;
		v_E.m_CatFirst = v_CatFirst;
		v_E.m_Parent = v_Parent;
		m_SmSub.Insert(v_E);
	}

	protected Widget AddRow(Widget v_Panel, array<Widget> v_Rows, string v_Label, bool v_Active)
	{
		Widget v_Row = GetGame().GetWorkspace().CreateWidgets(Row_Path, v_Panel);
		if (!v_Row)
			return null;
		TextWidget v_Text = TextWidget.Cast(v_Row.FindAnyWidget("SmText"));
		if (v_Text)
			v_Text.SetText(v_Label);
		// The hover wash is the theme's primary colour, at the same soft strength as the header
		// buttons' hover (SortButton.ApplyHover).
		Widget v_Wash = v_Row.FindAnyWidget("SmWash");
		if (v_Wash)
			v_Wash.SetColor((colorScheme.BrandColor() & 0x00FFFFFF) | 0x2E000000);
		if (v_Active)
			v_Row.SetHandler(this);
		else
			v_Row.SetFlags(WidgetFlags.IGNOREPOINTER);
		v_Rows.Insert(v_Row);
		return v_Row;
	}

	// The caret on a first-level row, the tick on the row in use.
	protected void Mark(Widget v_Row, bool v_Caret, bool v_Ticked)
	{
		if (!v_Row)
			return;
		Widget v_C = v_Row.FindAnyWidget("SmCaret");
		if (v_C)
			v_C.Show(v_Caret && !v_Ticked);
		Widget v_K = v_Row.FindAnyWidget("SmCheck");
		if (v_K)
		{
			v_K.Show(v_Ticked);
			if (v_Ticked)
				v_K.SetColor(colorScheme.BrandColor());
		}
	}

	// Rows stacked from the top with the panel's padding, and the panel sized to them.
	protected void Fit(Widget v_Panel, array<Widget> v_Rows)
	{
		float v_S = m_SmScale;
		float v_Y = PAD * v_S;
		float v_RW = 190 * v_S;
		for (int v_I = 0; v_I < v_Rows.Count(); v_I++)
		{
			if (!v_Rows[v_I])
				continue;
			v_Rows[v_I].SetPos(0, v_Y);
			float v_W, v_H;
			v_Rows[v_I].GetSize(v_W, v_H);
			v_RW = v_W;
			v_Y += v_H;
		}
		v_Panel.SetSize(v_RW, v_Y + PAD * v_S);
	}

	protected void PlaceOnScreen(Widget v_Panel, float v_X, float v_Y)
	{
		float v_PX = 0;
		float v_PY = 0;
		Widget v_Parent = v_Panel.GetParent();
		if (v_Parent)
			v_Parent.GetScreenPos(v_PX, v_PY);
		v_Panel.SetPos(v_X - v_PX, v_Y - v_PY);
	}

	// The second level for first-level entry v_Top, beside its row.
	protected void OpenSub(int v_Top)
	{
		if (v_Top == m_SmOpenTop)
			return;
		m_SmOpenTop = v_Top;
		for (int v_R = 0; v_R < m_SmRows2.Count(); v_R++)
		{
			if (m_SmRows2[v_R])
				m_SmRows2[v_R].Unlink();
		}
		m_SmRows2.Clear();
		m_SmSub.Clear();

		int v_Mode = m_SmTop[v_Top].m_Mode;
		if (v_Mode == Packer.MODE_CATEGORY)
		{
			AddSub(v_Top, SortStrings.SORTBY_TAB_ORDER, false, 0);
			AddSub(v_Top, SortStrings.CAT_WEAPONS + SortStrings.SORTBY_FIRST, false, Packer.CAT_WEAPONS);
			AddSub(v_Top, SortStrings.CAT_AMMO + SortStrings.SORTBY_FIRST, false, Packer.CAT_AMMO);
			AddSub(v_Top, SortStrings.CAT_MAGAZINES + SortStrings.SORTBY_FIRST, false, Packer.CAT_MAGAZINES);
			AddSub(v_Top, SortStrings.CAT_MEDICAL + SortStrings.SORTBY_FIRST, false, Packer.CAT_MEDICAL);
			AddSub(v_Top, SortStrings.CAT_FOOD + SortStrings.SORTBY_FIRST, false, Packer.CAT_FOOD);
			AddSub(v_Top, SortStrings.CAT_CLOTHING + SortStrings.SORTBY_FIRST, false, Packer.CAT_CLOTHING);
			AddSub(v_Top, SortStrings.CAT_TOOLS + SortStrings.SORTBY_FIRST, false, Packer.CAT_TOOLS);
			AddSub(v_Top, SortStrings.CAT_SUPPLIES + SortStrings.SORTBY_FIRST, false, Packer.CAT_SUPPLIES);
		}
		else if (v_Mode == Packer.MODE_SIZE)
		{
			AddSub(v_Top, SortStrings.SORTBY_SIZE_BIG, false, 0);
			AddSub(v_Top, SortStrings.SORTBY_SIZE_SMALL, true, 0);
		}
		else
		{
			AddSub(v_Top, SortStrings.SORTBY_NAME_AZ, false, 0);
			AddSub(v_Top, SortStrings.SORTBY_NAME_ZA, true, 0);
		}

		for (int v_I = 0; v_I < m_SmSub.Count(); v_I++)
		{
			SmEntry v_E = m_SmSub[v_I];
			Widget v_Row = AddRow(m_SmLevel2, m_SmRows2, v_E.m_Label, true);
			bool v_InUse = v_E.m_Mode == Packer.s_Mode && v_E.m_Reverse == Packer.s_Reverse;
			if (v_E.m_Mode == Packer.MODE_CATEGORY)
				v_InUse = v_E.m_Mode == Packer.s_Mode && v_E.m_CatFirst == Packer.s_CatFirst;
			Mark(v_Row, false, v_InUse);
		}
		Fit(m_SmLevel2, m_SmRows2);

		// Beside the hovered row, its top on the row's top; on the left if the screen ends.
		Widget v_TopRow = m_SmRows1[v_Top + 1];
		float v_RX, v_RY, v_RW, v_RH;
		v_TopRow.GetScreenPos(v_RX, v_RY);
		v_TopRow.GetScreenSize(v_RW, v_RH);
		float v_L1X, v_L1Y, v_L1W, v_L1H;
		m_SmLevel1.GetScreenPos(v_L1X, v_L1Y);
		m_SmLevel1.GetScreenSize(v_L1W, v_L1H);
		float v_MW, v_MH;
		m_SmLevel2.GetScreenSize(v_MW, v_MH);
		int v_SW, v_SH;
		GetScreenSize(v_SW, v_SH);
		float v_X = v_L1X + v_L1W + GAP * m_SmScale;
		if (v_X + v_MW > v_SW - 8)
			v_X = v_L1X - GAP * m_SmScale - v_MW;
		float v_Y = v_RY - PAD * m_SmScale;
		if (v_Y + v_MH > v_SH - 8)
			v_Y = v_SH - 8 - v_MH;
		PlaceOnScreen(m_SmLevel2, v_X, v_Y);
		m_SmLevel2.Show(true);
	}

	protected void Pick(SmEntry v_E)
	{
		Packer.s_Mode = v_E.m_Mode;
		Packer.s_Reverse = v_E.m_Reverse;
		Packer.s_CatFirst = v_E.m_CatFirst;
		EntityAI v_Owner = m_SmOwner;
		int v_Idx = m_SmIdx;
		SortButton v_Btn = m_SmBtn;
		CloseLater();
		SortRunner.Start(v_Owner, v_Idx, v_Btn);
	}

	// A press outside both levels, or Escape, closes the menu. The press that opened it is still
	// held on the first frame, so a press only counts once the mouse has been seen up.
	protected void Update()
	{
		if (KeyState(KeyCode.KC_ESCAPE) != 0 || !m_SmAnchor || !m_SmAnchor.IsVisibleHierarchy())
		{
			CloseLater();
			return;
		}
		bool v_Down = (GetMouseState(MouseState.LEFT) & 0x80000000) != 0 || (GetMouseState(MouseState.RIGHT) & 0x80000000) != 0;
		if (v_Down && !m_SmMouseWasDown && !IsInside(GetWidgetUnderCursor()))
		{
			CloseLater();
			return;
		}
		m_SmMouseWasDown = v_Down;
	}

	protected bool IsInside(Widget v_W)
	{
		while (v_W)
		{
			if (v_W == m_SmLevel1 || v_W == m_SmLevel2)
				return true;
			v_W = v_W.GetParent();
		}
		return false;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		Widget v_Wash = w.FindAnyWidget("SmWash");
		if (v_Wash)
			v_Wash.Show(true);
		int v_Top = m_SmRows1.Find(w);
		if (v_Top >= 1)
			OpenSub(v_Top - 1);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		Widget v_Wash = w.FindAnyWidget("SmWash");
		if (v_Wash)
			v_Wash.Show(false);
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int v_Sub = m_SmRows2.Find(w);
		if (v_Sub >= 0 && v_Sub < m_SmSub.Count())
		{
			Pick(m_SmSub[v_Sub]);
			return true;
		}
		int v_Top = m_SmRows1.Find(w);
		if (v_Top >= 1)
		{
			OpenSub(v_Top - 1);
			return true;
		}
		return false;
	}
}
