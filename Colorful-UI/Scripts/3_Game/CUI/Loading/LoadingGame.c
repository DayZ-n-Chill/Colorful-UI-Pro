// // Referecnes ----------------------------------------------------
// // This modded class is origianlly located in the DayZGame.c file. 
// // It has been moved here for ease of use, & organization
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

// Start at Main Menu ----------------------------------
modded class DayZGame
{
    override void ConnectLaunch() 
    {
        if(StartMainMenu) { MainMenuLaunch(); }
        else { ConnectFromCLI(); };
    };
};
