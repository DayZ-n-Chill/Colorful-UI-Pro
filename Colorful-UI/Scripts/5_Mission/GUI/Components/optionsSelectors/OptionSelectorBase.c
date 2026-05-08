modded class OptionSelectorBase extends ScriptedWidgetEventHandler
{	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (!IsFocusable(w))
			return true;

		if (m_ParentClass)
		{
			m_ParentClass.OnFocus(m_Root.GetParent(), -1, m_SelectorType);
		}

		UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();

		if (menu && menu.IsInherited(CharacterCreationMenu))
		{
			menu.OnFocus(m_Root.GetParent(), -1, m_SelectorType);
			menu.OnMouseEnter(m_Root.GetParent().GetParent(), x, y);
		}

		ColorHighlightConsole(w);
		if (m_ParentClass)
		{
			m_ParentClass.OnFocus(m_Root.GetParent(), -1, m_SelectorType);
		}

		return true;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		Widget p = enterW;
		while (p)
		{
			if (p == w)
			{
				return true;
			}
			p = p.GetParent();
		}
		if (m_ParentClass)
		{
			m_ParentClass.OnFocus(null, x, y);
		}

		UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();

		if (menu && menu.IsInherited(CharacterCreationMenu))
		{
			menu.OnFocus(null, x, y);
			menu.OnMouseLeave(m_Root.GetParent().GetParent(), enterW, x, y);
		}

		ColorNormalConsole(w);
		if (m_ParentClass)
		{
			m_ParentClass.OnFocusLost(w, x, y);
		}

		return true;
	}

	override bool OnFocus(Widget w, int x, int y)
	{
		if (IsFocusable(w))
		{
			ColorHighlightConsole(w);
			if (m_ParentClass)
			{
				m_ParentClass.OnFocus(m_Root.GetParent(), -1, m_SelectorType);
			}
			return true;
		}
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		ColorNormalConsole(w);
		if (m_ParentClass)
		{
			m_ParentClass.OnFocusLost(w, x, y);
		}
		return true;
	}

	override void Focus()
	{
		SetFocus(m_Parent);
	}

	override void Enable()
	{
		m_Enabled = true;
		m_Parent.ClearFlags(WidgetFlags.IGNOREPOINTER);
		ColorNormal(m_Parent);
	}

	override void Disable()
	{
		m_Enabled = false;
		m_Parent.SetFlags(WidgetFlags.IGNOREPOINTER);
		ColorDisabled(m_Parent);
	}

	override void ColorHighlight(Widget w)
	{
		if (!w)
			return;

		ButtonSetColor(w, colorScheme.OptionBGHover());
		ButtonSetIconColor(w, colorScheme.OptionIconHover());
	}

	override void ColorNormal(Widget w)
	{
		if (!w)
			return;

		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.PrimaryText(); // White text

		ButtonSetColor(w, color_pnl);
		ButtonSetIconColor(w, colorScheme.OptionIconNormal());

		Widget title_label = w.FindAnyWidget(w.GetName() + "_label");
		Widget option_label = w.FindAnyWidget("option_label");

		if (title_label)
		{
			title_label.SetColor(color_lbl);
		}

		if (option_label)
		{
			option_label.SetColor(color_lbl);
		}
	}

	override void ColorDisabled(Widget w)
	{
		if (!w)
			return;

		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.DisabledText();

		ButtonSetColor(w, color_pnl);
		// Icon disabled color? Maybe DisabledText() too?
		ButtonSetIconColor(w, colorScheme.DisabledText()); // Assuming disabled text color for disabled icon

		Widget title_label = w.FindAnyWidget(w.GetName() + "_label");
		Widget option_label = w.FindAnyWidget("option_label");

		if (title_label)
		{
			title_label.SetColor(color_lbl);
		}

		if (option_label)
		{
			option_label.SetColor(color_lbl);
		}		
	}

	override void ButtonSetColor(Widget w, int color)
	{
		Widget option = w.FindAnyWidget(w.GetName() + "_image");

		if (option)
		{
			option.SetColor(color);
		}
	}
	
	void ButtonSetIconColor(Widget w, int color)
	{
		if (!w) return;
		
		Widget child = w.GetChildren();
		while (child)
		{
			// Check if it is an image widget OR item preview (generated icons might be items)
			if (child.IsInherited(ImageWidget) || child.IsInherited(ItemPreviewWidget))
			{
				// Exclude known background images if they have specific names or styles?
				// For now, color all ImageWidgets found.
				child.SetColor(color);
			}
			
			// Recursively check children of child
			ButtonSetIconColor(child, color);
			
			child = child.GetSibling();
		}
	}

	override void ColorHighlightConsole(Widget w)
	{
		if (!w)
			return;

		int color_pnl = colorScheme.OptionBGHover(); // Background hover color
		int color_lbl = colorScheme.TextHover(); // Text hover color

		ButtonSetColorConsole(w, color_pnl);
		ButtonSetAlphaAnimConsole(null);
		ButtonSetTextColorConsole(w, color_lbl);
		ButtonSetIconColor(w, colorScheme.OptionIconHover());
	}

	override void ColorNormalConsole(Widget w)
	{
		if (!w)
			return;

		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.PrimaryText(); // White text

		ButtonSetColorConsole(w, color_pnl);
		ButtonSetAlphaAnimConsole(null);
		ButtonSetTextColorConsole(w, color_lbl);
		ButtonSetIconColor(w, colorScheme.OptionIconNormal());
	}

	override void ColorDisabledConsole(Widget w)
	{
		if (!w)
			return;

		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.DisabledText();

		ButtonSetColorConsole(w, color_pnl);
		ButtonSetAlphaAnimConsole(null);
		ButtonSetTextColorConsole(w, color_lbl);
		ButtonSetIconColor(w, colorScheme.DisabledText());
	}

	override void ButtonSetColorConsole(Widget w, int color)
	{
		w.SetColor(color);
	}

	override void ButtonSetAlphaAnimConsole(Widget w)
	{
		if (!w)
			return;

		Widget panel = w.FindAnyWidget(w.GetName() + "_panel");

		if (panel)
		{
			//m_Root.SetWidgetAnimAlpha(panel);
		}
	}

	override void ButtonSetTextColorConsole(Widget w, int color)
	{
		if (!w)
			return;

		TextWidget label = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_label"));
		TextWidget text = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text"));
		TextWidget text2 = TextWidget.Cast(w.FindAnyWidget(w.GetName() + "_text_1"));

		if (label)
		{
			label.SetColor(color);
		}

		if (text)
		{
			text.SetColor(color);
		}

		if (text2)
		{
			text2.SetColor(color);
		}
	}
}

modded class OptionSelectorMultistate extends OptionSelectorBase
{
	override void ColorHighlight(Widget w)
	{
		if (!w)
			return;

		int color_pnl = colorScheme.OptionBGHover();
		int color_lbl = colorScheme.OptionSelectionText();
		int color_icon = colorScheme.OptionIconHover();

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColorRecursive(w, color_lbl);
		ButtonSetIconColor(w, color_icon);
		
		// Explicitly color the Next/Prev arrows (Carats)
		Widget prev = w.FindAnyWidget("prev_option");
		Widget next = w.FindAnyWidget("next_option");
		if (prev) prev.SetColor(color_icon);
		if (next) next.SetColor(color_icon);
	}

	override void ColorNormal(Widget w)
	{
		if (!w)
			return;

		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.PrimaryText();
		int color_icon = colorScheme.OptionIconNormal();

		ButtonSetColor(w, color_pnl);
		ButtonSetTextColorRecursive(w, color_lbl);
		ButtonSetIconColor(w, color_icon);
		
		// Explicitly color the Next/Prev arrows (Carats)
		Widget prev = w.FindAnyWidget("prev_option");
		Widget next = w.FindAnyWidget("next_option");
		if (prev) prev.SetColor(color_icon);
		if (next) next.SetColor(color_icon);
	}

	override void ColorHighlightConsole(Widget w)
	{
		if (!w)
			return;

		int color_pnl = colorScheme.OptionBGHover();
		int color_lbl = colorScheme.OptionSelectionText();
		int color_icon = colorScheme.OptionIconHover();

		ButtonSetColorConsole(w, color_pnl);
		ButtonSetTextColorRecursive(w, color_lbl);
		ButtonSetIconColor(w, color_icon);
		
		Widget prev = w.FindAnyWidget("prev_option");
		Widget next = w.FindAnyWidget("next_option");
		if (prev) prev.SetColor(color_icon);
		if (next) next.SetColor(color_icon);
	}

	override void ColorNormalConsole(Widget w)
	{
		if (!w)
			return;

		int color_pnl = UIColor.Transparent();
		int color_lbl = colorScheme.PrimaryText();
		int color_icon = colorScheme.OptionIconNormal();

		ButtonSetColorConsole(w, color_pnl);
		ButtonSetTextColorRecursive(w, color_lbl);
		ButtonSetIconColor(w, color_icon);
		
		Widget prev = w.FindAnyWidget("prev_option");
		Widget next = w.FindAnyWidget("next_option");
		if (prev) prev.SetColor(color_icon);
		if (next) next.SetColor(color_icon);
	}
	
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


