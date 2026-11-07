// A small icon button on the EQUIPMENT or VICINITY bar (the Transfer arrows and Grab all), plus the
// chip over it that says what the run behind it is doing ("Moving 3/9", "Grabbed 5") or, while idle
// and hovered, what the button does. It only shows state; the work is ItemTransfer or GrabAll.
// The transfer feature's own copy of the header button: the sort and search features have theirs.
//
// The chip sits ABOVE the button, centred on it and kept inside the bar's width. The bars clip their
// children, so the chip is its own layout parented to the inventory menu's root, built the first time
// it is needed and placed in screen space from the button's and the bar's screen rects.
//
// Managed so the runners' raw static pointers and the queued CallLater see null once the bar that
// owns this is destroyed, instead of a dangling pointer.
class TransferButton : Managed
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/transfer/TransferButton.layout";
	static const string Chip_Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/transfer/TransferTipChip.layout";

	static const int STATE_IDLE = 0;
	static const int STATE_RUNNING = 1;
	static const int STATE_DONE = 2;
	static const int STATE_FAILED = 3;

	// How long a finished or failed result stays on the chip before the button goes back to idle.
	static const int FLASH_MS = 1800;
	// Design px (1080p): gap from the bar's right edge, and one button plus its gap.
	static const float EDGE_PAD = 10;
	static const float SLOT_W = 33;
	// On the EQUIPMENT and VICINITY bars (collapsible_header.layout: 60 px tall, the last 5 px its
	// Border strip) the buttons centre on the 55 px blue band above the strip, which is exactly a card
	// header's height, so bar and card buttons sit at the same height: 2.5 px up from the bar's centre.
	static const float BAR_Y = -6.5;
	// The button box's authored side in the layout. Layout sizes are scaled by the engine but script
	// SetPos takes physical px, so every script offset is multiplied by the loaded box over this
	// (m_Scale): the boxes then sit on exactly the same grid as the layout-placed X at any
	// resolution or UI scale.
	static const float BOX_W = 29;

	// The chip, in design px: authored height, fallback width, text padding, gap over the bar, and how
	// far it stays in from the bar's sides.
	static const float CHIP_H = 23;
	static const float CHIP_W = 130;
	static const float CHIP_PAD = 10;
	static const float CHIP_GAP = 4;
	static const float CHIP_INSET = 6;

	protected Widget		m_TransferRoot;
	protected Widget		m_TransferBar;
	protected ButtonWidget	m_TransferBtn;
	protected Widget		m_TransferBg;
	protected ImageWidget	m_TransferIcon;
	protected Widget		m_TransferChip;
	protected TextWidget	m_TransferChipText;
	protected float			m_TransferChipScale = 1;
	protected float			m_TransferChipX = -1;
	protected float			m_TransferChipY = -1;
	protected float			m_TransferChipW = -1;
	protected int			m_TransferState = STATE_IDLE;
	protected bool			m_TransferHover;
	protected bool			m_TransferActive;
	protected string		m_TransferIdleIcon;
	protected string		m_TransferTooltip;
	protected float			m_TransferSpinDeg;

	// v_Slot counts buttons from the bar's right edge (0 is the rightmost place).
	void TransferButton(Widget v_Bar, string v_IdleIcon, string v_Tip, int v_Slot)
	{
		m_TransferIdleIcon = v_IdleIcon;
		m_TransferTooltip = v_Tip;
		m_TransferBar = v_Bar;
		if (!v_Bar)
			return;

		m_TransferRoot = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_Bar);
		if (!m_TransferRoot)
			return;
		// The same units and offsets the card headers' sort and stack buttons use (SortButton),
		// so every header's buttons line up with the same right padding and gaps.
		float v_BW, v_BH;
		m_TransferRoot.GetSize(v_BW, v_BH);
		float v_S = 1;
		if (v_BW > 0)
			v_S = v_BW / BOX_W;
		m_TransferRoot.SetPos((EDGE_PAD + SLOT_W * v_Slot) * v_S, BAR_Y * v_S);

		m_TransferBtn = ButtonWidget.Cast(m_TransferRoot.FindAnyWidget("XferBtn"));
		m_TransferBg = m_TransferRoot.FindAnyWidget("XferBg");
		m_TransferIcon = ImageWidget.Cast(m_TransferRoot.FindAnyWidget("XferIcon"));

		// Hover goes through WidgetEventHandler too, because RegisterOnClick makes it the button's one
		// and only handler.
		if (m_TransferBtn)
		{
			WidgetEventHandler.GetInstance().RegisterOnMouseEnter(m_TransferBtn, this, "TransferOnEnter");
			WidgetEventHandler.GetInstance().RegisterOnMouseLeave(m_TransferBtn, this, "TransferOnLeave");
		}
		ApplyState();
	}

	void ~TransferButton()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(BackToIdle);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(SpinStep);
		if (m_TransferChip)
			m_TransferChip.Unlink();
		if (m_TransferRoot)
			m_TransferRoot.Unlink();
	}

	ButtonWidget GetButton()
	{
		return m_TransferBtn;
	}

	// Every frame from the bar's owner: keeps the chip over the button as the screen changes.
	void Layout()
	{
		PlaceChip();
	}

	// Lit (brand icon over the hover wash) while the mode the button switches is on.
	void SetActive(bool v_On)
	{
		if (v_On == m_TransferActive)
			return;
		m_TransferActive = v_On;
		ApplyState();
	}

	void SetState(int v_State, string v_Text)
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(BackToIdle);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(SpinStep);
		m_TransferState = v_State;
		m_TransferSpinDeg = 0;
		if (m_TransferIcon)
			m_TransferIcon.SetRotation(0, 0, 0);
		ApplyState();

		// The running icon spins for as long as the work goes on.
		if (v_State == STATE_RUNNING)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(SpinStep, 16, true);
		if (v_State == STATE_IDLE)
			return;

		SetChip(v_Text, ARGB(255, 255, 255, 255));
		if (v_State == STATE_DONE || v_State == STATE_FAILED)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(BackToIdle, FLASH_MS, false);
	}

	protected void SpinStep()
	{
		if (!m_TransferIcon)
			return;
		m_TransferSpinDeg += 9;
		if (m_TransferSpinDeg >= 360)
			m_TransferSpinDeg -= 360;
		m_TransferIcon.SetRotation(0, 0, m_TransferSpinDeg);
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
		if (m_TransferIcon)
		{
			string v_Image = m_TransferIdleIcon;
			if (m_TransferState == STATE_RUNNING)
				v_Image = "ArrowsClockwise";
			else if (m_TransferState == STATE_DONE)
				v_Image = "Check";
			else if (m_TransferState == STATE_FAILED)
				v_Image = "X";
			m_TransferIcon.LoadImageFile(0, "set:InventoryIcons image:" + v_Image);
			m_TransferIcon.SetImage(0);

			if (m_TransferState == STATE_IDLE && (m_TransferHover || m_TransferActive))
				m_TransferIcon.SetColor(colorScheme.BrandColor());
			else
				m_TransferIcon.SetColor(ColorFor(m_TransferState));
		}

		if (m_TransferBg)
		{
			// Brand colour at alpha 46 of 255: a soft wash, so the icon stays the brightest thing.
			if (m_TransferHover || m_TransferActive)
				m_TransferBg.SetColor((colorScheme.BrandColor() & 0x00FFFFFF) | 0x2E000000);
			else
				m_TransferBg.SetColor(0);
		}

		// Idle shows the tip only while hovered; any other state owns the chip.
		if (m_TransferState == STATE_IDLE)
		{
			if (m_TransferHover)
			{
				string v_Tip = m_TransferTooltip;
				if (m_TransferActive)
					v_Tip = TransferStrings.TRANSFER_MODE_TIP;
				SetChip(v_Tip, ARGB(255, 255, 255, 255));
			}
			else if (m_TransferChip)
			{
				m_TransferChip.Show(false);
			}
		}
	}

	// Built on first use under the inventory menu's root, which is full screen, does not clip and
	// hides with the menu.
	protected bool EnsureChip()
	{
		if (m_TransferChip)
			return true;
		UIScriptedMenu v_Menu = GetGame().GetUIManager().FindMenu(MENU_INVENTORY);
		if (!v_Menu || !v_Menu.GetLayoutRoot())
			return false;
		m_TransferChip = GetGame().GetWorkspace().CreateWidgets(Chip_Layout_Path, v_Menu.GetLayoutRoot());
		if (!m_TransferChip)
			return false;
		m_TransferChipText = TextWidget.Cast(m_TransferChip.FindAnyWidget("XferTipChipText"));

		// The layout's own scale, read off the chip as loaded, the same yardstick every tip uses.
		float v_W, v_H;
		m_TransferChip.GetSize(v_W, v_H);
		if (v_H > 0)
			m_TransferChipScale = v_H / CHIP_H;
		return true;
	}

	protected void SetChip(string v_Text, int v_Color)
	{
		if (v_Text == "")
		{
			if (m_TransferChip)
				m_TransferChip.Show(false);
			return;
		}
		if (!EnsureChip() || !m_TransferChipText)
			return;
		m_TransferChipText.SetText(v_Text);
		m_TransferChipText.SetColor(v_Color);
		m_TransferChip.Show(true);
		// A new text is a new width, so the next placement writes even if nothing else moved.
		m_TransferChipW = -1;
		PlaceChip();
	}

	protected void PlaceChip()
	{
		if (!m_TransferChip || !m_TransferChip.IsVisible() || !m_TransferBtn || !m_TransferBar)
			return;

		float v_BX, v_BY, v_BW, v_BH;
		m_TransferBtn.GetScreenPos(v_BX, v_BY);
		m_TransferBtn.GetScreenSize(v_BW, v_BH);
		float v_HX, v_HY, v_HW, v_HH;
		m_TransferBar.GetScreenPos(v_HX, v_HY);
		m_TransferBar.GetScreenSize(v_HW, v_HH);
		if (v_BW <= 0 || v_HW <= 0)
			return;

		float v_S = m_TransferChipScale;
		int v_TW, v_TH;
		if (m_TransferChipText)
			m_TransferChipText.GetTextSize(v_TW, v_TH);
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
		Widget v_Parent = m_TransferChip.GetParent();
		if (v_Parent)
			v_Parent.GetScreenPos(v_PX, v_PY);
		// A bar at the very top of the screen gets its chip underneath instead.
		if (v_Y < v_PY)
			v_Y = v_HY + v_HH + CHIP_GAP * v_S;

		if (v_X == m_TransferChipX && v_Y == m_TransferChipY && v_CW == m_TransferChipW)
			return;
		m_TransferChipX = v_X;
		m_TransferChipY = v_Y;
		m_TransferChipW = v_CW;
		m_TransferChip.SetSize(v_CW, v_CH);
		m_TransferChip.SetPos(v_X - v_PX, v_Y - v_PY);
	}

	bool TransferOnEnter(Widget w, int x, int y)
	{
		m_TransferHover = true;
		ApplyState();
		return true;
	}

	bool TransferOnLeave(Widget w, Widget enter_w, int x, int y)
	{
		m_TransferHover = false;
		ApplyState();
		return true;
	}
}
