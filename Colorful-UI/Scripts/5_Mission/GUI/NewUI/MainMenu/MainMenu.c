modded class MainMenu extends UIScriptedMenu {
    private ButtonWidget m_testBtn;
    private ButtonWidget m_testBtn2;
    private ButtonWidget m_testBtn3;

    override Widget Init() {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/new_ui/testing.layout");

        // Find the buttons in your layout
        m_testBtn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn"));
        m_testBtn2 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn2"));
        m_testBtn3 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn3"));

        // Call the custom button styles method with button type specified as the second parameter
        CustomButtonUtility.customBtnStyles(m_testBtn, ButtonType.DEFAULT, "Bitch Boi", 0xFFFFFFFF, 0xFFFF0088); // For testBtn
        CustomButtonUtility.customBtnStyles(m_testBtn2, ButtonType.TEXT_WIDGET, "Bitch Ass", 0xFFFFFFFF, 0xFFFF0000); // For testBtn2
        CustomButtonUtility.customBtnStyles(m_testBtn3, ButtonType.TEXT_WIDGET, "Another Button", 0xFFFFFFFF, UIColor.discord()); // For testBtn3

        return layoutRoot;
    }
}
