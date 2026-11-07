// The dim drawn over one inventory cell that does not match an active search. Matches are left as
// they are and only the rest is darkened.
//
// An overlay rather than SetAlpha on the cell, so it works whatever the cell holds and never touches
// the "Color", "Col" and "Cursor" widgets vanilla toggles during a drag.
//
// Created lazily, the first time a cell actually needs it, so a player who never searches pays nothing.
class CellMark
{
	static const string Layout_Path = "Colorful-UI/GUI/layouts/inventory_tools/search/CellMark.layout";

	static Widget Paint(Widget v_Cell, Widget v_Mark, bool v_Show, bool v_Hit)
	{
		bool v_Dim = v_Show && !v_Hit;
		if (!v_Mark)
		{
			if (!v_Dim || !v_Cell)
				return null;
			v_Mark = GetGame().GetWorkspace().CreateWidgets(Layout_Path, v_Cell);
			if (!v_Mark)
				return null;
		}
		v_Mark.Show(v_Dim);
		return v_Mark;
	}
}
