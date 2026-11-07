// The spinner over a queued item's icon while its hover transfer counts down (HoverMove, one per
// queued item): the sort cover's arrow (SortCover), centred on the cell, turned once round as the hold
// fills. Made under the item's cell, and made again under its new cell if the item moves to another.
class HoverSpinner : Managed
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/transfer/HoverSpinner.layout";
	// The arrow's side as a share of one cell's side, so it sits a bit inside the cell.
	static const float SIZE_SHARE = 0.7;

	// Raw on purpose: the spinner lives under the cell, so both go when the cell goes.
	protected Widget		m_Cell;
	protected Widget		m_Root;
	protected ImageWidget	m_Spin;

	void ~HoverSpinner()
	{
		if (m_Root)
			m_Root.Unlink();
	}

	// v_CellSide: one cell's side in screen pixels. v_Turn: how far the hold is, 0 to 1.
	void Show(Widget v_Cell, float v_CellSide, float v_Turn)
	{
		if (!v_Cell)
		{
			Hide();
			return;
		}
		if (v_Cell != m_Cell || !m_Root)
		{
			if (m_Root)
				m_Root.Unlink();
			m_Cell = v_Cell;
			m_Root = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_Cell);
			if (!m_Root)
				return;
			m_Spin = ImageWidget.Cast(m_Root.FindAnyWidget("HoverSpin"));
			if (m_Spin)
			{
				float v_Side = v_CellSide * SIZE_SHARE;
				m_Spin.SetSize(v_Side, v_Side);
				m_Spin.SetColor(colorScheme.BrandColor());
			}
		}
		if (!m_Root.IsVisible())
			m_Root.Show(true);
		if (m_Spin)
			m_Spin.SetRotation(0, 0, 360 * v_Turn);
	}

	void Hide()
	{
		if (m_Root && m_Root.IsVisible())
			m_Root.Show(false);
	}
}
