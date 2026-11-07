// Wires the transfer tools into the vanilla inventory screen:
// - every cargo card's header takes a still click on its title as "make this the TARGET" and shows
//   the target mark (QuickMove.c);
// - the EQUIPMENT bar gets the Transfer-to-vicinity arrow, the VICINITY bar the Transfer-to-gear arrow
//   and Grab all (ItemTransfer.c, GrabAll.c);
// - the inventory menu ticks the runs every frame and stops them when it closes.
//
// The sort and search features mod the same vanilla classes in their own files. Modded classes chain,
// so every override here calls super, and every member and method added to a vanilla class carries
// the Transfer prefix.

// ---- the target mark on a card's header -------------------------------------------------------------
modded class ClosableHeader
{
	static const string XFER_TARGET_PATH = "Colorful-UI/GUI/layouts/inventory_tools/transfer/TargetMark.layout";
	// Design px between the end of the title text and the TARGET tag.
	static const float XFER_TAG_GAP = 8;

	protected Widget		m_TransferMark;
	protected Widget		m_TransferEdge;
	// The same wash with all four corners round, for a folded card (Deluxe's TargetEdgeFolded).
	protected Widget		m_TransferEdgeFolded;
	protected bool			m_TransferFolded;
	protected TextWidget	m_TransferTag;
	protected bool			m_TransferOn;
	protected bool			m_TransferNoRoom;
	protected float			m_TransferTagX = -1;
	protected bool			m_TransferHooked;

	// Every frame from the owning card. Only touches widgets when the state changes, and the tag's place
	// only when the title's width moved.
	void TransferUpdateTarget(bool v_On, bool v_NoRoom)
	{
		if (v_On != m_TransferOn || v_NoRoom != m_TransferNoRoom)
		{
			m_TransferOn = v_On;
			m_TransferNoRoom = v_NoRoom;
			m_TransferTagX = -1;
			TransferPaintMark();
		}
		if (m_TransferOn)
		{
			TransferPlaceTag();
			// The wash follows the header's shape: top corners only over an open card, all four over a
			// folded one (the fold feature's FoldBarSwapped), so its corners never stand out.
			if (m_FoldBarSwapped != m_TransferFolded)
			{
				m_TransferFolded = m_FoldBarSwapped;
				TransferPaintMark();
			}
		}
	}

	protected void TransferPaintMark()
	{
		if (!m_TransferMark)
		{
			if (!m_TransferOn || !m_PanelWidget)
				return;
			m_TransferMark = GetGame().GetWorkspace().CreateWidgets(XFER_TARGET_PATH, m_PanelWidget);
			if (!m_TransferMark)
				return;
			m_TransferEdge = m_TransferMark.FindAnyWidget("TargetEdge");
			m_TransferEdgeFolded = m_TransferMark.FindAnyWidget("TargetEdgeFolded");
			m_TransferTag = TextWidget.Cast(m_TransferMark.FindAnyWidget("TargetTag"));
		}

		m_TransferMark.Show(m_TransferOn);
		if (!m_TransferOn)
			return;

		// The brand colour as a soft wash over the whole header says "target", at alpha 46 of 255. While
		// it flashes "NO ROOM" it turns rust (#c46b5f) at alpha 38.
		int v_Color = colorScheme.BrandColor();
		int v_Wash = (v_Color & 0x00FFFFFF) | 0x2E000000;
		string v_Text = TransferStrings.TARGET_TAG;
		if (m_TransferNoRoom)
		{
			v_Color = ARGB(255, 196, 107, 95);
			v_Wash = (v_Color & 0x00FFFFFF) | 0x26000000;
			v_Text = TransferStrings.TARGET_NO_ROOM;
		}
		if (m_TransferEdge)
		{
			m_TransferEdge.SetColor(v_Wash);
			m_TransferEdge.Show(!m_TransferFolded);
		}
		if (m_TransferEdgeFolded)
		{
			m_TransferEdgeFolded.SetColor(v_Wash);
			m_TransferEdgeFolded.Show(m_TransferFolded);
		}
		if (m_TransferTag)
		{
			m_TransferTag.SetText(v_Text);
			m_TransferTag.SetColor(v_Color);
		}
	}

	// Just past the end of the title text.
	protected void TransferPlaceTag()
	{
		if (!m_TransferTag || !m_HeaderText || !m_PanelWidget)
			return;
		float v_PX, v_PY;
		m_PanelWidget.GetScreenPos(v_PX, v_PY);
		float v_TX, v_TY;
		m_HeaderText.GetScreenPos(v_TX, v_TY);
		int v_TW, v_TH;
		m_HeaderText.GetTextSize(v_TW, v_TH);
		float v_X = v_TX - v_PX + v_TW + InventoryMenu.GetHeightMultiplied(XFER_TAG_GAP);
		if (v_X == m_TransferTagX)
			return;
		m_TransferTagX = v_X;
		m_TransferTag.SetPos(v_X, 0);
	}

	// The header takes a mouse down and up, routed to the owning card, which turns a still click on the
	// title into a target toggle. Hooked once, only on cards that show a cargo.
	void TransferAttachTarget(Container v_Owner)
	{
		if (m_TransferHooked || !v_Owner || !GetMainWidget())
			return;
		m_TransferHooked = true;
		WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown(GetMainWidget(), v_Owner, "TransferOnHeaderDown");
		WidgetEventHandler.GetInstance().RegisterOnMouseButtonUp(GetMainWidget(), v_Owner, "TransferOnHeaderUp");
	}

	// Whether screen x is on the title text (or its tag), and so not on the item icon, the reorder
	// arrows or any right-hand button. A press on a header button can bubble up to the bar, hence the test.
	bool TransferIsOnTitle(int v_ScreenX)
	{
		if (!m_HeaderText || !m_HeaderText.IsVisible())
			return false;
		float v_TX, v_TY;
		m_HeaderText.GetScreenPos(v_TX, v_TY);
		int v_TW, v_TH;
		m_HeaderText.GetTextSize(v_TW, v_TH);
		float v_Right = v_TX + v_TW;
		if (m_TransferOn && m_TransferTag)
		{
			int v_GW, v_GH;
			m_TransferTag.GetTextSize(v_GW, v_GH);
			v_Right += InventoryMenu.GetHeightMultiplied(XFER_TAG_GAP) + v_GW;
		}
		float v_X = v_ScreenX;
		return v_X >= v_TX && v_X <= v_Right;
	}
}

// ---- the card: its column, its cargo and the header click ------------------------------------------
modded class ClosableContainer
{
	// Which column the card is in (QuickMove.SIDE_*), found once the parent chain reaches one.
	protected int	m_TransferSide;
	// Where a press on the header started, so only a still click (not a header drag) toggles the target.
	protected int	m_TransferDownX;
	protected int	m_TransferDownY;
	protected bool	m_TransferDownOnTitle;

	static const int XFER_CLICK_SLOP = 4;

	EntityAI TransferGetEntity()
	{
		return m_Entity;
	}

	// The cargo index this card can take quick transfers into, or -1 if it is not a cargo card.
	int TransferTargetIdx()
	{
		return -1;
	}

	// RightArea is the player's own column, LeftArea the vicinity's; anything else (hands) is neither.
	int TransferSide()
	{
		if (m_TransferSide != QuickMove.SIDE_NONE)
			return m_TransferSide;
		LayoutHolder v_P = GetParent();
		while (v_P)
		{
			if (RightArea.Cast(v_P))
			{
				m_TransferSide = QuickMove.SIDE_PLAYER;
				break;
			}
			if (LeftArea.Cast(v_P))
			{
				m_TransferSide = QuickMove.SIDE_VICINITY;
				break;
			}
			v_P = v_P.GetParent();
		}
		return m_TransferSide;
	}

	// Cargo cards that may be targets: a real cargo, on the player or on nobody alive.
	static bool TransferCanOffer(EntityAI v_Owner, int v_Idx)
	{
		if (!v_Owner || v_Idx < 0)
			return false;
		CargoBase v_Cargo = v_Owner.GetInventory().GetCargoFromIndex(v_Idx);
		if (!v_Cargo || v_Cargo.IsProxyCargo())
			return false;
		Man v_Root = v_Owner.GetHierarchyRootPlayer();
		if (!v_Root || v_Root == GetGame().GetPlayer())
			return true;
		return !v_Root.IsAlive();
	}

	void TransferOnHeaderDown(Widget w, int x, int y, int button)
	{
		m_TransferDownOnTitle = false;
		if (button != MouseState.LEFT || !m_ClosableHeader || TransferTargetIdx() < 0)
			return;
		m_TransferDownOnTitle = m_ClosableHeader.TransferIsOnTitle(x);
		m_TransferDownX = x;
		m_TransferDownY = y;
	}

	void TransferOnHeaderUp(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT || !m_TransferDownOnTitle)
			return;
		m_TransferDownOnTitle = false;
		if (Math.AbsInt(x - m_TransferDownX) > XFER_CLICK_SLOP || Math.AbsInt(y - m_TransferDownY) > XFER_CLICK_SLOP)
			return;
		if (ItemManager.GetInstance().IsDragging())
			return;
		QuickMove.ToggleTarget(this);
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		if (m_ClosableHeader)
			m_ClosableHeader.TransferUpdateTarget(QuickMove.IsTarget(this), QuickMove.IsFlashing(this));
	}
}

modded class ContainerWithCargo
{
	protected bool m_TransferCanTarget;

	override void SetEntity(EntityAI entity, int cargo_index = 0, bool immedUpdate = true)
	{
		super.SetEntity(entity, cargo_index, immedUpdate);

		// m_CargoIndex, not the grid's own copy: vanilla passes immedUpdate into the grid's cargo index
		// argument (containerwithcargo.c:196), so that one is not trustworthy.
		m_TransferCanTarget = TransferCanOffer(entity, m_CargoIndex);
		if (m_TransferCanTarget && m_ClosableHeader)
			m_ClosableHeader.TransferAttachTarget(this);
	}

	override int TransferTargetIdx()
	{
		if (!m_TransferCanTarget)
			return -1;
		return m_CargoIndex;
	}
}

modded class ContainerWithCargoAndAttachments
{
	protected bool m_TransferCanTarget;

	override void SetEntity(EntityAI entity, bool immedUpdate = true)
	{
		super.SetEntity(entity, immedUpdate);

		// This class only ever shows cargo index 0.
		m_TransferCanTarget = m_CargoGrid != null && TransferCanOffer(entity, 0);
		if (m_TransferCanTarget && m_ClosableHeader)
			m_ClosableHeader.TransferAttachTarget(this);
	}

	override int TransferTargetIdx()
	{
		if (!m_TransferCanTarget)
			return -1;
		return 0;
	}
}

// ---- the bars' buttons -------------------------------------------------------------------------------
// Places count from the bar's right edge (TransferButton.SLOT_W apart), in Deluxe's order with its
// pin and lock slots (not ported) closed up. Vanilla's own +/- is hidden on both bars in Colorful UI
// (collapsible_header.layout), so it takes no place.
// EQUIPMENT, right to left: 0 Search, 1 Sort all, 2 Show/close all, 3 Collapse/expand all, 4 Transfer.
// VICINITY, right to left: 0 Show/close all, 1 Collapse/expand all, 2 Grab all, 3 Transfer.
modded class PlayerContainer
{
	static const int XFER_EQ_TRANSFER_SLOT = 4;

	// Transfer to vicinity, lit while Transfer mode is on.
	protected ref TransferButton m_TransferEqTransfer;

	// Vanilla's constructor hands the EQUIPMENT bar to the slot group and clears its own header
	// (playercontainer.c: SetHeader(GetHeader()) on the group, then SetHeader(null)), so the bar is
	// found through the group.
	void PlayerContainer(LayoutHolder parent, int sort = -1)
	{
		if (!m_PlayerAttachmentsContainer)
			return;
		Header v_Header = m_PlayerAttachmentsContainer.GetHeader();
		if (!v_Header || !v_Header.GetMainWidget())
			return;
		m_TransferEqTransfer = new TransferButton(v_Header.GetMainWidget(), "ArrowLeft", TransferStrings.TRANSFER_TO_VICINITY_TIP, XFER_EQ_TRANSFER_SLOT);
		if (m_TransferEqTransfer.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_TransferEqTransfer.GetButton(), this, "TransferOnEqTransferClick");
	}

	void TransferOnEqTransferClick(Widget w, int x, int y, int button)
	{
		ItemTransfer.OnArrow(QuickMove.SIDE_PLAYER, m_TransferEqTransfer);
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		if (m_TransferEqTransfer)
		{
			m_TransferEqTransfer.SetActive(TransferMode.IsOn());
			m_TransferEqTransfer.Layout();
		}
	}
}

modded class VicinityContainer
{
	static const int XFER_VIC_GRAB_SLOT = 2;
	static const int XFER_VIC_TRANSFER_SLOT = 3;

	// Grab all: every loose item on the ground into the player's gear (GrabAll).
	protected ref TransferButton m_TransferVicGrab;
	// Transfer to gear, lit while Transfer mode is on.
	protected ref TransferButton m_TransferVicTransfer;

	void VicinityContainer(LayoutHolder parent, int sort = -1)
	{
		if (!GetHeader() || !GetHeader().GetMainWidget())
			return;
		Widget v_Bar = GetHeader().GetMainWidget();

		m_TransferVicGrab = new TransferButton(v_Bar, "HandGrabbing", TransferStrings.GRAB_ALL_TIP, XFER_VIC_GRAB_SLOT);
		if (m_TransferVicGrab.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_TransferVicGrab.GetButton(), this, "TransferOnVicGrabClick");

		m_TransferVicTransfer = new TransferButton(v_Bar, "ArrowRight", TransferStrings.TRANSFER_TO_GEAR_TIP, XFER_VIC_TRANSFER_SLOT);
		if (m_TransferVicTransfer.GetButton())
			WidgetEventHandler.GetInstance().RegisterOnClick(m_TransferVicTransfer.GetButton(), this, "TransferOnVicTransferClick");
	}

	void TransferOnVicGrabClick(Widget w, int x, int y, int button)
	{
		GrabAll.Start(m_TransferVicGrab);
	}

	void TransferOnVicTransferClick(Widget w, int x, int y, int button)
	{
		ItemTransfer.OnArrow(QuickMove.SIDE_VICINITY, m_TransferVicTransfer);
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();

		if (m_TransferVicGrab)
			m_TransferVicGrab.Layout();
		if (m_TransferVicTransfer)
		{
			m_TransferVicTransfer.SetActive(TransferMode.IsOn());
			m_TransferVicTransfer.Layout();
		}
	}
}

// ---- the menu's lifecycle ------------------------------------------------------------------------------
modded class InventoryMenu
{
	// The "CTRL + HOVER  Quick move" hint, built the first time the menu shows, on its root.
	protected ref HoverHint m_TransferHoverHint;

	override void OnShow()
	{
		super.OnShow();

		if (!m_TransferHoverHint && layoutRoot)
			m_TransferHoverHint = new HoverHint(layoutRoot);
		if (m_TransferHoverHint)
			m_TransferHoverHint.OnShow();
	}

	// Before super, so a run in flight stops rather than carrying on with nothing on screen, and the
	// mode and its picks are gone before vanilla tears the screen down.
	override void OnHide()
	{
		GrabAll.Abort();
		ItemTransfer.Abort();
		TransferMode.Off();
		HoverMove.Reset();

		super.OnHide();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		GrabAll.Tick();
		ItemTransfer.Tick();
		// Targets whose card went away clear themselves.
		QuickMove.Tick();
		// Ctrl held on an item: the spinner, then the move.
		HoverMove.Tick(timeslice);
		// The hint's look follows Ctrl and the search box.
		if (m_TransferHoverHint)
			m_TransferHoverHint.Tick();
	}
}
