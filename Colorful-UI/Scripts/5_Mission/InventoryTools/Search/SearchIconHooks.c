// Per-cell search dimming for both kinds of inventory cell: Icon (cargo grids) and SlotsIcon
// (equipment strip, attachment rows, vicinity list).
//
// Vanilla rebuilds and re-inits these cells constantly, so the dim is not applied once and trusted.
// Each cell re-checks in UpdateInterval, which vanilla already runs for every visible cell on every
// inventory frame, and repaints only when the item, the filter generation or the drag state moved on.
//
// Every override calls super first and only adds; the sort and transfer features mod these same
// classes in their own files, and Enforce chains the modded classes. Everything added here carries
// the Search prefix so the three features never collide.
modded class Icon
{
	protected Widget	m_SearchMark;
	protected EntityAI	m_SearchItem;
	protected int		m_SearchGen = -1;
	protected bool		m_SearchHit;
	protected bool		m_SearchLock;
	protected bool		m_SearchDrag;

	override void InitEx(EntityAI obj, bool refresh = true)
	{
		super.InitEx(obj, refresh);

		m_SearchLock = false;
		m_SearchGen = -1;
	}

	// A reserved cell shows a placeholder for an item on its way, so it is never dimmed or counted.
	override void InitLock(EntityAI parent, EntityAI obj, int x_pos, int y_pos, bool flip)
	{
		super.InitLock(parent, obj, x_pos, y_pos, flip);

		m_SearchLock = true;
		m_SearchGen = -1;
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		if (InventorySearch)
			SearchRefresh();
	}

	protected void SearchRefresh()
	{
		Widget v_Main = GetMainWidget();
		if (!v_Main)
			return;
		EntityAI v_E = EntityAI.Cast(GetObject());
		bool v_Drag = IsDragged();

		if (v_E != m_SearchItem || m_SearchGen != CUISearchFilter.Gen() || v_Drag != m_SearchDrag)
		{
			m_SearchItem = v_E;
			m_SearchGen = CUISearchFilter.Gen();
			m_SearchDrag = v_Drag;

			// Hidden while dragged so the item under the cursor never looks greyed out.
			bool v_Show = CUISearchFilter.IsActive() && v_E != null && !m_SearchLock && !v_Drag;
			m_SearchHit = v_Show && SearchItemWords.Matches(v_E);
			m_SearchMark = CellMark.Paint(v_Main, m_SearchMark, v_Show, m_SearchHit);
		}

		// A collapsed container still ticks its grid, so only cells actually on screen count.
		if (m_SearchHit && v_Main.IsVisibleHierarchy())
			CUISearchFilter.CountHit(v_E);
	}
}

modded class SlotsIcon
{
	protected Widget	m_SearchMark;
	protected EntityAI	m_SearchItem;
	protected int		m_SearchGen = -1;
	protected bool		m_SearchHit;
	protected bool		m_SearchLock;
	protected bool		m_SearchDrag;

	override void Init(EntityAI obj, bool reservation = false)
	{
		super.Init(obj, reservation);

		m_SearchLock = reservation;
		m_SearchGen = -1;
	}

	// The one place a slot lets go of its item (Clear and a changing Init both route here).
	override void ClearRemainSelected()
	{
		super.ClearRemainSelected();

		m_SearchLock = false;
		m_SearchGen = -1;
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		if (InventorySearch)
			SearchRefresh();
	}

	protected void SearchRefresh()
	{
		Widget v_Main = GetMainWidget();
		if (!v_Main)
			return;
		// Null for an empty slot, so empty ghost slots are never dimmed.
		EntityAI v_E = GetEntity();
		bool v_Drag = m_IsDragged;

		if (v_E != m_SearchItem || m_SearchGen != CUISearchFilter.Gen() || v_Drag != m_SearchDrag)
		{
			m_SearchItem = v_E;
			m_SearchGen = CUISearchFilter.Gen();
			m_SearchDrag = v_Drag;

			bool v_Show = CUISearchFilter.IsActive() && v_E != null && !m_SearchLock && !v_Drag;
			m_SearchHit = v_Show && SearchItemWords.Matches(v_E);
			m_SearchMark = CellMark.Paint(v_Main, m_SearchMark, v_Show, m_SearchHit);
		}

		if (m_SearchHit && v_Main.IsVisibleHierarchy())
			CUISearchFilter.CountHit(v_E);
	}
}
