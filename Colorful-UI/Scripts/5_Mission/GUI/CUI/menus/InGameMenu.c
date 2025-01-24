modded class InGameMenu extends UIScriptedMenu
{
    protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider;
    protected ButtonWidget m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook;

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/inGame/cui.ingame.layout");
        m_HintPanel = new UiHintPanel(layoutRoot.FindAnyWidget("hint_frame"));

        m_ExitButton        = layoutRoot.FindAnyWidget("ExitBtn");
        m_ContinueButton    = layoutRoot.FindAnyWidget("ContinueBtn");
        m_OptionsButton     = layoutRoot.FindAnyWidget("OptionsBtn");
        m_RestartButton     = layoutRoot.FindAnyWidget("RestartBtn");
        m_RespawnButton     = layoutRoot.FindAnyWidget("respawn_button");
                
		m_PrioQ             = layoutRoot.FindAnyWidget("QueueBtn");
        m_Website           = layoutRoot.FindAnyWidget("WebsiteBtn");
        m_Discord           = layoutRoot.FindAnyWidget("DiscordBtn");
        m_Twitter           = layoutRoot.FindAnyWidget("TwitterBtn");
        m_Youtube           = layoutRoot.FindAnyWidget("YoutubeBtn");
        m_Reddit            = layoutRoot.FindAnyWidget("RedditBtn");
        m_Facebook          = layoutRoot.FindAnyWidget("FacebookBtn");

        m_TopShader         = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_BottomShader      = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
		m_MenuDivider       = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider"));

        if (m_TopShader)    m_TopShader.SetColor(colorScheme.TopShader());
        if (m_BottomShader) m_BottomShader.SetColor(colorScheme.BottomShader());
        if (m_MenuDivider)  m_MenuDivider.SetColor(colorScheme.Separator());
   
        cuiElmnt.proBtn(m_ExitButton,"#main_menu_exit", colorScheme.PrimaryText(), colorScheme.ButtonHover(),"", this,"OnClick_Exit");
        cuiElmnt.proBtn(m_ContinueButton,"#main_menu_continue",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"OnClick_Continue");
        cuiElmnt.proBtn(m_OptionsButton,"#layout_xbox_ingame_menu_options",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"OnClick_Options");
        cuiElmnt.proBtn(m_RestartButton,"#main_menu_restart",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"OnClick_Restart");
        cuiElmnt.proBtn(m_RespawnButton,"#main_menu_respawn",colorScheme.PrimaryText(),colorScheme.ButtonHover(),"",this,"OnClick_Respawn");

        cuiElmnt.proBtn(m_PrioQ,"Priority Queue",colorScheme.PrimaryText(),colorScheme.ButtonHover(),CustomURL.PriorityQ);           
        cuiElmnt.proBtn(m_Website,"Visit Website",colorScheme.PrimaryText(),colorScheme.ButtonHover(),CustomURL.Website);

        cuiElmnt.proBtn(m_Discord,"Discord",colorScheme.PrimaryText(),UIColor.Discord(),SocialURL.Discord);
        cuiElmnt.proBtn(m_Twitter,"Twitter",colorScheme.PrimaryText(),UIColor.Twitter(),SocialURL.Twitter);  
        cuiElmnt.proBtn(m_Youtube,"Youtube",colorScheme.PrimaryText(),UIColor.YouTube(),SocialURL.Youtube);
        cuiElmnt.proBtn(m_Reddit,"Reddit",colorScheme.PrimaryText(),UIColor.Reddit(),SocialURL.Reddit);
        cuiElmnt.proBtn(m_Facebook,"Facebook",colorScheme.PrimaryText(),UIColor.Facebook(),SocialURL.Facebook);

        CheckURL(m_PrioQ,    CustomURL.PriorityQ);
        CheckURL(m_Website,  CustomURL.Website);

        CheckURL(m_Discord,  SocialURL.Discord);
        CheckURL(m_Twitter,  SocialURL.Twitter);
        CheckURL(m_Youtube,  SocialURL.Youtube);
        CheckURL(m_Reddit,   SocialURL.Reddit);
        CheckURL(m_Facebook, SocialURL.Facebook);

        if (allInvalid && m_MenuDivider)
        {
            m_MenuDivider.Show(false);
        }

		HudShow(false);       
		return layoutRoot;
    }
}
