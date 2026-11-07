// Wires the sort feature into the vanilla inventory screen:
// - the sort and stack buttons in the header of every cargo card the player may rearrange, and the
//   spinner cover over the card's grid while it is being sorted;
// - "Sort all" on the EQUIPMENT bar;
// - ticking the runners from InventoryMenu.Update and stopping them when the inventory closes.
//
// The transfer and search features mod the same vanilla classes in their own files. Modded classes
// chain across files, so every override here calls super and only adds; every member and method added
// to a vanilla class carries the Sort prefix so the three never collide.
//
// Two container classes carry a cargo grid under a ClosableHeader and they are siblings (both extend
// ClosableContainer), so each is hooked on its own:
// - ContainerWithCargo: ground crates and bags, car and tent cargo, zombies and bodies.
// - ContainerWithCargoAndAttachments: the player's clothing, backpacks and vests, and car attachments
//   that have their own cargo.

// The header owns the button widgets; the container owns the decision to show them.
modded class ClosableHeader
{
	protected ref SortButton m_SortBtn;
	protected ref SortButton m_SortStackBtn;

	// Created once, left of vanilla's RightSpacer (battery icon + X) and the fold chevron
	// (CollapseButton, slot 0): sort in slot 1, stack in slot 2, as on Deluxe's cards. The clicks are registered against the owning container, the same mechanism vanilla
	// uses for the reorder arrows, so the container can hand its own entity and cargo index to the runner.
	void SortAttach(Container v_Owner)
	{
		if (m_SortBtn || !m_PanelWidget || !v_Owner)
			return;

		m_SortBtn = new SortButton(m_PanelWidget, "SquaresFour", SortStrings.SORT_TIP, SortButton.SLOT_W);
		if (m_SortBtn.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_SortBtn.GetButton(), v_Owner, "SortOnSortClick");

		m_SortStackBtn = new SortButton(m_PanelWidget, "ArrowsInLineVertical", SortStrings.STACK_TIP, SortButton.SLOT_W * 2);
		if (m_SortStackBtn.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_SortStackBtn.GetButton(), v_Owner, "SortOnStackClick");
	}

	SortButton SortGetSort()
	{
		return m_SortBtn;
	}

	SortButton SortGetStack()
	{
		return m_SortStackBtn;
	}

	// Every frame from the owning container: keep the buttons placed and shown together.
	void SortUpdateButtons(bool v_Show)
	{
		if (m_SortBtn)
		{
			m_SortBtn.Layout();
			m_SortBtn.Show(v_Show);
		}
		if (m_SortStackBtn)
		{
			m_SortStackBtn.Layout();
			m_SortStackBtn.Show(v_Show);
		}
	}
}

modded class ContainerWithCargo
{
	protected bool m_SortCanSort;
	protected ref SortCover m_SortCover;

	override void SetEntity(EntityAI entity, int cargo_index = 0, bool immedUpdate = true)
	{
		super.SetEntity(entity, cargo_index, immedUpdate);

		// m_CargoIndex, not CargoContainer.GetCargoIndex(): vanilla passes immedUpdate into the grid's
		// cargo index argument, so the grid's copy is not trustworthy.
		m_SortCanSort = SortRunner.CanOffer(entity, m_CargoIndex);
		if (m_SortCanSort && m_ClosableHeader)
			m_ClosableHeader.SortAttach(this);
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		SortSyncCover(m_CargoIndex);
		if (m_ClosableHeader)
		{
			bool v_Show = m_SortCanSort && !m_LockCargo && !m_Closed;
			m_ClosableHeader.SortUpdateButtons(v_Show);
		}
	}

	// The spinner cover over this card's grid while its cargo is being sorted.
	protected void SortSyncCover(int v_Idx)
	{
		bool v_On = m_SortCanSort && SortRunner.IsSortingFor(m_Entity, v_Idx);
		if (!v_On && !m_SortCover)
			return;
		if (!m_SortCover)
			m_SortCover = new SortCover;
		Widget v_Grid = null;
		if (m_CargoGrid)
			v_Grid = m_CargoGrid.GetRootWidget();
		m_SortCover.Sync(v_Grid, v_On);
	}

	// The click opens the Sort by menu; picking an order there sorts (SortMenu).
	void SortOnSortClick(Widget w, int x, int y, int button)
	{
		if (!m_SortCanSort || !m_ClosableHeader)
			return;
		SortMenu.Open(m_Entity, m_CargoIndex, m_ClosableHeader.SortGetSort());
	}

	void SortOnStackClick(Widget w, int x, int y, int button)
	{
		if (!m_SortCanSort || !m_ClosableHeader)
			return;
		StackRunner.Start(m_Entity, m_CargoIndex, m_ClosableHeader.SortGetStack());
	}
}

modded class ContainerWithCargoAndAttachments
{
	protected bool m_SortCanSort;
	protected ref SortCover m_SortCover;

	override void SetEntity(EntityAI entity, bool immedUpdate = true)
	{
		super.SetEntity(entity, immedUpdate);

		// This class only ever shows cargo index 0.
		m_SortCanSort = m_CargoGrid != null && SortRunner.CanOffer(entity, 0);
		if (m_SortCanSort && m_ClosableHeader)
			m_ClosableHeader.SortAttach(this);
	}

	// Visibility follows the grid itself rather than m_LockCargo, which this class never sets: it hides
	// its cargo with HideCargo()/ShowCargo() instead.
	override void UpdateInterval()
	{
		super.UpdateInterval();

		SortSyncCover(0);
		if (m_ClosableHeader)
		{
			bool v_Show = m_SortCanSort && m_CargoGrid && m_CargoGrid.IsVisible() && !m_Closed;
			m_ClosableHeader.SortUpdateButtons(v_Show);
		}
	}

	protected void SortSyncCover(int v_Idx)
	{
		bool v_On = m_SortCanSort && SortRunner.IsSortingFor(m_Entity, v_Idx);
		if (!v_On && !m_SortCover)
			return;
		if (!m_SortCover)
			m_SortCover = new SortCover;
		Widget v_Grid = null;
		if (m_CargoGrid)
			v_Grid = m_CargoGrid.GetRootWidget();
		m_SortCover.Sync(v_Grid, v_On);
	}

	void SortOnSortClick(Widget w, int x, int y, int button)
	{
		if (!m_SortCanSort || !m_ClosableHeader)
			return;
		SortMenu.Open(m_Entity, 0, m_ClosableHeader.SortGetSort());
	}

	void SortOnStackClick(Widget w, int x, int y, int button)
	{
		if (!m_SortCanSort || !m_ClosableHeader)
			return;
		StackRunner.Start(m_Entity, 0, m_ClosableHeader.SortGetStack());
	}
}

// "Sort all" on the EQUIPMENT bar. The bar is PlayerContainer's CollapsibleHeader, moved into the
// right panel's SlotsHeader; height_wrapper is its root (collapsible_header.layout). The bar has no
// RightSpacer, so the button is placed at a fixed slot from the right edge: slot 0 is the search
// magnifier (SearchBar), Sort all takes slot 1 (Deluxe's order; see TransferHooks.c).
class SortAllBar : Managed
{
	static const int SLOT = 1;

	protected ref SortButton m_Btn;

	void SortAllBar(Widget v_MenuRoot)
	{
		if (!v_MenuRoot)
			return;
		Widget v_Panel = v_MenuRoot.FindAnyWidget("RightPanel");
		if (!v_Panel)
			return;
		Widget v_Slots = v_Panel.FindAnyWidget("SlotsHeader");
		if (!v_Slots)
			return;
		Widget v_EqHeader = v_Slots.FindAnyWidget("height_wrapper");
		if (!v_EqHeader)
			return;

		m_Btn = new SortButton(v_EqHeader, "SquaresFour_Fill", SortStrings.SORT_ALL_TIP, 0);
		m_Btn.PlaceAt(SortButton.EDGE_PAD + SortButton.SLOT_W * SLOT);
		if (m_Btn.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_Btn.GetButton(), this, "HandleClick");
	}

	// Every frame from InventoryMenu.Update: keeps the button's chip placed over it.
	void Update()
	{
		if (m_Btn)
			m_Btn.Layout();
	}

	void HandleClick(Widget w, int x, int y, int button)
	{
		SortAll.Start(m_Btn);
	}
}

modded class InventoryMenu
{
	protected ref SortAllBar m_SortAllBar;

	// The Inventory (and with it the EQUIPMENT bar) is built in the menu's constructor, so by the time
	// super.Init has handed back the root the bar is there to find.
	override Widget Init()
	{
		Widget v_Root = super.Init();
		m_SortAllBar = new SortAllBar(v_Root);
		return v_Root;
	}

	// Before super, so a sort in flight stops rather than carrying on with nothing on screen.
	override void OnHide()
	{
		SortAll.Abort();
		StackRunner.Abort(SortStrings.STACK_STOPPED);
		SortRunner.Abort(SortStrings.STOPPED);
		SortMenu.Close();

		super.OnHide();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		// Stack first: when it finishes it starts the sort, which then ticks in the same frame.
		StackRunner.Tick();
		SortRunner.Tick();
		SortMenu.Tick();
		// Last: it starts the next container once both runners above have gone idle.
		SortAll.Tick();
		if (m_SortAllBar)
			m_SortAllBar.Update();
	}
}
