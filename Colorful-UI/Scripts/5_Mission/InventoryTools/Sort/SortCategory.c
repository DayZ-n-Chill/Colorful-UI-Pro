// Decides each item's sort category (Packer.CAT_*), cached per class name. Lives in 5_Mission
// because the classes it tests (Weapon_Base, Edible_Base, ...) are 4_World and Packer is 3_Game.
class SortCategory
{
	protected static ref map<string, int> s_Cat = new map<string, int>;

	static int Of(EntityAI v_E)
	{
		string v_Type = v_E.GetType();
		int v_Cat;
		if (s_Cat.Find(v_Type, v_Cat))
			return v_Cat;

		v_Cat = Classify(v_E);
		s_Cat.Set(v_Type, v_Cat);
		return v_Cat;
	}

	// Most specific first: a loose ammo pile is also a Magazine, and most pills and disinfectants are
	// Edible_Base, so testing the parent first would file morphine tablets under Food. Anything none of
	// these claim is Supplies, so every item has exactly one category.
	protected static int Classify(EntityAI v_E)
	{
		if (Ammunition_Base.Cast(v_E))
			return Packer.CAT_AMMO;
		if (Magazine.Cast(v_E))
			return Packer.CAT_MAGAZINES;
		if (Weapon_Base.Cast(v_E))
			return Packer.CAT_WEAPONS;
		if (IsMedical(v_E))
			return Packer.CAT_MEDICAL;
		if (Edible_Base.Cast(v_E))
			return Packer.CAT_FOOD;
		if (Clothing.Cast(v_E))
			return Packer.CAT_CLOTHING;
		if (ToolBase.Cast(v_E))
			return Packer.CAT_TOOLS;
		return Packer.CAT_SUPPLIES;
	}

	// Vanilla has no medical base class, so the vanilla medical classes are listed one by one.
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
}
