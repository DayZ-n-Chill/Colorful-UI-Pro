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

        // Call the new button methods
        CUIButton.baseBtn(m_testBtn,  "Bitch Boi", UIColor.Violet(), UIColor.Yellow());
        CUIButton.baseBtn(m_testBtn2, "Bitch Ass", UIColor.Orange(), UIColor.Green());
        CUIButton.baseBtn(m_testBtn3, "Bitch Ass MF", UIColor.Blue(), UIColor.Blue());

        return layoutRoot;
    }
}
