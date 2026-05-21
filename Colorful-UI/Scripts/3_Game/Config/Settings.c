// Constants.c v3.0.0
static bool StartMainMenu      = true;  // If set to true, the main menu will be forced to show on startup.
static bool NoHints			   = false;  // If set to true, the hints will not be shown during load screens.
static bool UseImagesets       = false;  // If set to true, hints.json entries with m_ImageSet/m_ImageName load from the registered imageset; otherwise m_ImagePath is used. Off by default: the Loadscreens atlas .edds isn't committed (too large, no LFS), so imageset mode breaks Loadscreens/credits/main-menu bg on fresh clones. Flip back to true once you've baked Loadscreens-Imageset.edds locally.
static bool LoadVideo          = false;  // If set to true, a video will be shown during load screens along with tips.
static bool ShowGameOverOverlay = false;  // Internal runtime flag toggled by DayZPlayerImplement.ShowDeadScreen() to drive the custom game-over overlay in InGameMenu/Respawn. NOT a user-facing config (it is reset each death).

// The below two settings are not yet implemented.
static bool EnableMenuVideo    = false;  // Background video on main menu (via workspace-rooted CuiBackgroundVideo).
static bool EnableOptionsVideo = false;  // Background video on options menu (when opened from main menu).
static bool VideoDeathScreens  = false;  // If set to true, a random game over screen will be shown when the player dies.
// static bool RandomDeathScreens = false;  // If set to true, a random game over video that will be shown when the player dies.

// Server Information 
// (Possibly Buggy in the CUI Buttons. Not yet tested on live servers)
static const string SERVER_IP = "127.0.0.1";
static const int SERVER_PORT = 2302;

// Video Settings (Change them up)
static const string m_LoadingVideo     = "CUI_Video.mov";  // Video file name for loading screen video. 
static const string m_MainMenuVideo    = "CUI_Video.mov";  // Video file name for Main Menu screen video. 
static const string m_OptionsMenuVideo = "CUI_Video.mov";  // Video file name for Options screen video. 

// Main Menu Background — picks between imageset sprite and raw .edds
// based on UseImagesets. Edit either side to repoint.
string GetMainMenuBackground()
{
    if (UseImagesets)
        return "set:Loadscreens image:loadscreen_6";
    return "Colorful-UI/GUI/textures/LoadScreens/Loadscreen_6.edds";
}
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

