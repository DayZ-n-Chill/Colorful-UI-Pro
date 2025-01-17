/*************************************
 * FILE: InGameMenu.c (or wherever you place it)
 *************************************/

// “modded” so it overrides the existing DayZ InGameMenu
modded class InGameMenu extends UIScriptedMenu
{
    // Some extra references (as in your original code)
    protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider;
    protected ButtonWidget m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook;
    float m_TimerSlice;
    private Widget m_GameOverScreen;
    private ImageWidget m_GameOverScreenImage;

    // These are from vanilla InGameMenu
    override Widget Init()
    {
        // EXACT LAYOUT PATH as in your code
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/inGame/cui.ingame.layout");
        m_HintPanel = new UiHintPanel(layoutRoot.FindAnyWidget("hint_frame"));

        // Example references from vanilla dayz, plus your custom ones
        m_ContinueButton          = layoutRoot.FindAnyWidget("ContinueBtn");
        m_ExitButton             = layoutRoot.FindAnyWidget("ExitBtn");
        m_RestartButton          = layoutRoot.FindAnyWidget("RestartBtn");
        m_RespawnButton          = layoutRoot.FindAnyWidget("respawn_button");
        m_RestartDeadRandomButton= layoutRoot.FindAnyWidget("respawn_button_random");
        m_RestartDeadCustomButton= layoutRoot.FindAnyWidget("respawn_button_custom");
        m_OptionsButton          = layoutRoot.FindAnyWidget("OptionsBtn");
        m_PrioQ                  = layoutRoot.FindAnyWidget("QueueBtn");
        m_Website                = layoutRoot.FindAnyWidget("WebsiteBtn");
        m_MenuDivider            = ImageWidget.Cast(layoutRoot.FindAnyWidget("MenuDivider"));
        m_Discord                = layoutRoot.FindAnyWidget("DiscordBtn");
        m_Twitter                = layoutRoot.FindAnyWidget("TwitterBtn");
        m_Youtube                = layoutRoot.FindAnyWidget("YoutubeBtn");
        m_Reddit                 = layoutRoot.FindAnyWidget("RedditBtn");
        m_Facebook               = layoutRoot.FindAnyWidget("FacebookBtn");

        m_TopShader              = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_BottomShader           = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));

        // Example color scheme usage
        m_MenuDivider.SetColor(colorScheme.Separator());
        m_TopShader.SetColor(colorScheme.TopShader());
        m_BottomShader.SetColor(colorScheme.BottomShader());

        HudShow(false);

        // -----------------------------------------------------------
        // *** Buttons Setup with proBtn ***
        // -----------------------------------------------------------

        // 1) Without callback or URL
        //    (If you just want the button to do nothing but hover-color)
        cuiElmnt.proBtn(m_ContinueButton, "Continue", colorScheme.PrimaryText(), colorScheme.ButtonHover());
        cuiElmnt.proBtn(m_OptionsButton,  "Options",  colorScheme.PrimaryText(), colorScheme.ButtonHover());
        cuiElmnt.proBtn(m_RestartButton,  "Restart",  colorScheme.PrimaryText(), colorScheme.ButtonHover());
        cuiElmnt.proBtn(m_RestartDeadRandomButton, "Random Respawn", colorScheme.PrimaryText(), colorScheme.ButtonHover());
        cuiElmnt.proBtn(m_RestartDeadCustomButton, "Custom Respawn", colorScheme.PrimaryText(), colorScheme.ButtonHover());

        // -----------------------------------------------------------
        // 2) The m_ExitButton gets a callback
        //    (Remove or comment out the redundant call if you had it before!)
        //
        // ORIGINAL (redundant) call—COMMENTED OUT:
        // cuiElmnt.proBtn(m_ExitButton, "Exit", colorScheme.PrimaryText(), colorScheme.ButtonHover());
        //
        // CORRECT: one call with callback
        cuiElmnt.proBtn(
            m_ExitButton,
            "Exit",
            colorScheme.PrimaryText(),
            colorScheme.ButtonHover(),
            "",          // No URL
            this,        // targetClass => this InGameMenu instance
            "OnClick_Exit",  // callbackMethod => function name below
        );

        // -----------------------------------------------------------
        // 3) Example of a button that opens a URL
        cuiElmnt.proBtn(
            m_PrioQ,
            "Priority Queue",
            colorScheme.PrimaryText(),
            colorScheme.ButtonHover(),
            MenuURLS.urlPriorityQ,  // Non-empty => will do OpenURL(...)
        );

        cuiElmnt.proBtn(
            m_Website,
            "Visit Website",
            colorScheme.PrimaryText(),
            UIColor.Facebook(),    // or any color
            MenuURLS.urlFacebook,
        );

        cuiElmnt.proBtn(
            m_Discord,
            "Discord",
            colorScheme.PrimaryText(),
            UIColor.Discord(),
            MenuURLS.urlDiscord,
        );

        cuiElmnt.proBtn(
            m_Twitter,
            "Twitter",
            colorScheme.PrimaryText(),
            UIColor.Twitter(),
            MenuURLS.urlTwitter,
        );  

        cuiElmnt.proBtn(
            m_Youtube,
            "Youtube",
            colorScheme.PrimaryText(),
            UIColor.YouTube(),
            MenuURLS.urlYoutube,
        );

        cuiElmnt.proBtn(
            m_Reddit,
            "Reddit",
            colorScheme.PrimaryText(),
            UIColor.Reddit(),
            MenuURLS.urlReddit,
        );

        cuiElmnt.proBtn(
            m_Facebook,
            "Facebook",
            colorScheme.PrimaryText(),
            UIColor.Facebook(),
            MenuURLS.urlFacebook,
        );

        return layoutRoot;
    }

    // // -----------------------------------------------------------
    // // Example callback function for the exit button
    // protected void OnClick_Exit()
    // {
    //     Print("OnClick_Exit() called! Logging out now...");
    //     GetGame().LogoutRequestTime();
    //     GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(GetGame().GetMission().CreateLogoutMenu, this);
    // }

    // override void OnShow()
    // {
    //     super.OnShow();
    //     // any additional logic upon showing the menu
    // }

    // -----------------------------------------------------------
    // Example override OnClick (typical DayZ style).
    // This is from your snippet, but keep in mind your
    // CUIButtonHandler also intercepts OnClick. They can coexist,
    // but usually you rely on one system or the other.
    // override bool OnClick(Widget w, int x, int y, int button)
    // {
    //     super.OnClick(w, x, y, button);

    //     if (w == m_ContinueButton)
    //     {
    //         Print("Clicked continue => do something or OnClick_Continue");
    //         return true;
    //     }
    //     else if (w == m_RestartButton)
    //     {
    //         Print("Clicked restart => do OnClick_Restart or something else");
    //         return true;
    //     }
    //     else if (w == m_RespawnButton)
    //     {
    //         Print("Clicked respawn => do OnClick_Respawn");
    //         return true;
    //     }
    //     else if (w == m_OptionsButton)
    //     {
    //         Print("Clicked options => OnClick_Options");
    //         return true;
    //     }
    //     else if (w == m_ExitButton)
    //     {
    //         // We also have the callback above, but you can do it here:
    //         OnClick_Exit();
    //         return true;
    //     }
    //     else
    //     {
    //         // ...
    //     }

    //     return false;
    // }
}
