// ConnectionLost override
// -----------------------------------------------------------------------------
// Vanilla ConnectionLost (P:\scripts\3_game\dayzgame.c:346-397) is a bare
// FrameWidget + one red TextWidget with no panel/background at all — it's
// what renders kick/disconnect reason text (e.g. BattlEye kick codes like
// "0x00040033"). It is NOT a UIScriptedMenu (just a plain class with
// Show()/Hide()/SetText(), owned directly by DayZGame), and it's defined in
// 3_Game — same module as this override, so (unlike WarningMenuBase's
// CuiDialog-wrapper trick in Colorful-UI\Scripts\5_Mission\GUI\CUI\dialogs\
// ItemDropWarning.c) CuiDialog/CuiBackdrop aren't reachable here. Chrome is
// built inline instead, same pattern used for ANVICaptchaMenu.
//
// Deliberately no animation and no backdrop scrim here:
//   - This can appear during live gameplay for transient network hiccups,
//     not just hard kicks — vanilla's own version doesn't block clicks
//     (no ignorepointer trickery, just floating text), so this keeps that
//     same non-blocking behavior; only the visuals (a proper dark panel
//     instead of bare red text) change.
//   - After the captcha's Close()-in-the-exit-animation regression, this
//     stays deliberately simple: Show()/Hide() remain instant, exactly like
//     vanilla's own toggle, with nothing CallLater/animation-related that
//     could interact with connect/disconnect timing.
//
// Vanilla's own m_WidgetRoot (the tiny hidden red-text widget) still gets
// created by the untouched constructor — harmless dead weight, left alone
// rather than risking a modded-class constructor override.
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

        // Same "layout color drops alpha on DayZDefaultPanel" workaround as
        // CuiDialog.CuiDialog (Dialogs.c:110) — translucent panel tint is
        // applied at runtime via ARGB.
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
