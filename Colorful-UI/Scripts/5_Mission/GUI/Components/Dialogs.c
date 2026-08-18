// CuiDialog — the standard Colorful-UI popup.

class CuiDialog
{
    static ref array<ref CuiDialog> s_OpenDialogs = new array<ref CuiDialog>();

    static const float ANIM_IN_TOTAL_MS  = 360.0;
    static const float ANIM_OUT_TOTAL_MS = 600.0;
    static const int   ANIM_TICK_MS      = 16;

    static const float DLG_TOP_REGION    = 80.0;
    static const float DLG_BOTTOM_REGION = 88.0;
    static const int   BODY_MIN_H        = 32;

    protected Widget         m_Root;
    protected ref CuiBackdrop m_Backdrop;
    protected Widget         m_DialogBox;
    protected Widget         m_Separator;
    protected ButtonWidget   m_Confirm;
    protected ButtonWidget   m_Cancel;
    protected TextWidget     m_Caption;
    protected RichTextWidget m_Body;
    protected bool           m_Closing;

    protected Class          m_CallbackTarget;
    protected string         m_OnConfirmMethod;
    protected string         m_OnCancelMethod;

    protected float m_DlgBaseY;
    protected float m_CapBaseY;
    protected float m_BodyBaseY;
    protected float m_CancelBaseY;
    protected float m_ConfirmBaseY;
    protected float m_SepBaseW;
    protected float m_SepBaseH;

    protected int   m_AnimDir;
    protected float m_Elapsed;

    void CuiDialog(string title, string body, bool useBackdrop = true, Class callbackTarget = null, string onConfirm = "", string onCancel = "")
    {
        m_CallbackTarget  = callbackTarget;
        m_OnConfirmMethod = onConfirm;
        m_OnCancelMethod  = onCancel;

        if (useBackdrop) m_Backdrop = new CuiBackdrop();

        m_Root = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.dialogs.layout");
        if (!m_Root) return;

        m_DialogBox = m_Root.FindAnyWidget("DialogBox");
        if (m_DialogBox) m_DialogBox.SetColor(ARGB(220, 62, 62, 62));
        m_Separator = m_Root.FindAnyWidget("SeparatorPanel");
        if (m_Separator) m_Separator.SetColor(colorScheme.Separator());
        m_Caption   = TextWidget.Cast(m_Root.FindAnyWidget("Caption"));
        m_Body      = RichTextWidget.Cast(m_Root.FindAnyWidget("Body"));
        m_Confirm   = ButtonWidget.Cast(m_Root.FindAnyWidget("Confirm"));
        m_Cancel    = ButtonWidget.Cast(m_Root.FindAnyWidget("Cancel"));

        if (m_Caption) m_Caption.SetText(title);
        if (m_Body)    m_Body.SetText(body);

        // Measure once now (best effort) and again after the engine's first layout pass —
        // wrapped line count is not known until the widget has actually been laid out.
        ResizeToBody();
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.ResizeToBody, 1, false);

        cuiElmnt.proBtnCB(this, m_Confirm, "#dialog_confirm", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OnConfirm");
        cuiElmnt.proBtnCB(this, m_Cancel,  "#dialog_cancel",  colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OnCancel");

        if (m_Confirm) m_Confirm.SetColor(UIColor.Transparent());
        if (m_Cancel)  m_Cancel.SetColor(UIColor.Transparent());

        float xtmp;
        if (m_DialogBox) m_DialogBox.GetPos(xtmp, m_DlgBaseY);
        if (m_Caption)   m_Caption.GetPos(xtmp, m_CapBaseY);
        if (m_Body)      m_Body.GetPos(xtmp, m_BodyBaseY);
        if (m_Cancel)    m_Cancel.GetPos(xtmp, m_CancelBaseY);
        if (m_Confirm)   m_Confirm.GetPos(xtmp, m_ConfirmBaseY);
        if (m_Separator) m_Separator.GetSize(m_SepBaseW, m_SepBaseH);

        ApplyEntrance(0);
        StartAnim(1);
    }

    protected void ResizeToBody()
    {
        if (!m_Body || !m_DialogBox) return;

        m_Body.Update();
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
        if (!dlg.m_Root) return null;
        if (!s_OpenDialogs) return null;
        s_OpenDialogs.Insert(dlg);
        return dlg;
    }

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
        if (m_Closing) return;

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

    void Close()
    {
        if (m_Closing) return;
        m_Closing = true;
        StartAnim(-1);
    }

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

    protected void ApplyEntrance(float elapsed)
    {
        float p = EaseOutCubic(Track(elapsed, 0, 200));
        if (m_Backdrop) m_Backdrop.SetAlpha(p);

        p = EaseOutCubic(Track(elapsed, 0, 220));
        SetA(m_DialogBox, p);
        SetY(m_DialogBox, m_DlgBaseY, Lerp(27, 0, p));

        p = EaseOutCubic(Track(elapsed, 80, 160));
        SetA(m_Caption, p);
        SetY(m_Caption, m_CapBaseY, Lerp(9, 0, p));

        p = EaseInOutCubic(Track(elapsed, 110, 170));
        SetA(m_Separator, p);
        SetSepW(Lerp(0, m_SepBaseW, p));

        p = EaseOutCubic(Track(elapsed, 140, 160));
        SetA(m_Body, p);
        SetY(m_Body, m_BodyBaseY, Lerp(9, 0, p));

        p = EaseOutCubic(Track(elapsed, 180, 180));
        SetA(m_Cancel, p);
        SetA(m_Confirm, p);
        SetY(m_Cancel,  m_CancelBaseY,  Lerp(-15, 0, p));
        SetY(m_Confirm, m_ConfirmBaseY, Lerp(-15, 0, p));
    }

    protected void ApplyExit(float elapsed)
    {
        float p = EaseInCubic(Track(elapsed, 0, 160));
        SetA(m_Cancel,  1.0 - p);
        SetA(m_Confirm, 1.0 - p);
        SetY(m_Cancel,  m_CancelBaseY,  Lerp(0, -12, p));
        SetY(m_Confirm, m_ConfirmBaseY, Lerp(0, -12, p));

        p = EaseInCubic(Track(elapsed, 60, 160));
        SetA(m_Body, 1.0 - p);
        SetY(m_Body, m_BodyBaseY, Lerp(0, 8, p));

        p = EaseInOutCubic(Track(elapsed, 90, 170));
        SetA(m_Separator, 1.0 - p);
        SetSepW(Lerp(m_SepBaseW, 0, p));

        p = EaseInCubic(Track(elapsed, 120, 160));
        SetA(m_Caption, 1.0 - p);
        SetY(m_Caption, m_CapBaseY, Lerp(0, 6, p));

        p = EaseInCubic(Track(elapsed, 140, 220));
        SetA(m_DialogBox, 1.0 - p);
        SetY(m_DialogBox, m_DlgBaseY, Lerp(0, 18, p));

        p = Track(elapsed, 0, 600);
        if (m_Backdrop) m_Backdrop.SetAlpha(1.0 - p);
    }

    protected void DoClose()
    {
        if (GetGame())
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.AnimTick);
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.ResizeToBody);
        }
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
        if (GetGame())
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.AnimTick);
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.ResizeToBody);
        }
        cuiElmnt.CleanupForOwner(this);
    }
}
