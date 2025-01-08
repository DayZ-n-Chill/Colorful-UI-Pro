class cuiUtils
{
    static int m_RandomIndex = -1;

    // Returns a random index while avoiding consecutive duplicates
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

    // Combines ARGB color values into a single integer
    static int ARGB(int alpha, int red, int green, int blue) {
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }
};
