modded class TabberUI extends ScriptedWidgetEventHandler
{
	void ApplyTabColor(Widget tab_control, bool selected, bool hovered = false)
	{
		if (!tab_control) return;

		TextWidget title = TextWidget.Cast(tab_control.FindAnyWidget(tab_control.GetName() + "_Title"));
		if (!title) return;

		if (selected)
		{
			title.SetColor(colorScheme.TabSelectedColor());
			tab_control.SetColor(colorScheme.TabBackground());
			return;
		}

		if (hovered)
		{
			title.SetColor(colorScheme.TabHoverColor());
			tab_control.SetColor(colorScheme.TabBackground());
			return;
		}

		title.SetColor(colorScheme.TabIdle());
		tab_control.SetColor(UIColor.Transparent());
	}

	override void SelectTabControl(int index)
	{
		Widget tab_control = m_TabControls.Get(index);
		ApplyTabColor(tab_control, true, false);
		m_SelectedIndex = index;
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		int index = m_TabControls.GetKeyByValue(w);
		if (index != m_SelectedIndex)
			ApplyTabColor(m_TabControls.Get(index), false, true);
		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		int index = m_TabControls.GetKeyByValue(w);
		if (index != m_SelectedIndex)
			ApplyTabColor(m_TabControls.Get(index), false, false);
		return false;
	}

	override int AddTab(string name)
	{
		int new_index = m_Tabs.Count();

		Widget tab = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/components/tabber_prefab/tab.layout", m_Root);
		Widget control = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/components/tabber_prefab/cui.tab_control.layout", m_Root.FindAnyWidget("Tab_Control_Container"));
		TextWidget title = TextWidget.Cast(control.FindAnyWidget("Tab_Control_x_Title"));

		string id = "" + new_index;

		tab.SetName("Tab_" + id);
		control.SetName("Tab_Control_" + id);
		title.SetName("Tab_Control_" + id + "_Title");
		control.FindAnyWidget("Tab_Control_x_Background").SetName("Tab_Control_" + id + "_Background");

		title.SetText(name);

		control.SetHandler(this);
		m_TabControls.Insert(new_index, control);
		m_Tabs.Insert(new_index, tab);

		AlignTabbers();
		return new_index;
	}
}
