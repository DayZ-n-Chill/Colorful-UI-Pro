// CUIButton class
class CUIButton {
    static ref array<ref CUIButtonHandler> s_Handlers = new array<ref CUIButtonHandler>();

    static void baseBtn(ButtonWidget button, string text, int textColor, int hoverTextColor, int baseColor, int hoverColor) {
        if (!button) return;

        button.SetText(text);
        button.SetTextColor(textColor);
        button.SetColor(baseColor);

        string textWidgetName = button.GetName() + "_label";
        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(textWidgetName));

        if (textWidget) {
            textWidget.SetColor(textColor);
            textWidget.SetText(text);
            button.SetText("");
        }

        CUIButtonHandler handler = new CUIButtonHandler(button, textWidget, textColor, hoverTextColor, baseColor, hoverColor);

        WidgetEventHandler.GetInstance().RegisterOnMouseEnter(button, handler, "OnMouseEnter");
        WidgetEventHandler.GetInstance().RegisterOnMouseLeave(button, handler, "OnMouseLeave");
        WidgetEventHandler.GetInstance().RegisterOnClick(button, handler, "OnClick");

        s_Handlers.Insert(handler);
    }
}

// CUIButtonHandler class
class CUIButtonHandler {
    private ButtonWidget m_Button;
    private TextWidget m_TextWidget;
    private int m_BaseColor;
    private int m_HoverColor;
    private int m_TextColor;
    private int m_HoverTextColor;

    void CUIButtonHandler(ButtonWidget button, TextWidget textWidget, int textColor, int hoverTextColor, int baseColor, int hoverColor) {
        m_Button = button;
        m_TextWidget = textWidget;
        m_TextColor = textColor;
        m_HoverTextColor = hoverTextColor;
        m_BaseColor = baseColor;
        m_HoverColor = hoverColor;

        if (m_Button) {
            m_Button.SetColor(m_BaseColor);
            if (m_TextWidget) {
                m_TextWidget.SetColor(m_TextColor);
            } else {
                m_Button.SetTextColor(m_TextColor);
            }
        }
    }

    bool OnMouseEnter(Widget w, int x, int y) {
        if (w == m_Button) {
            m_Button.SetColor(m_HoverColor);
            if (m_TextWidget) {
                m_TextWidget.SetColor(m_HoverTextColor);
            } else {
                m_Button.SetTextColor(m_HoverTextColor);
            }
        }
        return true;
    }

    bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
        if (w == m_Button) {
            m_Button.SetColor(m_BaseColor);
            if (m_TextWidget) {
                m_TextWidget.SetColor(m_TextColor);
            } else {
                m_Button.SetTextColor(m_TextColor);
            }
        }
        return true;
    }

    bool OnClick(Widget w, int x, int y, int button) {
        if (w == m_Button && button == MouseState.LEFT) {
            return true;
        }
        return false;
    }
}
