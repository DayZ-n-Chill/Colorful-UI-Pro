// CuiBackdrop
// -----------------------------------------------------------------------------
// Reusable full-screen dim overlay. Used by CuiDialog and any other UI that
// needs a modal scrim.
//
//   m_Bd = new CuiBackdrop();          // uses layout-defined tint (default)
//   m_Bd.SetTint(ARGB(220, 12, 18, 40));  // override tint at runtime
//   m_Bd.SetAlpha(0.5);                // fade animation
//   delete m_Bd;                       // or just drop the ref
//
// Layout (`Colorful-UI/GUI/layouts/components/cuibackdrop.layout`):
//   - PanelWidget root (style `blank`, doesn't render its own color) sits at
//     priority 999 so anything drawn at priority >= 1000 (e.g. CuiDialog at
//     1000) renders on top.
//   - Inner ImageWidget child carries the visible color/alpha. The dim is
//     defined in the layout; SetTint() lets callers override it per-instance.
//
// Alpha cascading: SetAlpha() targets the root and propagates multiplicatively
// to the child (default inheritalpha=1), which is what drives the fade
// animation. The child's own color alpha (set in the layout, e.g. 0.6) is the
// final dim ceiling — root SetAlpha 1.0 = 60% visible, root SetAlpha 0 = gone.

class CuiBackdrop
{
    protected Widget      m_Root;
    protected ImageWidget m_Tint;

    void CuiBackdrop()
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/components/cuibackdrop.layout");
        if (!m_Root) return;

        m_Tint = ImageWidget.Cast(m_Root.FindAnyWidget("ImageWidget0"));
        m_Root.SetAlpha(0);   // start hidden — caller drives fade-in
    }

    void ~CuiBackdrop()
    {
        if (m_Root)
        {
            m_Root.Unlink();
            m_Root = null;
        }
    }

    Widget GetWidget()        { return m_Root; }
    void   SetAlpha(float a)  { if (m_Root) m_Root.SetAlpha(a); }
    void   SetTint(int argb)  { if (m_Tint) m_Tint.SetColor(argb); }
    bool   IsValid()          { return m_Root != null; }
}
