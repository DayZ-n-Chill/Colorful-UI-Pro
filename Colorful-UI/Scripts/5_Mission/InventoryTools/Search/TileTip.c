// The hover name over the SEARCH tab's category tiles. One instance, owned by SearchBar.
//
// Parented to the inventory menu's root so nothing clips it, centred over the hovered widget with its
// bottom a small gap above that widget's top, and slid sideways so it stays inside the card. If there
// is no room above it drops below the widget instead.
class TileTip : Managed
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/search/SearchTipChip.layout";

	// Design px: authored height (the scale yardstick), fallback width, text padding either side,
	// gap over the anchor, inset from the bounds' sides.
	static const float TIP_H = 23;
	static const float TIP_W = 100;
	static const float TIP_PAD = 10;
	static const float TIP_GAP = 6;
	static const float TIP_INSET = 4;

	protected Widget		m_TipRoot;
	protected TextWidget	m_TipText;
	protected float			m_TipScale = 1;
	protected Widget		m_TipBounds;
	protected Widget		m_TipAnchor;
	protected float			m_TipX = -1;
	protected float			m_TipY = -1;
	protected float			m_TipW = -1;

	// v_Bounds may be null (no sideways clamp).
	void TileTip(Widget v_Bounds)
	{
		m_TipBounds = v_Bounds;
	}

	void ~TileTip()
	{
		if (m_TipRoot)
			m_TipRoot.Unlink();
	}

	// Built on first use: while the search card is being built the inventory menu may not be findable yet.
	protected bool Ensure()
	{
		if (m_TipRoot)
			return true;

		UIScriptedMenu v_Menu = GetGame().GetUIManager().FindMenu(MENU_INVENTORY);
		if (!v_Menu || !v_Menu.GetLayoutRoot())
			return false;

		m_TipRoot = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_Menu.GetLayoutRoot());
		if (!m_TipRoot)
			return false;
		m_TipText = TextWidget.Cast(m_TipRoot.FindAnyWidget("TipChipText"));

		float v_W, v_H;
		m_TipRoot.GetSize(v_W, v_H);
		if (v_H > 0)
			m_TipScale = v_H / TIP_H;
		return true;
	}

	void ShowFor(Widget v_Anchor, string v_Text)
	{
		if (!v_Anchor || v_Text == "" || !Ensure() || !m_TipText)
			return;
		m_TipAnchor = v_Anchor;
		m_TipText.SetText(v_Text);
		m_TipText.SetColor(ARGB(255, 255, 255, 255));
		m_TipRoot.Show(true);
		m_TipW = -1;
		Place();
	}

	// Only hides if the tip still points at v_Anchor, so a leave that arrives after the next tile's
	// enter does not take the new tip down.
	void HideFor(Widget v_Anchor)
	{
		if (v_Anchor == m_TipAnchor)
			Hide();
	}

	void Hide()
	{
		m_TipAnchor = null;
		if (m_TipRoot && m_TipRoot.IsVisible())
			m_TipRoot.Show(false);
	}

	// Every frame from SearchBar.Update, so the tip follows the column and picks up the text's
	// real width once it has been drawn.
	void Place()
	{
		if (!m_TipRoot || !m_TipRoot.IsVisible() || !m_TipAnchor)
			return;

		float v_AX, v_AY, v_AW, v_AH;
		m_TipAnchor.GetScreenPos(v_AX, v_AY);
		m_TipAnchor.GetScreenSize(v_AW, v_AH);
		if (v_AW <= 0)
			return;

		float v_S = m_TipScale;
		int v_TW, v_TH;
		if (m_TipText)
			m_TipText.GetTextSize(v_TW, v_TH);
		float v_TextW = v_TW;
		float v_CW = TIP_W * v_S;
		if (v_TextW > 0)
			v_CW = v_TextW + TIP_PAD * 2 * v_S;
		float v_CH = TIP_H * v_S;

		float v_X = v_AX + v_AW * 0.5 - v_CW * 0.5;
		if (m_TipBounds)
		{
			float v_BX, v_BY, v_BW, v_BH;
			m_TipBounds.GetScreenPos(v_BX, v_BY);
			m_TipBounds.GetScreenSize(v_BW, v_BH);
			if (v_BW > 0)
			{
				float v_Max = v_BX + v_BW - TIP_INSET * v_S - v_CW;
				float v_Min = v_BX + TIP_INSET * v_S;
				if (v_X > v_Max)
					v_X = v_Max;
				if (v_X < v_Min)
					v_X = v_Min;
			}
		}

		float v_Y = v_AY - TIP_GAP * v_S - v_CH;

		float v_PX = 0;
		float v_PY = 0;
		Widget v_Parent = m_TipRoot.GetParent();
		if (v_Parent)
			v_Parent.GetScreenPos(v_PX, v_PY);
		if (v_Y < v_PY)
			v_Y = v_AY + v_AH + TIP_GAP * v_S;

		if (v_X == m_TipX && v_Y == m_TipY && v_CW == m_TipW)
			return;
		m_TipX = v_X;
		m_TipY = v_Y;
		m_TipW = v_CW;
		m_TipRoot.SetSize(v_CW, v_CH);
		m_TipRoot.SetPos(v_X - v_PX, v_Y - v_PY);
	}
}
