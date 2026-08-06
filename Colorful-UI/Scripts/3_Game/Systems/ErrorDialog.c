// DialogueErrorProperties override — generic capture, no 3_Game widgets
// -----------------------------------------------------------------------------
// Vanilla DialogueErrorProperties.HandleError (errorproperties.c:55-74) is the
// generic handler behind every dialog-shown error in the game, rendering via
// g_Game.GetUIManager().ShowDialog(...), a `proto native` call with no
// .layout file behind it.
//
// Scoping — generalized from an earlier ClientKicked-only version: ANY error
// that reaches HandleError() with NO attached UIScriptedMenu handler is now
// captured and routed to the CUI dialog system, regardless of category or
// code. Handler presence is the only thing that decides it, checked via
// GetHandler() (DialogueErrorProperties.GetHandler(), errorproperties.c:80),
// because a handler means real interactive semantics (Yes/No/Cancel driving
// OnModalResult callbacks — e.g. ConnectErrorScriptModuleUI's
// DisconnectSessionForce()/DisconnectSessionScript() dance for
// ALREADY_CONNECTING, connecterrorscriptmodule.c:53-93) that an info-only
// CuiDialog can't replicate. Those stay on the native path untouched — this
// only ever intercepts info dialogs (m_Handler left at its default null,
// errorhandlermodule.c:67), which is what every OTHER category
// (ConnectErrorClient, ConnectErrorServer, ClientKicked, BIOSError, Generic)
// registers.
//
// History of this file, briefly: an earlier version created a custom
// inline-chrome widget panel directly from HandleError (deferred one
// CallLater tick), fully suppressing the native ShowDialog call for kicks.
// It worked functionally but a native access-violation crash followed
// shortly after, during a rapid double-kick test — root cause was never
// conclusively pinned to the widget creation itself, but the design changed
// afterward to remove that whole class of risk: this file creates no widgets
// and calls nothing UI-related at HandleError() time. It only captures data
// into the CuiPendingError static holder and, further below, asks whatever
// Mission is currently running to flush it.
//
// Cross-module dispatch - how 3_Game reaches a 5_Mission method:
// CuiFlushPendingError() is declared on `modded class MissionBase`
// (Colorful-UI\Scripts\5_Mission\GUI\CUI\menus\MainMenu.c). MissionBase lives
// in the missionScriptModule (P:\scripts\5_mission\mission\missionbase.c:1),
// which this gameScriptModule file cannot name at compile time. The obvious
// workaround - an empty CuiFlushPendingError() stub on `modded class Mission`
// here in 3_Game, overridden there - does NOT compile: Mission
// (P:\scripts\3_game\gameplay.c:685) is instantiated and owned by the engine,
// and the compiler rejects it with "engine class Mission cannot be modded".
//
// So the call is dispatched dynamically by name instead:
//   GetGame().GameScript.CallFunction(mission, "CuiFlushPendingError", null, 0)
// ScriptModule.CallFunction(Class inst, string function, out void returnVal,
// void parm) is declared at P:\scripts\1_core\proto\enscript.c:146; a null
// returnVal for a void method plus 0 for the unused parameter is the vanilla
// idiom (P:\scripts\3_game\gui\effects\radialmenu.c:144,
// P:\scripts\3_game\entities\dayzanimal.c:443) and is already how this mod
// dispatches its own UI callbacks (Components/buttons.c:224,
// Components/Dialogs.c:202 and :213). It resolves against the runtime type of
// the instance, so no compile-time knowledge of MissionBase is needed here.
// Trade-off: a rename on the 5_Mission side fails at runtime, not at compile
// time. That is the price of crossing the module boundary.
//
// The instance handed to CallFunction is whatever GetMission() returns, and
// MissionMainMenu / MissionGameplay / MissionServer all extend MissionBase,
// so one implementation covers "player is at the main menu" and "player is
// in a mission" alike.
//
// (Historical note: an earlier revision of this file tried the
// stub-in-an-early-module / override-in-a-later-module pattern that vanilla
// itself uses for Mission -> MissionBaseWorld -> MissionBase and for
// Hud -> IngameHud. That pattern only works between script-only classes;
// Mission is not one of them, hence the dispatch-by-name above.)
//
// Two flush points exist:
//   1. Immediate-deferred: one GUI-queue tick after capture, TryImmediateFlush()
//      checks the exact same "is it safe to show a dialog right now" gate
//      vanilla's own dialog-queue draining uses — DayZGame.CheckDialogs()
//      (dayzgame.c:3386-3396): `mission && !m_loading.IsLoading()`, exposed
//      publicly as DayZGame.IsLoading() (dayzgame.c:3410-3413). If stable,
//      shows the dialog right then, wherever the player is (main menu or
//      in-game). If not, it's a no-op — the error stays pending.
//   2. Catch-all: Colorful-UI\Scripts\5_Mission\GUI\CUI\menus\MainMenu.c's
//      Init() flushes any still-pending error once the main menu is back up,
//      covering every return-to-menu path (including one that raced past
//      flush #1's stability window).
// Both funnel through the same MissionBase.CuiFlushPendingError() method,
// which clears CuiPendingError.s_Pending on entry — so if both flush points
// (or the error-test harness's own explicit flush call, ErrorTestScreen.c's
// CUI_ErrorTest_FlushPendingError) fire for the same pending error, every
// call after the first is a harmless no-op.
class CuiPendingError
{
    static bool   s_Pending;
    static string s_Caption;
    static string s_Message;
    static int    s_ErrorCode;

    // Dialogs blow up in height (CuiDialog.ResizeToBody, Dialogs.c:149-166,
    // has no upper bound) if fed an unbounded string — some kick reasons
    // (BattlEye, missing-mod lists) can be long. Clamp defensively here
    // rather than touching the shared CuiDialog component, which other
    // callers (exit confirm, item-drop warning, etc.) also depend on.
    static const int MAX_MESSAGE_LEN = 600;

    static void Set(string caption, string message, int errorCode)
    {
        if (message.Length() > MAX_MESSAGE_LEN)
            message = message.Substring(0, MAX_MESSAGE_LEN) + "...";

        s_Caption   = caption;
        s_Message   = message;
        s_ErrorCode = errorCode;
        s_Pending   = true;
    }

    static void Clear()
    {
        s_Pending   = false;
        s_Caption   = "";
        s_Message   = "";
        s_ErrorCode = 0;
    }
}

modded class DialogueErrorProperties
{
    override void HandleError(int errorCode, string additionalInfo = "")
    {
        ErrorCategory category = ErrorModuleHandler.GetCategoryFromError(errorCode);
        UIScriptedMenu handler = GetHandler();

        Print(string.Format(
            "[CUI ErrorDialog] HandleError errorCode=%1 hex=%2 category=%3 hasHandler=%4 additionalInfo='%5'",
            errorCode, ErrorModuleHandler.GetErrorHex(errorCode), category, handler != null, additionalInfo));

        // Generalized: any category/code with no attached UI handler goes
        // through the CUI dialog system. See file header for why handler
        // presence (not category) is the deciding signal.
        bool wouldUseCui = !handler;
        Print(string.Format("[CUI ErrorDialog] wouldUseCui=%1", wouldUseCui));

#ifdef NO_GUI
        Print("[CUI ErrorDialog] NO_GUI build - calling super.HandleError");
        super.HandleError(errorCode, additionalInfo);
        return;
#endif

#ifdef SERVER
        Print("[CUI ErrorDialog] SERVER build - calling super.HandleError");
        super.HandleError(errorCode, additionalInfo);
        return;
#else
        if (!wouldUseCui)
        {
            Print("[CUI ErrorDialog] wouldUseCui=false (has UI handler) - calling super.HandleError (native ShowDialog path)");
            super.HandleError(errorCode, additionalInfo);
            return;
        }

        string message;
        if (m_DisplayAdditionalInfo && additionalInfo != "")
            message = string.Format(EP_MESSAGE_FORMAT_STRING, m_Message, additionalInfo);
        else
            message = m_Message;

        string caption = string.Format(EP_HEADER_FORMAT_STRING, m_Header, ErrorModuleHandler.GetErrorHex(errorCode));

        // Translate "#"-prefixed stringtable keys now, same pattern vanilla
        // itself uses before putting this kind of text into a widget (e.g.
        // P:\scripts\5_mission\gui\itemdropwarningmenu.c:38, logoutmenu.c:151,
        // invitemenu.c:117 all call Widget.TranslateString() before display).
        // caption/message here are the same composite "#key\n#key2" shape
        // those call sites translate — the native ShowDialog we're bypassing
        // would otherwise have relied on the engine's own text pipeline to
        // resolve them.
        string translatedCaption = Widget.TranslateString(caption);
        string translatedMessage = Widget.TranslateString(message);

        Print(string.Format("[CUI ErrorDialog] wouldUseCui=true - storing pending CUI error. caption='%1' message='%2'", translatedCaption, translatedMessage));

        // Latest-error-wins: CuiPendingError.Set() has no accumulation, so a
        // second HandleError before the first is shown just replaces the
        // stored data. Only one dialog shows, and it's the latest one.
        CuiPendingError.Set(translatedCaption, translatedMessage, errorCode);

        // NO immediate flush. The old TryImmediateFlush() ran one GUI tick
        // after capture and its "stable" gate (mission present, not loading)
        // read TRUE during the post-kick teardown — so it consumed the
        // pending error into a dialog that could never render, leaving
        // nothing for MainMenu.Init() to show. That is why kicks stopped
        // producing a dialog on the main menu. The pending error now
        // survives until a display site that is provably safe flushes it:
        //   - MainMenu.Init()               (real kicks: back at the menu)
        //   - CUI_ErrorTest_FlushPendingError() (test screen: instant)

        // Deliberately NOT calling super.HandleError() here. This no-handler
        // case is fully owned by the CUI display path now — the native
        // dialog never fires for it.
#endif
    }

}
