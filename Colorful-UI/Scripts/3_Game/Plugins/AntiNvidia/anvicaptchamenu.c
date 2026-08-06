// ANVICaptchaMenu
// -----------------------------------------------------------------------------
// AntiNvidia's captcha gate, restyled to match the mod's CuiDialog modal
// chrome and animation choreography (see Colorful-UI/Scripts/5_Mission/GUI/
// Components/Dialogs.c + Backdrop.c for the source of truth).
//
// This class lives in 3_Game (it's constructed from DayZGame.Connect(), a
// 3_Game override, before any mission/5_Mission context is guaranteed to
// exist). 3_Game compiles as a parent module to 5_Mission, so it cannot
// declare a variable of, cast to, or otherwise reference CuiDialog,
// CuiBackdrop, or cuiElmnt (all 5_Mission-only types) — doing so is a
// compile-time "unknown type" error, not a runtime risk. The dialog chrome,
// backdrop dim, and button hover styling below are therefore reimplemented
// inline rather than reusing those 5_Mission components.
//
// Animation choreography (ported from CuiDialog, same easing/timing so it
// reads identically to every other modal in the mod):
//   Entrance (~360ms): backdrop fade -> dialogbox rise -> caption rise ->
//   separator draws in by width -> body rise -> buttons rise from below.
//   Exit (~600ms, reverse order, backdrop gets a slow visible tail).
//
// Functional behavior preserved from the pre-restyle version (unchanged):
//   - two-digit image captcha via SetCorrectAnswer/DrawNumber
//   - answer input clamped to 2 chars in OnChange
//   - 3 attempts, decrement + Reset() on wrong answer, Leave() when exhausted
//   - Quit -> reveals QuitWarning + ConfirmQuitButton (now swaps into the
//     same bottom-row slot instead of a separate tiny corner widget)
//   - correct answer -> green feedback, "JOINING..", controls disabled,
//     queued DoConnect() -> game.Connect() (non-overloaded wrapper; do not
//     replace with a method pointer to the overloaded Connect())
//
// New in this version: on correct answer or on Leave(), the exit animation
// plays FIRST, and the original queued action (DoConnect / RequestExit) only
// fires once the exit animation's AnimTick completes. This keeps the timing
// of DoConnect's CallLater identical in spirit (still queued, still a direct
// call to the non-overloaded wrapper) — it's just deferred behind ~600ms of
// motion instead of a flat 10ms.
class ANVICaptchaMenu extends UIScriptedMenu
{
  private int m_CorrectAnswer;
  protected ImageWidget m_CorrectNumberDigit_0;
  protected ImageWidget m_CorrectNumberDigit_1;
  protected TextWidget m_CaptchaAttemptsText;
  protected EditBoxWidget m_TextAnswerInput;
  protected ButtonWidget m_AnswerSubmit;
  protected TextWidget m_QuitWarning;
  protected ButtonWidget m_QuitButton;
  protected ButtonWidget m_QuitConfirmButton;

  // ----- Dialog chrome (CuiDialog-equivalent widgets, found in this menu's
  // own layoutRoot instead of a separately-created component) -----
  protected ImageWidget m_Backdrop;
  protected float       m_BackdropMaxAlpha;
  protected Widget      m_DialogBox;
  protected Widget      m_Separator;
  protected Widget      m_Caption;
  protected Widget      m_Body;

  protected bool m_Closing;

  // Deferred post-exit-animation action.
  protected static const int ACTION_NONE    = 0;
  protected static const int ACTION_CONNECT = 1;
  protected static const int ACTION_LEAVE   = 2;
  protected int m_PendingAction;

  // Base layout values, captured once after widget creation so the
  // animation can compute offsets relative to where the layout puts them.
  protected float m_DlgBaseY;
  protected float m_CapBaseY;
  protected float m_BodyBaseY;
  protected float m_QuitBaseY;
  protected float m_SubmitBaseY;
  protected float m_SepBaseW;
  protected float m_SepBaseH;

  // Animation state. m_AnimDir: +1 entering, -1 exiting, 0 idle.
  protected static const float ANIM_IN_TOTAL_MS  = 360.0;
  protected static const float ANIM_OUT_TOTAL_MS = 600.0;
  protected static const int   ANIM_TICK_MS      = 16;
  protected int   m_AnimDir;
  protected float m_Elapsed;

  void ANVICaptchaMenu() {}

  void ~ANVICaptchaMenu()
  {
    // GetGame() can already be gone when this destructor runs during script
    // module teardown (same shutdown window documented in
    // Components/buttons.c, cuiElmnt.Cleanup) - guard before touching it.
    if (GetGame())
      GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.AnimTick);
  }

  override void OnShow() {
    super.OnShow();
    PPEffects.SetBlurMenu(1);
    GetGame().GetInput().ChangeGameFocus(1);
    SetFocus(layoutRoot);
  }

  override void OnHide() {
    super.OnHide();
    PPEffects.SetBlurMenu(0);
    GetGame().GetInput().ResetGameFocus();
  }

  override Widget Init() {
    string p1 = "Colorful-UI/GUI/layouts/plugins/AntiNvidia/";
    string p2 = "dialog_anvi_captcha.layout";
    layoutRoot = GetGame().GetWorkspace().CreateWidgets(p1 + p2);
    if (!layoutRoot) return layoutRoot;

    Widget w;
    w = layoutRoot.FindAnyWidget("DigitLeft");
    m_CorrectNumberDigit_0 = ImageWidget.Cast(w);
    w = layoutRoot.FindAnyWidget("DigitRight");
    m_CorrectNumberDigit_1 = ImageWidget.Cast(w);
    w = layoutRoot.FindAnyWidget("CaptchaAttempts");
    m_CaptchaAttemptsText = TextWidget.Cast(w);
    w = layoutRoot.FindAnyWidget("AnswerInput");
    m_TextAnswerInput = EditBoxWidget.Cast(w);
    w = layoutRoot.FindAnyWidget("AnswerSubmitButton");
    m_AnswerSubmit = ButtonWidget.Cast(w);
    w = layoutRoot.FindAnyWidget("QuitWarning");
    m_QuitWarning = TextWidget.Cast(w);
    w = layoutRoot.FindAnyWidget("QuitButton");
    m_QuitButton = ButtonWidget.Cast(w);
    w = layoutRoot.FindAnyWidget("ConfirmQuitButton");
    m_QuitConfirmButton = ButtonWidget.Cast(w);

    m_Backdrop  = ImageWidget.Cast(layoutRoot.FindAnyWidget("Backdrop"));
    m_DialogBox = layoutRoot.FindAnyWidget("DialogBox");
    m_Separator = layoutRoot.FindAnyWidget("SeparatorPanel");
    m_Caption   = layoutRoot.FindAnyWidget("Caption");
    m_Body      = layoutRoot.FindAnyWidget("BodyPanel");

    SetupChrome();
    CaptureBaseLayout();

    ApplyEntrance(0);
    StartAnim(1);

    return layoutRoot;
  }

  // ----- Chrome setup (CuiDialog/CuiBackdrop-equivalent runtime styling) -----

  protected void SetupChrome()
  {
    // Layout's `color R G B A` on a DayZDefaultPanel drops the alpha
    // component, so the translucent dialog panel color is applied at
    // runtime via ARGB, same as CuiDialog.CuiDialog (Dialogs.c:110).
    if (m_DialogBox) m_DialogBox.SetColor(ARGB(220, 62, 62, 62));
    if (m_Separator) m_Separator.SetColor(colorScheme.Separator());

    // Backdrop: capture the layout-baked peak alpha, force the widget's
    // color to fully opaque, then drive 0..1 via SetAlpha as a multiplier.
    // Mirrors CuiBackdrop.CuiBackdrop (Backdrop.c:42-55) — reimplemented
    // inline because CuiBackdrop is a 5_Mission-only type.
    if (m_Backdrop)
    {
      int layoutColor = m_Backdrop.GetColor();
      int alphaByte    = (layoutColor >> 24) & 0xFF;
      m_BackdropMaxAlpha = alphaByte / 255.0;
      m_Backdrop.SetColor(layoutColor | 0xFF000000);
      m_Backdrop.SetAlpha(0);
    }

    if (m_TextAnswerInput) m_TextAnswerInput.SetColor(colorScheme.OptionInputColors());

    // Bottom-row buttons: same treatment as CuiDialog's Confirm/Cancel via
    // cuiElmnt.proBtnCB (buttons.c:353-377) — text tinted with PrimaryText,
    // base color forced transparent so only the EmptyHighlight style shows,
    // hover swap handled manually via OnMouseEnter/OnMouseLeave below since
    // CUIButtonHandler is 5_Mission-only.
    ApplyButtonBaseStyle(m_QuitButton);
    ApplyButtonBaseStyle(m_QuitConfirmButton);
    ApplyButtonBaseStyle(m_AnswerSubmit);
  }

  protected void ApplyButtonBaseStyle(ButtonWidget btn)
  {
    if (!btn) return;
    btn.SetTextColor(colorScheme.PrimaryText());
    btn.SetColor(UIColor.Transparent());
  }

  protected void CaptureBaseLayout()
  {
    float xtmp;
    if (m_DialogBox)       m_DialogBox.GetPos(xtmp, m_DlgBaseY);
    if (m_Caption)         m_Caption.GetPos(xtmp, m_CapBaseY);
    if (m_Body)            m_Body.GetPos(xtmp, m_BodyBaseY);
    if (m_QuitButton)      m_QuitButton.GetPos(xtmp, m_QuitBaseY);
    if (m_AnswerSubmit)    m_AnswerSubmit.GetPos(xtmp, m_SubmitBaseY);
    if (m_Separator)       m_Separator.GetSize(m_SepBaseW, m_SepBaseH);
  }

  // ----- Hover styling (manual CUIButtonHandler-equivalent) -----

  override bool OnMouseEnter(Widget w, int x, int y)
  {
    if (w == m_QuitButton || w == m_QuitConfirmButton || w == m_AnswerSubmit)
    {
      ButtonWidget.Cast(w).SetTextColor(colorScheme.ButtonHover());
      return true;
    }
    return super.OnMouseEnter(w, x, y);
  }

  override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
  {
    if (w == m_QuitButton || w == m_QuitConfirmButton || w == m_AnswerSubmit)
    {
      ButtonWidget.Cast(w).SetTextColor(colorScheme.PrimaryText());
      return true;
    }
    return super.OnMouseLeave(w, enterW, x, y);
  }

  override bool OnClick(Widget w, int x, int y, int button) {
    super.OnClick(w, x, y, button);

    // Ignore clicks that land during the exit animation — buttons stay
    // alive (just invisible/fading) until the menu actually tears down.
    if (m_Closing) return true;

    // A null `w` would compare equal to any widget member that failed to
    // resolve in Init(), which would then be dereferenced below.
    if (!w) return false;

    // Clicking anything other than the quit controls themselves cancels a
    // pending quit confirmation and restores the QUIT button (it's hidden
    // while ConfirmQuitButton occupies its slot below).
    if (w != m_QuitButton && w != m_QuitConfirmButton) {
      if (m_QuitButton)        m_QuitButton.Show(true);
      if (m_QuitConfirmButton) m_QuitConfirmButton.Show(false);
      if (m_QuitWarning)       m_QuitWarning.Show(false);
    }
    switch (w) {
    case m_QuitButton: {
      m_QuitButton.Show(false);
      if (m_QuitConfirmButton) m_QuitConfirmButton.Show(true);
      if (m_QuitWarning)       m_QuitWarning.Show(true);
      return true;
    }
    case m_QuitConfirmButton: {
      Leave();
      return true;
    }
    case m_AnswerSubmit: {
      if (!m_TextAnswerInput) return true;
      string answer = m_TextAnswerInput.GetText();
      if (answer != string.Empty) {
        HandleAnswer(answer.ToInt());
      }
      return true;
    }
    }
    return false;
  }

  override bool OnChange(Widget w, int x, int y, bool finished) {
    super.OnChange(w, x, y, finished);
    if (w && w == m_TextAnswerInput) {
      string fieldText = m_TextAnswerInput.GetText();
      if (fieldText.Length() > 2) {
        m_TextAnswerInput.SetText(fieldText.Substring(0, 2));
      }
      return true;
    }
    return false;
  }

  private void HandleAnswer(int answer) {
    DayZGame game = DayZGame.Cast(GetGame());
    if (game && game.m_CaptchaCompleted)
      return;
    bool isCorrect = CheckAnswer(answer);
    if (isCorrect) {
      HandleCorrectAnswer();
    } else {
      HandleIncorrectAnswer();
    }
  }

  private void HandleCorrectAnswer() {
    DayZGame game = DayZGame.Cast(GetGame());
    if (game) {
      game.m_CaptchaCompleted = true;
      if (m_TextAnswerInput)
      {
        m_TextAnswerInput.SetColor(COLOR_GREEN);
        m_TextAnswerInput.SetFlags(WidgetFlags.NOFOCUS);
      }
      if (m_AnswerSubmit)
      {
        m_AnswerSubmit.SetTextColor(COLOR_GREEN);
        m_AnswerSubmit.SetText("JOINING..");
        m_AnswerSubmit.SetFlags(WidgetFlags.DISABLED);
      }
      if (m_QuitButton) m_QuitButton.SetFlags(WidgetFlags.DISABLED);
      if (m_QuitConfirmButton) m_QuitConfirmButton.SetFlags(WidgetFlags.DISABLED);

      m_PendingAction = ACTION_CONNECT;
      StartExitAnim();
    }
  }

  // DayZGame.Connect is overloaded (no-arg + multi-arg), and CallLater cannot
  // bind to an overloaded method pointer. This non-overloaded wrapper calls
  // the no-arg Connect() directly (a direct call to an overloaded method is
  // legal), which is safe to use here: m_ConnectAddress/m_ConnectPort are set
  // by whatever initiated the join (server browser, invite/ConnectFromJoin,
  // CLI) before AntiNvidia's Connect() override intercepted, and neither
  // DisconnectSessionScript() nor DisconnectSessionEx() clears them
  // (P:\scripts\3_game\dayzgame.c:2689-2758), so they're still valid here.
  private void DoConnect() {
    DayZGame game = DayZGame.Cast(GetGame());
    if (game)
      game.Connect();
  }

  private void HandleIncorrectAnswer() {
    DayZGame game = DayZGame.Cast(GetGame());
    if (game) {
      SetAttempts(game.m_CaptchaAttempts - 1);
      if (game.m_CaptchaAttempts > 0) {
        Reset();
      } else {
        Leave();
      }
    }
  }

  private void Leave() {
    if (m_Closing) return;
    m_PendingAction = ACTION_LEAVE;
    StartExitAnim();
  }

  // Reset() and SetAttempts() are called from outside this class (DayZGame.
  // CreateCaptchaMenu, right after EnterScriptedMenu), so they can run with
  // every widget member still null if Init()'s CreateWidgets failed.
  void Reset() {
    if (m_TextAnswerInput) m_TextAnswerInput.SetText("");
    m_CorrectAnswer = DrawNumber(100);
    SetCorrectAnswer(m_CorrectAnswer);
  }

  private void SetCorrectAnswer(int number) {
    if (!m_CorrectNumberDigit_0 || !m_CorrectNumberDigit_1) return;
    string numberString = number.ToStringLen(2);
    string prefix = "Colorful-UI/GUI/textures/plugins/AntiNvidia/numbers/";
    m_CorrectNumberDigit_0.LoadImageFile(0, prefix + numberString[0] + ".edds");
    m_CorrectNumberDigit_1.LoadImageFile(0, prefix + numberString[1] + ".edds");
  }

  void SetAttempts(int attempts) {
    DayZGame game = DayZGame.Cast(GetGame());
    if (game)
      game.m_CaptchaAttempts = attempts;
    if (m_CaptchaAttemptsText)
      m_CaptchaAttemptsText.SetText(string.Format("attempts left: %1", attempts));
  }

  private bool CheckAnswer(int answer) { return m_CorrectAnswer == answer; }

  private int DrawNumber(int range = 100) { return Math.RandomInt(0, 100); }

  // ----- Animation driver (ported from CuiDialog.AnimTick, Dialogs.c:237-260) -----

  protected void StartAnim(int dir) {
    m_AnimDir = dir;
    m_Elapsed = 0;
    GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(this.AnimTick);
    GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.AnimTick, ANIM_TICK_MS, false);
  }

  protected void StartExitAnim() {
    if (m_Closing) return;
    m_Closing = true;
    StartAnim(-1);
  }

  void AnimTick() {
    if (m_AnimDir == 0 || !layoutRoot) return;

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
      if (finishedDir < 0) DoFinishExit();
      return;
    }

    GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.AnimTick, ANIM_TICK_MS, false);
  }

  // Dispatches whatever action triggered the exit animation, once the
  // animation has actually finished playing. DoConnect keeps going through
  // a queued CallLater on the same category as before the restyle — only
  // the trigger point moved (from immediately-on-correct-answer to
  // after-the-exit-animation).
  //
  // Do NOT call Close() here. UIMenuPanel.Close() on a submenu entered via
  // EnterScriptedMenu(id, parentMenu) hands control back to that PARENT
  // menu (the screen you were on before you hit Play) — it does not just
  // tear down this menu's widgets. Calling it on the connect path was a
  // regression: it sent the player back to the parent menu instead of
  // letting game.Connect() proceed to the loading screen. The original
  // (pre-restyle) code never closed the menu explicitly on a correct
  // answer either — it relies on the real connect's own scene transition
  // to tear down the whole menu stack, captcha included. That must stay
  // untouched; any leftover-widget cleanup has to happen without touching
  // the menu's Close()/parent-navigation path.
  protected void DoFinishExit() {
    int action = m_PendingAction;
    m_PendingAction = ACTION_NONE;

    if (action == ACTION_CONNECT) {
      ScriptCallQueue queue = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM);
      if (queue)
        queue.CallLater(DoConnect, 10, false);
    } else if (action == ACTION_LEAVE) {
      GetGame().RequestExit(0);
    }
  }

  // ----- Easing + interpolation (ported from Dialogs.c:264-291) -----

  protected static float Track(float elapsed, float delay, float duration) {
    if (duration <= 0) return 1.0;
    float t = (elapsed - delay) / duration;
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    return t;
  }

  protected static float EaseOutCubic(float t) {
    float inv = 1.0 - t;
    return 1.0 - inv * inv * inv;
  }

  protected static float EaseInCubic(float t) {
    return t * t * t;
  }

  protected static float EaseInOutCubic(float t) {
    if (t < 0.5) return 4.0 * t * t * t;
    float k = -2.0 * t + 2.0;
    return 1.0 - (k * k * k) / 2.0;
  }

  protected static float Lerp(float a, float b, float t) { return a + (b - a) * t; }

  // ----- Per-element setters (null-safe, ported from Dialogs.c:295-312) -----

  protected static void SetA(Widget w, float a) {
    if (w) w.SetAlpha(a);
  }

  protected static void SetY(Widget w, float baseY, float dy) {
    if (!w) return;
    float x;
    float oldY;
    w.GetPos(x, oldY);
    w.SetPos(x, baseY + dy);
  }

  protected void SetSepW(float width) {
    if (m_Separator) m_Separator.SetSize(width, m_SepBaseH);
  }

  protected void SetBackdropAlpha(float a) {
    if (m_Backdrop) m_Backdrop.SetAlpha(a * m_BackdropMaxAlpha);
  }

  // ----- Choreography (ported from Dialogs.c:316-388) -----
  //
  // Groups map to CuiDialog's six tracks: backdrop, dialogbox, caption,
  // separator (draws in by width), body (here: question + digit box +
  // attempts + input row + quit warning, as one panel), buttons (quit-slot
  // button + submit, rising together from below).

  protected void ApplyEntrance(float elapsed) {
    float p = EaseOutCubic(Track(elapsed, 0, 200));
    SetBackdropAlpha(p);

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

    // Buttons rise from below (valign bottom_ref: lower Y = lower on
    // screen, so start with Y - 15 and animate to base).
    p = EaseOutCubic(Track(elapsed, 180, 180));
    SetA(m_QuitButton, p);
    SetA(m_QuitConfirmButton, p);
    SetA(m_AnswerSubmit, p);
    SetY(m_QuitButton, m_QuitBaseY, Lerp(-15, 0, p));
    SetY(m_QuitConfirmButton, m_QuitBaseY, Lerp(-15, 0, p));
    SetY(m_AnswerSubmit, m_SubmitBaseY, Lerp(-15, 0, p));
  }

  protected void ApplyExit(float elapsed) {
    float p = EaseInCubic(Track(elapsed, 0, 160));
    SetA(m_QuitButton, 1.0 - p);
    SetA(m_QuitConfirmButton, 1.0 - p);
    SetA(m_AnswerSubmit, 1.0 - p);
    SetY(m_QuitButton, m_QuitBaseY, Lerp(0, -12, p));
    SetY(m_QuitConfirmButton, m_QuitBaseY, Lerp(0, -12, p));
    SetY(m_AnswerSubmit, m_SubmitBaseY, Lerp(0, -12, p));

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

    // Backdrop: linear fade across the full exit window — dialog elements
    // are all gone by ~220ms, so the second half is a backdrop-only fade.
    p = Track(elapsed, 0, 600);
    SetBackdropAlpha(1.0 - p);
  }
}
