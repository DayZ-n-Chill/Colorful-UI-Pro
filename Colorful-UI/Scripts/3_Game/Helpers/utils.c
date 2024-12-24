// These are utility functions that are used in the mod that you can use in your own U.
class cuiUtils
{
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

    static int ARGB(int alpha, int red, int green, int blue) {
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }
};
