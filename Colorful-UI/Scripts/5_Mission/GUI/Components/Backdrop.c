// CuiBackdrop — full-screen dim overlay behind dialogs. Blocks clicks on the
// UI underneath, so a dialog using it is properly modal.
//
//   m_Bd = new CuiBackdrop();     // dim colour comes from the layout
//   m_Bd.SetTint(ARGB(...));      // override colour + peak opacity
//   m_Bd.SetAlpha(0.5);           // 0..1 fade, scaled to that peak
//   delete m_Bd;                  // or just drop the reference
//
// Layout: Colorful-UI/GUI/layouts/components/cuibackdrop.layout. It sits at
// priority 999 so dialogs (priority 1000) draw on top of it.

class CuiBackdrop
{
    protected Widget      m_Root;
    protected ImageWidget m_Tint;
    protected float       m_MaxAlpha;   // captured from layout color alpha

    void CuiBackdrop()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/components/cuibackdrop.layout");
        if (!m_Root) return;

        m_Tint = ImageWidget.Cast(m_Root.FindAnyWidget("ImageWidget0"));

        if (m_Tint)
        {
            // Take the layout's opacity as the maximum, then make the colour
            // fully opaque so SetAlpha() can fade cleanly from 0 to it.
            int layoutColor = m_Tint.GetColor();
            int alphaByte   = (layoutColor >> 24) & 0xFF;
            m_MaxAlpha      = alphaByte / 255.0;

            // Bitwise OR with 0xFF000000 to opaque the alpha byte while
            // preserving RGB.
            m_Tint.SetColor(layoutColor | 0xFF000000);
            m_Tint.SetAlpha(0);   // start hidden — caller drives fade-in
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

    // Takes 0..1 and maps it onto the backdrop's maximum opacity.
    void SetAlpha(float a)
    {
        if (m_Tint) m_Tint.SetAlpha(a * m_MaxAlpha);
        if (m_Root) m_Root.SetAlpha(a);
    }

    // Change the dim colour. The alpha you pass becomes the new maximum
    // opacity that SetAlpha() fades towards.
    void SetTint(int argb)
    {
        if (!m_Tint) return;
        int alphaByte = (argb >> 24) & 0xFF;
        m_MaxAlpha = alphaByte / 255.0;
        m_Tint.SetColor(argb | 0xFF000000);
    }
}
