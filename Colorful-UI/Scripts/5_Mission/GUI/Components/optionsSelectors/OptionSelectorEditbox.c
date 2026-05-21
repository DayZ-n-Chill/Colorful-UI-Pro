// OptionSelectorEditbox — minimal scheme color swap.
// Vanilla source: P:\scripts\5_mission\gui\newui\optionselectoreditbox.c:145-170

modded class OptionSelectorEditbox
{
	override void ColorHighlight(Widget w)
	{
		if (!w) return;

		// super runs vanilla's body which paints m_EditBox red ARGB(255,200,0,0).
		// Call super FIRST, then overwrite with our scheme color last.
		super.ColorHighlight(w);

		if (m_EditBox)
		{
			SetFocus(m_EditBox);
			m_EditBox.SetColor(colorScheme.ButtonHover());
		}
	}
}
