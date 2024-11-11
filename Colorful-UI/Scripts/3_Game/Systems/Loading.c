modded class LoadingScreen
{
    protected ImageWidget cui_Background, cui_topShader, cui_bottomShader;
    protected TextWidget m_loadingMsg;   

    void LoadingScreen(DayZGame game)
    {
        m_DayZGame = game;
        m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("Colorful-UI/gui/layouts/cui.loading.layout");

        cui_Background = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));
        m_ProgressLoading = ProgressBarWidget.Cast(m_WidgetRoot.FindAnyWidget("LoadingBar"));
        
        Class.CastTo(cui_topShader, m_WidgetRoot.FindAnyWidget("TopShader"));
        Class.CastTo(cui_bottomShader, m_WidgetRoot.FindAnyWidget("BottomShader"));
        Class.CastTo(m_loadingMsg, m_WidgetRoot.FindAnyWidget("LoadingMsg"));

        ProgressAsync.SetProgressData(m_ProgressLoading);
        ProgressAsync.SetUserData(cui_Background);
   
        cui_Background.Show(true);
        m_loadingMsg.SetText("GAME IS LOADING!");

        cui_topShader.SetColor(colorScheme.TopShader());
        cui_bottomShader.SetColor(colorScheme.BottomShader());
        m_loadingMsg.SetColor(colorScheme.LoadingMsg());
        m_ProgressLoading.SetColor(colorScheme.Loadingbar());
    }

    void NoHints()
    {
        m_WidgetRoot.FindAnyWidget("hint_frame").Show(false);
    }

    override void ShowEx(DayZGame game)
    {
        if (!NoHints)
        {
            m_HintPanel = new UiHintPanelLoading(m_WidgetRoot.FindAnyWidget("hint_frame"));
            m_HintPanel.Init(game);
        } else {
            NoHints();
        }
        Show();
    }

    override void Show()
    {

        string randomBackground = SetRandomBackground();
        
        cui_Background = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));        
        cui_Background.LoadImageFile(0, randomBackground);
    }
}

// Logging In Screen ------------------------------
modded class LoginTimeBase extends LoginScreenBase
{
    protected ImageWidget m_LSBackground;
    protected ImageWidget m_tShader;
    protected ImageWidget m_bShader;
    protected ImageWidget m_tipIcon;
    protected TextWidget m_loadingMsg;
    protected ImageWidget m_exitIcon;
    protected TextWidget m_ExitText;
    protected ProgressBarWidget m_ProgressLoading;

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/cui.dialog_login_time.layout");
        
        m_LSBackground = ImageWidget.Cast(layoutRoot.FindAnyWidget("Background"));
        m_LSBackground = ImageWidget.Cast(layoutRoot.FindAnyWidget("ImageBackground"));
        
        m_exitIcon = ImageWidget.Cast(layoutRoot.FindAnyWidget("Exit"));
        
        m_tShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_bShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));

        m_loadingMsg = TextWidget.Cast(layoutRoot.FindAnyWidget("LoadingMsg"));
        m_ProgressLoading = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
        m_ExitText = TextWidget.Cast(layoutRoot.FindAnyWidget("ExitText"));
        
        m_txtLabel = TextWidget.Cast(layoutRoot.FindAnyWidget("txtLabel"));
        m_btnLeave = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
        m_txtDescription.Show(true);
        layoutRoot.FindAnyWidget("notification_root").Show(false);     
        // Theme the elements. 
        m_LSBackground.LoadImageFile(0, SetRandomBackground()); 
        m_ProgressLoading.SetColor(colorScheme.Loadingbar());
        m_tShader.SetColor(colorScheme.TopShader());
        m_bShader.SetColor(colorScheme.BottomShader());
        m_exitIcon.SetColor(colorScheme.TipIcon());
        m_txtLabel.SetColor(colorScheme.LoadingMsg());
        return layoutRoot;
    }   
    
    override bool OnMouseEnter(Widget w, int x, int y)
    {
        if (w == m_btnLeave)
        {
            m_ExitText.SetColor(colorScheme.TipIcon());
            m_btnLeave.SetColor(UIColor.Transparent());
            return true;
        }
        return false;
    }
    
    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        if (w == m_btnLeave)
        {
            m_ExitText.SetColor(colorScheme.PrimaryText());
            return true;
        }
        return false;
    }   
    
    // Change the wording in the countdown timer text
    override void SetTime(int time)
    {
        string text = "";
        TimeConversions.ConvertSecondsToFullTime(time, m_FullTime);
        if (!m_IsRespawn)
            text = "#menu_loading_in_";
        else
            text = "#dayz_game_spawning_in_";

        if (m_FullTime.m_Days > 0)
            text += "dhms";
        else if (m_FullTime.m_Hours > 0)
            text += "hms";
        else if (m_FullTime.m_Minutes > 0)
            text += "ms";
        else
            text += "s";
            
        text = Widget.TranslateString(text);
        text = string.Format(text, m_FullTime.m_Seconds, m_FullTime.m_Minutes, m_FullTime.m_Hours, m_FullTime.m_Days);
        m_txtLabel.SetText(text);
        
        if (m_IsRespawn && time <= 1)
            GetGame().SetLoginTimerFinished();
        // Change this text in quotes to whatever you want.
        m_txtLabel.SetText("CONNECTING TO SERVER IN " + time.ToString());
    }   
};

// Priority Queue ----------------------------------
modded class LoginQueueBase extends LoginScreenBase
{
    protected ImageWidget m_tShader;
    protected ImageWidget m_bShader;
    protected ProgressBarWidget m_ProgressLoading;

    override Widget Init()
    {    
        // Use CUI Layout    
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/cui.dialog_queue_position.layout");
        // Vanilla Elements
        m_HintPanel = new UiHintPanelLoading(layoutRoot.FindAnyWidget("hint_frame0"));
        m_txtPosition = TextWidget.Cast(layoutRoot.FindAnyWidget("LoadingMsg"));
        m_txtNote = TextWidget.Cast(layoutRoot.FindAnyWidget("txtNote"));
        m_btnLeave = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
        
        m_tShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        m_bShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
        m_ProgressLoading = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
        
        m_btnLeave.SetColor(colorScheme.TipIcon());
        m_tShader.SetColor(colorScheme.TopShader());
        m_bShader.SetColor(colorScheme.BottomShader());
        m_ProgressLoading.SetColor(colorScheme.Loadingbar());

        return layoutRoot;
    }

    override void SetPosition(int position)
    {
        if (position != m_iPosition)
        {
            m_iPosition = position;
            m_txtPosition.SetText("Position in Queue " + position.ToString());
            m_txtPosition.SetColor(colorScheme.LoadingMsg());
        }
    }
};

// Start at Main Menu ----------------------------------
modded class DayZGame
{
    override void ConnectLaunch() 
    {
        if(StartMainMenu) { MainMenuLaunch(); }
        else { ConnectFromCLI(); };
    };
};
