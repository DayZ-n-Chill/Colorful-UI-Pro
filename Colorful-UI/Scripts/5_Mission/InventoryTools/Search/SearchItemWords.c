// What a search is matched against: the display name, the class name and a few plain category
// words, so "ammo" or "food" finds things whose names never say so. Built once per class name and
// cached, because every visible cell asks again whenever the filter changes.
//
// Also the one place an item's tile category (SearchFilter.CAT_*) is decided, cached the same way.
class SearchItemWords
{
	protected static ref map<string, string> s_Hay = new map<string, string>;
	protected static ref map<string, int> s_Cat = new map<string, int>;

	static string HayOf(EntityAI v_E)
	{
		string v_Type = v_E.GetType();
		string v_Hay;
		if (s_Hay.Find(v_Type, v_Hay))
			return v_Hay;

		v_Hay = v_E.GetDisplayName() + " " + v_Type + " " + Words(v_E);
		v_Hay.ToLower();
		s_Hay.Set(v_Type, v_Hay);
		return v_Hay;
	}

	// The typed text and the tiles stack: an item must match the text AND one of the selected
	// categories. Each part passes on its own when it is empty.
	static bool Matches(EntityAI v_E)
	{
		if (!SearchFilter.MatchesCat(CategoryOf(v_E)))
			return false;
		return SearchFilter.MatchesHay(HayOf(v_E));
	}

	static int CategoryOf(EntityAI v_E)
	{
		string v_Type = v_E.GetType();
		int v_Cat;
		if (s_Cat.Find(v_Type, v_Cat))
			return v_Cat;

		v_Cat = Classify(v_E);
		s_Cat.Set(v_Type, v_Cat);
		return v_Cat;
	}

	// Most specific first: a loose ammo pile is also a Magazine, and most pills are Edible_Base.
	// Anything none of these claim is Supplies, so every item has exactly one tile.
	protected static int Classify(EntityAI v_E)
	{
		if (Ammunition_Base.Cast(v_E))
			return SearchFilter.CAT_AMMO;
		if (Magazine.Cast(v_E))
			return SearchFilter.CAT_MAGAZINES;
		if (Weapon_Base.Cast(v_E))
			return SearchFilter.CAT_WEAPONS;
		if (IsMedical(v_E))
			return SearchFilter.CAT_MEDICAL;
		if (Edible_Base.Cast(v_E))
			return SearchFilter.CAT_FOOD;
		if (Clothing.Cast(v_E))
			return SearchFilter.CAT_CLOTHING;
		if (ToolBase.Cast(v_E))
			return SearchFilter.CAT_TOOLS;
		return SearchFilter.CAT_SUPPLIES;
	}

	// Vanilla has no medical base class, so the vanilla medical classes are listed one by one.
	// Casts rather than config IsKindOf, so a mod item that inherits one of these is caught too.
	protected static bool IsMedical(EntityAI v_E)
	{
		if (BandageDressing.Cast(v_E) || Rag.Cast(v_E) || Splint.Cast(v_E))
			return true;
		if (Morphine.Cast(v_E) || Epinephrine.Cast(v_E) || AntiChemInjector.Cast(v_E))
			return true;
		if (SalineBag.Cast(v_E) || SalineBagIV.Cast(v_E) || StartKitIV.Cast(v_E) || BloodContainerBase.Cast(v_E))
			return true;
		if (BloodTestKit.Cast(v_E) || Defibrillator.Cast(v_E) || Thermometer.Cast(v_E) || FirstAidKit.Cast(v_E))
			return true;
		if (DisinfectantSpray.Cast(v_E) || DisinfectantAlcohol.Cast(v_E) || IodineTincture.Cast(v_E))
			return true;
		if (PainkillerTablets.Cast(v_E) || CharcoalTablets.Cast(v_E) || TetracyclineAntibiotics.Cast(v_E))
			return true;
		if (VitaminBottle.Cast(v_E) || ChelatingTablets.Cast(v_E) || PurificationTablets.Cast(v_E))
			return true;
		return false;
	}

	// The words follow the tile categories, so typing "medical" or "tool" finds what that tile finds.
	protected static string Words(EntityAI v_E)
	{
		if (Ammunition_Base.Cast(v_E))
			return "ammo rounds bullets";
		if (Magazine.Cast(v_E))
			return "ammo magazine mag";
		if (Weapon_Base.Cast(v_E))
			return "weapon gun";
		if (IsMedical(v_E))
			return "medical medicine med";
		if (Bottle_Base.Cast(v_E))
			return "drink water liquid";
		if (Edible_Base.Cast(v_E))
			return "food";
		if (Clothing.Cast(v_E))
			return "clothing wear";
		if (ItemOptics.Cast(v_E))
			return "optic scope attachment";
		if (ItemSuppressor.Cast(v_E))
			return "suppressor attachment";
		if (ToolBase.Cast(v_E))
			return "tool tools";
		return "";
	}
}
