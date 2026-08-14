// OptionSelectorEditbox — minimal scheme color swap.
// Vanilla source: P:\scripts\5_mission\gui\newui\optionselectoreditbox.c

modded class OptionSelectorEditbox
{
	override void ColorHighlight(Widget w)
	{
		if (!w) return;

		super.ColorHighlight(w);

		if (m_EditBox)
		{
			SetFocus(m_EditBox);
			m_EditBox.SetColor(colorScheme.ButtonHover());
		}
	}
}
