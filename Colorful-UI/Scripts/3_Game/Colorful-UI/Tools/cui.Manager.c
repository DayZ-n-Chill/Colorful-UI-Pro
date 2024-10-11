// Global instance of CUIManager
ref CUIManager g_CUIManager;

// Function to initialize the CUIManager globally
void InitCUIManager()
{
    if (!g_CUIManager)
    {
        g_CUIManager = new CUIManager();
    }
}

// Function to retrieve the CUIManager instance
CUIManager GetCUIManager()
{
    return g_CUIManager;
}

// CUIManager class that wraps around UIManager
class CUIManager
{
    private UIManager m_UIManager;

    void CUIManager()
    {
        m_UIManager = GetGame().GetUIManager();
    }

    // ShowDialog method with optional color customization
    void ShowDialog(string caption, string text, int id, int butts, int def, int type, UIScriptedMenu handler, int backgroundColor = 0xFFFFFFFF, int buttonColor = 0xFFC8C8C8)
    {
        // Show the dialog
        m_UIManager.ShowDialog(caption, text, id, butts, def, type, handler);
        
        // Apply the custom styling to the dialog with provided colors
        CustomizeDialogAppearance(id, backgroundColor, buttonColor);
    }

    // CloseDialog method
    void CloseDialog()
    {
        m_UIManager.CloseDialog();
    }

    // Apply the custom colors to the dialog appearance
    private void CustomizeDialogAppearance(int dialogId, int backgroundColor, int buttonColor)
    {
        UIScriptedMenu dialog = m_UIManager.FindMenu(dialogId);
        if (dialog)
        {
            Widget rootWidget = dialog.GetLayoutRoot();
            if (rootWidget)
            {
                // Set background color
                rootWidget.SetColor(backgroundColor);
                
                // Apply button color customization
                CustomizeButtonColors(rootWidget, buttonColor);
            }
        }
    }

    // Customize button colors in the dialog
    private void CustomizeButtonColors(Widget rootWidget, int buttonColor)
    {
        if (!rootWidget) return;

        // Locate button widgets within the dialog
        Widget buttonWidget = rootWidget.FindAnyWidget("Button"); // Update "Button" if specific button IDs are known

        while (buttonWidget)
        {
            // Set button background color
            buttonWidget.SetColor(buttonColor);

            // Move to the next button (if any)
            buttonWidget = buttonWidget.GetSibling();
        }
    }
}
