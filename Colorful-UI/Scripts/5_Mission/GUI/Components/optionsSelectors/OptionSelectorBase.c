// OptionSelectorBase — minimal scheme color swap.
// Vanilla source: P:\scripts\5_mission\gui\newui\optionselectorbase.c

modded class OptionSelectorBase
{
	override void ColorHighlight(Widget w)
	{
		if (!w) return;
		ButtonSetColor(w, colorScheme.ButtonHover());

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
