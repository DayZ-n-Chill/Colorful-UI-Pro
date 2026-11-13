// Keeps the game's own keys away while the player is typing into the inventory search box.
//
// The inventory screen still polls gameplay inputs every frame (Tab and Escape close it, R rotates,
// C combines, the number keys use quickbar slots - inventory.c UpdateInterval and missiongameplay.c),
// and a focused edit box does not stop any of that. So while the box has focus this adds the "menu"
// exclude (chat's idiom; it stops movement) and locks the individual inputs the inventory reads.
//
// The exclude goes on before the locks: a lock lasts "until unlock called or exclusion is selected"
// (uainput.c), so refreshing excludes after locking could drop them.
class InputGuard
{
	protected static bool s_On;

	static void Engage()
	{
		if (s_On)
			return;
		s_On = true;

		MissionGameplay v_Mission = MissionGameplay.Cast(GetGame().GetMission());
		if (v_Mission)
			v_Mission.AddActiveInputExcludes({"menu"});

		SetLocked(true);
		CUISearchFilter.SetTyping(true);
	}

	static void Release()
	{
		if (!s_On)
			return;
		s_On = false;

		SetLocked(false);

		MissionGameplay v_Mission = MissionGameplay.Cast(GetGame().GetMission());
		if (v_Mission)
			v_Mission.RemoveActiveInputExcludes({"menu"}, false);

		CUISearchFilter.SetTyping(false);
	}

	static bool IsOn()
	{
		return s_On;
	}

	protected static void SetLocked(bool v_Lock)
	{
		LockOne(GetUApi().GetInputByID(UAGear), v_Lock);
		LockOne(GetUApi().GetInputByID(UAUIMenu), v_Lock);
		LockOne(GetUApi().GetInputByID(UAUIBack), v_Lock);
		LockOne(GetUApi().GetInputByID(UAUIRotateInventory), v_Lock);
		LockOne(GetUApi().GetInputByID(UAUICombine), v_Lock);

		// UAItem0..UAItem9 are what Inventory.UpdateInterval polls for quickbar use.
		for (int v_I = 0; v_I < 10; v_I++)
		{
			LockOne(GetUApi().GetInputByName("UAItem" + v_I.ToString()), v_Lock);
		}
	}

	protected static void LockOne(UAInput v_Input, bool v_Lock)
	{
		if (!v_Input)
			return;
		if (v_Lock)
			v_Input.Lock();
		else
			v_Input.Unlock();
	}
}
