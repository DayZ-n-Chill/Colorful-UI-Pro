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
        }
        super.DisconnectSessionScript(displayJoinError);
    }
#endif

    void CreateCaptchaMenu()
    {
        if (!m_ANVICaptcha && !m_CaptchaCompleted)
        {
            m_ANVICaptcha = ANVICaptchaMenu.Cast(GetUIManager().EnterScriptedMenu(MENU_ANVI_CAPTCHA, GetUIManager().GetMenu()));
            if (m_ANVICaptcha)
            {
                m_ANVICaptcha.Reset();
                m_ANVICaptcha.SetAttempts(m_CaptchaAttempts);
            }
        }
    }
}