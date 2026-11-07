// Gives a header icon that is not a SortButton the same hover the sort button has: a soft
// brand-tinted square behind it, its glyphs in the brand colour, and a tip chip above the header.
// Used for the card's X (vanilla's collapse_button in closable_header.layout), the card's fold
// chevron (CollapseButton).
//
// The icon widget is not ours, so this only adds a background child to it and listens for enter and
// leave through WidgetEventHandler, whose per-event maps keep those apart from the click, mouse-down
// and drop handlers already registered on the same widget (header.c:22-23).
//
// The chip is FoldTipChip.layout (Deluxe's tip chip, white text like every tip) under the
// inventory menu's root, placed exactly the way SortButton.PlaceChip places its own. While hovered a short GUI timer keeps it placed and drops
// the hover if the icon disappears under the mouse (clicking the X closes the card, and a hidden widget
// is never sent a mouse-leave).
class HeaderHover : Managed
{
	static const string Bg_Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/fold/HoverBg.layout";
	static const string Chip_Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/fold/FoldTipChip.layout";
	static const int WATCH_MS = 100;
	// The chip in design px, the same as every header button's (SortButton): authored height, fallback
	// width, text padding, gap over the header, inset from the header's sides.
	static const float CHIP_H = 23;
	static const float CHIP_W = 130;
	static const float CHIP_PAD = 10;
	static const float CHIP_GAP = 4;
	static const float CHIP_INSET = 6;

	protected Widget				m_HvTarget;
	protected Widget				m_HvHeader;
	protected Widget				m_HvBg;
	protected ref array<ImageWidget>	m_HvGlyphs;
	protected ref array<int>		m_HvInks;
	protected Widget				m_HvChip;
	protected TextWidget			m_HvChipText;
	protected float					m_HvChipScale = 1;
	protected string				m_HvTip;
	protected bool					m_HvOn;

	void HeaderHover(Widget v_Target, Widget v_Header, string v_Tip)
	{
		m_HvTarget = v_Target;
		m_HvHeader = v_Header;
		m_HvTip = v_Tip;
		m_HvGlyphs = new array<ImageWidget>;
		m_HvInks = new array<int>;
		if (!v_Target)
			return;

		// The glyphs are the target's direct image children; their layout colour is the idle ink.
		Widget v_Child = v_Target.GetChildren();
		while (v_Child)
		{
			ImageWidget v_Img = ImageWidget.Cast(v_Child);
			if (v_Img)
			{
				m_HvGlyphs.Insert(v_Img);
				m_HvInks.Insert(v_Img.GetColor());
			}
			v_Child = v_Child.GetSibling();
		}

		m_HvBg = GetGame().GetWorkspace().CreateWidgets(Bg_Layout_Path, v_Target);

		WidgetEventHandler.GetInstance().RegisterOnMouseEnter(v_Target, this, "OnHvEnter");
		WidgetEventHandler.GetInstance().RegisterOnMouseLeave(v_Target, this, "OnHvLeave");
	}

	void ~HeaderHover()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(HvWatch);
		if (m_HvChip)
			m_HvChip.Unlink();
	}

	// For a button whose meaning flips (Collapse / Expand). Rewrites a showing chip.
	void SetTip(string v_Tip)
	{
		if (v_Tip == m_HvTip)
			return;
		m_HvTip = v_Tip;
		if (m_HvOn)
			ShowChip();
	}

	protected void SetHover(bool v_On)
	{
		if (v_On == m_HvOn)
			return;
		m_HvOn = v_On;

		int v_Brand = colorScheme.BrandColor();
		for (int v_I = 0; v_I < m_HvGlyphs.Count(); v_I++)
		{
			ImageWidget v_Img = m_HvGlyphs[v_I];
			if (!v_Img)
				continue;
			if (v_On)
				v_Img.SetColor(v_Brand);
			else
				v_Img.SetColor(m_HvInks[v_I]);
		}
		// Brand colour at alpha 46 out of 255, the same wash as SortButton.ApplyHover.
		if (m_HvBg)
		{
			if (v_On)
				m_HvBg.SetColor((v_Brand & 0x00FFFFFF) | 0x2E000000);
			else
				m_HvBg.SetColor(0);
		}

		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(HvWatch);
		if (v_On)
		{
			ShowChip();
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(HvWatch, WATCH_MS, true);
		}
		else if (m_HvChip)
		{
			m_HvChip.Show(false);
		}
	}

	protected void HvWatch()
	{
		if (!m_HvTarget || !m_HvTarget.IsVisibleHierarchy())
		{
			SetHover(false);
			return;
		}
		PlaceChip();
	}

	protected bool EnsureChip()
	{
		if (m_HvChip)
			return true;

		UIScriptedMenu v_Menu = GetGame().GetUIManager().FindMenu(MENU_INVENTORY);
		if (!v_Menu || !v_Menu.GetLayoutRoot())
			return false;

		m_HvChip = GetGame().GetWorkspace().CreateWidgets(Chip_Layout_Path, v_Menu.GetLayoutRoot());
		if (!m_HvChip)
			return false;
		m_HvChipText = TextWidget.Cast(m_HvChip.FindAnyWidget("TipChipText"));

		float v_W, v_H;
		m_HvChip.GetSize(v_W, v_H);
		if (v_H > 0)
			m_HvChipScale = v_H / CHIP_H;
		return true;
	}

	protected void ShowChip()
	{
		if (m_HvTip == "")
		{
			if (m_HvChip)
				m_HvChip.Show(false);
			return;
		}
		if (!EnsureChip() || !m_HvChipText)
			return;
		m_HvChipText.SetText(m_HvTip);
		m_HvChipText.SetColor(ARGB(255, 255, 255, 255));
		m_HvChip.Show(true);
		PlaceChip();
	}

	// Centred over the icon, its bottom a small gap above the header's top edge, kept inside the
	// header's sides; under the header instead if it would leave the screen at the top.
	protected void PlaceChip()
	{
		if (!m_HvChip || !m_HvChip.IsVisible() || !m_HvTarget || !m_HvHeader)
			return;

		float v_BX, v_BY, v_BW, v_BH;
		m_HvTarget.GetScreenPos(v_BX, v_BY);
		m_HvTarget.GetScreenSize(v_BW, v_BH);
		float v_HX, v_HY, v_HW, v_HH;
		m_HvHeader.GetScreenPos(v_HX, v_HY);
		m_HvHeader.GetScreenSize(v_HW, v_HH);
		if (v_BW <= 0 || v_HW <= 0)
			return;

		float v_S = m_HvChipScale;
		int v_TW, v_TH;
		if (m_HvChipText)
			m_HvChipText.GetTextSize(v_TW, v_TH);
		// Through a float first, so the sum below is float arithmetic and not int.
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
		Widget v_Parent = m_HvChip.GetParent();
		if (v_Parent)
			v_Parent.GetScreenPos(v_PX, v_PY);
		if (v_Y < v_PY)
			v_Y = v_HY + v_HH + CHIP_GAP * v_S;

		m_HvChip.SetSize(v_CW, v_CH);
		m_HvChip.SetPos(v_X - v_PX, v_Y - v_PY);
	}

	bool OnHvEnter(Widget w, int x, int y)
	{
		SetHover(true);
		return false;
	}

	bool OnHvLeave(Widget w, Widget enter_w, int x, int y)
	{
		SetHover(false);
		return false;
	}
}
