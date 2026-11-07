// Item hover timing and placement as in DayZ Deluxe (Escape from Tarkov style): the tooltip waits a
// moment before it appears, then sits beside the cursor and follows it. The slot name tooltip
// ("Face", "Strap") gets the same pause and follow, and is sized to its name.
//
// Vanilla's flow is kept up to the point the panel would show: PrepareTooltip fills and places it and
// its timer calls ShowTooltip. ShowTooltip lets vanilla show the panel, takes it straight back down in
// the same frame (so nothing is drawn), waits the extra pause, then shows it by the cursor and starts
// the follow. HideTooltip (mouse leave, drag, close) stops it.
modded class ItemManager
{
	// Vanilla waits TOOLTIP_DELAY (0.25 s) before ShowTooltip; this is added on top, so the tooltip
	// appears after about 0.6 s of resting on an item.
	protected static const int TooltipEXTRAMS = 350;
	protected static const int TooltipFOLLOWMS = 16;
	// Screen px from the cursor's tip to the tooltip's top-left corner.
	protected static const int TooltipDX = 20;
	protected static const int TooltipDY = 16;
	// The slot tooltip's padding left and right of the name, and its authored height, in design px
	// (TooltipSlot.layout).
	protected static const float TooltipSLOTPAD = 12;
	protected static const float TooltipSLOTDESIGNH = 38.4201;

	override void ShowTooltip()
	{
		super.ShowTooltip();
		TooltipStop();
		if (m_TooltipWidget)
			m_TooltipWidget.Show(false);
		if (!m_HoveredItem)
			return;
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TooltipShow, TooltipEXTRAMS, false);
	}

	override void HideTooltip()
	{
		TooltipStop();
		super.HideTooltip();
	}

	override void ShowTooltipSlot()
	{
		super.ShowTooltipSlot();
		TooltipSlotStop();
		if (m_TooltipSlotWidget)
			m_TooltipSlotWidget.Show(false);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TooltipSlotShow, TooltipEXTRAMS, false);
	}

	override void HideTooltipSlot()
	{
		TooltipSlotStop();
		super.HideTooltipSlot();
	}

	protected void TooltipStop()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(TooltipShow);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(TooltipFollow);
	}

	protected void TooltipSlotStop()
	{
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(TooltipSlotShow);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(TooltipSlotFollow);
	}

	protected void TooltipShow()
	{
		if (!m_HoveredItem || !m_TooltipWidget || !m_ItemPreviewWidget || IsDragging())
			return;
		TooltipFollow();
		m_TooltipWidget.Show(true);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TooltipFollow, TooltipFOLLOWMS, true);
	}

	// The item tooltip follows the cursor every frame while shown.
	protected void TooltipFollow()
	{
		if (!m_TooltipWidget || !m_HoveredItem || IsDragging())
		{
			TooltipStop();
			return;
		}
		TooltipPlaceByCursor(m_TooltipWidget);
	}

	protected void TooltipSlotShow()
	{
		if (!m_TooltipSlotWidget || !m_SlotInfoShown || IsDragging())
			return;
		TooltipFitSlot();
		TooltipSlotFollow();
		m_TooltipSlotWidget.Show(true);
		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(TooltipSlotFollow, TooltipFOLLOWMS, true);
	}

	protected void TooltipSlotFollow()
	{
		if (!m_TooltipSlotWidget || !m_SlotInfoShown || IsDragging())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(TooltipSlotFollow);
			return;
		}
		TooltipPlaceByCursor(m_TooltipSlotWidget);
	}

	// The box as wide as the name plus the padding each side. The engine scales a layout's exact sizes
	// when it loads it while script sizes are physical px, so the padding is scaled the same way, by
	// the box's loaded height over its authored one.
	protected void TooltipFitSlot()
	{
		TextWidget v_Name = TextWidget.Cast(m_TooltipSlotWidget.FindAnyWidget("ItemNameWidget"));
		if (!v_Name)
			return;
		float v_W, v_H;
		m_TooltipSlotWidget.GetSize(v_W, v_H);
		if (v_H <= 0)
			return;
		float v_Scale = v_H / TooltipSLOTDESIGNH;
		int v_TextW, v_TextH;
		v_Name.GetTextSize(v_TextW, v_TextH);
		if (v_TextW <= 0)
			return;
		m_TooltipSlotWidget.SetSize(v_TextW + TooltipSLOTPAD * 2 * v_Scale, v_H);
	}

	// Beside the cursor, kept on screen: flipped to the cursor's left, or pushed up, near an edge.
	protected void TooltipPlaceByCursor(Widget v_Tip)
	{
		int v_MX, v_MY;
		GetMousePos(v_MX, v_MY);
		int v_SW, v_SH;
		GetScreenSize(v_SW, v_SH);
		float v_W, v_H;
		v_Tip.GetScreenSize(v_W, v_H);

		float v_X = v_MX + TooltipDX;
		float v_Y = v_MY + TooltipDY;
		if (v_X + v_W > v_SW - 10)
			v_X = v_MX - TooltipDX - v_W;
		if (v_Y + v_H > v_SH - 10)
			v_Y = v_SH - 10 - v_H;
		if (v_X < 10)
			v_X = 10;
		if (v_Y < 10)
			v_Y = 10;
		v_Tip.SetPos(v_X, v_Y);
	}
}
