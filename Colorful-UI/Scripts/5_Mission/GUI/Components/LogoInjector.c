class LogoInjector
{
    private Widget m_LogoWidget;

    // Constructor
    void LogoInjector(Widget parent, string logoPath, float width, float height, vector position)
    {
        // Create the logo widget in the specified parent layout
        m_LogoWidget = ImageWidget.Cast(GetGame().GetWorkspace().CreateWidgets("DayZModGUI/layouts/logo.layout", parent));

        // Ensure the widget was created successfully
        if (m_LogoWidget)
        {
            ImageWidget logo = ImageWidget.Cast(m_LogoWidget);
            if (logo)
            {
                // Set the logo image, size, and position
                logo.LoadImageFile(0, logoPath);
                logo.SetSize(width, height);
                logo.SetPos(position[0], position[1]);
            }
        }
        else
        {
            Print("[LogoInjector] Failed to create the logo widget.");
        }
    }

    // Destructor for cleanup
    void ~LogoInjector()
    {
        if (m_LogoWidget)
        {
            m_LogoWidget.Unlink();
        }
    }
}
