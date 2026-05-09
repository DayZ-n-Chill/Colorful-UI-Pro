// OptionSelectorSliderSetup — minimal scheme color swap.
// Vanilla source: P:\scripts\5_mission\gui\newui\optionselectorslidersetup.c:144-169

modded class OptionSelectorSliderSetup
{
	override void ColorHighlight(Widget w)
	{
		if (!w) return;

		// super runs vanilla's body which paints m_Slider red ARGB(255,200,0,0).
		// Call super FIRST, then overwrite the slider color with our scheme so
		// vanilla's red doesn't clobber us.
		super.ColorHighlight(w);

		if (m_Slider)
		{
			SetFocus(m_Slider);
			m_Slider.SetColor(colorScheme.ButtonHover());
		}
	}
}
