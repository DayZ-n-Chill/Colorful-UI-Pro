// Ensure this file is in the 5_Mission folder so it loads after CUIManager.c

modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit();

        // Initialize the CUIManager
        InitCUIManager();
    }

    override void OnKeyPress(int key)
    {
        super.OnKeyPress(key);
        if (key == KeyCode.KC_P)
        {
            // Show dialog with a light blue background and green buttons
            GetCUIManager().ShowDialog("Test Dialog", "This is a test message.", 1, 1, 1, 1, null, 0xFF3232C8, 0xFF00C800);
        }
    }
}
