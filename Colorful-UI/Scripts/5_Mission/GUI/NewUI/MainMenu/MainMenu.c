// MainMenu.c

modded class MainMenu extends UIScriptedMenu {
    private ButtonWidget m_testBtn;
    private ButtonWidget m_testBtn2;
    private ButtonWidget m_testBtn3;
    private ButtonWidget m_textBtn;

    override Widget Init() {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/new_ui/testing.layout");

        m_testBtn  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn"));
        m_testBtn2 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn2"));
        m_testBtn3 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn3"));
        m_textBtn  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("textBtn"));

        // Existing baseBtn examples  Button Text,TextColor,HoverTextColor,Base/Focus Color,HoverColor
        CUIButton.baseBtn(
            m_testBtn,       // ButtonWidget
            "Button 1",      // Button Text
            -1,              // TextColor
            -1,              // HoverTextColor
            UIColor.Green(), // Base/Focus Color
            UIColor.Red());  // HoverColor
        
        
        
        CUIButton.baseBtn(m_testBtn2, "Button 2",   UIColor.White(),  UIColor.Violet(),  UIColor.Transparent(),  UIColor.Transparent());
        CUIButton.baseBtn(m_testBtn3);

        return layoutRoot;
    }
}
