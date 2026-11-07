// A small button in a cargo container's header, plus the chip over it that says what the runner
// behind it is doing ("Sorting 3/9", "Sorted", "No room to sort") or, while idle and hovered, what
// the button does. It only shows state; the work is SortRunner or StackRunner, and the
// click is routed to the owning container by SortHooks.c. Each cargo header has two of
// these, left of vanilla's X: sort, then stack, so the idle icon, the hover tip and the offset
// from the header's right edge are per instance. The EQUIPMENT bar's Sort all button is one too
// (SortAllBar in SortHooks.c), placed with PlaceAt.
//
// ---- the chip ---------------------------------------------------------------------------------
// It sits ABOVE the button, centred on it, clear of the header: its bottom a small gap over the
// header's top edge and slid sideways so it never hangs past the header's left or right edge. Item
// headers clip their children (closable_header.layout's PanelWidget) and the EQUIPMENT bar sits in a
// clipping SlotsHeader, so a chip inside the header could never be drawn above it. It is therefore
// its own small layout (SortChip.layout) parented to the inventory menu's root, created the first
// time it is needed and placed in screen space from the button's and the header's screen rects.
//
// Managed so the runner's raw static pointer and the queued CallLater both see null once the header
// that owns this is destroyed, instead of a dangling pointer.
class SortButton : Managed
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/sort/SortButton.layout";
	static const string Chip_Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/sort/SortChip.layout";

	static const int STATE_IDLE = 0;
	static const int STATE_RUNNING = 1;
	static const int STATE_DONE = 2;
	static const int STATE_FAILED = 3;

	// How long a finished or failed result stays on the button before it goes back to idle.
	static const int FLASH_MS = 1500;
	// Gap between the button and whatever vanilla shows at the right edge of the header.
	static const float EDGE_PAD = 10;
	static const float SPACER_GAP = 4;
	// One button plus the gap: how far the second button sits from the first.
	static const float SLOT_W = 33;
	// On the EQUIPMENT and VICINITY bars (collapsible_header.layout: 60 px tall, the last 5 px its
	// Border strip) the buttons centre on the 55 px blue band above the strip, which is exactly a card
	// header's height. The slot grid below also overlaps the bar's lower edge, so the visible blue is
	// shorter still; measured in game, the buttons centre on it at 6.5 px up from the bar's centre.
	static const float BAR_Y = -6.5;
	// The button box's authored side in the layout. Layout sizes are scaled by the engine but script
	// SetPos takes physical px, so every script offset is multiplied by the loaded box over this
	// (m_Scale): the boxes then sit on exactly the same grid as the layout-placed X at any
	// resolution or UI scale.
	static const float BOX_W = 29;

	// The chip, in design px: authored height (the yardstick for the layout scale), fallback width
	// for a text that has not been measured yet, padding either side of the text, the gap between the
	// chip and the header's top edge, and how far it stays in from the header's sides.
	static const float CHIP_H = 23;
	static const float CHIP_W = 130;
	static const float CHIP_PAD = 10;
	static const float CHIP_GAP = 4;
	static const float CHIP_INSET = 6;

	protected Widget		m_Root;
	protected Widget		m_Header;
	protected ButtonWidget	m_Btn;
	protected Widget		m_Bg;
	protected ImageWidget	m_Icon;
	protected Widget		m_Chip;
	protected TextWidget	m_ChipText;
	protected Widget		m_RightSpacer;
	protected float			m_LastW = -1;
	protected float			m_ChipScale = 1;
	protected float			m_Scale = 1;
	protected float			m_ChipX = -1;
	protected float			m_ChipY = -1;
	protected float			m_ChipW = -1;
	protected int			m_State = STATE_IDLE;
	protected bool			m_Hover;
	protected bool			m_Active;
	protected string		m_IdleIcon;
	protected string		m_Tooltip;
	protected float			m_Offset;

	// Created into the header's PanelWidget as a sibling of RightSpacer, not inside it: RightSpacer is a
	// two-column GridSpacer (battery icon + X, closable_header.layout), so a third child would wrap.
	// v_Offset pushes the button left (the stack button sits one slot left of sort).
	void SortButton(Widget v_HeaderPanel, string v_IdleIcon, string v_Tip, float v_Offset)
	{
		m_IdleIcon = v_IdleIcon;
		m_Tooltip = v_Tip;
		m_Offset = v_Offset;
		m_Header = v_HeaderPanel;

		if (!v_HeaderPanel)
			return;

		m_Root = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_HeaderPanel);
		if (!m_Root)
			return;

		m_Btn = ButtonWidget.Cast(m_Root.FindAnyWidget("SortBtn"));
		m_Bg = m_Root.FindAnyWidget("SortBg");
		m_Icon = ImageWidget.Cast(m_Root.FindAnyWidget("SortIcon"));
		m_RightSpacer = v_HeaderPanel.FindAnyWidget("RightSpacer");
		float v_BW, v_BH;
		m_Root.GetSize(v_BW, v_BH);
		if (v_BW > 0)
			m_Scale = v_BW / BOX_W;

		// Hover goes through WidgetEventHandler too, because RegisterOnClick has already made it the
		// button's one and only handler.
		if (m_Btn)
		{
			WidgetEventHandler.GetInstance().RegisterOnMouseEnter(m_Btn, this, "HandleEnter");
			WidgetEventHandler.GetInstance().RegisterOnMouseLeave(m_Btn, this, "HandleLeave");
		}

		ApplyState();
	}

	void ~SortButton()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(BackToIdle);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(SpinStep);
		if (m_Chip)
			m_Chip.Unlink();
		if (m_Root)
			m_Root.Unlink();
	}

	ButtonWidget GetButton()
	{
		return m_Btn;
	}

	// Running, or still showing a result.
	bool IsBusy()
	{
		return m_State != STATE_IDLE;
	}

	// The header uses this to keep its reorder arrows up while the mouse moves onto the button.
	bool Owns(Widget v_W)
	{
		return v_W != null && (v_W == m_Btn || v_W == m_Root);
	}

	// Screen x of the button's left edge while it is on screen, or -1. The item header uses it to stop
	// its title short of the button row.
	float GetLeftEdge()
	{
		if (!m_Root || !m_Btn || !m_Root.IsVisible())
			return -1;
		float v_X, v_Y;
		m_Btn.GetScreenPos(v_X, v_Y);
		return v_X;
	}

	// Every frame from the container's UpdateInterval (and from SortAllBar.Update for Sort all).
	// Keeps the chip over the button as the column scrolls. The only thing that moves the button
	// itself is the battery icon appearing or vanishing in RightSpacer, so it is repositioned only
	// when that spacer's width changes.
	void Layout()
	{
		PlaceChip();

		if (!m_Root || !m_RightSpacer)
			return;

		float v_W, v_H;
		m_RightSpacer.GetScreenSize(v_W, v_H);
		if (v_W == m_LastW)
			return;
		m_LastW = v_W;
		m_Root.SetPos((EDGE_PAD + SPACER_GAP + m_Offset) * m_Scale + v_W, 0);
	}

	// Moves the button to another slot from the right edge (a hidden neighbour closes up the row);
	// applied on the next Layout.
	void SetOffset(float v_Offset)
	{
		if (v_Offset == m_Offset)
			return;
		m_Offset = v_Offset;
		m_LastW = -1;
	}

	// For a bar with no RightSpacer to measure from: a fixed offset from the right edge, centred on
	// the bar's blue band (BAR_Y).
	void PlaceAt(float v_X)
	{
		if (m_Root)
			m_Root.SetPos(v_X * m_Scale, BAR_Y * m_Scale);
	}

	// Lit (brand icon over the hover wash) for as long as what the button opens is open.
	void SetActive(bool v_On)
	{
		if (v_On == m_Active)
			return;
		m_Active = v_On;
		ApplyState();
	}

	// Swaps the idle icon and hover tip, for a button whose meaning flips. Only repaints when either
	// actually changed.
	void SetIdle(string v_Icon, string v_Tip)
	{
		if (v_Icon == m_IdleIcon && v_Tip == m_Tooltip)
			return;
		m_IdleIcon = v_Icon;
		m_Tooltip = v_Tip;
		ApplyState();
	}

	void Show(bool v_On)
	{
		if (m_Root && m_Root.IsVisible() != v_On)
			m_Root.Show(v_On);
		// The chip lives outside the header, so it has to be put away with the button.
		if (!v_On && m_Chip && m_Chip.IsVisible())
			m_Chip.Show(false);
	}

	void SetState(int v_State, string v_Text)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(BackToIdle);
		m_State = v_State;
		ApplyState();

		// The running icon spins for as long as the work goes on.
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(SpinStep);
		m_SpinDeg = 0;
		if (m_Icon)
			m_Icon.SetRotation(0, 0, 0);
		if (v_State == STATE_RUNNING)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(SpinStep, 16, true);

		if (v_State == STATE_IDLE)
			return;

		SetChip(v_Text, ARGB(255, 255, 255, 255));
		if (v_State == STATE_DONE || v_State == STATE_FAILED)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(BackToIdle, FLASH_MS, false);
	}

	protected float m_SpinDeg;

	protected void SpinStep()
	{
		if (!m_Icon)
			return;
		m_SpinDeg += 9;
		if (m_SpinDeg >= 360)
			m_SpinDeg -= 360;
		m_Icon.SetRotation(0, 0, m_SpinDeg);
	}

	protected void BackToIdle()
	{
		SetState(STATE_IDLE, "");
	}

	protected int ColorFor(int v_State)
	{
		if (v_State == STATE_RUNNING)
			return colorScheme.BrandColor();
		if (v_State == STATE_DONE)
			return UIColor.Green();
		if (v_State == STATE_FAILED)
			return UIColor.Red();
		// DD_TabletTheme.Text() in Deluxe (#dcd9cf).
		return ARGB(255, 220, 217, 207);
	}

	protected void ApplyState()
	{
		if (m_Icon)
		{
			string v_Image = m_IdleIcon;
			if (m_State == STATE_RUNNING)
				v_Image = "ArrowsClockwise";
			else if (m_State == STATE_DONE)
				v_Image = "Check";
			else if (m_State == STATE_FAILED)
				v_Image = "X";
			m_Icon.LoadImageFile(0, "set:InventoryIcons image:" + v_Image);
			m_Icon.SetImage(0);

			if (m_State == STATE_IDLE && (m_Hover || m_Active))
				m_Icon.SetColor(colorScheme.BrandColor());
			else
				m_Icon.SetColor(ColorFor(m_State));
		}

		ApplyHover();

		// Idle shows the hover tip only while hovered; any other state owns the chip.
		if (m_State == STATE_IDLE)
		{
			if (m_Hover)
				SetChip(m_Tooltip, ARGB(255, 255, 255, 255));
			else if (m_Chip)
				m_Chip.Show(false);
		}
	}

	protected void ApplyHover()
	{
		if (!m_Bg)
			return;
		// Brand colour at alpha 46 out of 255: a soft wash, so the icon stays the brightest thing.
		if (m_Hover || m_Active)
			m_Bg.SetColor((colorScheme.BrandColor() & 0x00FFFFFF) | 0x2E000000);
		else
			m_Bg.SetColor(0);
	}

	// Built on first use under the inventory menu's root, which is full screen and does not clip, and
	// hides with the menu.
	protected bool EnsureChip()
	{
		if (m_Chip)
			return true;

		UIScriptedMenu v_Menu = GetGame().GetUIManager().FindMenu(MENU_INVENTORY);
		if (!v_Menu || !v_Menu.GetLayoutRoot())
			return false;

		m_Chip = GetGame().GetWorkspace().CreateWidgets(Chip_Layout_Path, v_Menu.GetLayoutRoot());
		if (!m_Chip)
			return false;
		m_ChipText = TextWidget.Cast(m_Chip.FindAnyWidget("TipChipText"));

		float v_W, v_H;
		m_Chip.GetSize(v_W, v_H);
		if (v_H > 0)
			m_ChipScale = v_H / CHIP_H;
		return true;
	}

	protected void SetChip(string v_Text, int v_Color)
	{
		if (v_Text == "")
		{
			if (m_Chip)
				m_Chip.Show(false);
			return;
		}
		if (!EnsureChip() || !m_ChipText)
			return;
		m_ChipText.SetText(v_Text);
		m_ChipText.SetColor(v_Color);
		m_Chip.Show(true);
		// A new text is a new width, so the next placement writes even if nothing else moved.
		m_ChipW = -1;
		PlaceChip();
	}

	// Centred over the button, its bottom CHIP_GAP above the header's top edge, slid sideways to stay
	// CHIP_INSET in from the header's sides. If the header is at the very top of the screen the chip
	// drops under the header instead, so it is never cut off.
	protected void PlaceChip()
	{
		if (!m_Chip || !m_Chip.IsVisible() || !m_Btn || !m_Header)
			return;

		float v_BX, v_BY, v_BW, v_BH;
		m_Btn.GetScreenPos(v_BX, v_BY);
		m_Btn.GetScreenSize(v_BW, v_BH);
		float v_HX, v_HY, v_HW, v_HH;
		m_Header.GetScreenPos(v_HX, v_HY);
		m_Header.GetScreenSize(v_HW, v_HH);
		if (v_BW <= 0 || v_HW <= 0)
			return;

		float v_S = m_ChipScale;
		int v_TW, v_TH;
		if (m_ChipText)
			m_ChipText.GetTextSize(v_TW, v_TH);
		// Through a float first, so the sum below is float arithmetic and not int.
		float v_TextW = v_TW;
		float v_CW = CHIP_W * v_S;
		if (v_TextW > 0)
			v_CW = v_TextW + CHIP_PAD * 2 * v_S;
		float v_CH = CHIP_H * v_S;

		float v_X = v_BX + v_BW * 0.5 - v_CW * 0.5;
		float v_Max = v_HX + v_HW - CHIP_INSET * v_S - v_CW;
		float v_Min = v_HX + CHIP_INSET * v_S;
		if (v_X > v_Max)
			v_X = v_Max;
		if (v_X < v_Min)
			v_X = v_Min;

		float v_Y = v_HY - CHIP_GAP * v_S - v_CH;

		float v_PX = 0;
		float v_PY = 0;
		Widget v_Parent = m_Chip.GetParent();
		if (v_Parent)
			v_Parent.GetScreenPos(v_PX, v_PY);
		if (v_Y < v_PY)
			v_Y = v_HY + v_HH + CHIP_GAP * v_S;

		if (v_X == m_ChipX && v_Y == m_ChipY && v_CW == m_ChipW)
			return;
		m_ChipX = v_X;
		m_ChipY = v_Y;
		m_ChipW = v_CW;
		m_Chip.SetSize(v_CW, v_CH);
		m_Chip.SetPos(v_X - v_PX, v_Y - v_PY);
	}

	bool HandleEnter(Widget w, int x, int y)
	{
		m_Hover = true;
		ApplyState();
		return true;
	}

	bool HandleLeave(Widget w, Widget enter_w, int x, int y)
	{
		m_Hover = false;
		ApplyState();
		return true;
	}
}
