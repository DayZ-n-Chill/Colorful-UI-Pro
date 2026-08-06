// DialogueErrorProperties override — DIAGNOSTIC / DISABLED MODE
// -----------------------------------------------------------------------------
// Status: the custom CUI panel is temporarily DISABLED. On the last in-game
// test, a kick produced NO dialog at all (neither the CUI panel nor the
// native one) — worse than doing nothing. Per explicit instruction: native
// dialog beats no dialog, so HandleError now ALWAYS falls through to
// super.HandleError() (the original vanilla body, which calls the native
// g_Game.GetUIManager().ShowDialog(...)) regardless of category or handler.
// The interception/decision logic below still runs and still Prints what it
// WOULD have done, purely for diagnosis — it does not change behavior.
//
// What the RPT from that test actually showed
// (G:\DayZ n Chill\Servers\The-Vale\.server\!ClientDiagLogs\
//  DayZDiag_x64_2026-08-05_23-03-21.RPT:529,546,575,699):
//   [ErrorModuleHandler] :: Error thrown: 0x00040033 ()
// Decoding against ErrorModuleHandler.CreateError's own documented example
// (errormodulehandler.c:36-41: CreateError(ConnectErrorClient, -1) ==
// 0x0002FFFF, i.e. upper 16 bits == the ErrorCategory enum's raw int value)
// pins the ErrorCategory enum's implicit numbering as Unknown=0, Generic=1,
// ConnectErrorClient=2, ConnectErrorServer=3, ConnectErrorScript=4,
// ClientKicked=5, BIOSError=6. 0x0004 == ConnectErrorScript, NOT
// ClientKicked. ConnectErrorScriptModule unconditionally sets a real
// UI handler (connecterrorscriptmodule.c:23: m_UIHandler = new
// ConnectErrorScriptModuleUI()), which this override's own GetHandler()
// check was already built to skip. So the specific failure captured in that
// test almost certainly never reached ShowCui() at all — category alone
// should have sent it straight to super.HandleError() — and the "no dialog"
// symptom needs to be explained some other way (rapid-repeat suppression
// from the sub-10-second reconnect loop under the TimeLogin=3 test setting,
// dialog replacement by the next connect attempt before it could render,
// etc). That's a hypothesis, not a conclusion — hence leaving the panel
// disabled and instrumenting instead of re-guessing at a second fix blind.
//
// Original background (still accurate): vanilla DialogueErrorProperties.
// HandleError (errorproperties.c:55-74) is the generic handler behind every
// dialog-shown error in the game, rendering via a `proto native` ShowDialog
// call with no .layout file behind it — genuinely unreachable for reskinning
// except by intercepting HandleError() itself, which is a plain script
// method (not proto). The interception below is scoped to
// ErrorCategory.ClientKicked with no UI handler, since every kick/BattlEye/
// DB/auth error in ClientKickedModule.c leaves m_UIHandler at its default of
// null (only ConnectErrorScriptModule ever sets a real one) — meaning no
// OnModalResult contract to replicate for those specifically. That scoping
// reasoning stands; what's disabled is ACTING on it until a test run proves
// out the CreateWidgets-during-transition hypothesis one way or the other.
modded class DialogueErrorProperties
{
    protected Widget       m_CuiRoot;
    protected Widget       m_CuiPanel;
    protected TextWidget   m_CuiCaption;
    protected TextWidget   m_CuiMessage;
    protected ButtonWidget m_CuiOk;

    override void HandleError(int errorCode, string additionalInfo = "")
    {
        ErrorCategory category = ErrorModuleHandler.GetCategoryFromError(errorCode);
        UIScriptedMenu handler = GetHandler();

        Print(string.Format(
            "[CUI ErrorDialog] HandleError errorCode=%1 hex=%2 category=%3 hasHandler=%4 additionalInfo='%5'",
            errorCode, ErrorModuleHandler.GetErrorHex(errorCode), category, handler != null, additionalInfo));

        bool wouldUseCui = (category == ErrorCategory.ClientKicked) && !handler;
        Print(string.Format("[CUI ErrorDialog] wouldUseCui=%1 (panel currently DISABLED - always falling through to super.HandleError)", wouldUseCui));

        // DISABLED: always defer to vanilla's real body (the native dialog).
        // See file header for why. Re-enabling means restoring the guarded
        // branch this used to have: skip straight to super.HandleError()
        // under NO_GUI or SERVER builds, and whenever !wouldUseCui; only
        // when wouldUseCui is true, build `message` from m_DisplayAdditionalInfo
        // / m_Message / additionalInfo (via EP_MESSAGE_FORMAT_STRING, same as
        // vanilla's own body) and call
        // ShowCui(string.Format(EP_HEADER_FORMAT_STRING, m_Header,
        // ErrorModuleHandler.GetErrorHex(errorCode)), message) instead of
        // falling through — pending a test run that actually confirms a
        // ClientKicked/no-handler case reaches here.
        Print("[CUI ErrorDialog] calling super.HandleError (native ShowDialog path)");
        super.HandleError(errorCode, additionalInfo);
    }

    protected void ShowCui(string caption, string message)
    {
        Print(string.Format("[CUI ErrorDialog] ShowCui caption='%1' message='%2'", caption, message));

        if (!m_CuiRoot)
        {
            m_CuiRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.errordialog.layout");
            Print(string.Format("[CUI ErrorDialog] CreateWidgets returned %1", m_CuiRoot != null));
            if (!m_CuiRoot) return;

            m_CuiPanel   = m_CuiRoot.FindAnyWidget("DialogBox");
            m_CuiCaption = TextWidget.Cast(m_CuiRoot.FindAnyWidget("Caption"));
            m_CuiMessage = TextWidget.Cast(m_CuiRoot.FindAnyWidget("MessageText"));
            m_CuiOk      = ButtonWidget.Cast(m_CuiRoot.FindAnyWidget("OkButton"));

            Print(string.Format("[CUI ErrorDialog] widgets found panel=%1 caption=%2 message=%3 ok=%4",
                m_CuiPanel != null, m_CuiCaption != null, m_CuiMessage != null, m_CuiOk != null));

            // Same "layout color drops alpha on DayZDefaultPanel" workaround
            // as CuiDialog.CuiDialog (Dialogs.c:110).
            if (m_CuiPanel) m_CuiPanel.SetColor(ARGB(220, 62, 62, 62));

            if (m_CuiOk)
            {
                m_CuiOk.SetTextColor(colorScheme.PrimaryText());
                m_CuiOk.SetColor(UIColor.Transparent());
                m_CuiOk.SetHandler(new CuiErrorDialogOkHandler(this));
            }
        }

        if (g_Game.GetUIManager().IsDialogVisible())
        {
            g_Game.GetUIManager().HideDialog();
        }

        if (m_CuiCaption) m_CuiCaption.SetText(caption);
        if (m_CuiMessage) m_CuiMessage.SetText(message);

        if (m_CuiMessage && m_CuiPanel)
        {
            int sx, sy;
            m_CuiMessage.GetTextSize(sx, sy);
            if (sy < 24) sy = 24;

            float panelW, panelH;
            m_CuiPanel.GetSize(panelW, panelH);
            m_CuiPanel.SetSize(panelW, 74.0 + sy + 80.0);
        }

        if (m_CuiRoot)
        {
            m_CuiRoot.Show(true);
            Print(string.Format("[CUI ErrorDialog] Show(true) called, IsVisible=%1", m_CuiRoot.IsVisible()));
        }
    }

    void CloseCui()
    {
        if (m_CuiRoot) m_CuiRoot.Show(false);
    }
}

class CuiErrorDialogOkHandler : ScriptedWidgetEventHandler
{
    protected DialogueErrorProperties m_Owner;

    void CuiErrorDialogOkHandler(DialogueErrorProperties owner)
    {
        m_Owner = owner;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (m_Owner) m_Owner.CloseCui();
        return true;
    }
}
