// Ensure this file is in the 5_Mission folder so it loads after CUIManager.c

modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit();

        // Initialize the CUIManager
        InitCUIManager();

        // Initialize Logo Overlay: creates the root widget and loads the logo image, 
        // ensuring it appears consistently across various screens throughout the session.
        LogoRootWidget = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/cui_pro/logo.layout");
        LogoWidget = ImageWidget.Cast(LogoRootWidget.FindAnyWidget("logoWidget"));
        LogoWidget.LoadImageFile(0, "Colorful-UI/gui/textures/gloabals/Colorful UI Logo.edds");

    }

    override void OnKeyPress(int key)
    {
        super.OnKeyPress(key);
        if (key == KeyCode.KC_P)
        {
            // Toggle the logo overlay visibility
            LogoEnabled = !LogoWidget.IsVisible();
            SaveOverlayData();
        }
    }
}
