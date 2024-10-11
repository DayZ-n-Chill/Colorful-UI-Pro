// Global instance of CustomUIManager
ref CustomUIManager g_CustomUIManager;

// Function to initialize the CustomUIManager globally
void InitCustomUIManager()
{
    if (!g_CustomUIManager)
    {
        g_CustomUIManager = new CustomUIManager();
    }
}

// Function to retrieve the CustomUIManager instance
CustomUIManager GetCustomUIManager()
{
    return g_CustomUIManager;
}

// CustomUIManager class that wraps around UIManager and adds global dialog customizations
class CustomUIManager
{
    private UIManager m_UIManager;

    void CustomUIManager()
    {
        // Initialize the native UIManager instance
        m_UIManager = GetGame().GetUIManager();
        
        // Start the background check to apply customizations to all dialogs
        CheckAndCustomizeDialogs();
    }

    /**
     * Shows a dialog with custom settings.
     * Wraps around UIManager's ShowDialog function to add custom behavior.
     *
     * @param caption - The title text of the dialog.
     * @param text - The main message content of the dialog.
     * @param id - Unique identifier for the dialog.
     * @param butts - Button configuration for the dialog.
     * @param def - Default button for the dialog.
     * @param type - Type of dialog message (e.g., question, info).
     * @param handler - Event handler for button responses.
     */
    void ShowCustomDialog(string caption, string text, int id, int butts, int def, int type, UIScriptedMenu handler)
    {
        // Use the native UIManager to show the dialog
        m_UIManager.ShowDialog(caption, text, id, butts, def, type, handler);

        // Apply customizations after the dialog is created
        CustomizeDialog(id);
    }

    /**
     * Customizes a dialog with a specified ID, applying default size, position,
     * and styling enhancements.
     *
     * @param id - Unique identifier for the dialog to customize.
     */
    private void CustomizeDialog(int id)
    {
        UIScriptedMenu dialog = m_UIManager.FindMenu(id);

        if (dialog)
        {
            Widget rootWidget = dialog.GetLayoutRoot();
            if (rootWidget)
            {
                // Set default dimensions for the dialog
                rootWidget.SetSize(0.8, 0.6); // Width and height as fractions of screen size

                // Set default position for the dialog
                rootWidget.SetPos(0.1, 0.1); // X and Y as fractions of screen position
            }

            // Apply additional custom styling
            ApplyCustomStyling(rootWidget);
        }
    }

    /**
     * Applies custom styling to a dialog, such as background colors or font changes.
     *
     * @param rootWidget - The root Widget instance representing the dialog's layout.
     */
    private void ApplyCustomStyling(Widget rootWidget)
    {
        if (rootWidget)
        {
            // Set background color to opaque white (example)
            rootWidget.SetColor(ARGB(255, 255, 255, 255));

            // Additional style modifications can go here, such as font size or borders
            // e.g., rootWidget.SetTextColor(ARGB(255, 0, 0, 0));
        }
    }

    /**
     * Performs a background check at regular intervals to detect and customize any dialogs
     * that were not created through CustomUIManager. This function is useful for modifying
     * vanilla dialogs or dialogs created outside of this mod.
     */
    private void CheckAndCustomizeDialogs()
    {
        // Run this check every 100 milliseconds to apply customizations to any visible dialogs
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CustomizeAllVisibleDialogs, 100, true);
    }

    /**
     * Loops through all currently open dialogs and applies global customizations.
     */
    private void CustomizeAllVisibleDialogs()
    {
        // Get the top-level UIManager menu (dialogs are typically stacked as menus)
        UIScriptedMenu menu = m_UIManager.GetMenu();

        // Loop through each open dialog and apply customizations
        while (menu)
        {
            Widget rootWidget = menu.GetLayoutRoot();
            if (rootWidget && ShouldCustomize(menu))
            {
                // Set a standard size and position
                rootWidget.SetSize(0.8, 0.6); // Example: width=80% of screen, height=60%
                rootWidget.SetPos(0.1, 0.1);  // Example: positioned 10% from top-left

                ApplyCustomStyling(rootWidget);
            }

            // Move to the next menu in the stack (parent menu)
            menu = UIScriptedMenu.Cast(menu.GetParentMenu());
        }
    }

    /**
     * Determines if a dialog should be customized (useful for preventing redundant customizations).
     * This could check a flag or other properties to avoid reapplying customizations unnecessarily.
     *
     * @param menu - The UIScriptedMenu instance representing the dialog to check.
     * @return - Returns true if customization should be applied, false otherwise.
     */
    private bool ShouldCustomize(UIScriptedMenu menu)
    {
        // Placeholder for custom logic to prevent redundant customization
        return true; // Always return true for now to customize all dialogs
    }

    // Access to any other UIManager function directly through the wrapper
    void CloseDialog()
    {
        m_UIManager.CloseDialog();
    }

    bool IsDialogVisible()
    {
        return m_UIManager.IsDialogVisible();
    }
}
