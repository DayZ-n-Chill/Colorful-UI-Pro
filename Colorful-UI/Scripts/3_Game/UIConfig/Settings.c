// Constants.c v3.0 (These are a WIP and will be updated as the mod progresses)
static bool StartMainMenu     = false;   // If set to true, the main menu will be forced to show on startup.
static bool NoHints			  = false;   // If set to true, the hints will not be shown during load screens.
static bool SyncHintImages    = true;	 // If set to true, the hints are synced to BG Images, all controlled by the hints.json.	
static bool RandomBackgrounds = false;	 // If set to true, a random background will be shown during load screens (SyncHintImages must be false. Image is changed in the layout files).
static bool ShowDeadScreen    = false;   // If set to true, a custom game over screen will be shown when the player dies. if false, the default game over screen will be shown.
static bool RandomDeadScreens = false;   // If set to true, a random game over screen will be shown when the player dies.

// Load Screen Hints
ref TStringArray loadscreens = {
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG1.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG2.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG3.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG4.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG5.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG6.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG7.edds"
};

// Game Over Screen
string SetRandomDeathScreen()
{
    TStringArray images = {
        "Colorful-UI/GUI/textures/DeathScreens/Cui3-DS1.edds", 
        "Colorful-UI/GUI/textures/DeathScreens/Cui3-DS2.edds",
        "Colorful-UI/GUI/textures/DeathScreens/Cui3-DS3.edds"
    };

    int randomIndex = cuiUtils.GetRandomIndex(images.Count());
    return images.Get(randomIndex);
}

// Link URLs add more if you want.
class MenuURLS {
	static string urlDiscord    = "#";
	static string urlFacebook   = "#";
	static string urlTwitter    = "#";
	static string urlReddit     = "#";
	static string urlYoutube    = "#";
	static string urlWebsite    = "#"; 
	static string urlPriorityQ  = "#";
	static string urlCustom     = "#";
}

// Menu IDs
const int COLORFUL_EXIT		    = 666;
const int COLORFUL_CONFIGURE    = 667;
const int COLORFUL_DEFAULTS     = 668;
