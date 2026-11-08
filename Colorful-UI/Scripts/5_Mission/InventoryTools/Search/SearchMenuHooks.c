// Wires the search bar into the inventory screen's own lifecycle. Every override calls super; the
// sort and transfer features mod InventoryMenu in their own files and Enforce chains them.
// MissionGameplay.DestroyInventory builds a whole new menu, which runs Init again; the old bar's
// destructor unlinks its widgets and hands the keyboard back.
modded class InventoryMenu
{
	protected ref SearchBar m_SearchBar;

	// The right column the search card goes into is built in the menu's constructor, so by the time
	// super.Init has handed back the root it is there to find.
	override Widget Init()
	{
		Widget v_Root = super.Init();
		if (InventorySearch)
			m_SearchBar = new SearchBar(v_Root);
		return v_Root;
	}

	override void OnShow()
	{
		super.OnShow();

		if (m_SearchBar)
			m_SearchBar.OnInventoryShow();
	}

	// Before super, so the filter and the keyboard guard are gone before vanilla tears the screen down.
	override void OnHide()
	{
		if (m_SearchBar)
			m_SearchBar.OnInventoryHide();

		super.OnHide();
	}

	// The cells count their matches while vanilla updates them inside super, so the hit set is opened
	// before it and closed after it; the bar then reads the finished count.
	override void Update(float timeslice)
	{
		if (!InventorySearch)
		{
			super.Update(timeslice);
			return;
		}

		SearchFilter.BeginFrame();
		super.Update(timeslice);
		SearchFilter.EndFrame();

		if (m_SearchBar)
			m_SearchBar.Update();
	}
}
