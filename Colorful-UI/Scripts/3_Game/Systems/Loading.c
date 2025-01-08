modded class LoadingScreen
{
    protected ImageWidget cui_Background, cui_TopShader, cui_BottomShader, cui_Mask;
    protected TextWidget cui_LoadingMsg;   

    void LoadingScreen(DayZGame game)
    {
        m_DayZGame = game;
        m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/loading/cui.loading.layout");

        Class.CastTo(cui_Background, m_WidgetRoot.FindAnyWidget("ImageBackground"));
        Class.CastTo(cui_TopShader, m_WidgetRoot.FindAnyWidget("TopShader"));
        Class.CastTo(cui_BottomShader, m_WidgetRoot.FindAnyWidget("BottomShader"));
        Class.CastTo(cui_LoadingMsg, m_WidgetRoot.FindAnyWidget("LoadingMsg"));
        Class.CastTo(m_ProgressLoading, m_WidgetRoot.FindAnyWidget("LoadingBar"));

        if (cui_TopShader) cui_TopShader.SetColor(colorScheme.TopShader());
        if (cui_BottomShader) cui_BottomShader.SetColor(colorScheme.BottomShader());
        if (cui_LoadingMsg) cui_LoadingMsg.SetColor(colorScheme.LoadingMsg());
        if (cui_LoadingMsg) cui_LoadingMsg.SetText("GAME IS LOADING!");
        if (m_ProgressLoading) m_ProgressLoading.SetColor(colorScheme.Loadingbar());

        ProgressAsync.SetProgressData(m_ProgressLoading);
        ProgressAsync.SetUserData(cui_Background);
    }

	override void ShowEx(DayZGame game)
	{
		if (!NoHints)
		{
			m_HintPanel	= new UiHintPanelLoading(m_WidgetRoot.FindAnyWidget("hint_frame"));
			m_HintPanel.Init(game);
		}
		Show();
	}

    override void Show()
    {
        if (cui_Background) cui_Background.LoadImageFile(0, loadscreens.GetRandomElement());
    }
}

modded class LoginTimeBase extends LoginScreenBase
{
    protected ImageWidget cui_Background, cui_TopShader, cui_BottomShader, cui_ExitIcon;
    protected TextWidget cui_LoadingMsg, cui_ExitText;
    protected ProgressBarWidget m_ProgressLoading;
    

    override Widget Init()
    {

        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/loading/cui.loggingIn.layout");
        
        cui_Background = ImageWidget.Cast(layoutRoot.FindAnyWidget("ImageBackground"));
        cui_TopShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        cui_BottomShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
        cui_LoadingMsg = TextWidget.Cast(layoutRoot.FindAnyWidget("LoadingMsg"));
        m_ProgressLoading = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));      
        
        m_btnLeave = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
        cui_ExitText = TextWidget.Cast(layoutRoot.FindAnyWidget("ExitText"));
        cui_ExitIcon = ImageWidget.Cast(layoutRoot.FindAnyWidget("Exit"));

        if (cui_Background) cui_Background.LoadImageFile(0, loadscreens.GetRandomElement()); 
        if (cui_TopShader) cui_TopShader.SetColor(colorScheme.TopShader());
        if (cui_BottomShader) cui_BottomShader.SetColor(colorScheme.BottomShader());
        if (cui_LoadingMsg) cui_LoadingMsg.SetColor(colorScheme.LoadingMsg());
        if (m_ProgressLoading) m_ProgressLoading.SetColor(colorScheme.Loadingbar());
        if (cui_ExitIcon) cui_ExitIcon.SetColor(colorScheme.Icons());
        
        return layoutRoot;
    }   
    
    override void SetTime(int time) {
        super.SetTime(time);
        cui_LoadingMsg.SetText("CONNECTING TO SERVER IN " + time.ToString());
    }
 
 	override void Show()
	{
		if (!NoHints)
		{
			layoutRoot.Show(true);
			m_HintPanel	= new UiHintPanelLoading(layoutRoot.FindAnyWidget("hint_frame0"));
		}
	}
    
    // Bohemias Legacy button method,
    // My new method will not work in loadscreen due to the mission not being loaded yet.
    override bool OnMouseEnter(Widget w, int x, int y)
    {
        if (w == m_btnLeave)
        {
            cui_ExitText.SetColor(colorScheme.ButtonHover());
            m_btnLeave.SetColor(UIColor.Transparent());
            return true;
        }
        return false;
    }

    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        if (w == m_btnLeave)
        {
            cui_ExitText.SetColor(colorScheme.PrimaryText());
            return true;
        }
        return false;
    }   
};

// Priority Queue ----------------------------------
modded class LoginQueueBase extends LoginScreenBase
{
    protected ImageWidget cui_TopShader, cui_BottomShader;
    protected ProgressBarWidget m_ProgressLoading;

    override Widget Init()
    {    
        // Use CUI Layout    
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/loading/cui.priorityQueue.layout");

        m_HintPanel = new UiHintPanelLoading(layoutRoot.FindAnyWidget("hint_frame0"));
        m_txtPosition = TextWidget.Cast(layoutRoot.FindAnyWidget("LoadingMsg"));
        m_txtNote = TextWidget.Cast(layoutRoot.FindAnyWidget("txtNote"));
        m_btnLeave = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btnLeave"));
        
        cui_TopShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("TopShader"));
        cui_BottomShader = ImageWidget.Cast(layoutRoot.FindAnyWidget("BottomShader"));
        m_ProgressLoading = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
        
        cui_TopShader.SetColor(colorScheme.TopShader());
        cui_BottomShader.SetColor(colorScheme.BottomShader());
        m_ProgressLoading.SetColor(colorScheme.Loadingbar());
        m_btnLeave.SetColor(colorScheme.ButtonHover());

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

