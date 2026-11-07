// Covers a container's cargo grid while SortRunner sorts it, with a spinner, and lifts the
// cover when the sort ends - so the player sees the finished layout, not each move on the way.
// Owned by the container card (SortHooks.c), which calls Sync every UpdateInterval.
class SortCover : Managed
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/sort/SortCover.layout";
	// Degrees per spinner step, one step per GUI frame callback.
	static const float SPIN_STEP = 9;
	static const int SPIN_MS = 16;

	protected Widget		m_ScRoot;
	protected ImageWidget	m_ScSpin;
	protected float			m_ScDeg;

	void ~SortCover()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(ScStep);
		if (m_ScRoot)
			m_ScRoot.Unlink();
	}

	void Sync(Widget v_Grid, bool v_On)
	{
		if (!v_On)
		{
			if (m_ScRoot && m_ScRoot.IsVisible())
			{
				m_ScRoot.Show(false);
				GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(ScStep);
			}
			return;
		}
		if (!m_ScRoot)
		{
			if (!v_Grid)
				return;
			m_ScRoot = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_Grid);
			if (!m_ScRoot)
				return;
			m_ScSpin = ImageWidget.Cast(m_ScRoot.FindAnyWidget("SortCoverSpin"));
		}
		if (m_ScRoot.IsVisible())
			return;
		m_ScRoot.Show(true);
		if (m_ScSpin)
			m_ScSpin.SetColor(colorScheme.BrandColor());
		m_ScDeg = 0;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(ScStep);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(ScStep, SPIN_MS, true);
	}

	protected void ScStep()
	{
		if (!m_ScSpin)
			return;
		m_ScDeg += SPIN_STEP;
		if (m_ScDeg >= 360)
			m_ScDeg -= 360;
		m_ScSpin.SetRotation(0, 0, m_ScDeg);
	}
}
