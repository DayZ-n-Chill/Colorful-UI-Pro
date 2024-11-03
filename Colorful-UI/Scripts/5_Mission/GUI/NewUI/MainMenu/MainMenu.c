// MainMenu.c

modded class MainMenu extends UIScriptedMenu {
    private ButtonWidget m_testBtn;
    private ButtonWidget m_testBtn2;
    private ButtonWidget m_testBtn3;

    override Widget Init() {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/new_ui/testing.layout");

        m_testBtn  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn"));
        m_testBtn2 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn2"));
        m_testBtn3 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn3"));

        // Now you can call baseBtn with optional parameters
        // Example 1: Provide all parameters
        CUIButton.baseBtn(m_testBtn);

        // Example 2: Provide only some parameters
        CUIButton.baseBtn(m_testBtn2);

        // Example 3: Use all default values
        CUIButton.baseBtn(m_testBtn3);

        return layoutRoot;
    }
}
