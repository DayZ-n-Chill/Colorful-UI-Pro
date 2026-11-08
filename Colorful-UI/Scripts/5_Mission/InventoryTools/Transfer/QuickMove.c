// Ctrl+click quick transfer, with one TARGET card per side.
//
// Clicking a cargo card's header title makes it the target for its side: one on the player side
// (RightArea: anything worn) and one on the vicinity side (LeftArea: car cargo, a crate, a bag on the
// ground, a body's clothing). Clicking it again clears it, and a target whose card goes away, closes
// or stops being a cargo card clears itself on the next frame (Tick).
//
// Ctrl+left-click on an item then moves it across:
// - from the player's gear to the vicinity target, or, with no vicinity target, vanilla's drop;
// - from the vicinity to the player target, or, with no player target, the first worn container it
//   fits in: vest, pants, jacket, backpack, then any other worn cargo.
// A target with no room flashes "NO ROOM" on its header and nothing moves. Every move is one real
// inventory move through InventoryLocation, and a click while the previous move is still settling is
// ignored, so there is only ever one in flight.
//
// A plain double click on a vicinity item (or the one in hands) goes into the player-side target
// when one is set; with none, vanilla's double click runs as before.
class QuickMove
{
	static const int SIDE_NONE = 0;
	static const int SIDE_PLAYER = 1;
	static const int SIDE_VICINITY = 2;

	// How long "NO ROOM" stays on a target header.
	static const int FLASH_MS = 1500;

	// Raw on purpose: cards are Managed, so a destroyed card reads as null.
	protected static ClosableContainer s_PlayerTarget;
	protected static ClosableContainer s_VicinityTarget;
	protected static ClosableContainer s_FlashCard;
	protected static int s_FlashUntil;

	static bool IsTarget(ClosableContainer v_Card)
	{
		if (!v_Card)
			return false;
		return v_Card == s_PlayerTarget || v_Card == s_VicinityTarget;
	}

	static bool IsFlashing(ClosableContainer v_Card)
	{
		if (!v_Card || v_Card != s_FlashCard)
			return false;
		return GetGame().GetTime() < s_FlashUntil;
	}

	// From a still click on the card's header title.
	static void ToggleTarget(ClosableContainer v_Card)
	{
		if (!v_Card || !CanTarget(v_Card))
			return;

		int v_Side = v_Card.TransferSide();
		if (v_Side == SIDE_PLAYER)
		{
			if (s_PlayerTarget == v_Card)
				s_PlayerTarget = null;
			else
				s_PlayerTarget = v_Card;
		}
		else if (v_Side == SIDE_VICINITY)
		{
			if (s_VicinityTarget == v_Card)
				s_VicinityTarget = null;
			else
				s_VicinityTarget = v_Card;
		}
	}

	// A card can be a target while it is open, shows a real cargo, and sits in one of the two columns.
	static bool CanTarget(ClosableContainer v_Card)
	{
		if (!v_Card || !v_Card.IsOpened() || v_Card.TransferTargetIdx() < 0)
			return false;
		if (v_Card.TransferSide() == SIDE_NONE)
			return false;
		EntityAI v_E = v_Card.TransferGetEntity();
		if (!v_E)
			return false;
		return v_E.GetInventory().GetCargoFromIndex(v_Card.TransferTargetIdx()) != null;
	}

	// Every frame from InventoryMenu.Update: a target that went away clears itself.
	static void Tick()
	{
		if (s_PlayerTarget && !CanTarget(s_PlayerTarget))
			s_PlayerTarget = null;
		if (s_VicinityTarget && !CanTarget(s_VicinityTarget))
			s_VicinityTarget = null;
	}

	// Ctrl+left-click on an item. True when the click was taken (whether or not anything moved), false
	// to leave it to vanilla, which drops the item.
	static bool OnItemCtrlClick(EntityAI v_Item)
	{
		PlayerBase v_Player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!v_Player || !v_Item)
			return false;
		if (v_Item == v_Player.GetHumanInventory().GetEntityInHands())
			return false;

		Tick();

		if (v_Item.GetHierarchyRootPlayer() == v_Player)
		{
			if (!s_VicinityTarget)
				return false;
			if (CanMove(v_Player, v_Item))
				MoveToCard(v_Player, v_Item, s_VicinityTarget);
			return true;
		}

		if (!CanMove(v_Player, v_Item))
			return true;
		if (s_PlayerTarget)
			MoveToCard(v_Player, v_Item, s_PlayerTarget);
		else
			MoveToWorn(v_Player, v_Item);
		return true;
	}

	// A plain double click, vanilla's "take it": an item from the vicinity, or the one in hands, goes
	// into the player-side TARGET card when there is one. True when it was taken here; false leaves it
	// to vanilla, which puts it wherever it fits (or, for gear, into hands).
	static bool OnItemDoubleClick(EntityAI v_Item)
	{
		PlayerBase v_Player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!v_Player || !v_Item)
			return false;
		Tick();
		if (!s_PlayerTarget)
			return false;
		bool v_InHands = v_Item == v_Player.GetHumanInventory().GetEntityInHands();
		if (!v_InHands && v_Item.GetHierarchyRootPlayer() == v_Player)
			return false;
		if (CanMove(v_Player, v_Item))
			MoveToCard(v_Player, v_Item, s_PlayerTarget);
		return true;
	}

	// ---- grab all (GrabAll) --------------------------------------------------------------------
	// Where a ground item would go on the player: the player-side TARGET card if one is set, then the
	// worn cargo order quick transfer uses, then any free attachment slot (a rifle on a shoulder).
	static bool FindGrabSpot(PlayerBase v_Player, EntityAI v_Item, InventoryLocation v_Dst)
	{
		Tick();
		if (s_PlayerTarget && FindSpot(s_PlayerTarget.TransferGetEntity(), s_PlayerTarget.TransferTargetIdx(), v_Item, v_Dst))
			return true;
		return FindWornSpot(v_Player, v_Item, v_Dst);
	}

	// The worn cargo order quick transfer uses, then any free attachment slot.
	static bool FindWornSpot(PlayerBase v_Player, EntityAI v_Item, InventoryLocation v_Dst)
	{
		if (FindWornCargoSpot(v_Player, v_Item, v_Dst))
			return true;
		return v_Player.GetInventory().FindFreeLocationFor(v_Item, FindInventoryLocationType.ATTACHMENT, v_Dst);
	}

	static bool CanGrab(PlayerBase v_Player, EntityAI v_Item)
	{
		return CanMove(v_Player, v_Item);
	}

	static void IssueGrab(PlayerBase v_Player, EntityAI v_Item, InventoryLocation v_Dst)
	{
		Issue(v_Player, v_Item, v_Dst);
	}

	// ---- transfer (ItemTransfer) -------------------------------------------------------------------
	// The TARGET card on a side, or null; one that went away is cleared first.
	static ClosableContainer TargetOn(int v_Side)
	{
		Tick();
		if (v_Side == SIDE_PLAYER)
			return s_PlayerTarget;
		if (v_Side == SIDE_VICINITY)
			return s_VicinityTarget;
		return null;
	}

	static bool FindCardSpot(ClosableContainer v_Card, EntityAI v_Item, InventoryLocation v_Dst)
	{
		if (!v_Card)
			return false;
		return FindSpot(v_Card.TransferGetEntity(), v_Card.TransferTargetIdx(), v_Item, v_Dst);
	}

	// The player can start a move now: nothing is in flight and no reservation is held. HoverMove waits
	// on this so queued items go one after another instead of being refused.
	static bool IsPlayerFree(PlayerBase v_Player)
	{
		if (!v_Player || !v_Player.CanManipulateInventory() || !ScriptInputUserData.CanStoreInputUserData())
			return false;
		if (v_Player.GetDayZPlayerInventory().IsProcessing())
			return false;
		return v_Player.GetInventory().GetAnyInventoryReservationCount() == 0;
	}

	// The player is free to move something, no transfer run is moving items, and the item can leave
	// where it is.
	protected static bool CanMove(PlayerBase v_Player, EntityAI v_Item)
	{
		if (!IsPlayerFree(v_Player))
			return false;

		if (!ItemBase.Cast(v_Item) || !v_Item.IsTakeable())
			return false;
		if (v_Item.GetInventory().IsInventoryLocked() || !v_Item.GetInventory().CanRemoveEntity())
			return false;
		return !GameInventory.HasInventoryReservation(v_Item, null);
	}

	protected static void MoveToCard(PlayerBase v_Player, EntityAI v_Item, ClosableContainer v_Card)
	{
		InventoryLocation v_Dst = new InventoryLocation;
		if (!FindSpot(v_Card.TransferGetEntity(), v_Card.TransferTargetIdx(), v_Item, v_Dst))
		{
			s_FlashCard = v_Card;
			s_FlashUntil = GetGame().GetTime() + FLASH_MS;
			return;
		}
		Issue(v_Player, v_Item, v_Dst);
	}

	protected static void MoveToWorn(PlayerBase v_Player, EntityAI v_Item)
	{
		InventoryLocation v_Dst = new InventoryLocation;
		if (FindWornCargoSpot(v_Player, v_Item, v_Dst))
			Issue(v_Player, v_Item, v_Dst);
	}

	// Vest, pants, jacket, backpack, then whatever else is worn with a cargo.
	protected static bool FindWornCargoSpot(PlayerBase v_Player, EntityAI v_Item, InventoryLocation v_Dst)
	{
		TStringArray v_Slots = {"Vest", "Legs", "Body", "Back"};
		array<EntityAI> v_Tried = new array<EntityAI>;
		foreach (string v_Slot : v_Slots)
		{
			EntityAI v_Worn = v_Player.FindAttachmentBySlotName(v_Slot);
			if (!v_Worn)
				continue;
			v_Tried.Insert(v_Worn);
			if (TryWorn(v_Worn, v_Item, v_Dst))
				return true;
		}
		int v_Count = v_Player.GetInventory().AttachmentCount();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			EntityAI v_Att = v_Player.GetInventory().GetAttachmentFromIndex(v_I);
			if (!v_Att || v_Tried.Find(v_Att) >= 0)
				continue;
			if (TryWorn(v_Att, v_Item, v_Dst))
				return true;
		}
		return false;
	}

	protected static bool TryWorn(EntityAI v_Worn, EntityAI v_Item, InventoryLocation v_Dst)
	{
		if (v_Worn == v_Item || !v_Worn.GetInventory().GetCargo())
			return false;
		return FindSpot(v_Worn, 0, v_Item, v_Dst);
	}

	// First cell, row by row, where the item can go: in the orientation it has now, then turned. The
	// engine's own move check decides, so bounds, overlaps and what the container accepts are all its.
	protected static bool FindSpot(EntityAI v_Owner, int v_Idx, EntityAI v_Item, InventoryLocation v_Dst)
	{
		if (!v_Owner || v_Idx < 0 || v_Owner == v_Item)
			return false;
		CargoBase v_Cargo = v_Owner.GetInventory().GetCargoFromIndex(v_Idx);
		if (!v_Cargo)
			return false;

		InventoryLocation v_Src = new InventoryLocation;
		if (!v_Item.GetInventory().GetCurrentInventoryLocation(v_Src))
			return false;

		int v_W, v_H;
		g_Game.GetInventoryItemSize(InventoryItem.Cast(v_Item), v_W, v_H);
		bool v_First = v_Item.GetInventory().GetFlipCargo();
		int v_Rows = v_Cargo.GetHeight();
		int v_Cols = v_Cargo.GetWidth();
		int v_Passes = 2;
		if (v_W == v_H)
			v_Passes = 1;

		bool v_Flip;
		int v_R;
		int v_C;
		for (int v_Pass = 0; v_Pass < v_Passes; v_Pass++)
		{
			v_Flip = v_First;
			if (v_Pass == 1)
				v_Flip = !v_First;
			for (v_R = 0; v_R < v_Rows; v_R++)
			{
				for (v_C = 0; v_C < v_Cols; v_C++)
				{
					v_Dst.SetCargoAuto(v_Cargo, v_Item, v_R, v_C, v_Flip);
					if (GameInventory.LocationCanMoveEntity(v_Src, v_Dst))
						return true;
				}
			}
		}
		return false;
	}

	// Through vanilla's take-or-split, so an oversized stack splits the way a drag would.
	protected static void Issue(PlayerBase v_Player, EntityAI v_Item, InventoryLocation v_Dst)
	{
		if (v_Item.GetInventory().GetFlipCargo() != v_Dst.GetFlip())
			v_Item.GetInventory().SetFlipCargo(v_Dst.GetFlip());
		SplitItemUtils.TakeOrSplitToInventoryLocation(v_Player, v_Dst);
	}
}

// Ctrl+left-click on an item in the vicinity list (the ground items). Vanilla runs this on mouse up
// (vicinityslotscontainer.c:26) and a left click there opens the item's card; with Ctrl held it is a
// quick transfer instead. In Transfer mode (TransferMode.c) the press picks the item on mouse down,
// and the mouse up (which would open its card) and the double click (which would pick it up) are
// swallowed.
modded class VicinitySlotsContainer
{
	override void MouseButtonDown(Widget w, int x, int y, int button)
	{
		if (!InventoryTransfer)
		{
			super.MouseButtonDown(w, x, y, button);
			return;
		}
		if (TransferMode.TakeClick(w, button, TransferMode.ItemOf(w)))
			return;
		super.MouseButtonDown(w, x, y, button);
	}

	override void DoubleClick(Widget w, int x, int y, int button)
	{
		if (!InventoryTransfer)
		{
			super.DoubleClick(w, x, y, button);
			return;
		}
		if (TransferMode.Eats(button))
			return;
		if (button == MouseState.LEFT && !TransferMode.CtrlDown() && QuickMove.OnItemDoubleClick(TransferMode.ItemOf(w)))
		{
			HideOwnedTooltip();
			return;
		}
		super.DoubleClick(w, x, y, button);
	}

	override void MouseClick(Widget w, int x, int y, int button)
	{
		if (!InventoryTransfer)
		{
			super.MouseClick(w, x, y, button);
			return;
		}
		if (TransferMode.Eats(button))
			return;
		if (button == MouseState.LEFT && TransferMode.CtrlDown() && w)
		{
			string v_Name = w.GetName();
			v_Name.Replace("PanelWidget", "Render");
			ItemPreviewWidget v_Ipw = ItemPreviewWidget.Cast(w.FindAnyWidget(v_Name));
			if (v_Ipw && v_Ipw.GetItem() && QuickMove.OnItemCtrlClick(v_Ipw.GetItem()))
			{
				HideOwnedTooltip();
				return;
			}
		}
		super.MouseClick(w, x, y, button);
	}
}

// Cargo grid cells. Transfer mode takes a plain left click as a pick; Ctrl+left-click is a quick
// transfer to the other side's target, and when that declines (gear with no vicinity target)
// vanilla's drop runs as before.
modded class Icon
{
	protected Widget	m_TransferPickBadge;
	protected bool		m_TransferPickShown;

	override void MouseClick(Widget w, int x, int y, int button)
	{
		if (!InventoryTransfer)
		{
			super.MouseClick(w, x, y, button);
			return;
		}
		// A reserved placeholder is swallowed but not picked.
		EntityAI v_PickItem = m_Item;
		if (m_Lock)
			v_PickItem = null;
		if (TransferMode.TakeClick(w, button, v_PickItem))
			return;

		if (button == MouseState.LEFT && TransferMode.CtrlDown() && !m_Lock && m_Item)
		{
			if (QuickMove.OnItemCtrlClick(m_Item))
			{
				ItemManager.GetInstance().SetWidgetDraggable(w, false);
				return;
			}
		}
		super.MouseClick(w, x, y, button);
	}

	// In Transfer mode the second click of a double click only picks (in MouseClick); it must not also
	// take the item to hands. Otherwise a plain double click on a vicinity item or the one in hands goes
	// into the player-side TARGET card when one is set, and to vanilla when none is.
	override void DoubleClick(Widget w, int x, int y, int button)
	{
		if (!InventoryTransfer)
		{
			super.DoubleClick(w, x, y, button);
			return;
		}
		if (TransferMode.Eats(button))
			return;
		if (button == MouseState.LEFT && !TransferMode.CtrlDown() && QuickMove.OnItemDoubleClick(m_Obj))
			return;
		super.DoubleClick(w, x, y, button);
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();
		if (!InventoryTransfer)
			return;

		EntityAI v_E = EntityAI.Cast(GetObject());
		bool v_Pick = !m_Lock && !IsDragged() && ItemTransfer.IsPicked(v_E);
		if (v_Pick != m_TransferPickShown)
		{
			m_TransferPickShown = v_Pick;
			m_TransferPickBadge = TransferPickMark.Paint(GetMainWidget(), m_TransferPickBadge, v_Pick, false);
		}
	}
}
