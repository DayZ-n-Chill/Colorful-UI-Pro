// "Sort all" on the EQUIPMENT bar: stack then sort every cargo container the player is wearing
// (clothing, vest, backpack, belt pouches attached to the player), one container after another.
//
// It does no moves itself. For each container it starts StackRunner with no button of its own;
// the stack runner hands over to SortRunner as usual, and this waits for both to go idle before
// starting the next. Everything those runners respect - locked, in hands, reserved -
// is respected here for free. SortRunner.LastState says how each one ended.
//
// The list is taken once, at the click. A container taken off meanwhile just fails its own turn.
// Ticked from InventoryMenu.Update (SortHooks.c), after the two runners; OnHide aborts it.
class SortAll
{
	protected static ref array<EntityAI> s_Queue;
	protected static int s_Next;
	protected static int s_Sorted;
	protected static bool s_Running;
	// Raw on purpose: SortButton is Managed, so this goes null if the bar is destroyed.
	protected static SortButton s_Btn;

	static bool IsRunning()
	{
		return s_Running;
	}

	static void Start(SortButton v_Btn)
	{
		if (s_Running || SortRunner.IsRunning() || StackRunner.IsRunning())
			return;

		s_Btn = v_Btn;
		PlayerBase v_Player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!v_Player || !v_Player.CanManipulateInventory())
		{
			Done(SortButton.STATE_FAILED, SortStrings.STOPPED);
			return;
		}

		// Cargo index 0 only, the one cargo the clothing cards show (ContainerWithCargoAndAttachments).
		s_Queue = new array<EntityAI>;
		int v_Count = v_Player.GetInventory().AttachmentCount();
		for (int v_I = 0; v_I < v_Count; v_I++)
		{
			EntityAI v_Att = v_Player.GetInventory().GetAttachmentFromIndex(v_I);
			if (!v_Att)
				continue;
			if (SortRunner.CanOffer(v_Att, 0))
				s_Queue.Insert(v_Att);
		}
		if (s_Queue.Count() == 0)
		{
			Done(SortButton.STATE_DONE, SortStrings.NOTHING_TO_SORT);
			return;
		}

		s_Next = 0;
		s_Sorted = 0;
		s_Running = true;
		StartNext();
	}

	// Every frame from InventoryMenu.Update, after the stack and sort runners have ticked.
	static void Tick()
	{
		if (!s_Running)
			return;
		if (StackRunner.IsRunning() || SortRunner.IsRunning())
			return;

		// The container started last has finished, one way or the other.
		if (SortRunner.LastState() == SortButton.STATE_DONE)
			s_Sorted++;
		StartNext();
	}

	protected static void StartNext()
	{
		if (s_Next >= s_Queue.Count())
		{
			int v_Total = s_Queue.Count();
			if (s_Sorted == v_Total)
				Done(SortButton.STATE_DONE, SortStrings.SORTED_ALL);
			else
				Done(SortButton.STATE_FAILED, string.Format(SortStrings.SORTED_OF, s_Sorted, v_Total));
			return;
		}

		EntityAI v_Owner = s_Queue[s_Next];
		s_Next++;
		if (s_Btn)
			s_Btn.SetState(SortButton.STATE_RUNNING, SortStrings.SORTING_ALL + " " + s_Next.ToString() + "/" + s_Queue.Count().ToString());

		// A stack that fails never reaches the sort, so LastState stays -1 and the container counts
		// as not sorted.
		SortRunner.ClearLast();
		if (!v_Owner)
			return;
		StackRunner.Start(v_Owner, 0, null);
	}

	// Stops the queue and whichever runner it had going. Safe to call when idle.
	static void Abort()
	{
		if (!s_Running)
			return;
		StackRunner.Abort(SortStrings.STACK_STOPPED);
		SortRunner.Abort(SortStrings.STOPPED);
		Done(SortButton.STATE_FAILED, SortStrings.STOPPED);
	}

	protected static void Done(int v_State, string v_Text)
	{
		if (s_Btn)
			s_Btn.SetState(v_State, v_Text);
		s_Running = false;
		s_Btn = null;
		s_Queue = null;
		s_Next = 0;
		s_Sorted = 0;
	}
}
