// DialogueErrorProperties override — capture only, no 3_Game widgets
// -----------------------------------------------------------------------------
// Vanilla DialogueErrorProperties.HandleError (errorproperties.c:55-74) is the
// generic handler behind every dialog-shown error in the game, rendering via
// g_Game.GetUIManager().ShowDialog(...), a `proto native` call with no
// .layout file behind it.
//
// History of this file, briefly: an earlier version created a custom
// inline-chrome widget panel directly from HandleError (deferred one
// CallLater tick), fully suppressing the native ShowDialog call for kicks.
// It worked functionally (diagnostics confirmed the panel rendered) but a
// native access-violation crash followed shortly after, during a rapid
// double-kick (duplicate-UID) test. Research into the crash found:
//   - DisconnectSessionEx explicitly closes the NATIVE dialog when tearing
//     a session down (dayzgame.c:2721-2729: CloseAllSubmenus() + CloseDialog()
//     if IsDialogVisible()) — a script-owned widget tree is invisible to that
//     cleanup and has no equivalent teardown path.
//   - Vanilla's dialog system has its own built-in queue/safety mechanism
//     (UIManager.IsDialogQueued()/ShowQueuedDialog(), uimanager.c:49-50) that
//     the engine drives via a native DialogQueuedEventTypeID event
//     (dayzgame.c:1573-1577) into DayZGame.CheckDialogs()
//     (dayzgame.c:3386-3396): only actually shows a queued dialog once
//     `!m_loading.IsLoading()`. Fully suppressing the native ShowDialog call
//     meant we never got any of that engine-side "is it actually safe right
//     now" gating — we were creating widgets on our own guess (one deferred
//     tick) instead of on the engine's own signal.
//   - The AV's actual crash stack was in the native session/connection layer
//     (CDPCreateServer), not conclusively pinned to widget creation itself —
//     but suppressing the native dialog path removes engine-level coupling
//     between dialog state and connection state that we don't have full
//     visibility into from script source alone. Given that, the design
//     changed: this file no longer creates ANY widgets and calls nothing
//     UI-related at HandleError() time. It only captures data.
//
// Current design: for the case this is scoped to (ErrorCategory.ClientKicked
// with no UI handler — see below for why that scoping is safe), the resolved
// caption/message are translated and stored in the CuiPendingError static
// holder, and HandleError() returns WITHOUT calling super.HandleError() —
// the native dialog never fires for these. Display happens later, from
// 5_Mission, once the main menu is reliably back up after the kick (see
// Colorful-UI\Scripts\5_Mission\GUI\CUI\menus\MainMenu.c, CheckPendingCuiError,
// hooked at the end of Init()). That is provably a safe, stable point: it's
// the same call path vanilla itself already uses every time the player
// returns to the main menu after ANY disconnect/kick (confirmed via repeated
// "Creating Mission: ...intro.ChernarusPlus\mission.c" log lines across the
// rapid-reconnect test session), so widget creation there carries no more
// risk than the main menu's own widget construction already does on every
// single kick, successfully, today.
//
// Scoping (unchanged from before): every kick/BattlEye/DB/auth error in
// ClientKickedModule.c (P:\scripts\3_game\global\errormodulehandler\
// clientkickedmodule.c) is inserted with m_UIHandler left at its default of
// null (only ConnectErrorScriptModule ever assigns a real one,
// connecterrorscriptmodule.c:23) — so intercepting only ClientKicked-with-
// no-handler never swallows a callback contract we'd need to replicate.
// Live diagnostic data (client script_2026-08-05_23-10-58.log) confirmed a
// real kick (duplicate-UID) takes exactly this path: category matched
// ErrorCategory.ClientKicked and hasHandler was false. Everything outside
// that — Generic, ConnectErrorClient/Server/Script, BIOSError, or any
// ClientKicked error that somehow does carry a handler — still calls
// super.HandleError() untouched, i.e. the native dialog, exactly as before
// this mod ever touched this class.
class CuiPendingError
{
    static bool   s_Pending;
    static string s_Caption;
    static string s_Message;
    static int    s_ErrorCode;

    static void Set(string caption, string message, int errorCode)
    {
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

        bool wouldUseCui = (category == ErrorCategory.ClientKicked) && !handler;
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
            Print("[CUI ErrorDialog] wouldUseCui=false - calling super.HandleError (native ShowDialog path)");
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

        // Double-kicks (two HandleError calls back to back, observed live)
        // simply overwrite: CuiPendingError.Set() has no accumulation, so
        // the second call's data replaces the first's. Only one dialog shows
        // at the menu, and it's the latest one.
        CuiPendingError.Set(translatedCaption, translatedMessage, errorCode);

        // Deliberately NOT calling super.HandleError() here. This category/
        // no-handler case is fully owned by the CUI display path in
        // MainMenu.c now — the native dialog never fires for it.
#endif
    }
}
