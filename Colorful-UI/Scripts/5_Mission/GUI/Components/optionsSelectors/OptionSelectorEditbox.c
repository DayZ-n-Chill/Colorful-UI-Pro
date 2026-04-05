modded class OptionSelectorEditbox extends OptionSelectorBase
{
	override void ColorHighlight( Widget w )
	{
		if( !w )
			return;
		
		if ( m_EditBox )
		{
			SetFocus( m_EditBox );
			// Use Brand Color for input on hover
			m_EditBox.SetColor(colorScheme.OptionInputColors());
		}
		
		// Custom ColorHighlight logic (replaces super.ColorHighlight)
		int color_pnl = colorScheme.OptionBGHover();
		int color_lbl = colorScheme.OptionSelectionText();

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColorRecursive(w, color_lbl);
		ButtonSetIconColor(w, colorScheme.OptionIconHover());
	}
	
	override void ColorNormal( Widget w )
	{
		if( !w )
			return;
		
		if ( m_EditBox )
		{
			// Revert to semi-trans white or a scheme color?
			// Scheme has OptionInputColors() which is BrandColor. 
			// Standard behavior might be transparent or semi-trans white.
			// Let's stick to existing SemiTransWhite for normal state unless requested, 
			// OR use OptionInputColors() if that's what scheme intended.
			// Reverting to previous default for normal state but keeping structure.
			m_EditBox.SetColor(UIColor.SemiTransWhite());
		}
		
		// Custom ColorNormal logic
		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.PrimaryText();

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColorRecursive(w, color_lbl);
		ButtonSetIconColor(w, colorScheme.OptionIconNormal());
	}

	// Helper for recursive coloring
	void ButtonSetTextColorRecursive(Widget w, int color)
	{
		if (!w)
			return;

		Widget child = w.GetChildren();
		while (child)
		{
			if (child.IsInherited(TextWidget))
			{
				child.SetColor(color);
			}
			else
			{
				ButtonSetTextColorRecursive(child, color);
			}
			
			child = child.GetSibling();
		}
	}
}
