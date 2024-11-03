// custmBtn.c

class CUIButton {
    static ref array<ref CUIButtonHandler> s_Handlers = new array<ref CUIButtonHandler>();

    static int ARGB(int alpha, int red, int green, int blue) {
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }

    // Existing baseBtn method
    static void baseBtn(ButtonWidget button, string text = "", int textColor = -1, int hoverTextColor = -1, int baseColor = -1, int hoverColor = -1) {
        if (!button) return;

        // Assign default values if parameters are not provided
        if (text == "") text = "Button";
        if (textColor == -1) textColor = ARGB(255, 255, 255, 255); // Default to white
        if (hoverTextColor == -1) hoverTextColor = ARGB(255, 200, 200, 200); // Default to light gray
        if (baseColor == -1) baseColor = ARGB(255, 50, 50, 50);   // Default to dark gray
        if (hoverColor == -1) hoverColor = ARGB(255, 70, 70, 70); // Default to slightly lighter gray

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

    // New textBtn method
    static void textBtn(ButtonWidget button, string text = "", int textColor = -1, int hoverTextColor = -1) {
        if (!button) return;

        // Assign default values specific to textBtn
        if (text == "") text = "Text Button";
        if (textColor == -1) textColor = ARGB(255, 0, 0, 0);       // Default to black text
        if (hoverTextColor == -1) hoverTextColor = ARGB(255, 100, 100, 100); // Default to gray on hover

        // Use transparent background for text buttons
        int baseColor = ARGB(0, 0, 0, 0);       // Fully transparent
        int hoverColor = ARGB(20, 0, 0, 0);     // Slightly visible on hover

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

        // No image widget for textBtn, but if needed, you can handle it similarly

        CUIButtonHandler handler = new CUIButtonHandler(button, textWidget, null, textColor, hoverTextColor, baseColor, hoverColor);

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
    private int m_BaseColor;
    private int m_HoverColor;
    private int m_TextColor;
    private int m_HoverTextColor;

    void CUIButtonHandler(ButtonWidget button, TextWidget textWidget, ImageWidget imageWidget, int textColor, int hoverTextColor, int baseColor, int hoverColor) {
        m_Button = button;
        m_TextWidget = textWidget;
        m_ImageWidget = imageWidget;
        m_TextColor = textColor;
        m_HoverTextColor = hoverTextColor;
        m_BaseColor = baseColor;
        m_HoverColor = hoverColor;

        // Set initial button properties
        ApplyBaseStyles();
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

    private void ApplyBaseStyles() {
        m_Button.SetColor(m_BaseColor);
        if (m_TextWidget) {
            m_TextWidget.SetColor(m_TextColor);
        } else {
            m_Button.SetTextColor(m_TextColor);
        }
        if (m_ImageWidget) {
            m_ImageWidget.SetColor(m_TextColor);
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
