// CuiDialog
// -----------------------------------------------------------------------------
// Self-managed reusable dialog. No UIScriptedMenu, no MissionBase routing.
//
//   CuiDialog.Show("Title", "Body text");
//   CuiDialog.Show("Title", "Body text", false);  // no dim overlay behind dialog
//
// With confirm/cancel callbacks:
//   CuiDialog.Show("Exit?", "Are you sure?", true, this, "DoExit", "");
//
// callbackTarget + method names are dispatched via GameScript.CallFunction;
// pass "" as a method name to skip that side. Callbacks fire BEFORE the exit
// animation, matching how vanilla CALL_CATEGORY_GUI handlers work.
//
// Lifecycle: instances are kept alive by s_OpenDialogs while visible. On close,
// the exit animation runs; when it finishes, button handlers are disposed,
// the widget tree is unlinked, and the instance is removed from s_OpenDialogs
// (last ref drops -> destructor fires as a safety net).
//
// Animation: multi-stage choreography. Each element animates on its own
// timeline (delay + duration + curve), so the dialog assembles itself piece
// by piece on entrance and disassembles in reverse on exit.
//
// All elements rise UP on entrance and fall DOWN on exit for consistent motion.
//
// Entrance (~360ms):
//   backdrop  alpha 0 -> 0.7         [0..200]   ease-out
//   dialogbox alpha 0 -> 1, Y +27→0  [0..220]   ease-out cubic
//   caption   alpha 0 -> 1, Y +9→0   [80..240]  ease-out cubic
//   separator alpha 0 -> 1, w 0→base [110..280] ease-in-out
//   body      alpha 0 -> 1, Y +9→0   [140..300] ease-out cubic
//   buttons   alpha 0 -> 1, Y -15→0  [180..360] ease-out cubic   (rises from below; valign bottom_ref)
//
// Exit (~600ms — backdrop fade gets a visible tail after dialog elements leave):
//   buttons   alpha 1 -> 0, Y 0→-12  [0..160]   ease-in cubic    (drops; valign bottom_ref)
//   body      alpha 1 -> 0, Y 0→+8   [60..220]  ease-in cubic
//   separator alpha 1 -> 0, w base→0 [90..260]  ease-in-out
//   caption   alpha 1 -> 0, Y 0→+6   [120..280] ease-in cubic
//   dialogbox alpha 1 -> 0, Y 0→+18  [140..360] ease-in cubic
//   backdrop  alpha 0.7 -> 0         [0..600]   linear (slow visible fade)
//
// Driven by a CallLater that re-schedules itself every 16ms (~60fps).
// CleanupForOwner is called BEFORE Unlink so SetHandler(null) runs against
// still-valid widgets.

class CuiDialog
{
    static ref array<ref CuiDialog> s_OpenDialogs = new array<ref CuiDialog>();

    static const float ANIM_IN_TOTAL_MS  = 360.0;
    static const float ANIM_OUT_TOTAL_MS = 600.0;   // backdrop needs a long tail after dialog elements leave
    static const int   ANIM_TICK_MS      = 16;

    // Vertical layout constants for responsive height. Sum = total non-body
    // chrome height of DialogBox; body content height is added on top.
    //   top region = caption(20+32) + separator(10+2) + body-top-padding(16) = 80
    //   bottom region = body-bottom-gap(16) + button(56) + bottom-padding(16) = 88
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

    // Callback dispatch (target + method name, vanilla GameScript.CallFunction
    // pattern). Empty method name on either side = skip that callback.
    protected Class          m_CallbackTarget;
    protected string         m_OnConfirmMethod;
    protected string         m_OnCancelMethod;

    // Base layout values, captured at construction so each animation can
    // compute offsets relative to where the layout puts each element.
    protected float m_DlgBaseY;
    protected float m_CapBaseY;
    protected float m_BodyBaseY;
    protected float m_CancelBaseY;
    protected float m_ConfirmBaseY;
    protected float m_SepBaseW;
    protected float m_SepBaseH;

    // Animation state. m_AnimDir: +1 entering, -1 exiting, 0 idle.
    protected int   m_AnimDir;
    protected float m_Elapsed;       // ms since current animation started

    void CuiDialog(string title, string body, bool useBackdrop = true, Class callbackTarget = null, string onConfirm = "", string onCancel = "")
    {
        m_CallbackTarget  = callbackTarget;
        m_OnConfirmMethod = onConfirm;
        m_OnCancelMethod  = onCancel;

        // Backdrop is its own atomic-design component. Created BEFORE the
        // dialog tree so it sits underneath at workspace level (priority 999
        // vs dialog's 1000). When useBackdrop=false, simply skip creating it.
        if (useBackdrop) m_Backdrop = new CuiBackdrop();

        m_Root = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.dialogs.layout");
        if (!m_Root) return;

        m_DialogBox = m_Root.FindAnyWidget("DialogBox");
        // Layout's `color R G B A` drops the alpha component, so apply the
        // translucent panel color at runtime via ARGB. Widget-level alpha
        // (animated by SetA below) multiplies on top of this.
        if (m_DialogBox) m_DialogBox.SetColor(ARGB(220, 62, 62, 62));
        m_Separator = m_Root.FindAnyWidget("SeparatorPanel");
        if (m_Separator) m_Separator.SetColor(colorScheme.Separator());
        m_Caption   = TextWidget.Cast(m_Root.FindAnyWidget("Caption"));
        m_Body      = RichTextWidget.Cast(m_Root.FindAnyWidget("Body"));
        m_Confirm   = ButtonWidget.Cast(m_Root.FindAnyWidget("Confirm"));
        m_Cancel    = ButtonWidget.Cast(m_Root.FindAnyWidget("Cancel"));

        if (m_Caption) m_Caption.SetText(title);
        if (m_Body)    m_Body.SetText(body);

        // Size DialogBox to fit body content height. Body has size_to_text_v=1
        // so it auto-sizes its bounds to the wrapped text; we read that size and
        // grow DialogBox so the buttons and chrome stay correctly framed.
        ResizeToBody();

        cuiElmnt.proBtnCB(this, m_Confirm, "Confirm", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OnConfirm");
        cuiElmnt.proBtnCB(this, m_Cancel,  "Cancel",  colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OnCancel");

        // Layout bakes Cancel/Confirm with hardcoded blue colors that leak
        // for one frame before the button handler's ApplyBaseStyles runs.
        // Force transparent here so the layout color never shows.
        if (m_Confirm) m_Confirm.SetColor(UIColor.Transparent());
        if (m_Cancel)  m_Cancel.SetColor(UIColor.Transparent());

        // Capture base layout values so animations can interpolate around them.
        float xtmp;
        if (m_DialogBox) m_DialogBox.GetPos(xtmp, m_DlgBaseY);
        if (m_Caption)   m_Caption.GetPos(xtmp, m_CapBaseY);
        if (m_Body)      m_Body.GetPos(xtmp, m_BodyBaseY);
        if (m_Cancel)    m_Cancel.GetPos(xtmp, m_CancelBaseY);
        if (m_Confirm)   m_Confirm.GetPos(xtmp, m_ConfirmBaseY);
        if (m_Separator) m_Separator.GetSize(m_SepBaseW, m_SepBaseH);

        // Snap every element to its t=0 entrance state, then drive the timer.
        ApplyEntrance(0);
        StartAnim(1);
    }

    protected void ResizeToBody()
    {
        if (!m_Body || !m_DialogBox) return;

        // RichTextWidget exposes the wrapped/rendered text height directly via
        // GetContentHeight() (P:\scripts\1_core\proto\enwidgets.c:226). The body
        // widget has size_to_text_v=1 and wrap=1 in the layout, so this returns
        // the laid-out content height in pixels synchronously after SetText —
        // matches how vanilla TextWidget.GetTextSize() is used immediately
        // post-SetText (e.g. P:\scripts\5_mission\gui\scriptconsole.c:476).
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
        // Don't keep an instance whose widgets failed to load — it would
        // never be removed (no buttons -> no OnCancel/OnConfirm path).
        if (!dlg.m_Root) return null;
        s_OpenDialogs.Insert(dlg);
        return dlg;
    }

    void OnConfirm()
    {
        // Fire the caller's callback BEFORE starting the exit animation. The
        // dialog is still alive (m_Closing not yet set), so the callback can
        // even open another dialog if it wants — that one stacks on top.
        if (m_CallbackTarget && m_OnConfirmMethod != "")
        {
            GetGame().GameScript.CallFunction(m_CallbackTarget, m_OnConfirmMethod, null, 0);
        }
        Close();
    }

    void OnCancel()
    {
        if (m_CallbackTarget && m_OnCancelMethod != "")
        {
            GetGame().GameScript.CallFunction(m_CallbackTarget, m_OnCancelMethod, null, 0);
        }
        Close();
    }

    // Idempotent: a fast double-click that fires both OnConfirm and OnCancel
    // before the exit animation finishes will only run one close sequence.
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
        // Backdrop component handles its own widget; we just drive alpha.
        // Final rendered alpha = backdrop's color_alpha (~0.7) * widget alpha.
        float p = EaseOutCubic(Track(elapsed, 0, 200));
        if (m_Backdrop) m_Backdrop.SetAlpha(p);

        // Dialog box: alpha + Y rise [0..220]
        p = EaseOutCubic(Track(elapsed, 0, 220));
        SetA(m_DialogBox, p);
        SetY(m_DialogBox, m_DlgBaseY, Lerp(27, 0, p));

        // Caption: alpha + Y rise [80..240]
        p = EaseOutCubic(Track(elapsed, 80, 160));
        SetA(m_Caption, p);
        SetY(m_Caption, m_CapBaseY, Lerp(9, 0, p));

        // Separator line draws in by width [110..280]
        p = EaseInOutCubic(Track(elapsed, 110, 170));
        SetA(m_Separator, p);
        SetSepW(Lerp(0, m_SepBaseW, p));

        // Body: alpha + Y rise [140..300]
        p = EaseOutCubic(Track(elapsed, 140, 160));
        SetA(m_Body, p);
        SetY(m_Body, m_BodyBaseY, Lerp(9, 0, p));

        // Buttons: alpha + rise from below (valign bottom_ref: lower Y = lower
        // on screen, so start with Y - 15 and animate to base) [180..360]
        p = EaseOutCubic(Track(elapsed, 180, 180));
        SetA(m_Cancel, p);
        SetA(m_Confirm, p);
        SetY(m_Cancel,  m_CancelBaseY,  Lerp(-15, 0, p));
        SetY(m_Confirm, m_ConfirmBaseY, Lerp(-15, 0, p));
    }

    protected void ApplyExit(float elapsed)
    {
        // Buttons drop first [0..160]
        float p = EaseInCubic(Track(elapsed, 0, 160));
        SetA(m_Cancel,  1.0 - p);
        SetA(m_Confirm, 1.0 - p);
        SetY(m_Cancel,  m_CancelBaseY,  Lerp(0, -12, p));
        SetY(m_Confirm, m_ConfirmBaseY, Lerp(0, -12, p));

        // Body fades + drifts down [60..220]
        p = EaseInCubic(Track(elapsed, 60, 160));
        SetA(m_Body, 1.0 - p);
        SetY(m_Body, m_BodyBaseY, Lerp(0, 8, p));

        // Separator collapses [90..260]
        p = EaseInOutCubic(Track(elapsed, 90, 170));
        SetA(m_Separator, 1.0 - p);
        SetSepW(Lerp(m_SepBaseW, 0, p));

        // Caption fades + drifts down [120..280]
        p = EaseInCubic(Track(elapsed, 120, 160));
        SetA(m_Caption, 1.0 - p);
        SetY(m_Caption, m_CapBaseY, Lerp(0, 6, p));

        // Dialog box fades + drops [140..360]
        p = EaseInCubic(Track(elapsed, 140, 220));
        SetA(m_DialogBox, 1.0 - p);
        SetY(m_DialogBox, m_DlgBaseY, Lerp(0, 18, p));

        // Backdrop: linear fade across the full exit window. Dialog elements
        // are all gone by ~220ms, so the second half of this window is a
        // backdrop-only fade — clearly visible to the user. Linear because
        // both EaseOut (front-loaded, lingers) and EaseIn (back-loaded,
        // snaps) read poorly here. [0..600]
        p = Track(elapsed, 0, 600);
        if (m_Backdrop) m_Backdrop.SetAlpha(1.0 - p);
    }

    // ----- Teardown -----

    protected void DoClose()
    {
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

        int idx = s_OpenDialogs.Find(this);
        if (idx >= 0) s_OpenDialogs.Remove(idx);
    }

    void ~CuiDialog()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.AnimTick);
        cuiElmnt.CleanupForOwner(this);
    }
}
