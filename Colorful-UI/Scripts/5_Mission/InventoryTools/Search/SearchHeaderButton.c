// The magnifier on the EQUIPMENT bar that opens the SEARCH tab: an icon button with a soft brand
// wash on hover, lit while the tab is open, and a hover tip above it. The search feature's own copy
// of Deluxe's header button (SortButton), cut down to what the magnifier uses, so it never
// collides with the sort feature's button class.
//
// The tip is its own small layout parented to the inventory menu's root, because SlotsHeader clips
// its children and a tip inside the bar could never be drawn above it.
class SearchHeaderButton : Managed
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/search/SearchHeaderButton.layout";
	static const string Chip_Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/search/SearchTipChip.layout";

	// Gap between the right edge and the first button, and one button plus the gap.
	static const float EDGE_PAD = 10;
	static const float SLOT_W = 33;
	// On the EQUIPMENT and VICINITY bars (collapsible_header.layout: 60 px tall, the last 5 px its
	// Border strip) the buttons centre on the 55 px blue band above the strip, which is exactly a card
	// header's height, so bar and card buttons sit at the same height: 2.5 px up from the bar's centre.
	static const float BAR_Y = -6.5;
	// The button box's authored side in the layout. Layout sizes are scaled by the engine but script
	// SetPos takes physical px, so every script offset is multiplied by the loaded box over this
	// (m_Scale): the boxes then sit on exactly the same grid as the layout-placed X at any
	// resolution or UI scale.
	static const float BOX_W = 29;

	// The chip, in design px: authored height (scale yardstick), fallback width, text padding, gap
	// over the header's top edge, inset from the header's sides.
	static const float CHIP_H = 23;
	static const float CHIP_W = 130;
	static const float CHIP_PAD = 10;
	static const float CHIP_GAP = 4;
	static const float CHIP_INSET = 6;

	protected Widget		m_Root;
	protected Widget		m_Header;
	protected ButtonWidget	m_Btn;
	protected Widget		m_Bg;
	protected ImageWidget	m_Icon;
	protected Widget		m_Chip;
	protected TextWidget	m_ChipText;
	protected float			m_ChipScale = 1;
	protected float			m_Scale = 1;
	protected float			m_ChipX = -1;
	protected float			m_ChipY = -1;
	protected float			m_ChipW = -1;
	protected bool			m_Hover;
	protected bool			m_Active;
	protected string		m_Tooltip;
	protected int			m_IdleColor;

	// v_IdleColor is the icon colour at rest, read by the caller from the bar's own icons so the
	// magnifier matches whatever the theme draws there.
	void SearchHeaderButton(Widget v_Header, string v_Image, string v_Tip, int v_IdleColor)
	{
		m_Tooltip = v_Tip;
		m_Header = v_Header;
		m_IdleColor = v_IdleColor;

		if (!v_Header)
			return;

		m_Root = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_Header);
		if (!m_Root)
			return;

		float v_BW, v_BH;
		m_Root.GetSize(v_BW, v_BH);
		if (v_BW > 0)
			m_Scale = v_BW / BOX_W;

		m_Btn = ButtonWidget.Cast(m_Root.FindAnyWidget("SrchBtn"));
		m_Bg = m_Root.FindAnyWidget("SrchBg");
		m_Icon = ImageWidget.Cast(m_Root.FindAnyWidget("SrchIcon"));
		if (m_Icon)
		{
			m_Icon.LoadImageFile(0, v_Image);
			m_Icon.SetImage(0);
		}

		// Hover through WidgetEventHandler too, because the owner's RegisterOnClick makes it the
		// button's one and only handler.
		if (m_Btn)
		{
			WidgetEventHandler.GetInstance().RegisterOnMouseEnter(m_Btn, this, "HandleEnter");
			WidgetEventHandler.GetInstance().RegisterOnMouseLeave(m_Btn, this, "HandleLeave");
		}

		ApplyState();
	}

	void ~SearchHeaderButton()
	{
		if (m_Chip)
			m_Chip.Unlink();
		if (m_Root)
			m_Root.Unlink();
	}

	ButtonWidget GetButton()
	{
		return m_Btn;
	}

	// v_X is the offset from the header's right edge (the root is right-anchored), in layout units.
	void PlaceAt(float v_X)
	{
		if (m_Root)
			m_Root.SetPos(v_X * m_Scale, BAR_Y * m_Scale);
	}

	// Every frame from SearchBar.Update: keeps the tip over the button as the column moves.
	void Layout()
	{
		PlaceChip();
	}

	void SetActive(bool v_On)
	{
		if (v_On == m_Active)
			return;
		m_Active = v_On;
		ApplyState();
	}

	void SetTip(string v_Tip)
	{
		if (v_Tip == m_Tooltip)
			return;
		m_Tooltip = v_Tip;
		ApplyState();
	}

	void HideTip()
	{
		m_Hover = false;
		if (m_Chip && m_Chip.IsVisible())
			m_Chip.Show(false);
	}

	protected void ApplyState()
	{
		if (m_Icon)
		{
			if (m_Hover || m_Active)
				m_Icon.SetColor(colorScheme.BrandColor());
			else
				m_Icon.SetColor(m_IdleColor);
		}

		// Brand colour at alpha 46 out of 255: a soft wash, so the icon stays the brightest thing.
		if (m_Bg)
		{
			if (m_Hover || m_Active)
				m_Bg.SetColor((colorScheme.BrandColor() & 0x00FFFFFF) | 0x2E000000);
			else
				m_Bg.SetColor(0);
		}

		if (m_Hover)
			SetChip(m_Tooltip);
		else if (m_Chip)
			m_Chip.Show(false);
	}

	protected bool EnsureChip()
	{
		if (m_Chip)
			return true;

		UIScriptedMenu v_Menu = GetGame().GetUIManager().FindMenu(MENU_INVENTORY);
		if (!v_Menu || !v_Menu.GetLayoutRoot())
			return false;

		m_Chip = GetGame().GetWorkspace().CreateWidgets(Chip_Layout_Path, v_Menu.GetLayoutRoot());
		if (!m_Chip)
			return false;
		m_ChipText = TextWidget.Cast(m_Chip.FindAnyWidget("TipChipText"));

		float v_W, v_H;
		m_Chip.GetSize(v_W, v_H);
		if (v_H > 0)
			m_ChipScale = v_H / CHIP_H;
		return true;
	}

	protected void SetChip(string v_Text)
	{
		if (v_Text == "")
		{
			if (m_Chip)
				m_Chip.Show(false);
			return;
		}
		if (!EnsureChip() || !m_ChipText)
			return;
		m_ChipText.SetText(v_Text);
		m_ChipText.SetColor(ARGB(255, 255, 255, 255));
		m_Chip.Show(true);
		m_ChipW = -1;
		PlaceChip();
	}

	// Centred over the button, its bottom CHIP_GAP above the header's top edge, slid sideways to stay
	// CHIP_INSET in from the header's sides; drops under the header if there is no room above.
	protected void PlaceChip()
	{
		if (!m_Chip || !m_Chip.IsVisible() || !m_Btn || !m_Header)
			return;

		float v_BX, v_BY, v_BW, v_BH;
		m_Btn.GetScreenPos(v_BX, v_BY);
		m_Btn.GetScreenSize(v_BW, v_BH);
		float v_HX, v_HY, v_HW, v_HH;
		m_Header.GetScreenPos(v_HX, v_HY);
		m_Header.GetScreenSize(v_HW, v_HH);
		if (v_BW <= 0 || v_HW <= 0)
			return;

		float v_S = m_ChipScale;
		int v_TW, v_TH;
		if (m_ChipText)
			m_ChipText.GetTextSize(v_TW, v_TH);
		float v_TextW = v_TW;
		float v_CW = CHIP_W * v_S;
		if (v_TextW > 0)
			v_CW = v_TextW + CHIP_PAD * 2 * v_S;
		float v_CH = CHIP_H * v_S;

		float v_X = v_BX + v_BW * 0.5 - v_CW * 0.5;
		float v_Max = v_HX + v_HW - CHIP_INSET * v_S - v_CW;
		float v_Min = v_HX + CHIP_INSET * v_S;
		if (v_X > v_Max)
			v_X = v_Max;
		if (v_X < v_Min)
			v_X = v_Min;

		float v_Y = v_HY - CHIP_GAP * v_S - v_CH;

		float v_PX = 0;
		float v_PY = 0;
		Widget v_Parent = m_Chip.GetParent();
		if (v_Parent)
			v_Parent.GetScreenPos(v_PX, v_PY);
		if (v_Y < v_PY)
			v_Y = v_HY + v_HH + CHIP_GAP * v_S;

		if (v_X == m_ChipX && v_Y == m_ChipY && v_CW == m_ChipW)
			return;
		m_ChipX = v_X;
		m_ChipY = v_Y;
		m_ChipW = v_CW;
		m_Chip.SetSize(v_CW, v_CH);
		m_Chip.SetPos(v_X - v_PX, v_Y - v_PY);
	}

	bool HandleEnter(Widget w, int x, int y)
	{
		m_Hover = true;
		ApplyState();
		return true;
	}

	bool HandleLeave(Widget w, Widget enter_w, int x, int y)
	{
		m_Hover = false;
		ApplyState();
		return true;
	}
}
