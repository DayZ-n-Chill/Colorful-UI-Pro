// DialogueErrorProperties — captures engine error popups and stores them.
// Vanilla source: P:\scripts\3_game\global\errormodulehandler\errorproperties.c

class CuiPendingError
{
    static bool   s_Pending;
    static string s_Caption;
    static string s_Message;
    static int    s_ErrorCode;

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
            "[CUI ErrorRouter] HandleError errorCode=%1 hex=%2 category=%3 hasHandler=%4 additionalInfo='%5'",
            errorCode, ErrorModuleHandler.GetErrorHex(errorCode), category, handler != null, additionalInfo));

        bool wouldUseCui = !handler;
        Print(string.Format("[CUI ErrorRouter] wouldUseCui=%1", wouldUseCui));

#ifdef NO_GUI
        Print("[CUI ErrorRouter] NO_GUI build - calling super.HandleError");
        super.HandleError(errorCode, additionalInfo);
        return;
#endif

#ifdef SERVER
        Print("[CUI ErrorRouter] SERVER build - calling super.HandleError");
        super.HandleError(errorCode, additionalInfo);
        return;
#else
        if (!wouldUseCui)
        {
            Print("[CUI ErrorRouter] wouldUseCui=false (has UI handler) - calling super.HandleError (native ShowDialog path)");
            super.HandleError(errorCode, additionalInfo);
            return;
        }

        string message;
        if (m_DisplayAdditionalInfo && additionalInfo != "")
            message = string.Format(EP_MESSAGE_FORMAT_STRING, m_Message, additionalInfo);
        else
            message = m_Message;

        string caption = string.Format(EP_HEADER_FORMAT_STRING, m_Header, ErrorModuleHandler.GetErrorHex(errorCode));

        string translatedCaption = Widget.TranslateString(caption);
        string translatedMessage = Widget.TranslateString(message);

        Print(string.Format("[CUI ErrorRouter] wouldUseCui=true - storing pending CUI error. caption='%1' message='%2'", translatedCaption, translatedMessage));

        CuiPendingError.Set(translatedCaption, translatedMessage, errorCode);
#endif
    }
}
