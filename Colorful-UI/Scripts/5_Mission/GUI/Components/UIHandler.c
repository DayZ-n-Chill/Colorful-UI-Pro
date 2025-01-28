class cuiElmnt
{
    static ref array<ref CUIButtonHandler> s_Handlers = new array<ref CUIButtonHandler>();

    // Pro Buttons ---------------------------------------------------------------
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


    // Pro Dialogs ---------------------------------------------------------------
    // static CUIDialogMenu proDialog(string title, string body,
    //                             string okText, int okTextColor, int okHoverColor,
    //                             Class okTargetClass, string okCallbackMethod,
    //                             string cancelText, int cancelTextColor, int cancelHoverColor,
    //                             Class cancelTargetClass, string cancelCallbackMethod)
    // {
    //     // same steps: create CUIDialogMenu, set title/body
    //     CUIDialogMenu dialogMenu = CUIDialogMenu.Cast(GetGame().GetUIManager().CreateScriptedMenu(MENU_NONE, null));
    //     if (!dialogMenu) return null;

    //     dialogMenu.SetTitle(title);
    //     dialogMenu.SetBody(body);

    //     // OK button
    //     proBtnCB(
    //         dialogMenu.GetButtonOK(),
    //         okText,
    //         okTextColor,
    //         okHoverColor,
    //         okTargetClass,
    //         okCallbackMethod
    //     );

    //     // Cancel button
    //     proBtnCB(
    //         dialogMenu.GetButtonCancel(),
    //         cancelText,
    //         cancelTextColor,
    //         cancelHoverColor,
    //         cancelTargetClass,
    //         cancelCallbackMethod
    //     );

    //     GetGame().GetUIManager().ShowScriptedMenu(dialogMenu, null);
    //     return dialogMenu;
    // }


}

