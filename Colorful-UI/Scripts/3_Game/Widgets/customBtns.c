class CUIButton {
    static int ARGB(int alpha, int red, int green, int blue) {
        return (alpha << 24) | (red << 16) | (green << 8) | blue; 
    }

    static void baseBtn(ButtonWidget button, string text, int textColor, int hoverColor) {
        if (!button) return; 
        
        button.SetText(text);
        button.SetTextColor(textColor); 
        button.SetColor(hoverColor); 

        string textWidgetName = button.GetName() + "_label"; 
        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(textWidgetName));
        
        if (textWidget) {
            textWidget.SetColor(textColor);
            textWidget.SetText(text);
            button.SetText("");
        }

        // Attach the handler for hover and click interactions
        CUIButtonHandler handler = new CUIButtonHandler(button, textColor, hoverColor, hoverColor);
        button.SetHandler(handler);
    }
}

class CUIButtonHandler : ScriptedWidgetEventHandler {
    private ButtonWidget m_Button;
    private int m_BaseColor;
    private int m_HoverColor;
    private int m_TextColor;

    void CUIButtonHandler(ButtonWidget button, int textColor, int baseColor, int hoverColor) {
        m_Button = button;
        m_TextColor = textColor;
        m_BaseColor = baseColor;
        m_HoverColor = hoverColor;

        // Set initial button properties
        if (m_Button) {
            m_Button.SetTextColor(m_TextColor);
            m_Button.SetColor(m_BaseColor);
        }
    }

    override bool OnMouseEnter(Widget w, int x, int y) {
        if (w == m_Button) {
            m_Button.SetColor(m_HoverColor);
        }
        return true;
    }

    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
        if (w == m_Button) {
            m_Button.SetColor(m_BaseColor);
        }
        return true;
    }

    override bool OnClick(Widget w, int x, int y, int button) {
        if (w == m_Button && button == MouseState.LEFT) {
            return true;
        }
        return false;
    }
}
