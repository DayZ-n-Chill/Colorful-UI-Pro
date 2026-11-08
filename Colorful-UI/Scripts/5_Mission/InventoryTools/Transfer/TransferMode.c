// Transfer mode, switched by the Transfer arrows on the EQUIPMENT and VICINITY bars (ItemTransfer.c).
// While it is on, a plain left-click on any item cell (player gear, vicinity, cargo, attachment slot,
// hands) picks or unpicks that item instead of the normal click: nothing moves, opens or drags.
// Ctrl+clicks are left alone, so quick transfer still works. Closing the inventory turns it off.
//
// Vanilla spreads item clicks over many handlers (mouse down on some cells, mouse up on others, plus
// double clicks and the slot icons' open/close toggles), so each one is guarded below: the handler
// that sees the press first flips the pick, every other one for that press is swallowed. Every
// override hands the click to super whenever the mode is off, so vanilla and the other inventory
// features see every click they saw before.
class TransferMode
{
	protected static bool s_TransferOn;
	// The same press can reach more than one handler; it flips the pick once.
	protected static EntityAI s_TransferLastItem;
	protected static int s_TransferLastTime = -1;

	static bool IsOn()
	{
		return s_TransferOn;
	}

	static void Toggle()
	{
		Set(!s_TransferOn);
	}

	static void Off()
	{
		Set(false);
	}

	// Leaving the mode, however it happens, drops its picks.
	protected static void Set(bool v_On)
	{
		if (s_TransferOn && !v_On)
			ItemTransfer.ClearPicks();
		// Never on while transfer is switched off (Settings.c), so every click override below hands
		// straight to super.
		s_TransferOn = v_On && InventoryTransfer;
	}

	static bool CtrlDown()
	{
		return KeyState(KeyCode.KC_LCONTROL) != 0 || KeyState(KeyCode.KC_RCONTROL) != 0;
	}

	// True when a click with this button belongs to the mode and vanilla must not see it.
	static bool Eats(int v_Button)
	{
		return s_TransferOn && v_Button == MouseState.LEFT && !CtrlDown();
	}

	// For vanilla's open/close toggles on mouse up, which are not told the button.
	static bool EatsToggle()
	{
		return s_TransferOn && !CtrlDown();
	}

	// A press on an item cell: picks v_Item and makes sure the cell cannot start a drag. True when the
	// click was taken.
	static bool TakeClick(Widget v_W, int v_Button, EntityAI v_Item)
	{
		if (!Eats(v_Button))
			return false;
		if (v_W)
			ItemManager.GetInstance().SetWidgetDraggable(v_W, false);
		if (v_Item && (v_Item != s_TransferLastItem || GetGame().GetTime() != s_TransferLastTime))
		{
			s_TransferLastItem = v_Item;
			s_TransferLastTime = GetGame().GetTime();
			ItemTransfer.TogglePick(v_Item);
		}
		return true;
	}

	// The item on a slot cell, the way vanilla finds it: the SlotsIcon in the widget's user data, or
	// the ItemPreviewWidget named after the panel ("PanelWidget3" -> "Render3"), or one named "Render".
	static EntityAI ItemOf(Widget v_W)
	{
		if (!v_W)
			return null;
		SlotsIcon v_Icon;
		v_W.GetUserData(v_Icon);
		if (v_Icon)
			return v_Icon.GetEntity();
		string v_Name = v_W.GetName();
		v_Name.Replace("PanelWidget", "Render");
		ItemPreviewWidget v_Ipw = ItemPreviewWidget.Cast(v_W.FindAnyWidget(v_Name));
		if (!v_Ipw)
			v_Ipw = ItemPreviewWidget.Cast(v_W.FindAnyWidget("Render"));
		if (v_Ipw)
			return v_Ipw.GetItem();
		return null;
	}
}

// A picked item's mark: a brand-colour wash and a small icon in the cell's top-left corner, under the
// item picture so the item keeps its own brightness. Made on first use, then only shown and hidden.
class TransferPickMark
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/transfer/TransferPickBadge.layout";

	// v_Round: a slot cell, which is rounded, gets the rounded wash; a grid cell the square one.
	static Widget Paint(Widget v_Cell, Widget v_Badge, bool v_On, bool v_Round)
	{
		if (!v_Badge)
		{
			if (!v_On || !v_Cell)
				return null;
			v_Badge = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_Cell);
			if (!v_Badge)
				return null;
			// The layout holds Deluxe's brass colours.
			Widget v_Sq = v_Badge.FindAnyWidget("XferPickTint");
			Widget v_Rd = v_Badge.FindAnyWidget("XferPickTintRound");
			if (v_Sq)
				v_Sq.Show(!v_Round);
			if (v_Rd)
				v_Rd.Show(v_Round);
		}
		v_Badge.Show(v_On);
		return v_Badge;
	}
}

// The equipment slots: mouse down (playercontainer.c:96), double click (:89) and the open/close
// toggle on mouse up (:149, :165).
modded class PlayerContainer
{
	override void MouseClick(Widget w, int x, int y, int button)
	{
		if (TransferMode.TakeClick(w, button, TransferMode.ItemOf(w)))
			return;
		super.MouseClick(w, x, y, button);
	}

	override void DoubleClick(Widget w, int x, int y, int button)
	{
		if (TransferMode.Eats(button))
			return;
		super.DoubleClick(w, x, y, button);
	}

	override void ToggleWidget(Widget w)
	{
		if (TransferMode.EatsToggle())
			return;
		super.ToggleWidget(w);
	}
}

// Attachment slots on an item card (attachments.c:500-509).
modded class ContainerWithCargoAndAttachments
{
	override void MouseClick2(Widget w, int x, int y, int button)
	{
		if (TransferMode.TakeClick(w, button, TransferMode.ItemOf(w)))
			return;
		super.MouseClick2(w, x, y, button);
	}

	override void DoubleClick(Widget w, int x, int y, int button)
	{
		if (TransferMode.Eats(button))
			return;
		super.DoubleClick(w, x, y, button);
	}
}

// Attachment slots on the item in hands.
modded class HandsContainer
{
	override void MouseClick2(Widget w, int x, int y, int button)
	{
		if (TransferMode.TakeClick(w, button, TransferMode.ItemOf(w)))
			return;
		super.MouseClick2(w, x, y, button);
	}

	override void DoubleClick(Widget w, int x, int y, int button)
	{
		if (TransferMode.Eats(button))
			return;
		super.DoubleClick(w, x, y, button);
	}
}

// Category attachment rows (a vehicle's or a base part's slots).
modded class AttachmentCategoriesRow
{
	override void MouseClick(Widget w, int x, int y, int button)
	{
		if (TransferMode.TakeClick(w, button, TransferMode.ItemOf(w)))
			return;
		super.MouseClick(w, x, y, button);
	}

	override void DoubleClick(Widget w, int x, int y, int button)
	{
		if (TransferMode.Eats(button))
			return;
		super.DoubleClick(w, x, y, button);
	}
}

// A body's worn slots: mouse down, double click, and open/close on mouse up.
modded class ZombieContainer
{
	override void MouseClick(Widget w, int x, int y, int button)
	{
		if (TransferMode.TakeClick(w, button, TransferMode.ItemOf(w)))
			return;
		super.MouseClick(w, x, y, button);
	}

	override void DoubleClick(Widget w, int x, int y, int button)
	{
		if (TransferMode.Eats(button))
			return;
		super.DoubleClick(w, x, y, button);
	}

	override void ToggleContainer(Widget w)
	{
		if (TransferMode.EatsToggle())
			return;
		super.ToggleContainer(w);
	}
}

// Slot cells only see some clicks on mouse up, so a held press could still start a drag; while the
// mode is on it is cancelled before it begins.
modded class SlotsIcon
{
	protected Widget	m_TransferPickBadge;
	protected bool		m_TransferPickShown;

	override void OnIconDrag(Widget w)
	{
		if (TransferMode.IsOn())
		{
			CancelWidgetDragging();
			if (w)
				ItemManager.GetInstance().SetWidgetDraggable(w, false);
			return;
		}
		super.OnIconDrag(w);
	}

	override void UpdateInterval()
	{
		super.UpdateInterval();
		if (!InventoryTransfer)
			return;

		bool v_Pick = !m_IsDragged && ItemTransfer.IsPicked(GetEntity());
		if (v_Pick != m_TransferPickShown)
		{
			m_TransferPickShown = v_Pick;
			m_TransferPickBadge = TransferPickMark.Paint(GetMainWidget(), m_TransferPickBadge, v_Pick, true);
		}
	}
}
