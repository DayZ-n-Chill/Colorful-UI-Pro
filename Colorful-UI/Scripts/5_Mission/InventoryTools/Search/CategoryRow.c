// The category tiles under the search box: Weapons, Ammo, Magazines, Medical, Food, Clothing,
// Tools, Supplies. One line of eight square icon-only buttons that together fill the card's inner
// width. Each toggles one SearchFilter.CAT_* bit; several can be on at once, and they stack with
// the typed text (SearchItemWords.Matches). The cells pick the change up through
// SearchFilter.Gen(), exactly as they do for typing.
//
// The name is not on the tile; hovering shows it in the shared tip (TileTip).
//
// Sizes are design px; every value goes through m_Scale (measured by the search card) on its
// way out, because script SetPos/SetSize take physical px.
class CategoryRow : ScriptedWidgetEventHandler
{
	static const string Row_Path = "Colorful-UI/GUI/layouts/inventory_tools/search/CategoryRow.layout";
	static const string Chip_Path = "Colorful-UI/GUI/layouts/inventory_tools/search/CategoryChip.layout";

	static const float TILE_GAP = 8;

	protected Widget m_Root;
	protected TileTip m_CatTip;
	protected float m_Scale = 1;
	// The tile side after the last layout, in design px; also the row's height.
	protected float m_Tile = 56;

	// One entry per tile, same index in every array.
	protected ref array<ButtonWidget>	m_Btns = new array<ButtonWidget>;
	protected ref array<Widget>			m_Fills = new array<Widget>;
	protected ref array<Widget>			m_Washes = new array<Widget>;
	protected ref array<ImageWidget>	m_Icons = new array<ImageWidget>;
	protected ref array<string>			m_Names = new array<string>;
	protected ref array<int>			m_Bits = new array<int>;

	protected int	m_ShownMask = -1;
	protected int	m_ShownHover = -2;
	protected int	m_Hover = -1;

	void CategoryRow(Widget v_Parent, float v_Scale, TileTip v_Tip)
	{
		if (v_Scale > 0)
			m_Scale = v_Scale;
		m_CatTip = v_Tip;

		m_Root = GetGame().GetWorkspace().CreateWidgets(Row_Path, v_Parent);
		if (!m_Root)
			return;

		// Deluxe's InventoryIcons sprites, written out in full so each name can be checked against
		// InventoryIcons.imageset.
		AddTile(SearchFilter.CAT_WEAPONS, SearchStrings.CAT_WEAPONS, "set:InventoryIcons image:Crosshair");
		AddTile(SearchFilter.CAT_AMMO, SearchStrings.CAT_AMMO, "set:InventoryIcons image:Coins");
		AddTile(SearchFilter.CAT_MAGAZINES, SearchStrings.CAT_MAGAZINES, "set:InventoryIcons image:BatteryFull");
		AddTile(SearchFilter.CAT_MEDICAL, SearchStrings.CAT_MEDICAL, "set:InventoryIcons image:FirstAid");
		AddTile(SearchFilter.CAT_FOOD, SearchStrings.CAT_FOOD, "set:InventoryIcons image:Hamburger");
		AddTile(SearchFilter.CAT_CLOTHING, SearchStrings.CAT_CLOTHING, "set:InventoryIcons image:TShirt");
		AddTile(SearchFilter.CAT_TOOLS, SearchStrings.CAT_TOOLS, "set:InventoryIcons image:Wrench");
		AddTile(SearchFilter.CAT_SUPPLIES, SearchStrings.CAT_SUPPLIES, "set:InventoryIcons image:Package");

		Paint();
	}

	void ~CategoryRow()
	{
		if (m_Root)
			m_Root.Unlink();
	}

	protected void AddTile(int v_Bit, string v_Name, string v_Image)
	{
		Widget v_W = GetGame().GetWorkspace().CreateWidgets(Chip_Path, m_Root);
		ButtonWidget v_Btn = ButtonWidget.Cast(v_W);
		if (!v_Btn)
		{
			if (v_W)
				v_W.Unlink();
			return;
		}

		ImageWidget v_Icon = ImageWidget.Cast(v_Btn.FindAnyWidget("CatIcon"));
		if (v_Icon)
		{
			v_Icon.LoadImageFile(0, v_Image);
			v_Icon.SetImage(0);
		}

		v_Btn.SetHandler(this);

		m_Btns.Insert(v_Btn);
		m_Fills.Insert(v_Btn.FindAnyWidget("CatFill"));
		m_Washes.Insert(v_Btn.FindAnyWidget("CatWash"));
		m_Icons.Insert(v_Icon);
		m_Names.Insert(v_Name);
		m_Bits.Insert(v_Bit);
	}

	// Where the row sits inside the card body and how wide it runs, in design px.
	void SetFrame(float v_X, float v_Y, float v_W)
	{
		if (!m_Root)
			return;

		int v_Count = m_Btns.Count();
		if (v_Count < 1)
			return;

		float v_F = m_Scale;
		m_Tile = (v_W - TILE_GAP * (v_Count - 1)) / v_Count;

		m_Root.SetPos(v_X * v_F, v_Y * v_F);
		m_Root.SetSize(v_W * v_F, m_Tile * v_F);

		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			float v_TX = m_Tile + TILE_GAP;
			v_TX *= v_I;
			m_Btns[v_I].SetPos(v_TX * v_F, 0);
			m_Btns[v_I].SetSize(m_Tile * v_F, m_Tile * v_F);
		}
	}

	float GetHeight()
	{
		return m_Tile;
	}

	// Every frame while the inventory is open. Tiles can change from outside the row (Reset) only
	// through SearchFilter, so this catches that too.
	void Update()
	{
		if (!m_Root)
			return;
		if (SearchFilter.Cats() != m_ShownMask || m_Hover != m_ShownHover)
			Paint();
	}

	// For when the row is hidden under the mouse and no leave event will come.
	void ClearHover()
	{
		m_Hover = -1;
	}

	// Off: dark translucent tile, muted icon. Hovered: a white wash and a full icon. On: the brand
	// colour at ~70 % with a full icon.
	protected void Paint()
	{
		int v_Mask = SearchFilter.Cats();
		m_ShownMask = v_Mask;
		m_ShownHover = m_Hover;

		int v_Brand = colorScheme.BrandColor();
		int v_OnFill = ARGB(179, (v_Brand >> 16) & 0xFF, (v_Brand >> 8) & 0xFF, v_Brand & 0xFF);
		int v_OffFill = ARGB(89, 0, 0, 0);
		int v_InkFull = ARGB(255, 220, 217, 207);
		int v_InkMuted = ARGB(153, 220, 217, 207);

		int v_Count = m_Btns.Count();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			bool v_On = (v_Mask & m_Bits[v_I]) != 0;
			bool v_Hot = v_I == m_Hover;

			int v_Fill = v_OffFill;
			int v_Color = v_InkMuted;
			if (v_On)
				v_Fill = v_OnFill;
			if (v_On || v_Hot)
				v_Color = v_InkFull;

			if (m_Fills[v_I])
				m_Fills[v_I].SetColor(v_Fill);
			if (m_Washes[v_I])
				m_Washes[v_I].Show(v_Hot);
			if (m_Icons[v_I])
				m_Icons[v_I].SetColor(v_Color);
		}
	}

	protected int IndexOf(Widget w)
	{
		if (!w)
			return -1;
		int v_Count = m_Btns.Count();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			if (m_Btns[v_I] == w)
				return v_I;
		}
		return -1;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int v_I = IndexOf(w);
		if (v_I < 0)
			return false;
		SearchFilter.ToggleCat(m_Bits[v_I]);
		Paint();
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int v_I = IndexOf(w);
		if (v_I < 0)
			return false;
		m_Hover = v_I;
		if (m_CatTip)
			m_CatTip.ShowFor(w, m_Names[v_I]);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int v_I = IndexOf(w);
		if (v_I < 0)
			return false;
		if (v_I == m_Hover)
			m_Hover = -1;
		if (m_CatTip)
			m_CatTip.HideFor(w);
		return false;
	}
}
