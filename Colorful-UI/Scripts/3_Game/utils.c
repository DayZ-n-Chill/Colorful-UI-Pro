class cuiUtils
{
    static bool ForceMainMenu = true;  
    
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
};