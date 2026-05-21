modded class RespawnDialogue extends UIScriptedMenu
{
    protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider, m_GameOverScreenImage, m_Logo;
    protected ButtonWidget m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook, m_CancelBtn, m_RandomRespawnBtn, m_CustomRespawnBtn;
    protected Widget m_TopSpacer, m_BottomSpacer, m_GameOverScreen;
    float m_TimerSlice;
	
    override Widget Init()
    {
        layoutRoot                  = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/inGame/cui.respawn.layout");
        m_DetailsRoot               = layoutRoot.FindAnyWidget("menu_details_tooltip");
        m_DetailsLabel              = TextWidget.Cast(m_DetailsRoot.FindAnyWidget("menu_details_label"));
        m_DetailsText               = RichTextWidget.Cast(m_DetailsRoot.FindAnyWidget("menu_details_tooltip_content"));
		
        m_CancelBtn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CancelBtn"));
        m_RandomRespawnBtn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("RandoRespawnBtn"));
        m_CustomRespawnBtn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("CustomRespawnBtn"));
		
        m_PrioQ = ButtonWidget.Cast(layoutRoot.FindAnyWidget("QueueBtn"));
        m_Website = ButtonWidget.Cast(layoutRoot.FindAnyWidget("WebsiteBtn"));
        m_Discord = ButtonWidget.Cast(layoutRoot.FindAnyWidget("DiscordBtn"));
        m_Twitter = ButtonWidget.Cast(layoutRoot.FindAnyWidget("TwitterBtn"));
        m_Youtube = ButtonWidget.Cast(layoutRoot.FindAnyWidget("YoutubeBtn"));
        m_Reddit = ButtonWidget.Cast(layoutRoot.FindAnyWidget("RedditBtn"));
        m_Facebook = ButtonWidget.Cast(layoutRoot.FindAnyWidget("FacebookBtn"));

        m_TopShader                 = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_BottomShader              = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
        
        m_TopSpacer                 = layoutRoot.FindAnyWidget("TopSpacer");
        m_MenuDivider               = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider"));
        m_BottomSpacer              = layoutRoot.FindAnyWidget("BottomSpacer");
		
        m_GameOverScreen            = Widget.Cast(layoutRoot.FindAnyWidget("GameOverScreen"));
        if (m_GameOverScreen)
            m_GameOverScreenImage   = ImageWidget.Cast(m_GameOverScreen.FindAnyWidget("GameOverScreenImage"));

        m_Logo                      = ImageWidget.Cast(layoutRoot.FindAnyWidget("Logo"));

        m_TopShader.SetColor(colorScheme.TopShader());
        m_BottomShader.SetColor(colorScheme.BottomShader());
        m_MenuDivider.SetColor(colorScheme.Separator());

        // CALLBACK BUTTONS
        cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_CancelBtn),        "#menu_cancel",                colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "CancelBtn");
        cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_RandomRespawnBtn), "#main_menu_respawn_random",   colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OnRandomRespawnClick");
        cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_CustomRespawnBtn), "#main_menu_respawn_custom",   colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "OnCustomRespawnClick");

        // URL BUTTONS (FIXED)
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_PrioQ),   "Priority Queue", colorScheme.PrimaryText(), colorScheme.ButtonHover(), CustomURL.PriorityQ);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Website), "Visit Website",  colorScheme.PrimaryText(), colorScheme.ButtonHover(), CustomURL.Website);

        // SOCIAL URL BUTTONS (FIXED)
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Discord),  "Discord",  colorScheme.PrimaryText(), UIColor.Discord(),  SocialURL.Discord);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Twitter),  "Twitter",  colorScheme.PrimaryText(), UIColor.Twitter(),  SocialURL.Twitter);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Youtube),  "Youtube",  colorScheme.PrimaryText(), UIColor.YouTube(),  SocialURL.Youtube);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Reddit),   "Reddit",   colorScheme.PrimaryText(), UIColor.Reddit(),   SocialURL.Reddit);
        cuiElmnt.proBtnURL(this, ButtonWidget.Cast(m_Facebook), "Facebook", colorScheme.PrimaryText(), UIColor.Facebook(), SocialURL.Facebook);

        allInvalid = true; // reset shared state before this menu's checks
        CheckURL(m_PrioQ,      CustomURL.PriorityQ);
        CheckURL(m_Website,    CustomURL.Website);
        CheckSocials(m_Discord,  SocialURL.Discord);
        CheckSocials(m_Twitter,  SocialURL.Twitter);
        CheckSocials(m_Youtube,  SocialURL.Youtube);
        CheckSocials(m_Reddit,   SocialURL.Reddit);
        CheckSocials(m_Facebook, SocialURL.Facebook);

        if (allInvalid && m_MenuDivider)
        {
            m_TopSpacer.Show(false);
            m_MenuDivider.Show(false);
            m_BottomSpacer.Show(false);
        }

        m_GameOverScreen.SetAlpha(0);
        m_GameOverScreenImage.LoadImageFile(0, GameOverScreen.GameOverScreenImage());
        m_GameOverScreenImage.SetAlpha(0);
        Branding.ApplyLogo(m_Logo);
        return layoutRoot;
    }

    override void Update(float timeslice)
    {
        if (ShowGameOverOverlay)
        {
            ShowGameOverScreen();
        }
        super.Update(timeslice);
    };

    void ShowGameOverScreen()
    {
        m_GameOverScreen.SetAlpha(1);
        m_GameOverScreenImage.SetAlpha(1);
        m_GameOverScreen.Show(true);
    }

    void CancelBtn()
    {
        Close();
    }

    void OnCustomRespawnClick()
    {
        RequestRespawn(false);
    }

    void OnRandomRespawnClick()
    {
        RequestRespawn(true);
    }

	void ~RespawnDialogue()
	{
		cuiElmnt.CleanupForOwner(this);
	}
}
