class cuiElmnt
{
    static ref array<ref CUIButtonHandler> s_Handlers = new array<ref CUIButtonHandler>();

    static void proBtn(ButtonWidget button, string text, int textColor, int hoverColor,
                       string clickAction = "", Class targetClass = null, string callbackMethod = "",
                       string serverIP = "", int serverPort = 0)
    {
        if (!button) return;

        button.SetText(text);
        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(button.GetName() + "_label"));
        if (textWidget)
        {
            textWidget.SetText(text);
            button.SetText("");
        }

        ImageWidget imageWidget = ImageWidget.Cast(button.FindAnyWidget(button.GetName() + "_img"));

        CUIButtonHandler handler = new CUIButtonHandler(button, textWidget, imageWidget,
                                                        textColor, hoverColor, clickAction, targetClass,
                                                        callbackMethod, serverIP, serverPort);

        s_Handlers.Insert(handler);
    }

    static void proBtnDC(ButtonWidget button, string text, int textColor, int hoverColor, string serverIP, int serverPort)
    {
        proBtn(button, text, textColor, hoverColor, "", null, "", serverIP, serverPort);
    }

    static void proBtnURL(ButtonWidget button, string text, int textColor, int hoverColor, string clickAction)
    {
        proBtn(button, text, textColor, hoverColor, clickAction, null, "", "", 0);
    }

    static void proBtnCB(ButtonWidget button, string text, int textColor, int hoverColor, Class targetClass, string callbackMethod)
    {
        proBtn(button, text, textColor, hoverColor, "", targetClass, callbackMethod, "", 0);
    }
}
