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
            // Must be cleared here, or the captcha can never open again.
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