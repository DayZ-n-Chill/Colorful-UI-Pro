// Constants.c v3.0.0
static bool StartMainMenu      = false;   // If set to true, the main menu will be forced to show on startup.
static bool NoHints			   = false;   // If set to true, the hints will not be shown during load screens.
static bool LoadVideo          = false;   // If set to true, a video will be shown during load screens along with tips.
static bool ShowDeadScreen     = false;   // If set to true, a custom game over screen will be shown when the player dies. if false, the default game over screen will be shown.

// The below two settings are not yet implemented.
static bool MainMenuVideo      = false;   // If set to true, a video will be shown during Main Menu Screens along with tips.
static bool VideoDeathScreens  = false;   // If set to true, a random game over screen will be shown when the player dies.
static bool RandomDeathScreens = false;  // If set to true, a random game over video that will be shown when the player dies.

// Server Information 
// (Possibly Buggy in the CUI Buttons. Not yet tested on live servers)
static const string SERVER_IP = "127.0.0.1";
static const int SERVER_PORT = 2302;

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

// Gameover Screens (Array Not yet implemented)
ref TStringArray GameOverScreens = {
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG1.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG2.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG3.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG4.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG5.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG6.edds",
    "Colorful-UI/GUI/textures/LoadScreens/Cui3-BG7.edds"
};

// Set Single Game Over Screen ( Death Screen )
class GameOverScreen
{
    static string GameOverScreenImage() { return "Colorful-UI/GUI/textures/DeathScreens/DeathScreen.edds"; };
};

// Set Your Servers Logo
// NOTE:  Width and High Settings are not yet implemented here.
//        Recommended size is 512x512 But you must set the size manually in the layout file for the moment.
class Branding
{
    static string Logo() { return "Colorful-UI/GUI/textures/DeathScreens/DeathScreen.edds"; };
};

// Link URLs (Add your own links here)
// NOTE:  If you do not want to use a button, set the URL to "#" or "".
//        If you do not set them they will not show up in the layout when compiled.
//        This way it shows only the buttons you want to use.

class CustomURL {
	static string Website    = "#"; 
	static string PriorityQ  = "#";
	static string Custom     = "#";
}

class SocialURL {
	static string Discord    = "#";
	static string Facebook   = "#";
	static string Twitter    = "#";
	static string Reddit     = "#";
	static string Youtube    = "#";
}

// Menu IDs
const int COLORFUL_EXIT		    = 666;
const int COLORFUL_CONFIGURE    = 667;
const int COLORFUL_DEFAULTS     = 668;
