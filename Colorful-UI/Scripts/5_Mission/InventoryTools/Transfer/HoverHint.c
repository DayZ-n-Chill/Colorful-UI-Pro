// The one-line "CTRL + HOVER  Quick move" hint at the bottom-left of the inventory screen, so players
// find the hover transfer (HoverMove.c). It sits just below the HUD's stance icon and stamina bar
// (StancePanel in day_z_hud.layout's LeftHUDPanel), its left edge on the icon's left edge.
//
// Quiet by default (cream at 55%); while Ctrl is held it turns full opacity with the key in the brand
// colour, as feedback that the mode is on. Hidden while the player types in the inventory search.
// Built once on the inventory menu's root, so it goes with the menu. Widgets are only touched when
// the state, the key text's width or the stance icon's place changes.
class HoverHint
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/transfer/HoverHint.layout";
	// Design px: between the key and the label, and between the stance icon's bottom and the hint's top.
	static const float KEY_GAP = 8;
	static const float BAR_GAP = 12;
	// Where the hint goes if the HUD's stance icon cannot be found or is hidden: design px from the
	// screen's left and bottom edges.
	static const float FALLBACK_LEFT = 24;
	static const float FALLBACK_BOTTOM = 60;

	protected Widget		m_Root;
	protected TextWidget	m_Key;
	protected TextWidget	m_Label;
	// The HUD's stance icon (day_z_hud.layout), the hint's anchor.
	protected Widget		m_Stance;
	protected bool			m_Painted;
	protected bool			m_Visible;
	protected bool			m_Active;
	protected int			m_KeyW = -1;
	protected float			m_AnchorX = -1;
	protected float			m_AnchorY = -1;

	void HoverHint(Widget v_Parent)
	{
		if (!v_Parent)
			return;
		m_Root = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_Parent);
		if (!m_Root)
			return;
		m_Key = TextWidget.Cast(m_Root.FindAnyWidget("HoverHintKey"));
		m_Label = TextWidget.Cast(m_Root.FindAnyWidget("HoverHintLabel"));
		if (m_Key)
			m_Key.SetText(TransferStrings.HOVER_HINT_KEY);
		if (m_Label)
			m_Label.SetText(TransferStrings.HOVER_HINT_LABEL);
	}

	void ~HoverHint()
	{
		if (m_Root)
			m_Root.Unlink();
	}

	// From InventoryMenu.OnShow: the next tick repaints and re-places everything.
	void OnShow()
	{
		m_Painted = false;
		m_KeyW = -1;
		m_AnchorX = -1;
		m_AnchorY = -1;
	}

	// Every frame from InventoryMenu.Update.
	void Tick()
	{
		if (!m_Root)
			return;

		bool v_Visible = !InputGuard.IsOn();
		bool v_Active = v_Visible && TransferMode.CtrlDown();
		if (!m_Painted || v_Visible != m_Visible || v_Active != m_Active)
		{
			m_Painted = true;
			m_Visible = v_Visible;
			m_Active = v_Active;
			Paint();
		}
		if (m_Visible)
		{
			PlaceLabel();
			PlaceRoot();
		}
	}

	protected void Paint()
	{
		m_Root.Show(m_Visible);
		if (!m_Visible)
			return;

		// The tools' cream, #dcd9cf: at alpha 140 of 255 (about 55%) when idle, solid while Ctrl is held.
		int v_Cream = ARGB(140, 220, 217, 207);
		int v_KeyColor = v_Cream;
		if (m_Active)
		{
			v_Cream = ARGB(255, 220, 217, 207);
			v_KeyColor = colorScheme.BrandColor();
		}
		if (m_Key)
			m_Key.SetColor(v_KeyColor);
		if (m_Label)
			m_Label.SetColor(v_Cream);
	}

	// The label starts a gap after the end of the key text; only moved when the key's width changes.
	protected void PlaceLabel()
	{
		if (!m_Key || !m_Label)
			return;
		int v_W, v_H;
		m_Key.GetTextSize(v_W, v_H);
		if (v_W == m_KeyW)
			return;
		m_KeyW = v_W;
		m_Label.SetPos(v_W + InventoryMenu.GetHeightMultiplied(KEY_GAP), 0);
	}

	// Top of the hint a gap below the stance icon's bottom, left edges lined up. Positions are worked
	// out in screen px and set relative to the menu root.
	protected void PlaceRoot()
	{
		Widget v_Parent = m_Root.GetParent();
		if (!v_Parent)
			return;
		float v_PX, v_PY, v_PW, v_PH;
		v_Parent.GetScreenPos(v_PX, v_PY);
		v_Parent.GetScreenSize(v_PW, v_PH);
		float v_RW, v_RH;
		m_Root.GetScreenSize(v_RW, v_RH);

		float v_X, v_Y;
		Widget v_Stance = FindStance();
		if (v_Stance)
		{
			float v_SW, v_SH;
			v_Stance.GetScreenPos(v_X, v_Y);
			v_Stance.GetScreenSize(v_SW, v_SH);
			v_Y += v_SH + InventoryMenu.GetHeightMultiplied(BAR_GAP);
		}
		else
		{
			v_X = v_PX + InventoryMenu.GetHeightMultiplied(FALLBACK_LEFT);
			v_Y = v_PY + v_PH - InventoryMenu.GetHeightMultiplied(FALLBACK_BOTTOM) - v_RH;
		}

		if (v_X == m_AnchorX && v_Y == m_AnchorY)
			return;
		m_AnchorX = v_X;
		m_AnchorY = v_Y;
		m_Root.SetPos(v_X - v_PX, v_Y - v_PY);
	}

	// Looked up once and kept (a widget reads as null once destroyed); null while hidden.
	protected Widget FindStance()
	{
		if (!m_Stance)
		{
			Mission v_Mission = GetGame().GetMission();
			if (!v_Mission)
				return null;
			IngameHud v_Hud = IngameHud.Cast(v_Mission.GetHud());
			if (!v_Hud || !v_Hud.GetHudPanelWidget())
				return null;
			m_Stance = v_Hud.GetHudPanelWidget().FindAnyWidget("StancePanel");
			if (!m_Stance)
				return null;
		}
		if (!m_Stance.IsVisibleHierarchy())
			return null;
		return m_Stance;
	}
}
