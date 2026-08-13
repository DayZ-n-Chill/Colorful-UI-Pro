// CuiBackdrop — full-screen dim overlay behind dialogs.

class CuiBackdrop
{
    protected Widget      m_Root;
    protected ImageWidget m_Tint;
    protected float       m_MaxAlpha;

    void CuiBackdrop()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/components/cuibackdrop.layout");
        if (!m_Root) return;

        m_Tint = ImageWidget.Cast(m_Root.FindAnyWidget("ImageWidget0"));

        if (m_Tint)
        {
            int layoutColor = m_Tint.GetColor();
            int alphaByte   = (layoutColor >> 24) & 0xFF;
            m_MaxAlpha      = alphaByte / 255.0;

            m_Tint.SetColor(layoutColor | 0xFF000000);
            m_Tint.SetAlpha(0);
        }

        m_Root.SetAlpha(0);
    }

    void ~CuiBackdrop()
    {
        if (m_Root)
        {
            m_Root.Unlink();
            m_Root = null;
        }
    }

    Widget GetWidget()  { return m_Root; }
    bool   IsValid()    { return m_Root != null; }

    void SetAlpha(float a)
    {
        if (m_Tint) m_Tint.SetAlpha(a * m_MaxAlpha);
        if (m_Root) m_Root.SetAlpha(a);
    }

    void SetTint(int argb)
    {
        if (!m_Tint) return;
        int alphaByte = (argb >> 24) & 0xFF;
        m_MaxAlpha = alphaByte / 255.0;
        m_Tint.SetColor(argb | 0xFF000000);
    }
}
