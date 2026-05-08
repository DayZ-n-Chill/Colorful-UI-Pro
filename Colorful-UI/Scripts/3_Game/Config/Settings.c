// Constants.c v3.0.0
static bool StartMainMenu      = false;  // If set to true, the main menu will be forced to show on startup.
static bool NoHints			   = false;  // If set to true, the hints will not be shown during load screens.
static bool LoadVideo          = true;   // If set to true, a video will be shown during load screens along with tips.
static bool ShowGameOverOverlay = false;  // Internal runtime flag toggled by DayZPlayerImplement.ShowDeadScreen() to drive the custom game-over overlay in InGameMenu/Respawn. NOT a user-facing config (it is reset each death).

// The below two settings are not yet implemented.
static bool EnableMenuVideo    = true;   // If set to true, a video will be shown during Main Menu Screens along with tips.
static bool EnableOptionsVideo = true;   // If set to true, a video will be shown during Main Menu Screens along with tips.
static bool VideoDeathScreens  = false;  // If set to true, a random game over screen will be shown when the player dies.
static bool RandomDeathScreens = false;  // If set to true, a random game over video that will be shown when the player dies.

// Server Information 
// (Possibly Buggy in the CUI Buttons. Not yet tested on live servers)
static const string SERVER_IP = "127.0.0.1";
static const int SERVER_PORT = 2302;

// Video Settings (Change them up)
static const string m_LoadingVideo     = "CUI_Video.mov";  // Video file name for loading screen video. 
static const string m_MainMenuVideo    = "CUI_Video.mov";  // Video file name for Main Menu screen video. 
static const string m_OptionsMenuVideo = "CUI_Video.mov";  // Video file name for Options screen video. 

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
//        Recommended size is 512x512 But you can set the size manually in the layout file for the moment.
class Branding
{
    static string Logo() { return "Colorful-UI/GUI/textures/Shared/CuiPro_Logo.edds"; }; 
    static void ApplyLogo(ImageWidget widget)
    {
        if (!widget) return;
        widget.LoadImageFile(0, Logo());
        widget.SetFlags(WidgetFlags.STRETCH);
    }
};

// Link URLs (Add your own links here)
// NOTE:  If you do not want to use a button, set the URL to "#" or "".
//        If you do not set them they will not show up in the layout when compiled.
//        This way it shows only the buttons you want to use.

class CustomURL {
	static string Website    = "test"; 
	static string PriorityQ  = "test";
	static string Custom     = "test";
}

class SocialURL {
	static string Discord    = "test";
	static string Facebook   = "test";
	static string Twitter    = "test";
	static string Reddit     = "test";
	static string Youtube    = "test";
}

// Menu IDs
const int COLORFUL_EXIT		    = 666;
const int COLORFUL_CONFIGURE    = 667;
const int COLORFUL_DEFAULTS     = 668;
