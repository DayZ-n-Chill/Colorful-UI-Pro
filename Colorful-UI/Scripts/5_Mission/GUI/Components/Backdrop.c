// CuiBackdrop
// -----------------------------------------------------------------------------
// Reusable full-screen dim overlay. Used by CuiDialog and any other UI that
// needs a modal scrim.
//
//   m_Bd = new CuiBackdrop();         // uses layout-defined tint (default)
//   m_Bd.SetTint(ARGB(...));          // override tint+alpha at runtime
//   m_Bd.SetAlpha(0.5);               // fade animation: 0..1 -> 0..max alpha
//   delete m_Bd;                      // or just drop the ref
//
// Layout (`Colorful-UI/GUI/layouts/components/cuibackdrop.layout`):
//   - PanelWidget root (style `blank`, doesn't render its own color) at
//     priority 999 so anything drawn at priority >= 1000 (e.g. CuiDialog at
//     1000) renders on top.
//   - Inner ImageWidget child carries the visible color/alpha. The layout's
//     color alpha (e.g. 0.6) is captured at construction as the peak alpha
//     ceiling for SetAlpha() (i.e. SetAlpha(1.0) renders at that 0.6 max).
//
// Why we don't just SetAlpha on the root and rely on cascading: alpha cascade
// through a `style blank` parent doesn't fade the visible child reliably on
// this engine build (entrance worked, exit didn't). Targeting the ImageWidget
// directly is predictable. Why we also force its color to fully opaque and
// scale the alpha: ImageWidget.SetAlpha() OVERRIDES the color's alpha channel
// rather than multiplying with it, so without the rebase a SetAlpha(1.0)
// during steady-state would render at 100% opaque instead of the layout's
// intended 60%. See feedback memo on ARGB / layout-color alpha.

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
            // Capture the layout's intended peak alpha (e.g. 0.6) from the
            // packed ARGB color, then force the widget's color to fully
            // opaque so SetAlpha can act as a clean 0..max multiplier.
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

    // Caller passes 0..1 (normalized progress); we map to 0..m_MaxAlpha.
    void SetAlpha(float a)
    {
        if (m_Tint) m_Tint.SetAlpha(a * m_MaxAlpha);
        if (m_Root) m_Root.SetAlpha(a);
    }

    // Override the tint at runtime. The new ARGB's alpha byte BECOMES the new
    // peak, and the rendered color is rebased to opaque so SetAlpha keeps
    // working as a 0..1 progress mapping.
    void SetTint(int argb)
    {
        if (!m_Tint) return;
        int alphaByte = (argb >> 24) & 0xFF;
        m_MaxAlpha = alphaByte / 255.0;
        m_Tint.SetColor(argb | 0xFF000000);
    }
}
