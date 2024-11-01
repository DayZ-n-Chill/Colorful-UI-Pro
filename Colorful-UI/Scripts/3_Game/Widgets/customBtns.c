enum LinkType {
    URL,
    // Other types can be added here as needed
};

enum ButtonType {
    DEFAULT,
    TEXT_WIDGET,
    // Other types can be added here as needed
};

class CustomButtonUtility {
    static int ARGB(int alpha, int red, int green, int blue) {
        return (alpha << 24) | (red << 16) | (green << 8) | blue; // Combine ARGB into a single integer
    }

    static void customBtnStyles(ButtonWidget button, ButtonType buttonType, string text, int textColor, int baseColor, string link = "", LinkType linkType = LinkType.URL) {
        if (!button) return; // Ensure the button exists

        // Automatically find the text widget based on the button's name
        string textWidgetName = button.GetName() + "_label"; // Assuming the text widget follows the naming convention
        TextWidget textWidget = TextWidget.Cast(button.FindAnyWidget(textWidgetName));

        // Logic based on button type
        if (buttonType == ButtonType.TEXT_WIDGET && textWidget) {
            textWidget.SetColor(textColor); // Set the text color for the text widget
            textWidget.SetText(text); // Set the text in the text widget
            button.SetText(""); // Clear the text from the ButtonWidget if it has a corresponding text widget
        } else {
            button.SetText(text); // Set the button text if no text widget is found or if it's a default button
        }

        button.SetColor(baseColor); // Set the button background color

        // Optional: Logic to handle URL linking can be added here
    }
}
