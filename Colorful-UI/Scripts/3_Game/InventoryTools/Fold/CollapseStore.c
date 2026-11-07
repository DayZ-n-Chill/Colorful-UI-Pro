// Which inventory cards the player has folded down to their header, remembered across sessions.
//
// Keyed the way vanilla keys its own open/closed memory (ItemManager.SetDefaultOpenState): by the
// item's type name, so folding one raincoat folds every raincoat, the same as closing one does.
// Attachment category rows add their category to the key (FoldHooks.c).
//
// Kept in its own profile value rather than in vanilla's "defaultOpenStates" list, because that list
// means "closed" (the whole card hidden) and a folded card is still on screen. One string, the
// collapsed keys joined by ';', so an empty set saves as an empty string.
class CollapseStore
{
	static const string PROFILE_KEY = "CollapsedCards";
	static const string SEP = ";";

	protected static ref map<string, bool> s_Keys;

	protected static void Load()
	{
		if (s_Keys)
			return;
		s_Keys = new map<string, bool>;

		string v_Raw;
		GetGame().GetProfileString(PROFILE_KEY, v_Raw);
		TStringArray v_Parts = new TStringArray;
		v_Raw.Split(SEP, v_Parts);
		foreach (string v_Key : v_Parts)
		{
			if (v_Key != "")
				s_Keys.Set(v_Key, true);
		}
	}

	static bool IsCollapsed(string v_Key)
	{
		if (v_Key == "")
			return false;
		Load();
		return s_Keys.Contains(v_Key);
	}

	static void Set(string v_Key, bool v_Collapsed)
	{
		if (v_Key == "")
			return;
		Load();
		if (v_Collapsed == s_Keys.Contains(v_Key))
			return;

		if (v_Collapsed)
			s_Keys.Set(v_Key, true);
		else
			s_Keys.Remove(v_Key);

		string v_Out = "";
		foreach (string v_K, bool v_On : s_Keys)
		{
			if (v_Out != "")
				v_Out += SEP;
			v_Out += v_K;
		}
		GetGame().SetProfileString(PROFILE_KEY, v_Out);
		GetGame().SaveProfile();
	}
}
