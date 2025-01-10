// Constants.c v3.0.0
static bool StartMainMenu     = false;   // If set to true, the main menu will be forced to show on startup.
static bool NoHints			  = false;   // If set to true, the hints will not be shown during load screens.
static bool LoadVideo         = false;   // If set to true, a video will be shown during load screens along with tips.
static bool MainMenuVideo     = false;   // If set to true, a video will be shown during Main Menu Screens along with tips.
static bool ShowDeadScreen    = false;   // If set to true, a custom game over screen will be shown when the player dies. if false, the default game over screen will be shown.
static bool RandomDeadScreens = false;   // If set to true, a random game over screen will be shown when the player dies.

// Loading Screens
ref TStringArray loadscreens = {
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG1.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG2.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG3.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG4.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG5.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG6.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG7.edds"
};

// Gameover Screens
ref TStringArray GameOverScreens = {
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG1.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG2.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG3.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG4.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG5.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG6.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG7.edds"
};

// Link URLs add more if you want.
class MenuURLS {
	static string urlDiscord    = "http://discord.com";
	static string urlFacebook   = "http://facebook.com";
	static string urlTwitter    = "http://twitter.com";
	static string urlReddit     = "http://reddit.com";
	static string urlYoutube    = "http://youtube.com";
	static string urlWebsite    = "http://dayz.com"; 
	static string urlPriorityQ  = "http://feedback.dayz.com";
	static string urlCustom     = "http://google.com";
}

// Menu IDs
const int COLORFUL_EXIT		    = 666;
const int COLORFUL_CONFIGURE    = 667;
const int COLORFUL_DEFAULTS     = 668;
