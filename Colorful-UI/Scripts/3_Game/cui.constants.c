// Constants.c v3.0 (These are a WIP and will be updated as the mod progresses)
static bool noHints			  = false;    // If set to true, the hints will not be shown during load screens.
static bool SyncHintImages    = false;	 // If set to true, the hints are synced to BG Images, all controlled by the hints.json.	
static bool RandomBackgrounds = false;	 // If set to true, a random background will be shown during load screens (SyncHintImages must be false. Image is changed in the layout files).
static bool ShowDeadScreen    = false;   // If set to true, a custom game over screen will be shown when the player dies. if false, the default game over screen will be shown.
static bool RandomDeadScreens = false;   // If set to true, a random game over screen will be shown when the player dies.

string SetRandomBackground()
{
    TStringArray images = {
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG1.edds", 
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG2.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG3.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG4.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG5.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG6.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG7.edds"
    };

    int randomIndex = cuiUtils.GetRandomIndex(images.Count());
    return images.Get(randomIndex);
}

string SetRandomDeathScreen()
{
    TStringArray images = {
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG1.edds", 
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG2.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG3.edds"
    };

    int randomIndex = cuiUtils.GetRandomIndex(images.Count());
    return images.Get(randomIndex);
}

class GameOverScreen
{
    static string GameOverScreenImage() { return "Colorful-UI/gui/textures/globals/DeathScreen.edds"; };
};

class MenuURLS {
	static string urlDiscord   = "#";
	static string urlFacebook  = "#";
	static string urlTwitter   = "#";
	static string urlReddit    = "#";
	static string urlYoutube   = "#";
	static string urlWebsite   = "#"; 
	static string urlPriorityQ = "#";
	static string urlCustom    = "#";
}

const int COLORFUL_EXIT				= 666;
const int COLORFUL_CONFIGURE 		= 667;
const int COLORFUL_DEFAULTS 		= 668;
