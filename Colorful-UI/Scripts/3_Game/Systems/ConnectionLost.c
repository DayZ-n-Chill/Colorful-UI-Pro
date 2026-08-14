// ConnectionLost — CUI panel in place of the plain red text.
// Vanilla source: P:\scripts\3_game\dayzgame.c

modded class ConnectionLost
{
    protected Widget     m_CuiRoot;
    protected Widget     m_CuiPanel;
    protected TextWidget m_CuiText;
    protected bool       m_CuiInit;

    protected void EnsureCuiWidgets()
    {
        if (m_CuiInit) return;
        m_CuiInit = true;

        m_CuiRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.connectionlost.layout");
        if (!m_CuiRoot) return;

        m_CuiPanel = m_CuiRoot.FindAnyWidget("DialogBox");
        m_CuiText  = TextWidget.Cast(m_CuiRoot.FindAnyWidget("MessageText"));

        if (m_CuiPanel) m_CuiPanel.SetColor(ARGB(220, 62, 62, 62));

        m_CuiRoot.Show(false);
    }

    override void Show()
    {
        EnsureCuiWidgets();
        if (!m_CuiRoot) return;

        if (!m_CuiRoot.IsVisible())
        {
            if (g_Game.GetUIManager().IsDialogVisible())
            {
                g_Game.GetUIManager().HideDialog();
            }

            m_CuiRoot.Show(true);
            if (m_CuiText) m_CuiText.SetText("");
        }
    }

    override void Hide()
    {
        if (m_CuiRoot && m_CuiRoot.IsVisible())
        {
            m_CuiRoot.Show(false);
        }
    }

    override void SetText(string text)
    {
        EnsureCuiWidgets();
        if (m_CuiText) m_CuiText.SetText(text);
    }
}
