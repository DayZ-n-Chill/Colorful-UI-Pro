// MainMenu class
modded class MainMenu extends UIScriptedMenu {
    private ButtonWidget m_testBtn;
    private ButtonWidget m_testBtn2;
    private ButtonWidget m_testBtn3;

    override Widget Init() {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/new_ui/testing.layout");

        m_testBtn  = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn"));
        m_testBtn2 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn2"));
        m_testBtn3 = ButtonWidget.Cast(layoutRoot.FindAnyWidget("testBtn3"));

        CUIButton.baseBtn(m_testBtn,   "Button 1", UIColor.Blue(), UIColor.Violet(), UIColor.Orange(), UIColor.Green());
        CUIButton.baseBtn(m_testBtn2,  "Button 2", UIColor.Blue(), UIColor.Violet(), UIColor.Orange(), UIColor.Green());
        CUIButton.baseBtn(m_testBtn3,  "Button 3", UIColor.Blue(), UIColor.Violet(), UIColor.Orange(), UIColor.Green());

        return layoutRoot;
    }
}
