// CuiDialog — the standard Colorful-UI popup. Handles its own widgets and
// lifetime; nothing else needs to create or destroy it.
//
//   CuiDialog.Show("Title", "Body text");
//   CuiDialog.Show("Title", "Body text", false);       // no dim behind it
//   CuiDialog.Show("Exit?", "Are you sure?", true, this, "DoExit", "");
//
// The last two arguments name methods on the object you pass in, called when
// the player picks Confirm or Cancel. Pass "" to ignore a button.
//
// The dialog animates in and out one element at a time — box, title,
// separator, body, then buttons — rising on entry and dropping on exit.
// Adjust the timings in ApplyEntrance() and ApplyExit() below.

class CuiDialog
{
    static ref array<ref CuiDialog> s_OpenDialogs = new array<ref CuiDialog>();

    static const float ANIM_IN_TOTAL_MS  = 360.0;
    static const float ANIM_OUT_TOTAL_MS = 600.0;   // backdrop needs a long tail after dialog elements leave
    static const int   ANIM_TICK_MS      = 16;

    // Fixed height above and below the body text (title, separator, buttons
    // and padding). The body's own height is added between them.
    static const float DLG_TOP_REGION    = 80.0;
    static const float DLG_BOTTOM_REGION = 88.0;
    static const int   BODY_MIN_H        = 32;   // floor so single-line bodies aren't squashed

    protected Widget         m_Root;
    protected ref CuiBackdrop m_Backdrop;
    protected Widget         m_DialogBox;
    protected Widget         m_Separator;
    protected ButtonWidget   m_Confirm;
    protected ButtonWidget   m_Cancel;
    protected TextWidget     m_Caption;
    protected RichTextWidget m_Body;
    protected bool           m_Closing;

    // Object and method names to call when a button is pressed.
    protected Class          m_CallbackTarget;
    protected string         m_OnConfirmMethod;
    protected string         m_OnCancelMethod;

    // Where the layout puts each element, so animations can offset from it.
    protected float m_DlgBaseY;
    protected float m_CapBaseY;
    protected float m_BodyBaseY;
    protected float m_CancelBaseY;
    protected float m_ConfirmBaseY;
    protected float m_SepBaseW;
    protected float m_SepBaseH;

    // +1 entering, -1 exiting, 0 idle.
    protected int   m_AnimDir;
    protected float m_Elapsed;       // ms since current animation started

    void CuiDialog(string title, string body, bool useBackdrop = true, Class callbackTarget = null, string onConfirm = "", string onCancel = "")
    {
        m_CallbackTarget  = callbackTarget;
        m_OnConfirmMethod = onConfirm;
        m_OnCancelMethod  = onCancel;

        // Created first so the dim sits behind the dialog.
        if (useBackdrop) m_Backdrop = new CuiBackdrop();

        m_Root = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.dialogs.layout");
        if (!m_Root) return;

        m_DialogBox = m_Root.FindAnyWidget("DialogBox");
        // Layouts cannot express a see-through panel colour, so set it here.
        if (m_DialogBox) m_DialogBox.SetColor(ARGB(220, 62, 62, 62));
        m_Separator = m_Root.FindAnyWidget("SeparatorPanel");
        if (m_Separator) m_Separator.SetColor(colorScheme.Separator());
        m_Caption   = TextWidget.Cast(m_Root.FindAnyWidget("Caption"));
        m_Body      = RichTextWidget.Cast(m_Root.FindAnyWidget("Body"));
        m_Confirm   = ButtonWidget.Cast(m_Root.FindAnyWidget("Confirm"));
        m_Cancel    = ButtonWidget.Cast(m_Root.FindAnyWidget("Cancel"));

        if (m_Caption) m_Caption.SetText(title);
        if (m_Body)    m_Body.SetText(body);

        // Grow the box to fit however much text was passed in.
        ResizeToBody();

        cuiElmnt.proBtnCB(this, m_Confirm, "Confirm", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OnConfirm");
        cuiElmnt.proBtnCB(this, m_Cancel,  "Cancel",  colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OnCancel");

        // Hide the layout's placeholder button colour before it can flash.
        if (m_Confirm) m_Confirm.SetColor(UIColor.Transparent());
        if (m_Cancel)  m_Cancel.SetColor(UIColor.Transparent());

        // Remember each element's resting position for the animations.
        float xtmp;
        if (m_DialogBox) m_DialogBox.GetPos(xtmp, m_DlgBaseY);
        if (m_Caption)   m_Caption.GetPos(xtmp, m_CapBaseY);
        if (m_Body)      m_Body.GetPos(xtmp, m_BodyBaseY);
        if (m_Cancel)    m_Cancel.GetPos(xtmp, m_CancelBaseY);
        if (m_Confirm)   m_Confirm.GetPos(xtmp, m_ConfirmBaseY);
        if (m_Separator) m_Separator.GetSize(m_SepBaseW, m_SepBaseH);

        // Put everything in its start state, then run the entrance.
        ApplyEntrance(0);
        StartAnim(1);
    }

    protected void ResizeToBody()
    {
        if (!m_Body || !m_DialogBox) return;

        // Height of the wrapped text, available immediately after SetText.
        float contentH = m_Body.GetContentHeight();
        if (contentH < BODY_MIN_H) contentH = BODY_MIN_H;

        float dlgW;
        float dlgH;
        m_DialogBox.GetSize(dlgW, dlgH);
        m_DialogBox.SetSize(dlgW, DLG_TOP_REGION + contentH + DLG_BOTTOM_REGION);
    }

    static CuiDialog Show(string title, string body, bool useBackdrop = true, Class callbackTarget = null, string onConfirm = "", string onCancel = "")
    {
        CuiDialog dlg = new CuiDialog(title, body, useBackdrop, callbackTarget, onConfirm, onCancel);
        // If the layout failed to load there are no buttons to close it.
        if (!dlg.m_Root) return null;
        if (!s_OpenDialogs) return null;
        s_OpenDialogs.Insert(dlg);
        return dlg;
    }

    // Cancels the top-most open dialog and reports whether there was one.
    // Menus call this from Escape so it dismisses instead of opening another.
    static bool CancelTop()
    {
        if (!s_OpenDialogs) return false;
        int n = s_OpenDialogs.Count();
        if (n == 0) return false;
        CuiDialog top = s_OpenDialogs.Get(n - 1);
        if (top) top.OnCancel();
        return true;
    }

    void OnConfirm()
    {
        // Ignore extra clicks while the dialog is closing.
        if (m_Closing) return;

        // Call back before closing, so the handler may open another dialog.
        if (m_CallbackTarget && m_OnConfirmMethod != "")
        {
            GetGame().GameScript.CallFunction(m_CallbackTarget, m_OnConfirmMethod, null, 0);
        }
        Close();
    }

    void OnCancel()
    {
        if (m_Closing) return;

        if (m_CallbackTarget && m_OnCancelMethod != "")
        {
            GetGame().GameScript.CallFunction(m_CallbackTarget, m_OnCancelMethod, null, 0);
        }
        Close();
    }

    // Safe to call more than once; only the first close runs.
    void Close()
    {
        if (m_Closing) return;
        m_Closing = true;
        StartAnim(-1);
    }

    // ----- Animation driver -----

    protected void StartAnim(int dir)
    {
        m_AnimDir = dir;
        m_Elapsed = 0;
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.AnimTick);
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.AnimTick, ANIM_TICK_MS, false);
    }

    void AnimTick()
    {
        if (m_AnimDir == 0 || !m_Root) return;

        m_Elapsed += ANIM_TICK_MS;

        float total;
        if (m_AnimDir > 0) total = ANIM_IN_TOTAL_MS;
        else               total = ANIM_OUT_TOTAL_MS;
        if (m_Elapsed > total) m_Elapsed = total;

        if (m_AnimDir > 0) ApplyEntrance(m_Elapsed);
        else               ApplyExit(m_Elapsed);

        if (m_Elapsed >= total)
        {
            int finishedDir = m_AnimDir;
            m_AnimDir = 0;
            if (finishedDir < 0) DoClose();
            return;
        }

        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.AnimTick, ANIM_TICK_MS, false);
    }

    // ----- Easing + interpolation -----

    protected static float Track(float elapsed, float delay, float duration)
    {
        if (duration <= 0) return 1.0;
        float t = (elapsed - delay) / duration;
        if (t < 0) t = 0;
        if (t > 1) t = 1;
        return t;
    }

    protected static float EaseOutCubic(float t)
    {
        float inv = 1.0 - t;
        return 1.0 - inv * inv * inv;
    }

    protected static float EaseInCubic(float t)
    {
        return t * t * t;
    }

    protected static float EaseInOutCubic(float t)
    {
        if (t < 0.5) return 4.0 * t * t * t;
        float k = -2.0 * t + 2.0;
        return 1.0 - (k * k * k) / 2.0;
    }

    protected static float Lerp(float a, float b, float t) { return a + (b - a) * t; }

    // ----- Per-element setters (null-safe) -----

    protected static void SetA(Widget w, float a)
    {
        if (w) w.SetAlpha(a);
    }

    protected static void SetY(Widget w, float baseY, float dy)
    {
        if (!w) return;
        float x;
        float oldY;
        w.GetPos(x, oldY);
        w.SetPos(x, baseY + dy);
    }

    protected void SetSepW(float width)
    {
        if (m_Separator) m_Separator.SetSize(width, m_SepBaseH);
    }

    // ----- Choreography -----

    protected void ApplyEntrance(float elapsed)
    {
        // The backdrop fades itself; we only set how far along it is.
        float p = EaseOutCubic(Track(elapsed, 0, 200));
        if (m_Backdrop) m_Backdrop.SetAlpha(p);

        // Dialog box rises and fades in.
        p = EaseOutCubic(Track(elapsed, 0, 220));
        SetA(m_DialogBox, p);
        SetY(m_DialogBox, m_DlgBaseY, Lerp(27, 0, p));

        // Title follows.
        p = EaseOutCubic(Track(elapsed, 80, 160));
        SetA(m_Caption, p);
        SetY(m_Caption, m_CapBaseY, Lerp(9, 0, p));

        // Separator draws outwards from the centre.
        p = EaseInOutCubic(Track(elapsed, 110, 170));
        SetA(m_Separator, p);
        SetSepW(Lerp(0, m_SepBaseW, p));

        // Body text.
        p = EaseOutCubic(Track(elapsed, 140, 160));
        SetA(m_Body, p);
        SetY(m_Body, m_BodyBaseY, Lerp(9, 0, p));

        // Buttons rise into place last.
        p = EaseOutCubic(Track(elapsed, 180, 180));
        SetA(m_Cancel, p);
        SetA(m_Confirm, p);
        SetY(m_Cancel,  m_CancelBaseY,  Lerp(-15, 0, p));
        SetY(m_Confirm, m_ConfirmBaseY, Lerp(-15, 0, p));
    }

    protected void ApplyExit(float elapsed)
    {
        // Buttons leave first.
        float p = EaseInCubic(Track(elapsed, 0, 160));
        SetA(m_Cancel,  1.0 - p);
        SetA(m_Confirm, 1.0 - p);
        SetY(m_Cancel,  m_CancelBaseY,  Lerp(0, -12, p));
        SetY(m_Confirm, m_ConfirmBaseY, Lerp(0, -12, p));

        // Body text.
        p = EaseInCubic(Track(elapsed, 60, 160));
        SetA(m_Body, 1.0 - p);
        SetY(m_Body, m_BodyBaseY, Lerp(0, 8, p));

        // Separator closes back to nothing.
        p = EaseInOutCubic(Track(elapsed, 90, 170));
        SetA(m_Separator, 1.0 - p);
        SetSepW(Lerp(m_SepBaseW, 0, p));

        // Title.
        p = EaseInCubic(Track(elapsed, 120, 160));
        SetA(m_Caption, 1.0 - p);
        SetY(m_Caption, m_CapBaseY, Lerp(0, 6, p));

        // Dialog box last.
        p = EaseInCubic(Track(elapsed, 140, 220));
        SetA(m_DialogBox, 1.0 - p);
        SetY(m_DialogBox, m_DlgBaseY, Lerp(0, 18, p));

        // The dim keeps fading after the dialog itself has gone.
        p = Track(elapsed, 0, 600);
        if (m_Backdrop) m_Backdrop.SetAlpha(1.0 - p);
    }

    // ----- Teardown -----

    protected void DoClose()
    {
        if (GetGame())
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.AnimTick);
        cuiElmnt.CleanupForOwner(this);

        m_Confirm   = null;
        m_Cancel    = null;
        m_Caption   = null;
        m_Body      = null;
        m_DialogBox = null;
        m_Backdrop  = null;
        m_Separator = null;

        if (m_Root)
        {
            m_Root.Unlink();
            m_Root = null;
        }

        if (s_OpenDialogs)
        {
            int idx = s_OpenDialogs.Find(this);
            if (idx >= 0) s_OpenDialogs.Remove(idx);
        }
    }

    void ~CuiDialog()
    {
        // At game shutdown the engine may already be gone.
        if (GetGame())
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.AnimTick);
        cuiElmnt.CleanupForOwner(this);
    }
}
