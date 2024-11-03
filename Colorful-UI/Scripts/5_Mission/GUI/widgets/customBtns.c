class cuiElmnt {
    static ref array<ref CUIButtonHandler> s_Handlers = new array<ref CUIButtonHandler>();

    static int ARGB(int alpha, int red, int green, int blue) {
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }

    static void Btn(ButtonWidget button, string text = "", int textColor = -1, int hoverTextColor = -1, int baseColor = -1, int hoverColor = -1) {
        if (!button) return;

        if (text == "") text = "Button";
        if (textColor == -1) textColor = colorScheme.PrimaryText(); 
        if (hoverTextColor == -1) hoverTextColor = colorScheme.TextHover(); 
        if (baseColor == -1) baseColor = UIColor.Grey();     
        if (hoverColor == -1) hoverColor = colorScheme.ButtonHover(); 

        button.SetText(text);
        button.SetTextColor(textColor);
        button.SetColor(baseColor);

        string textWidgetName = button.GetName() + "_label";
        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(textWidgetName));

        string imageWidgetName = button.GetName() + "_img";
        ImageWidget imageWidget = ImageWidget.Cast(button.FindAnyWidget(imageWidgetName));

        if (textWidget) {
            textWidget.SetColor(textColor);
            textWidget.SetText(text);
            button.SetText(""); 
        }

        if (imageWidget) {
            imageWidget.SetColor(textColor);
        }

        CUIButtonHandler handler = new CUIButtonHandler(button, textWidget, imageWidget, textColor, hoverTextColor, baseColor, hoverColor);

        WidgetEventHandler.GetInstance().RegisterOnMouseEnter(button, handler, "OnMouseEnter");
        WidgetEventHandler.GetInstance().RegisterOnMouseLeave(button, handler, "OnMouseLeave");
        WidgetEventHandler.GetInstance().RegisterOnClick(button, handler, "OnClick");
        WidgetEventHandler.GetInstance().RegisterOnFocus(button, handler, "OnFocus");
        WidgetEventHandler.GetInstance().RegisterOnFocusLost(button, handler, "OnFocusLost");

        s_Handlers.Insert(handler);
    }
}

class CUIButtonHandler {
    private ButtonWidget m_Button;
    private TextWidget m_TextWidget;
    private ImageWidget m_ImageWidget;
    private int m_baseColor;
    private int m_HoverColor;
    private int m_TextColor;
    private int m_HoverTextColor;

    void CUIButtonHandler(ButtonWidget button, TextWidget textWidget, ImageWidget imageWidget, int textColor, int hoverTextColor, int baseColor, int hoverColor) {
        m_Button = button;
        m_TextWidget = textWidget;
        m_ImageWidget = imageWidget;
        m_TextColor = textColor;
        m_HoverTextColor = hoverTextColor;
        m_baseColor = baseColor;
        m_HoverColor = hoverColor;

        ApplyBaseStyles();
    }

    private void ApplyBaseStyles() {
        m_Button.SetColor(m_baseColor);
        if (m_TextWidget) {
            m_TextWidget.SetColor(m_TextColor);
        } else {
            m_Button.SetTextColor(m_TextColor);
        }
        if (m_ImageWidget) {
            m_ImageWidget.SetColor(m_TextColor);
        }
    }

    private void ApplyHoverStyles() {
        m_Button.SetColor(m_HoverColor);
        if (m_TextWidget) {
            m_TextWidget.SetColor(m_HoverTextColor);
        } else {
            m_Button.SetTextColor(m_HoverTextColor);
        }
        if (m_ImageWidget) {
            m_ImageWidget.SetColor(m_HoverTextColor);
        }
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

    bool OnClick(Widget w, int x, int y, int button) {
        if (w == m_Button && button == MouseState.LEFT) {
            return true;
        }
        return false;
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
}
