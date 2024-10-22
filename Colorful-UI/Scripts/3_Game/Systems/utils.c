// These utility classes are used to help with various tasks in creating the UI.
class cuiUtils
{
    // A Better Random Index Utility Function (Actually randomizes)
    static int m_RandomIndex = -1; 
    static int GetRandomIndex(int count)
    {
        int newIndex = Math.RandomIntInclusive(0, count - 1);
        while (newIndex == m_RandomIndex)
        {
            newIndex = Math.RandomIntInclusive(0, count - 1);
        }
        m_RandomIndex = newIndex;
        return newIndex;
    }

    // Better Widget Utilities
    static void SetWidgetPosition(Widget widget, float x, float y) {
        if (widget) {
            widget.SetPos(x, y);
        }
    }

    static void SetWidgetSize(Widget widget, float width, float height) {
        if (widget) {
            widget.SetSize(width, height);
        }
    }
};