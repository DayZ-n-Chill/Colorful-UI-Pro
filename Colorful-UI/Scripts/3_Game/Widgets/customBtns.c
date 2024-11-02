
class CUIButton {
    // Converts ARGB values into a single integer representation
    static int ARGB(int alpha, int red, int green, int blue) {
        return (alpha << 24) | (red << 16) | (green << 8) | blue; // Combine ARGB into a single integer
    }

    // Sets up a button with text and hover background color
    static void baseBtn(ButtonWidget button, string text, int textColor, int hoverColor) {
        if (!button) return; 
        
        button.SetText(text);
        button.SetTextColor(textColor); 
        button.SetColor(hoverColor); 
        // This sets the hover just fine for all butons and you dont need a event handler to do the backgroung hovr effcect

        string textWidgetName = button.GetName() + "_label"; 
        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(textWidgetName));
        
        if (textWidget) {
            textWidget.SetColor(textColor);
            textWidget.SetText(text);
            button.SetText("");
        }
    }
}


