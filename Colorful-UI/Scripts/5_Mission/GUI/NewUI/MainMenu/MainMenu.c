// MainMenu class
modded class MainMenu extends UIScriptedMenu {
    private ButtonWidget m_testBtn;
    private ButtonWidget m_testBtn2;
    private ButtonWidget m_testBtn3;

    override Widget Init() {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/new_ui/testing.layout");

        // Find the buttons in your layout
        m_testBtn  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn"));
        m_testBtn2 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn2"));
        m_testBtn3 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn3"));

        // Define colors using the ARGB method
        int textColor = CUIButton.ARGB(255, 255, 255, 255);        // White text
        int hoverTextColor = CUIButton.ARGB(255, 255, 255, 0);     // Yellow text on hover
        int baseColor = CUIButton.ARGB(255, 0, 122, 204);          // Blue button
        int hoverColor = CUIButton.ARGB(255, 0, 153, 255);         // Lighter blue on hover

        // Apply the baseBtn method to each button
        CUIButton.baseBtn(m_testBtn,  "Button 1", UIColor.Blue(),   UIColor.Violet(), baseColor, hoverColor);
        CUIButton.baseBtn(m_testBtn2, "Button 2", UIColor.Orange(), UIColor.Red(),    baseColor, hoverColor);
        CUIButton.baseBtn(m_testBtn3, "Button 3", UIColor.Green(),  UIColor.Indigo(), baseColor, hoverColor);

        return layoutRoot;
    }
}