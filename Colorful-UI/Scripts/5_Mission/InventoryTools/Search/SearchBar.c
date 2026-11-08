// The inventory search, as the SEARCH tab of the EQUIPMENT card. The magnifier at the right-hand end
// of the EQUIPMENT bar swaps the card's body between the equipment slot icons (the EQUIPMENT tab)
// and the search content (the SEARCH tab: the field, then one row of square category tiles); the
// bar's title follows the tab. It owns the text and the keyboard; the dimming itself is done by every
// cell on its own (SearchIconHooks.c) against the shared SearchFilter.
//
// Built once per InventoryMenu and kept for the menu's life.
//
// ---- where it lives -------------------------------------------------------------------------------
// The right column is vanilla's right_area.layout (narrow, medium or wide; Colorful UI does not
// redirect it). Its top is a WrapSpacer named "Header" holding SlotsHeader (the EQUIPMENT bar, moved
// in by PlayerContainer) and ScrollerSlotsContent (the equipment slot band). The search body is
// appended to that same WrapSpacer, and exactly one of the two bodies is shown at a time.
//
// The clothing Scroller under it is a fixed 77 % of the column anchored to the bottom, so FitScroller
// resizes it to whatever the top block leaves, every time either height changes.
//
// ---- units ----------------------------------------------------------------------------------------
// Everything below is in design px. The engine scales a layout's exact values when it loads it, but
// script SetPos/SetSize are physical px, so every script size goes through m_Scale: the card's
// loaded width over its authored width.
class SearchBar : ScriptedWidgetEventHandler
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/search/SearchBar.layout";
	// The magnifier on the EQUIPMENT bar, Deluxe's InventoryIcons sprite.
	static const string SEARCH_ICON = "set:InventoryIcons image:MagnifyingGlass";

	// Design px. CARD_W is the card root's authored width in the layout, the yardstick for the scale.
	static const float CARD_W = 526;
	static const float PAD = 10;
	static const float FIELD_H = 40;
	static const float FIELD_TO_TILES = 10;
	// Text starts after the magnifying glass and stops short of whatever is showing at the field's
	// right end: the X, then Reset, then the match count, packed from the right (PlaceTrailing).
	static const float TEXT_LEFT = 42;
	static const float TRAIL_PAD = 12;
	static const float CLEAR_W = 22;
	static const float RESET_W = 40;
	static const float COUNT_W = 170;
	static const float TRAIL_GAP = 4;
	// An edit box draws its text as tall as the box itself, so the box is sized to the text height
	// wanted and centred in the field.
	static const float INPUT_H = 15;
	static const float RESET_H = 22;
	// Breathing room between the top block and the clothing scroller below it.
	static const float SCROLL_GAP = 4;
	protected ref CategoryRow m_Cats;
	protected ref TileTip m_Tooltip;

	protected Widget		m_Card;
	protected Widget		m_Body;
	protected Widget		m_Root;
	protected EditBoxWidget	m_Input;
	protected TextWidget	m_Placeholder;
	protected TextWidget	m_Count;
	protected ButtonWidget	m_Clear;
	protected ImageWidget	m_ClearIcon;
	protected ButtonWidget	m_Reset;
	protected TextWidget	m_ResetText;
	protected float			m_FieldW = 506;
	protected int			m_LastFilters = -1;

	protected ref SearchHeaderButton m_EqSearch;
	protected TextWidget	m_EqTitle;
	// The bar's own title text and colour, put back on the EQUIPMENT tab.
	protected string		m_EqTitleText;
	protected int			m_EqTitleColor;
	protected Widget		m_SlotsBand;

	protected Widget		m_Wrap;
	protected Widget		m_Column;
	protected ScrollWidget	m_Scroller;

	protected float	m_Scale = 1;
	protected float	m_CardW = 526;
	protected float	m_FitColumnH = -1;
	protected float	m_FitWrapH = -1;

	protected bool	m_OnSearch;
	protected bool	m_ReleasePending;
	protected int	m_LastCount = -2;
	// Escape is handled from whichever arrives first, the box's OnKeyDown or the per-frame key poll.
	protected bool	m_EscHandled;

	void SearchBar(Widget v_MenuRoot)
	{
		if (!v_MenuRoot)
			return;
		Widget v_Panel = v_MenuRoot.FindAnyWidget("RightPanel");
		if (!v_Panel)
			return;
		Widget v_Slots = v_Panel.FindAnyWidget("SlotsHeader");
		if (!v_Slots)
			return;
		m_Wrap = v_Slots.GetParent();
		if (!m_Wrap)
			return;
		m_Column = m_Wrap.GetParent();
		m_Scroller = ScrollWidget.Cast(v_Panel.FindAnyWidget("Scroller"));
		m_SlotsBand = m_Wrap.FindAnyWidget("ScrollerSlotsContent");

		m_Card = GetGame().GetWorkspace().CreateWidgets(Layout_Path, m_Wrap);
		if (!m_Card)
			return;

		m_Body = m_Card.FindAnyWidget("CardBody");
		m_Root = m_Card.FindAnyWidget("SearchRoot");
		m_Input = EditBoxWidget.Cast(m_Card.FindAnyWidget("SearchInput"));
		m_Placeholder = TextWidget.Cast(m_Card.FindAnyWidget("SearchPlaceholder"));
		m_Count = TextWidget.Cast(m_Card.FindAnyWidget("SearchCount"));
		m_Clear = ButtonWidget.Cast(m_Card.FindAnyWidget("SearchClear"));
		m_ClearIcon = ImageWidget.Cast(m_Card.FindAnyWidget("ClearIcon"));
		m_Reset = ButtonWidget.Cast(m_Card.FindAnyWidget("SearchReset"));
		m_ResetText = TextWidget.Cast(m_Card.FindAnyWidget("SearchResetText"));
		if (m_ResetText)
			m_ResetText.SetText(SearchStrings.RESET);
		// The field's magnifier, the X and Reset keep the colours their layout gives them (Deluxe's).

		// The engine's layout factor, read off the card as loaded, and the card's design width taken
		// from SlotsHeader, so it is exactly as wide as EQUIPMENT on all three column widths.
		float v_CW, v_CH;
		m_Card.GetSize(v_CW, v_CH);
		if (v_CW > 0)
			m_Scale = v_CW / CARD_W;
		float v_SW, v_SH;
		v_Slots.GetSize(v_SW, v_SH);
		if (v_SW > 0)
			m_CardW = v_SW / m_Scale;

		if (m_Root)
			m_Root.SetHandler(this);
		if (m_Input)
			m_Input.SetHandler(this);
		if (m_Clear)
			m_Clear.SetHandler(this);
		if (m_Reset)
			m_Reset.SetHandler(this);

		// The EQUIPMENT bar is PlayerContainer's CollapsibleHeader, moved into SlotsHeader
		// (playercontainer.c); height_wrapper is its root (collapsible_header.layout).
		Widget v_EqHeader = v_Slots.FindAnyWidget("height_wrapper");
		if (v_EqHeader)
		{
			m_EqTitle = TextWidget.Cast(v_EqHeader.FindAnyWidget("TextWidget0"));
			if (m_EqTitle)
			{
				// TextWidget cannot read its text back; this is the title vanilla gives the
				// EQUIPMENT bar (playercontainer.c).
				m_EqTitleText = "#container_inventory";
				m_EqTitleColor = m_EqTitle.GetColor();
			}
			BuildEqSearch(v_EqHeader);
		}

		if (m_Placeholder)
			m_Placeholder.SetText(SearchStrings.PLACEHOLDER);

		if (m_Body)
		{
			m_Tooltip = new TileTip(m_Card);
			m_Cats = new CategoryRow(m_Body, m_Scale, m_Tooltip);
		}

		RefreshText();
		LayoutCard();
		ApplyTab();
	}

	void ~SearchBar()
	{
		InputGuard.Release();
		m_Cats = null;
		m_Tooltip = null;
		m_EqSearch = null;
		if (m_Card)
			m_Card.Unlink();
	}

	protected void SetPx(Widget v_W, float v_X, float v_Y, float v_Width, float v_Height)
	{
		if (!v_W)
			return;
		v_W.SetPos(v_X * m_Scale, v_Y * m_Scale);
		v_W.SetSize(v_Width * m_Scale, v_Height * m_Scale);
	}

	protected void SetSizePx(Widget v_W, float v_Width, float v_Height)
	{
		if (v_W)
			v_W.SetSize(v_Width * m_Scale, v_Height * m_Scale);
	}

	// Sizes the field and the tiles to the card's width, then the body to its contents.
	protected void LayoutCard()
	{
		if (!m_Card)
			return;

		float v_Inner = m_CardW - PAD - PAD;
		m_FieldW = v_Inner;

		SetPx(m_Root, PAD, PAD, v_Inner, FIELD_H);
		PlaceTrailing();

		float v_ChipsH = 0;
		if (m_Cats)
		{
			m_Cats.SetFrame(PAD, PAD + FIELD_H + FIELD_TO_TILES, v_Inner);
			v_ChipsH = FIELD_TO_TILES + m_Cats.GetHeight();
		}

		float v_BodyH = PAD + FIELD_H + v_ChipsH + PAD;
		// The body is relative across (it follows the card) and exact down.
		if (m_Body)
			m_Body.SetSize(1, v_BodyH * m_Scale);
		SetSizePx(m_Card, m_CardW, v_BodyH);

		if (m_Wrap)
			m_Wrap.Update();
		m_FitWrapH = -1;
	}

	// Packs the field's right end from the right edge inwards: the X while there is text, then Reset
	// and the match count while any filter is on. The typed text gets whatever is left.
	protected void PlaceTrailing()
	{
		bool v_HasText = m_Input != null && m_Input.GetText() != "";
		bool v_Active = SearchFilter.IsActive();

		float v_R = TRAIL_PAD;
		if (v_HasText)
			v_R += CLEAR_W + TRAIL_GAP;
		if (m_Reset)
		{
			m_Reset.Show(v_Active);
			if (v_Active)
			{
				// Right-anchored, so x is the offset from the field's right edge.
				m_Reset.SetPos(v_R * m_Scale, (FIELD_H - RESET_H) * 0.5 * m_Scale);
				v_R += RESET_W + TRAIL_GAP;
			}
		}
		if (m_Count && v_Active)
		{
			m_Count.SetPos(v_R * m_Scale, 0);
			v_R += COUNT_W + TRAIL_GAP;
		}

		float v_TextW = m_FieldW - TEXT_LEFT - v_R;
		if (v_TextW < 20)
			v_TextW = 20;
		SetSizePx(m_Placeholder, v_TextW, FIELD_H);
		if (m_Input)
		{
			m_Input.SetPos(TEXT_LEFT * m_Scale, (FIELD_H - INPUT_H) * 0.5 * m_Scale);
			SetSizePx(m_Input, v_TextW, INPUT_H);
		}
	}

	// Clears every tile and the text: the Reset button beside the count.
	protected void ResetAll()
	{
		if (m_Input)
			m_Input.SetText("");
		SearchFilter.Set("");
		SearchFilter.SetCats(0);
		RefreshText();
	}

	protected static int FilterCount()
	{
		int v_N = 0;
		int v_Cats = SearchFilter.Cats();
		int v_Bit = 1;
		for (int v_I = 0; v_I < SearchFilter.CAT_COUNT; v_I++)
		{
			if ((v_Cats & v_Bit) != 0)
				v_N++;
			v_Bit = v_Bit * 2;
		}
		return v_N;
	}

	// Gives the clothing scroller exactly the part of the column the top block leaves. Runs every
	// frame but only writes when the column or the top block changed height.
	protected void FitScroller()
	{
		if (!m_Scroller || !m_Column || !m_Wrap)
			return;

		float v_CW, v_CH, v_WW, v_WH;
		m_Column.GetScreenSize(v_CW, v_CH);
		m_Wrap.GetScreenSize(v_WW, v_WH);
		if (v_CH <= 0 || v_WH <= 0)
			return;
		if (v_CH == m_FitColumnH && v_WH == m_FitWrapH)
			return;
		m_FitColumnH = v_CH;
		m_FitWrapH = v_WH;

		// The scroller's height is a fraction of the column (vexactsize 0 in all three layouts), so
		// the fraction is all that changes; its exact width is written back as it was read.
		float v_Frac = (v_CH - v_WH - SCROLL_GAP * m_Scale) / v_CH;
		v_Frac = Math.Clamp(v_Frac, 0.1, 1);
		float v_SW, v_SH;
		m_Scroller.GetSize(v_SW, v_SH);
		m_Scroller.SetSize(v_SW, v_Frac);
		m_Scroller.Update();
	}

	// The magnifier, right-anchored on the EQUIPMENT bar. Its idle colour is read from the bar's own
	// +/- icon so it matches the theme's header icons.
	protected void BuildEqSearch(Widget v_Header)
	{
		// Deluxe's header button ink at rest (DD_TabletTheme.Text(), #dcd9cf).
		int v_Idle = ARGB(255, 220, 217, 207);

		m_EqSearch = new SearchHeaderButton(v_Header, SEARCH_ICON, SearchStrings.OPEN_SEARCH_TIP, v_Idle);
		m_EqSearch.PlaceAt(SearchHeaderButton.EDGE_PAD + SearchHeaderButton.SLOT_W * BarSlots.Equipment(BarSlots.EQ_SEARCH));
		if (m_EqSearch.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_EqSearch.GetButton(), this, "SearchOnEqSearchClick");
	}

	protected void ApplyTitle()
	{
		if (!m_EqTitle)
			return;
		if (m_OnSearch)
		{
			m_EqTitle.SetText(SearchStrings.CARD_TITLE);
		}
		else
		{
			m_EqTitle.SetText(m_EqTitleText);
		}
		m_EqTitle.SetColor(m_EqTitleColor);
	}

	// The magnifier toggles the tab. Opening SEARCH puts the cursor in the field.
	void SearchOnEqSearchClick(Widget w, int x, int y, int button)
	{
		if (m_OnSearch)
			ShowSearchTab(false);
		else
			Focus();
	}

	// Switching back to EQUIPMENT turns the search off completely, so nothing stays dimmed by a
	// filter the player has put away.
	void ShowSearchTab(bool v_On)
	{
		if (v_On == m_OnSearch)
			return;
		m_OnSearch = v_On;
		if (!v_On && IsFocused())
			SetFocus(null);
		if (!v_On)
			ResetAll();
		ApplyTab();
	}

	protected void ApplyTab()
	{
		if (m_Card)
			m_Card.Show(m_OnSearch);
		if (m_SlotsBand)
			m_SlotsBand.Show(!m_OnSearch);
		if (m_EqSearch)
		{
			m_EqSearch.SetActive(m_OnSearch);
			if (m_OnSearch)
				m_EqSearch.SetTip(SearchStrings.CLOSE_SEARCH_TIP);
			else
				m_EqSearch.SetTip(SearchStrings.OPEN_SEARCH_TIP);
		}
		// A hidden tile gets no mouse-leave, so drop its hover and the tip by hand.
		if (!m_OnSearch)
			DropHover();
		ApplyTitle();
		if (m_Wrap)
			m_Wrap.Update();
		m_FitWrapH = -1;
	}

	protected void DropHover()
	{
		if (m_Tooltip)
			m_Tooltip.Hide();
		if (m_Cats)
			m_Cats.ClearHover();
	}

	void Focus()
	{
		if (!m_Input)
			return;
		ShowSearchTab(true);
		SetFocus(m_Input);
	}

	bool IsFocused()
	{
		return m_Input != null && GetFocus() == m_Input;
	}

	// Drops the text (X, Escape or closing the inventory). The category tiles are kept for the session.
	void Clear()
	{
		if (m_Input)
			m_Input.SetText("");
		SearchFilter.Set("");
		RefreshText();
	}

	void OnInventoryShow()
	{
		// The column may have been resized while the inventory was shut.
		m_FitColumnH = -1;
		FitScroller();
	}

	// Closing the inventory always drops the typed text, so the next open starts with everything bright
	// unless a category tile is still on.
	void OnInventoryHide()
	{
		Clear();
		DropHover();
		if (m_EqSearch)
			m_EqSearch.HideTip();
		if (IsFocused())
			SetFocus(null);
		InputGuard.Release();
		m_ReleasePending = false;
		m_EscHandled = false;
	}

	// Every frame while the inventory is open, from InventoryMenu.Update.
	void Update()
	{
		if (!m_Card)
			return;

		FitScroller();
		if (m_EqSearch)
			m_EqSearch.Layout();

		bool v_Focused = IsFocused();

		// Escape, polled as well as caught in OnKeyDown, because whether a focused edit box is sent
		// OnKeyDown for Escape is not verified.
		bool v_EscDown = KeyState(KeyCode.KC_ESCAPE) != 0;
		if (!v_EscDown)
			m_EscHandled = false;
		else if (v_Focused && !m_EscHandled)
			HandleEscape();

		// The keyboard is handed back only once Escape and Tab are both up. Releasing on the press
		// itself would let that same press reach MissionGameplay and close the whole screen.
		if (m_ReleasePending && !IsFocused() && !v_EscDown && KeyState(KeyCode.KC_TAB) == 0)
		{
			InputGuard.Release();
			m_ReleasePending = false;
		}

		if (m_Cats)
			m_Cats.Update();
		if (m_Tooltip)
			m_Tooltip.Place();

		RefreshCount();
	}

	// With something typed, Escape clears it; with the field already empty it goes back to EQUIPMENT.
	protected void HandleEscape()
	{
		m_EscHandled = true;
		if (m_Input && m_Input.GetText() != "")
			Clear();
		else
			ShowSearchTab(false);
	}

	// Written only when the value changes, since this runs every frame.
	protected void RefreshCount()
	{
		if (!m_Count)
			return;

		int v_Value = -1;
		if (SearchFilter.IsActive())
			v_Value = SearchFilter.Count();
		int v_Filters = FilterCount();
		if (v_Value == m_LastCount && v_Filters == m_LastFilters)
			return;
		bool v_WasShown = m_LastCount >= 0;
		m_LastCount = v_Value;
		m_LastFilters = v_Filters;

		// Reset and the count come and go together with the filter, and the text makes room for them.
		if (v_WasShown != (v_Value >= 0))
			PlaceTrailing();

		if (v_Value < 0)
		{
			m_Count.Show(false);
			return;
		}

		m_Count.Show(true);
		string v_Line;
		if (v_Value > 0)
		{
			v_Line = v_Value.ToString() + SearchStrings.FOUND_SUFFIX;
			m_Count.SetColor(ARGB(255, 169, 168, 160));
		}
		else
		{
			v_Line = SearchStrings.NO_MATCH;
			m_Count.SetColor(UIColor.Red());
		}
		if (v_Filters == 1)
			v_Line = v_Line + SearchStrings.FILTERS_SEP + v_Filters.ToString() + SearchStrings.FILTER_ONE;
		else if (v_Filters > 1)
			v_Line = v_Line + SearchStrings.FILTERS_SEP + v_Filters.ToString() + SearchStrings.FILTER_MANY;
		m_Count.SetText(v_Line);
	}

	// The placeholder shows only while the box is empty, the clear button only while it is not.
	protected void RefreshText()
	{
		bool v_Empty = true;
		if (m_Input)
			v_Empty = m_Input.GetText() == "";
		if (m_Placeholder)
			m_Placeholder.Show(v_Empty);
		if (m_Clear)
			m_Clear.Show(!v_Empty);
		PlaceTrailing();
	}

	override bool OnChange(Widget w, int x, int y, bool finished)
	{
		if (w != m_Input)
			return false;
		SearchFilter.Set(m_Input.GetText());
		RefreshText();
		return false;
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (w != m_Input)
			return false;

		if (key == KeyCode.KC_ESCAPE)
		{
			if (!m_EscHandled)
				HandleEscape();
			return true;
		}
		// Enter puts the keyboard back and keeps the filter, so the player can act on what it found.
		if (key == KeyCode.KC_RETURN || key == KeyCode.KC_NUMPADENTER)
		{
			SetFocus(null);
			return true;
		}
		return false;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (w != m_Input)
			return false;
		InputGuard.Engage();
		m_ReleasePending = false;
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		if (w != m_Input)
			return false;
		m_ReleasePending = true;
		return false;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (m_Reset != null && w == m_Reset)
		{
			ResetAll();
			return true;
		}
		if (w != m_Clear)
			return false;
		Clear();
		Focus();
		return true;
	}

	// The edit box is only a line high inside a 40 px field, so a click anywhere on the field focuses it.
	override bool OnMouseButtonDown(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT || w != m_Root)
			return false;
		Focus();
		return true;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_Clear && m_ClearIcon)
			m_ClearIcon.SetColor(colorScheme.BrandColor());
		if (w == m_Reset && m_ResetText)
			m_ResetText.SetColor(ARGB(255, 220, 217, 207));
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_Clear && m_ClearIcon)
			m_ClearIcon.SetColor(ARGB(255, 169, 168, 160));
		if (w == m_Reset && m_ResetText)
			m_ResetText.SetColor(ARGB(153, 220, 217, 207));
		return false;
	}
}
