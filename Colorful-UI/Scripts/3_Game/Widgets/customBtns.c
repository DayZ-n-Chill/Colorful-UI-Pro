class CUIButton {
    static int ARGB(int alpha, int red, int green, int blue) {
        return (alpha << 24) | (red << 16) | (green << 8) | blue; // Combine ARGB into a single integer
    }

    static void defaultBtn(ButtonWidget button, string text, int textColor, int baseColor) {
        if (!button) return; // Ensure the button exists

        // Set the button text
        button.SetText(text);
        button.SetColor(baseColor); // Set the button background color

        // Assuming the text widget follows the naming convention
        string textWidgetName = button.GetName() + "_label"; 
        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(textWidgetName));
        
        // Set text color for text widget if it exists
        if (textWidget) {
            textWidget.SetColor(textColor);
            textWidget.SetText(text); // Set the text in the text widget
            button.SetText(""); // Clear the text from the ButtonWidget if it has a corresponding text widget
        }
    }

    static void textBtn(ButtonWidget button, string text, int textColor, int baseColor) {
        defaultBtn(button, text, textColor, baseColor); // Reuse defaultBtn logic
    }

    static void iconBtn(ButtonWidget button, string text, int textColor, int baseColor) {
        defaultBtn(button, text, textColor, baseColor); // Reuse defaultBtn logic
        // Additional logic for iconBtn can be added here
    }
}
