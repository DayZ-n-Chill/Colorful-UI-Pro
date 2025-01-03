modded class LoadingScreen
{
    protected ImageWidget cui_Background, cui_topShader, cui_bottomShader;
    protected TextWidget m_loadingMsg;   
    protected float m_LastProgressUpdate;

    void LoadingScreen(DayZGame game)
    {
        m_DayZGame = game;
        m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/loading/cui.loading.layout");

        cui_Background = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));
        m_ProgressLoading = ProgressBarWidget.Cast(m_WidgetRoot.FindAnyWidget("LoadingBar"));
        
        Class.CastTo(cui_topShader, m_WidgetRoot.FindAnyWidget("TopShader"));
        Class.CastTo(cui_bottomShader, m_WidgetRoot.FindAnyWidget("BottomShader"));
        Class.CastTo(m_loadingMsg, m_WidgetRoot.FindAnyWidget("LoadingMsg"));

        ProgressAsync.SetProgressData(m_ProgressLoading);
        ProgressAsync.SetUserData(cui_Background);
   
        m_loadingMsg.SetText("GAME IS LOADING!");

        cui_topShader.SetColor(colorScheme.TopShader());
        cui_bottomShader.SetColor(colorScheme.BottomShader());
        m_loadingMsg.SetColor(colorScheme.LoadingMsg());
        m_ProgressLoading.SetColor(colorScheme.Loadingbar());
    }

    override void ShowEx(DayZGame game)
    {
        cuiUtils.HandleHints(game, m_WidgetRoot, NoHints); 
        Show();
    }

    override void Show()
    {
        m_loadingMsg.Show(false);
        cui_topShader.SetColor(UIColor.Black());
        cui_bottomShader.SetColor(UIColor.Black());
        cui_Background.LoadImageFile(0, loadscreens.GetRandomElement());
        cui_Background.LoadMaskTexture("Colorful-UI/GUI/textures/Shared/PixelMask_Grey.edds");
    }
}
