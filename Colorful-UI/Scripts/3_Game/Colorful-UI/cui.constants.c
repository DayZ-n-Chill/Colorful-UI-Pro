// Constants.c v3.0
static bool noHints			  = true;    // If set to true, the hints will not be shown during load screens.
static bool SyncHintImages    = false;	 // If set to true, the hints are synced to BG Images, all controlled by the hints.json.	
static bool RandomBackgrounds = false;	 // If set to true, a random background will be shown during load screens (SyncHintImages must be false. Image is changed in the layout files).
static bool ShowDeadScreen    = false;   // If set to true, a custom game over screen will be shown when the player dies. if false, the default game over screen will be shown.
static bool RandomDeadScreens = false;   // If set to true, a random game over screen will be shown when the player dies.

int m_PreviousRandomIndex = -1; // Declare this globally if it's needed across multiple functions

string SetRandomBackground()
{
	int currentTime = GetGame().GetTime();
    Math.Randomize(currentTime);
    Math.RandomFloat01(); // throw-away value to ensure proper randomization
    
    TStringArray images = {
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG1.edds", 
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG2.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG3.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG4.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG5.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG6.edds",
        "Colorful-UI/gui/textures/loading_screens/CUI2-BG7.edds"
    };

    int bgIndex = Math.RandomIntInclusive(0, images.Count() - 1);
    while (bgIndex == m_PreviousRandomIndex)
        bgIndex = Math.RandomIntInclusive(0, images.Count() - 1);

    m_PreviousRandomIndex = bgIndex;

    return images.Get(bgIndex);
}


string GetRandomGameOverScreen()
{
	const string images[] = {
		"Colorful-UI/gui/textures/globals/DeathScreen-BG1.edds", 
		"Colorful-UI/gui/textures/globals/DeathScreen-BG2.edds",
		"Colorful-UI/gui/textures/globals/DeathScreen-BG3.edds"
	};

	const int IMAGES_COUNT = 3;
	int bgIndex = Math.RandomInt(0, IMAGES_COUNT - 1);
	return images[bgIndex];
}

class GameOverScreen
{
    static string GameOverScreenImage() { return "Colorful-UI/gui/textures/globals/DeathScreen.edds"; };
};

// SOCIALS & LINKS ---------------------------------------------------------------
// Set your links Here
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

// DIALOGS ------------------------------------------------------------------------
// There is No Need to mess with anything below this line. 
const int COLORFUL_EXIT				= 666;
const int COLORFUL_CONFIGURE 		= 667;
const int COLORFUL_DEFAULTS 		= 668;
