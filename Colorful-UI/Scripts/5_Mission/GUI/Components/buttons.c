class CUIButtonHandler
{
    private ButtonWidget m_Button;
    private TextWidget m_TextWidget;
    private ImageWidget m_ImageWidget;

    private int m_TextColor;
    private int m_HoverColor;

    private string m_ClickAction; // To store the action or URL
    private Class m_TargetClass;  // For method callbacks
    private string m_CallbackMethod;

    void CUIButtonHandler(
        ButtonWidget button,
        TextWidget textWidget,
        ImageWidget imageWidget,
        int textColor,
        int hoverColor,
        string clickAction = "",
        Class targetClass = null,
        string callbackMethod = "",
    ) {
        m_Button = button;
        m_TextWidget = textWidget;
        m_ImageWidget = imageWidget;
        m_TextColor = textColor;
        m_HoverColor = hoverColor;
        m_ClickAction = clickAction;
        m_TargetClass = targetClass;
        m_CallbackMethod = callbackMethod;

        ApplyBaseStyles();
    }

    private void ApplyBaseStyles() {
        if (!m_TextWidget && !m_ImageWidget) {
            m_Button.SetTextColor(m_TextColor);
            return;
        }

        if (m_TextWidget) {
            m_TextWidget.SetColor(m_TextColor);
        } else {
            m_Button.SetTextColor(m_TextColor);
        }

        if (m_ImageWidget) {
            m_ImageWidget.SetColor(m_HoverColor);
            m_ImageWidget.SetImage(1);
        }
    }

    private void ApplyHoverStyles() {
        if (!m_TextWidget && !m_ImageWidget) {
            m_Button.SetColor(m_HoverColor);
            return;
        }

        if (m_TextWidget) {
            m_TextWidget.SetColor(m_HoverColor);
        }

        if (m_ImageWidget) {
            m_ImageWidget.SetColor(m_HoverColor);
        }

        m_Button.SetColor(UIColor.Transparent());
    }

    bool OnMouseEnter(Widget w, int x, int y) {
        if (w == m_Button) {
            ApplyHoverStyles();
        }
        return true;
    }

    bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
        if (w == m_Button) {
            ApplyBaseStyles();
        }
        return true;
    }

    bool OnFocus(Widget w, int x, int y) {
        if (w == m_Button) {
            ApplyHoverStyles();
        }
        return true;
    }

    bool OnFocusLost(Widget w, int x, int y) {
        if (w == m_Button) {
            ApplyBaseStyles();
        }
        return true;
    }

    bool OnClick(Widget w, int x, int y, int button) {
        if (w == m_Button) {
            if (m_ClickAction != "") {
                // Open URL or handle string-based action
                GetGame().OpenURL(m_ClickAction);
            } else if (m_TargetClass && m_CallbackMethod != "") {
                // Trigger callback function
                GetGame().GameScript.CallFunction(m_TargetClass, m_CallbackMethod, null, 0);
            }
            return true;
        }
        return false;
    }
}

class cuiElmnt
{
    static ref array<ref CUIButtonHandler> s_Handlers = new array<ref CUIButtonHandler>();

    static void proBtn(ButtonWidget button, string text, int textColor, int hoverColor, string clickAction = "", Class targetClass = null, string callbackMethod = "") {
        if (!button) return;

        button.SetText(text);

        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(button.GetName() + "_label"));
        if (textWidget) {
            textWidget.SetText(text);
            button.SetText("");
        }

        ImageWidget imageWidget = ImageWidget.Cast(button.FindAnyWidget(button.GetName() + "_img"));

        CUIButtonHandler handler = new CUIButtonHandler(
            button, textWidget, imageWidget, textColor, hoverColor, clickAction, targetClass, callbackMethod,
        );

        WidgetEventHandler.GetInstance().RegisterOnMouseEnter(button, handler, "OnMouseEnter");
        WidgetEventHandler.GetInstance().RegisterOnMouseLeave(button, handler, "OnMouseLeave");
        WidgetEventHandler.GetInstance().RegisterOnFocus(button, handler, "OnFocus");
        WidgetEventHandler.GetInstance().RegisterOnFocusLost(button, handler, "OnFocusLost");
        WidgetEventHandler.GetInstance().RegisterOnClick(button, handler, "OnClick");

        s_Handlers.Insert(handler);
    }
}
