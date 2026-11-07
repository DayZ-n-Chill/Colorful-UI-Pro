// Folding, ported from Deluxe (ContainerHooks.c and the bars' toggles in SearchBar.c and
// QuickMove.c):
// - every closable card gets a fold chevron in its header, immediately left of the X (the sort and
//   stack buttons sit to its left, SortHooks.c). Folding hides the card's body and keeps its
//   header, unlike the X, which hides the whole card. Folds are remembered per item type
//   (CollapseStore);
// - the card's X gets the same hover as the header buttons, with a "Close" tip;
// - the EQUIPMENT and VICINITY bars get two toggles: show all / close all (Eye / EyeSlash) and
//   collapse all / expand all (ArrowLineUp / ArrowLineDown). Each does what its icon says and flips
//   with it; both re-read the cards every frame, so a single card's own X, slot icon or chevron flips
//   them too. They are one-shot actions, so only the hover lights them.
//
// The sort, transfer and search features mod the same vanilla classes in their own files. Modded
// classes chain, so every override here calls super, and every member and method added to a vanilla
// class carries the Fold prefix.

// Every closable card on screen, so the bars' toggles can visit one column's cards. The list holds
// plain (weak) references: cards are Managed, so a destroyed card reads as null and is dropped on
// the next Register.
class FoldCards
{
	static const int SIDE_NONE = 0;
	static const int SIDE_PLAYER = 1;
	static const int SIDE_VICINITY = 2;

	protected static ref array<ClosableContainer> s_FoldCards;

	static void Register(ClosableContainer v_Card)
	{
		if (!s_FoldCards)
			s_FoldCards = new array<ClosableContainer>;
		for (int v_I = s_FoldCards.Count() - 1; v_I >= 0; v_I--)
		{
			if (!s_FoldCards[v_I])
				s_FoldCards.Remove(v_I);
		}
		s_FoldCards.Insert(v_Card);
	}

	// The lists below are collected first: closing or opening a card reflows the column and can build
	// or drop cards (Register), which must not change what is visited.

	// Closes every open card on the side the way its own X does (ClosableContainer.Close), so the cards
	// leave the column and their slot icons show them closed.
	static void CloseAll(int v_Side)
	{
		if (!s_FoldCards)
			return;
		array<ClosableContainer> v_Todo = new array<ClosableContainer>;
		foreach (ClosableContainer v_Card : s_FoldCards)
		{
			if (IsOnSide(v_Card, v_Side) && v_Card.IsOpened() && v_Card.IsDisplayable())
				v_Todo.Insert(v_Card);
		}
		foreach (ClosableContainer v_Do : v_Todo)
		{
			if (v_Do && v_Do.FoldGetEntity())
				v_Do.Close();
		}
	}

	// Reopens every closed card on the side through vanilla's own Open (the path a slot icon click
	// takes), then unfolds every folded one, so one click shows everything.
	static void ShowAll(int v_Side)
	{
		if (!s_FoldCards)
			return;
		array<ClosableContainer> v_Open = new array<ClosableContainer>;
		foreach (ClosableContainer v_Card : s_FoldCards)
		{
			if (IsOnSide(v_Card, v_Side) && !v_Card.IsOpened() && v_Card.IsDisplayable())
				v_Open.Insert(v_Card);
		}
		foreach (ClosableContainer v_DoOpen : v_Open)
		{
			if (v_DoOpen && v_DoOpen.FoldGetEntity())
				v_DoOpen.Open();
		}

		// Re-collected after the opens, which may have built cards of their own.
		array<ClosableContainer> v_Unfold = new array<ClosableContainer>;
		foreach (ClosableContainer v_Folded : s_FoldCards)
		{
			if (IsOnSide(v_Folded, v_Side) && v_Folded.FoldIsCollapsed() && v_Folded.FoldKey() != "")
				v_Unfold.Insert(v_Folded);
		}
		foreach (ClosableContainer v_DoUnfold : v_Unfold)
		{
			if (v_DoUnfold)
				v_DoUnfold.FoldSetCollapsed(false);
		}
	}

	// True while any card on the side is closed (the toggle offers Show all), false once every one is
	// open (it offers Close all).
	static bool AnyClosedOnSide(int v_Side)
	{
		if (!s_FoldCards)
			return false;
		foreach (ClosableContainer v_Card : s_FoldCards)
		{
			if (IsOnSide(v_Card, v_Side) && !v_Card.IsOpened() && v_Card.IsDisplayable())
				return true;
		}
		return false;
	}

	// True while any open, foldable card on the side is expanded.
	static bool AnyExpandedOnSide(int v_Side)
	{
		if (!s_FoldCards)
			return false;
		foreach (ClosableContainer v_Card : s_FoldCards)
		{
			if (IsOnSide(v_Card, v_Side) && v_Card.IsOpened() && !v_Card.FoldIsCollapsed() && v_Card.FoldKey() != "")
				return true;
		}
		return false;
	}

	// Folds (or unfolds) every open card on the side through its own fold, so each is remembered per
	// type the same way a click on its chevron is.
	static void SetSideCollapsed(int v_Side, bool v_Collapsed)
	{
		if (!s_FoldCards)
			return;
		array<ClosableContainer> v_Todo = new array<ClosableContainer>;
		foreach (ClosableContainer v_Card : s_FoldCards)
		{
			if (IsOnSide(v_Card, v_Side) && v_Card.IsOpened() && v_Card.FoldIsCollapsed() != v_Collapsed && v_Card.FoldKey() != "")
				v_Todo.Insert(v_Card);
		}
		foreach (ClosableContainer v_Do : v_Todo)
		{
			if (v_Do)
				v_Do.FoldSetCollapsed(v_Collapsed);
		}
	}

	protected static bool IsOnSide(ClosableContainer v_Card, int v_Side)
	{
		if (!v_Card || !v_Card.GetRootWidget() || !v_Card.FoldGetEntity())
			return false;
		return v_Card.FoldSide() == v_Side;
	}
}

// ---- the card's header: the chevron and the X's hover --------------------------------------------------
modded class ClosableHeader
{
	protected ref CollapseButton m_FoldCollapse;
	protected ref HeaderHover m_FoldCloseHover;

	// The chevron takes the slot next to the X, so it is in the same place on every card. Its click is
	// registered against the owning card, which does the folding.
	void FoldAttach(Container v_Owner)
	{
		if (m_FoldCollapse || !m_PanelWidget || !v_Owner)
			return;

		m_FoldCollapse = new CollapseButton(m_PanelWidget);
		if (m_FoldCollapse.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_FoldCollapse.GetButton(), v_Owner, "FoldOnCollapseClick");

		if (m_CollapseButton && !m_FoldCloseHover)
			m_FoldCloseHover = new HeaderHover(m_CollapseButton, m_PanelWidget, FoldStrings.CLOSE_TIP);
	}

	// Every frame from the owning card. Only writes when something changed.
	void FoldUpdate(bool v_Collapsed)
	{
		FoldRoundFolded(v_Collapsed);
		if (!m_FoldCollapse)
			return;
		m_FoldCollapse.Layout();
		m_FoldCollapse.SetCollapsed(v_Collapsed);
	}

	// The bar's colour while it is swapped onto FoldBack, and whether it is.
	protected int	m_FoldBarColor;
	protected bool	m_FoldBarSwapped;

	// An open card's header is rounded on its top corners only (closable_header.layout), its body on the
	// bottom ones. A folded card is just the header, so while folded the bar goes clear and the fully
	// rounded FoldBack behind it takes its colour: all four corners round (Deluxe's rule).
	protected void FoldRoundFolded(bool v_Collapsed)
	{
		if (!RoundedInventory || !m_PanelWidget || v_Collapsed == m_FoldBarSwapped)
			return;
		Widget v_Back = m_PanelWidget.FindAnyWidget("FoldBack");
		if (!v_Back)
			return;
		if (v_Collapsed)
		{
			m_FoldBarColor = m_PanelWidget.GetColor();
			v_Back.SetColor(m_FoldBarColor);
			m_PanelWidget.SetColor(m_FoldBarColor & 0x00FFFFFF);
			v_Back.Show(true);
		}
		else
		{
			m_PanelWidget.SetColor(m_FoldBarColor);
			v_Back.Show(false);
		}
		m_FoldBarSwapped = v_Collapsed;
	}

	// Vanilla hides the reorder arrows whenever the mouse leaves the header for anything but its own
	// buttons. The chevron sits on the header too, so moving onto it is not leaving.
	override bool MouseLeave(Widget w, Widget enter_w, int x, int y)
	{
		if (m_FoldCollapse && m_FoldCollapse.Owns(enter_w))
		{
			HideOwnedTooltip();
			return true;
		}
		return super.MouseLeave(w, enter_w, x, y);
	}
}

// ---- the card: its fold state --------------------------------------------------------------------------
// The header and the card's "body" WrapSpacer are siblings under the card's root (the header is built
// before ClosableContainer's constructor points m_MainWidget at "body"), so hiding m_MainWidget leaves
// exactly the header. Vanilla's X is not touched: Close() still hides the whole card and the slot icon
// still reopens it. A folded card that is closed and reopened comes back folded.
modded class ClosableContainer
{
	protected bool		m_FoldCollapsed;
	// The entity whose remembered state was last read, so it is read once per entity, not per frame.
	protected EntityAI	m_FoldFor;
	// Which column the card is in (FoldCards.SIDE_*), found once the parent chain reaches one.
	protected int		m_FoldSide;

	void ClosableContainer(LayoutHolder parent, int sort = -1)
	{
		if (m_ClosableHeader)
			m_ClosableHeader.FoldAttach(this);
		FoldCards.Register(this);
	}

	EntityAI FoldGetEntity()
	{
		return m_Entity;
	}

	// RightArea is the player's own column, LeftArea the vicinity's; anything else (hands) is neither.
	int FoldSide()
	{
		if (m_FoldSide != FoldCards.SIDE_NONE)
			return m_FoldSide;
		LayoutHolder v_P = GetParent();
		while (v_P)
		{
			if (RightArea.Cast(v_P))
			{
				m_FoldSide = FoldCards.SIDE_PLAYER;
				break;
			}
			if (LeftArea.Cast(v_P))
			{
				m_FoldSide = FoldCards.SIDE_VICINITY;
				break;
			}
			v_P = v_P.GetParent();
		}
		return m_FoldSide;
	}

	// Per type, the way vanilla remembers its open/closed state.
	string FoldKey()
	{
		if (!m_Entity)
			return "";
		return m_Entity.GetType();
	}

	bool FoldIsCollapsed()
	{
		return m_FoldCollapsed;
	}

	void FoldOnCollapseClick(Widget w, int x, int y, int button)
	{
		FoldSetCollapsed(!m_FoldCollapsed);
	}

	void FoldSetCollapsed(bool v_Collapsed)
	{
		m_FoldFor = m_Entity;
		m_FoldCollapsed = v_Collapsed;
		CollapseStore.Set(FoldKey(), v_Collapsed);
		FoldShowBody(!v_Collapsed);
		if (m_ClosableHeader)
			m_ClosableHeader.FoldUpdate(v_Collapsed);
		// The slot's chevron is the same switch as the card's fold: a folded card reads as closed.
		if (m_SlotIcon)
			SetOpenForSlotIcon(IsOpened() && !v_Collapsed);
	}

	// Clicking the item's slot on a card that is open but folded unfolds it, rather than closing a card
	// the player cannot see the inside of.
	override void Toggle()
	{
		if (IsOpened() && m_FoldCollapsed)
		{
			FoldSetCollapsed(false);
			if (m_Parent)
				m_Parent.Refresh();
			return;
		}
		super.Toggle();
	}

	override void UpdateRadialIcon()
	{
		super.UpdateRadialIcon();
		if (m_SlotIcon && IsOpened() && m_FoldCollapsed)
			SetOpenForSlotIcon(false);
	}

	// Reflows the column the same way vanilla's Close() does.
	protected void FoldShowBody(bool v_Show)
	{
		if (!m_MainWidget)
			return;
		m_MainWidget.Show(v_Show);
		GetRootWidget().Update();
		if (m_Parent && m_Parent.m_Parent)
			m_Parent.m_Parent.Refresh();
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		if (m_Entity != m_FoldFor)
		{
			m_FoldFor = m_Entity;
			bool v_Want = CollapseStore.IsCollapsed(FoldKey());
			if (v_Want != m_FoldCollapsed)
			{
				m_FoldCollapsed = v_Want;
				if (!v_Want)
					FoldShowBody(true);
			}
		}

		// Held shut every frame: LayoutHolder.OnShow on any child shows its parent widget, which is this
		// body, and vanilla shows children from many places (Open, LockCargo, ShowCargo, a header drop).
		// Only acts when something actually re-opened it.
		if (m_FoldCollapsed && m_MainWidget && m_MainWidget.IsVisible())
			FoldShowBody(false);

		if (m_ClosableHeader)
			m_ClosableHeader.FoldUpdate(m_FoldCollapsed);
	}
}

// A vehicle's category rows share the vehicle's type, so each row is remembered by its category too.
modded class AttachmentCategoriesRow
{
	override string FoldKey()
	{
		string v_Key = super.FoldKey();
		if (v_Key == "")
			return "";
		return v_Key + ":" + m_CategoryIdentifier;
	}
}

// ---- the bars' toggles -----------------------------------------------------------------------------------
// Deluxe's header buttons (SortButton), placed by slot from the bar's right edge in Deluxe's order;
// the full slot map is in TransferHooks.c. EQUIPMENT: show/close all in slot 2, collapse/expand all in
// slot 3. VICINITY: show/close all in slot 0, collapse/expand all in slot 1.
class FoldBarToggles : Managed
{
	protected ref SortButton	m_FoldEye;
	protected ref SortButton	m_FoldAll;
	protected int					m_FoldSide;

	void FoldBarToggles(Widget v_Bar, int v_Side, int v_EyeSlot, int v_AllSlot)
	{
		m_FoldSide = v_Side;
		if (!v_Bar)
			return;

		m_FoldEye = new SortButton(v_Bar, "Eye", FoldStrings.SHOW_ALL_TIP, 0);
		m_FoldEye.PlaceAt(SortButton.EDGE_PAD + SortButton.SLOT_W * v_EyeSlot);
		if (m_FoldEye.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_FoldEye.GetButton(), this, "FoldOnEyeClick");

		m_FoldAll = new SortButton(v_Bar, "ArrowLineUp", FoldStrings.COLLAPSE_ALL_TIP, 0);
		m_FoldAll.PlaceAt(SortButton.EDGE_PAD + SortButton.SLOT_W * v_AllSlot);
		if (m_FoldAll.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_FoldAll.GetButton(), this, "FoldOnAllClick");
	}

	void FoldOnEyeClick(Widget w, int x, int y, int button)
	{
		if (FoldCards.AnyClosedOnSide(m_FoldSide))
			FoldCards.ShowAll(m_FoldSide);
		else
			FoldCards.CloseAll(m_FoldSide);
		Paint();
	}

	void FoldOnAllClick(Widget w, int x, int y, int button)
	{
		FoldCards.SetSideCollapsed(m_FoldSide, FoldCards.AnyExpandedOnSide(m_FoldSide));
		Paint();
	}

	// Every frame from the bar's owner. SetIdle only repaints on a change.
	void Update()
	{
		Paint();
		if (m_FoldEye)
			m_FoldEye.Layout();
		if (m_FoldAll)
			m_FoldAll.Layout();
	}

	protected void Paint()
	{
		if (m_FoldEye)
		{
			if (FoldCards.AnyClosedOnSide(m_FoldSide))
				m_FoldEye.SetIdle("Eye", FoldStrings.SHOW_ALL_TIP);
			else
				m_FoldEye.SetIdle("EyeSlash", FoldStrings.CLOSE_ALL_TIP);
		}
		if (m_FoldAll)
		{
			if (FoldCards.AnyExpandedOnSide(m_FoldSide))
				m_FoldAll.SetIdle("ArrowLineUp", FoldStrings.COLLAPSE_ALL_TIP);
			else
				m_FoldAll.SetIdle("ArrowLineDown", FoldStrings.EXPAND_ALL_TIP);
		}
	}
}

modded class PlayerContainer
{
	static const int FOLD_EQ_EYE_SLOT = 2;
	static const int FOLD_EQ_ALL_SLOT = 3;

	protected ref FoldBarToggles m_FoldBar;

	// Vanilla hands the EQUIPMENT bar to the slot group and clears its own header, so the bar is found
	// through the group (the same way TransferHooks.c finds it).
	void PlayerContainer(LayoutHolder parent, int sort = -1)
	{
		if (!m_PlayerAttachmentsContainer)
			return;
		Header v_Header = m_PlayerAttachmentsContainer.GetHeader();
		if (!v_Header || !v_Header.GetMainWidget())
			return;
		m_FoldBar = new FoldBarToggles(v_Header.GetMainWidget(), FoldCards.SIDE_PLAYER, FOLD_EQ_EYE_SLOT, FOLD_EQ_ALL_SLOT);
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		if (m_FoldBar)
			m_FoldBar.Update();
	}
}

modded class VicinityContainer
{
	static const int FOLD_VIC_EYE_SLOT = 0;
	static const int FOLD_VIC_ALL_SLOT = 1;

	protected ref FoldBarToggles m_FoldBar;

	void VicinityContainer(LayoutHolder parent, int sort = -1)
	{
		if (!GetHeader() || !GetHeader().GetMainWidget())
			return;
		m_FoldBar = new FoldBarToggles(GetHeader().GetMainWidget(), FoldCards.SIDE_VICINITY, FOLD_VIC_EYE_SLOT, FOLD_VIC_ALL_SLOT);
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		if (m_FoldBar)
			m_FoldBar.Update();
	}
}
