// OptionSelectorSliderSetup — minimal scheme color swap.
// Vanilla source: P:\scripts\5_mission\gui\newui\optionselectorslidersetup.c

modded class OptionSelectorSliderSetup
{
	override void ColorHighlight(Widget w)
	{
		if (!w) return;

		super.ColorHighlight(w);

		if (m_Slider)
		{
			SetFocus(m_Slider);
			m_Slider.SetColor(colorScheme.ButtonHover());
		}
	}
}
