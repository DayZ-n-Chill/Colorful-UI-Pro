// Routes DayZ's error popups (kicks, connect failures, BattlEye messages)
// into the Colorful-UI dialog instead of the plain engine dialog.
//
// Errors that carry their own interactive menu (Yes/No/Cancel prompts) are
// left alone — those still show the vanilla dialog, because their buttons
// drive game logic the info-only CUI dialog cannot reproduce.
//
// The error text is stored here and displayed by MainMenu, not shown on the
// spot: while the game is tearing down a session after a kick, a dialog
// created from script never renders and can crash the client.

class CuiPendingError
{
    static bool   s_Pending;
    static string s_Caption;
    static string s_Message;
    static int    s_ErrorCode;

    // Long kick reasons (BattlEye messages, missing-mod lists) would stretch
    // the dialog past the screen, since it sizes itself to its text.
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

        // Resolve "#"-prefixed stringtable keys, which the engine dialog we
        // are bypassing would have resolved itself.
        string translatedCaption = Widget.TranslateString(caption);
        string translatedMessage = Widget.TranslateString(message);

        Print(string.Format("[CUI ErrorDialog] wouldUseCui=true - storing pending CUI error. caption='%1' message='%2'", translatedCaption, translatedMessage));

        // Newest error wins if several arrive before one is shown.
        CuiPendingError.Set(translatedCaption, translatedMessage, errorCode);
#endif
    }
}
