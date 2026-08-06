modded class DayZGame
{
    bool m_CaptchaCompleted;
    int m_CaptchaAttempts = 3;
    private ref ANVICaptchaMenu m_ANVICaptcha;

#ifndef ANVI_InGame
    override void Connect()
    {
        if (AntiNvidia && !m_CaptchaCompleted)
        {
            DisconnectSessionScript();
            CreateCaptchaMenu();
        }
        else
        {
            super.Connect();
        }
    }

    override void DisconnectSessionScript(bool displayJoinError = false)
    {
        if (m_ANVICaptcha)
        {
            m_ANVICaptcha.Close();
            // Drop our ref immediately. m_ANVICaptcha is a `ref` to an engine-
            // Managed UIScriptedMenu: holding it past Close() keeps the script
            // instance alive after the engine has torn the menu down, which
            // (a) leaves a stale pointer for the next DisconnectSessionScript
            // to Close() a second time, (b) suppresses ~ANVICaptchaMenu, so a
            // mid-animation AnimTick stays queued against freed widgets, and
            // (c) makes CreateCaptchaMenu's `!m_ANVICaptcha` guard refuse to
            // ever build the captcha again.
            m_ANVICaptcha = null;
        }
        super.DisconnectSessionScript(displayJoinError);
    }
#endif

    void CreateCaptchaMenu()
    {
        if (!m_ANVICaptcha && !m_CaptchaCompleted)
        {
            UIManager ui = GetUIManager();
            if (!ui) return;

            m_ANVICaptcha = ANVICaptchaMenu.Cast(ui.EnterScriptedMenu(MENU_ANVI_CAPTCHA, ui.GetMenu()));
            if (m_ANVICaptcha)
            {
                m_ANVICaptcha.Reset();
                m_ANVICaptcha.SetAttempts(m_CaptchaAttempts);
            }
        }
    }
}