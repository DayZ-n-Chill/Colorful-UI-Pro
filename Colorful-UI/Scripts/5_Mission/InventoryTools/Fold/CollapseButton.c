// The fold button on every item card's header: Deluxe's InventoryIcons CaretUp/CaretDown pair at 16 px in
// a 29 px hit box, sitting immediately left of the card's X. The sort and stack buttons sit to its
// left (SortAttach in SortHooks.c). The click is routed to the owning container, which
// does the folding (FoldHooks.c); this only draws the state.
//
// Placed exactly the way SortButton places itself: measured off RightSpacer (battery icon + X)
// and moved only when that spacer's width changes.
class CollapseButton : Managed
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/fold/CollapseButton.layout";

	protected ButtonWidget	m_Btn;
	protected ImageWidget	m_Opened;
	protected ImageWidget	m_Closed;
	protected Widget		m_RightSpacer;
	protected float			m_LastW = -1;
	protected bool			m_Collapsed;
	// The hover (brand wash, brand glyph, "Collapse" / "Expand" tip), shared with the X.
	protected ref HeaderHover m_FoldHover;

	// Screen x of the button's left edge, or -1 when it is not showing, so a header title can stop
	// short of it (FoldHooks.c). Same contract as SortButton.GetLeftEdge.
	float GetLeftEdge()
	{
		if (!m_Btn || !m_Btn.IsVisible())
			return -1;
		float v_X, v_Y;
		m_Btn.GetScreenPos(v_X, v_Y);
		return v_X;
	}

	void CollapseButton(Widget v_HeaderPanel)
	{
		if (!v_HeaderPanel)
			return;

		Widget v_Root = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_HeaderPanel);
		m_Btn = ButtonWidget.Cast(v_Root);
		if (!m_Btn)
		{
			if (v_Root)
				v_Root.Unlink();
			return;
		}

		m_Opened = ImageWidget.Cast(m_Btn.FindAnyWidget("CollapseOpened"));
		m_Closed = ImageWidget.Cast(m_Btn.FindAnyWidget("CollapseClosed"));
		m_RightSpacer = v_HeaderPanel.FindAnyWidget("RightSpacer");

		// Hover goes through WidgetEventHandler inside HeaderHover, because the container's
		// RegisterOnClick makes it the button's handler.
		m_FoldHover = new HeaderHover(m_Btn, v_HeaderPanel, FoldStrings.COLLAPSE_TIP);

		Paint();
	}

	void ~CollapseButton()
	{
		m_FoldHover = null;
		if (m_Btn)
			m_Btn.Unlink();
	}

	ButtonWidget GetButton()
	{
		return m_Btn;
	}

	bool Owns(Widget v_W)
	{
		return v_W != null && (v_W == m_Btn || v_W == m_Opened || v_W == m_Closed);
	}

	void Layout()
	{
		if (!m_Btn || !m_RightSpacer)
			return;

		float v_W, v_H;
		m_RightSpacer.GetScreenSize(v_W, v_H);
		if (v_W == m_LastW)
			return;
		m_LastW = v_W;
		// Script offsets in physical px, scaled by the loaded box over its authored side, the same grid
		// SortButton uses (see SortButton.BOX_W).
		float v_S = 1;
		float v_BW, v_BH;
		m_Btn.GetSize(v_BW, v_BH);
		if (v_BW > 0)
			v_S = v_BW / SortButton.BOX_W;
		m_Btn.SetPos((SortButton.EDGE_PAD + SortButton.SPACER_GAP) * v_S + v_W, 0);
	}

	void SetCollapsed(bool v_Collapsed)
	{
		if (v_Collapsed == m_Collapsed)
			return;
		m_Collapsed = v_Collapsed;
		Paint();
	}

	// Only which glyph shows and the tip; the glyph colour belongs to HeaderHover.
	protected void Paint()
	{
		if (m_Opened)
			m_Opened.Show(!m_Collapsed);
		if (m_Closed)
			m_Closed.Show(m_Collapsed);
		if (m_FoldHover)
		{
			if (m_Collapsed)
				m_FoldHover.SetTip(FoldStrings.EXPAND_TIP);
			else
				m_FoldHover.SetTip(FoldStrings.COLLAPSE_TIP);
		}
	}
}
