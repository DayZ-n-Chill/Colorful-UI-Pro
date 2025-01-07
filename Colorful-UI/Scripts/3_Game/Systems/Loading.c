modded class LoadingScreen
{
    protected ImageWidget cui_Background, cui_topShader, cui_bottomShader, cui_FadeOut;
    protected TextWidget m_loadingMsg;   
    protected float m_LastProgressUpdate;

    void LoadingScreen(DayZGame game)
    {
        m_DayZGame = game;
        m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/loading/cui.loading.layout");

        // Initialize widgets
        Class.CastTo(cui_Background, m_WidgetRoot.FindAnyWidget("ImageBackground"));
        Class.CastTo(cui_FadeOut, m_WidgetRoot.FindAnyWidget("FadeOutWidgetName")); // Replace with actual name
        Class.CastTo(cui_topShader, m_WidgetRoot.FindAnyWidget("TopShader"));
        Class.CastTo(cui_bottomShader, m_WidgetRoot.FindAnyWidget("BottomShader"));
        Class.CastTo(m_loadingMsg, m_WidgetRoot.FindAnyWidget("LoadingMsg"));
        Class.CastTo(m_ProgressLoading, m_WidgetRoot.FindAnyWidget("LoadingBar"));

        // Set ProgressAsync
        ProgressAsync.SetProgressData(m_ProgressLoading);
        ProgressAsync.SetUserData(cui_Background);

        // Set text and colors
        if (m_loadingMsg) m_loadingMsg.SetText("GAME IS LOADING!");
        if (cui_topShader) cui_topShader.SetColor(colorScheme.TopShader());
        if (cui_bottomShader) cui_bottomShader.SetColor(colorScheme.BottomShader());
        if (m_loadingMsg) m_loadingMsg.SetColor(colorScheme.LoadingMsg());
        if (m_ProgressLoading) m_ProgressLoading.SetColor(colorScheme.Loadingbar());
    }

    override void ShowEx(DayZGame game)
    {
        cuiUtils.HandleHints(game, m_WidgetRoot, NoHints); 
        Show();
    }

    override void Show()
    {
        if (cui_Background && loadscreens)
        { cui_Background.LoadImageFile(0, loadscreens.GetRandomElement()); }
        else
        { Print("Error: cui_Background or loadscreens is NULL!"); }
        if (cui_FadeOut)
        { cui_FadeOut.LoadMaskTexture("Colorful-UI/GUI/textures/Shared/PixelMask_Grey.edds"); }
        else
        { Print("Error: cui_FadeOut is NULL!"); }
    }
}
