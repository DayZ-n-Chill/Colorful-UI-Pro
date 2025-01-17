/*************************************
 * FILE: buttons.c
 *************************************/

class CUIButtonHandler
{
    private ButtonWidget m_Button;
    private TextWidget   m_TextWidget;
    private ImageWidget  m_ImageWidget;

    private int    m_TextColor;
    private int    m_HoverColor;

    private string m_ClickAction;   // URL or string-based action
    private Class  m_TargetClass;   // For method callbacks
    private string m_CallbackMethod;

    void CUIButtonHandler(
        ButtonWidget button,
        TextWidget textWidget,
        ImageWidget imageWidget,
        int textColor,
        int hoverColor,
        string clickAction     = "",
        Class  targetClass     = null,
        string callbackMethod  = ""
    ) {
        m_Button         = button;
        m_TextWidget     = textWidget;
        m_ImageWidget    = imageWidget;
        m_TextColor      = textColor;
        m_HoverColor     = hoverColor;
        m_ClickAction    = clickAction;
        m_TargetClass    = targetClass;
        m_CallbackMethod = callbackMethod;

        ApplyBaseStyles();
    }

    private void ApplyBaseStyles()
    {
        if (!m_TextWidget && !m_ImageWidget)
        {
            // No child text or image: color the button text
            m_Button.SetTextColor(m_TextColor);
            return;
        }

        // If there's a TextWidget child, color that
        if (m_TextWidget)
        {
            m_TextWidget.SetColor(m_TextColor);
        }
        else
        {
            // Otherwise color the button text
            m_Button.SetTextColor(m_TextColor);
        }

        // If there's an ImageWidget child, set a hover color & image index
        if (m_ImageWidget)
        {
            m_ImageWidget.SetColor(m_HoverColor);
            m_ImageWidget.SetImage(1);
        }
    }

    private void ApplyHoverStyles()
    {
        if (!m_TextWidget && !m_ImageWidget)
        {
            // No child: color the whole button
            m_Button.SetColor(m_HoverColor);
            return;
        }

        // Otherwise color the text child
        if (m_TextWidget)
        {
            m_TextWidget.SetColor(m_HoverColor);
        }
        // And/or the image child
        if (m_ImageWidget)
        {
            m_ImageWidget.SetColor(m_HoverColor);
        }

        // Make the button background transparent if it has child text/image
        m_Button.SetColor(UIColor.Transparent());
    }

    bool OnMouseEnter(Widget w, int x, int y)
    {
        if (w == m_Button)
        {
            ApplyHoverStyles();
        }
        return true;
    }

    bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        if (w == m_Button)
        {
            ApplyBaseStyles();
        }
        return true;
    }

    bool OnFocus(Widget w, int x, int y)
    {
        if (w == m_Button)
        {
            ApplyHoverStyles();
        }
        return true;
    }

    bool OnFocusLost(Widget w, int x, int y)
    {
        if (w == m_Button)
        {
            ApplyBaseStyles();
        }
        return true;
    }

    bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_Button)
        {
            // 1) If we have a clickAction string, open it as a URL
            if (m_ClickAction != "")
            {
                GetGame().OpenURL(m_ClickAction);
            }
            // 2) Otherwise, if we have a target + callback, call that function
            else if (m_TargetClass && m_CallbackMethod != "")
            {
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

    static void proBtn(
        ButtonWidget button,
        string text,
        int textColor,
        int hoverColor,
        string clickAction    = "",
        Class  targetClass    = null,
        string callbackMethod = ""
    ) {
        if (!button) return;

        // Set text on the button (if no separate label)
        button.SetText(text);

        // If there's a child TextWidget named "<ButtonName>_label"
        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(button.GetName() + "_label"));
        if (textWidget)
        {
            textWidget.SetText(text);
            button.SetText("");
        }

        // If there's a child ImageWidget named "<ButtonName>_img"
        ImageWidget imageWidget = ImageWidget.Cast(button.FindAnyWidget(button.GetName() + "_img"));

        // Create the event handler
        CUIButtonHandler handler = new CUIButtonHandler(
            button, 
            textWidget, 
            imageWidget, 
            textColor, 
            hoverColor, 
            clickAction, 
            targetClass, 
            callbackMethod
        );

        // Register event callbacks
        WidgetEventHandler.GetInstance().RegisterOnMouseEnter(button,    handler, "OnMouseEnter");
        WidgetEventHandler.GetInstance().RegisterOnMouseLeave(button,    handler, "OnMouseLeave");
        WidgetEventHandler.GetInstance().RegisterOnFocus(button,         handler, "OnFocus");
        WidgetEventHandler.GetInstance().RegisterOnFocusLost(button,     handler, "OnFocusLost");
        WidgetEventHandler.GetInstance().RegisterOnClick(button,         handler, "OnClick");

        // Keep a reference so it won’t be garbage-collected
        s_Handlers.Insert(handler);
    }
}

