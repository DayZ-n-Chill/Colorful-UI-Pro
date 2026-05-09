// OptionSelectorBase — minimal scheme color swap.
// -----------------------------------------------------------------------------
// Vanilla highlights option rows with red ARGB(255,255,0,0) and uses white
// for the normal text. We just swap those two values for our scheme — every
// other vanilla widget-finding / event behavior is untouched.
//
// Vanilla source: P:\scripts\5_mission\gui\newui\optionselectorbase.c

modded class OptionSelectorBase
{
	override void ColorHighlight(Widget w)
	{
		if (!w) return;
		ButtonSetColor(w, colorScheme.ButtonHover());

		// Vanilla doesn't color labels on hover, but we want to. Same widget
		// names vanilla's ColorNormal looks up.
		int lbl = colorScheme.TextHover();
		Widget title_label  = w.FindAnyWidget(w.GetName() + "_label");
		Widget option_label = w.FindAnyWidget("option_label");
		if (title_label)  title_label.SetColor(lbl);
		if (option_label) option_label.SetColor(lbl);
	}

	override void ColorHighlightConsole(Widget w)
	{
		if (!w) return;

		ButtonSetColorConsole(w, colorScheme.ButtonHover());
		ButtonSetAlphaAnimConsole(null);
		ButtonSetTextColorConsole(w, colorScheme.PrimaryText());
	}
}
