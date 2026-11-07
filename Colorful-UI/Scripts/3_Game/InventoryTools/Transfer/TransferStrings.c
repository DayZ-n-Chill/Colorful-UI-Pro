// Every piece of text the transfer tools show (quick move, the Transfer arrows, Grab all), in one
// place so a translation pass only has to look here. The transfer feature's own copy: the sort and
// search features keep theirs in their own folders.
class TransferStrings
{
	// The Transfer arrows on the EQUIPMENT and VICINITY bars (ItemTransfer.c) and their progress chip.
	static const string TRANSFER_TO_VICINITY_TIP = "Transfer to vicinity: click, pick items or a target, click again";
	static const string TRANSFER_TO_GEAR_TIP = "Transfer to gear: click, pick items or a target, click again";
	static const string TRANSFER_MODE_TIP = "Transfer mode: click items to pick them, click an arrow to move";
	static const string MOVING = "Moving";
	static const string MOVED = "Moved";
	static const string DROPPED = "Dropped";
	static const string ON_GROUND = "on the ground";
	static const string TRANSFER_STOPPED = "Transfer stopped";

	// Grab all, on the VICINITY bar (GrabAll.c).
	static const string GRAB_ALL_TIP = "Grab all from the ground";
	static const string GRABBING = "Grabbing";
	static const string GRABBED = "Grabbed";
	// "Grabbed 5, 2 left": items that had no room or could not be picked up.
	static const string GRAB_LEFT = "left";
	static const string GRAB_NOTHING = "Nothing to grab";
	static const string GRAB_NO_ROOM = "No room";
	static const string GRAB_STOPPED = "Grab stopped";

	// Quick transfer: the tag after a target card's title, and what it flashes when the item does not fit.
	static const string TARGET_TAG = "TARGET";
	static const string TARGET_NO_ROOM = "NO ROOM";

	// The hover transfer's hint at the bottom-left of the inventory screen (HoverHint.c).
	static const string HOVER_HINT_KEY = "CTRL + HOVER";
	static const string HOVER_HINT_LABEL = "Quick move";
}
