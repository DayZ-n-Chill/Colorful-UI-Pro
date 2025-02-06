modded class UiHintPanel extends ScriptedWidgetEventHandler
{	
	protected const string m_DataPath = "Colorful-UI/Scripts/data/hints.json";
	protected string m_RootPath       = "Colorful-UI/Gui/layouts/loading/hints/cui.ingamehints.layout";	
}

modded class UiHintPanelLoading extends UiHintPanel
{
 	protected ImageWidget m_TipLineL;
	protected ImageWidget m_TipLineR;

	override void Init(DayZGame game)
    {
        m_RootPath = "Colorful-UI/gui/layouts/loading/hints/cui.loadinghints.layout";
        
        if (!game)
            return;

        m_Game = game;
        LoadContentList();
        if (m_ContentList)    
        {
            BuildLayout(m_ParentWidget);
            RandomizePageIndex();
            PopulateLayout();
            StartSlideshow();            
        }
        else 
        {
            Print("ERROR: UiHintPanel - Could not create the hint panel. The data are missing!");
        }
    }

	override protected void BuildLayout(Widget parent_widget)
	{
        protected ImageWidget m_TopShader;
	    protected ImageWidget m_BottomShader;
        protected ImageWidget m_Icon;
        VideoWidget m_Video;

		m_RootFrame = m_Game.GetWorkspace().CreateWidgets( m_RootPath, parent_widget );

        #ifdef WORKBENCH
            // Skip the video code entirely.
            // This allows Workbench to open without shitting iteslef.
            // NOTE: We put the video here so that that we dont have to call it multiple times while loading.
        #else
            if (LoadVideo) {
                Class.CastTo(m_Video, m_RootFrame.FindAnyWidget("LoadingVid"));
                CopyFile("Colorful-UI/GUI/video/LoadingVid.mp4", "$saves:LoadingVid.mp4");
                m_Video.Load("$saves:LoadingVid.mp4", true);
                m_Video.Play();
            }
        #endif

		if (m_RootFrame)
		{
            m_Icon              = ImageWidget.Cast(m_RootFrame.FindAnyWidget("hintIcon"));	
		    m_TipLineL          = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LinesImageLeft"));
            m_TipLineR          = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LinesImageRight"));
            m_TopShader         = ImageWidget.Cast(m_RootFrame.FindAnyWidget("TopShader"));
		    m_BottomShader      = ImageWidget.Cast(m_RootFrame.FindAnyWidget("BottomShader"));
            m_SpacerFrame		= m_RootFrame.FindAnyWidget("Hints");	
			m_UiLeftButton		= ButtonWidget.Cast(m_RootFrame.FindAnyWidget("LeftButton"));		
			m_UiRightButton		= ButtonWidget.Cast(m_RootFrame.FindAnyWidget("RightButton"));
			m_UiHeadlineLabel	= TextWidget.Cast(m_RootFrame.FindAnyWidget("HeadlineLabel"));	
			m_UiDescLabel		= RichTextWidget.Cast(m_RootFrame.FindAnyWidget("HintDescLabel"));
			m_UiHintImage		= ImageWidget.Cast(m_RootFrame.FindAnyWidget("HintImage"));
			m_UiPageingLabel	= TextWidget.Cast(m_RootFrame.FindAnyWidget("PageInfoLabel"));
            
		    m_BottomShader.SetColor(colorScheme.BottomShader());
            m_TopShader.SetColor(colorScheme.TopShader());
            m_Icon.SetColor(colorScheme.Icons());
            m_TipLineL.SetColor(colorScheme.TipLine());
            m_TipLineR.SetColor(colorScheme.TipLine());
            m_UiHeadlineLabel.SetColor(colorScheme.TipHeader());
            m_UiDescLabel.SetColor(colorScheme.TipText());
			
            m_RootFrame.SetHandler(this);

            if (NoHints) { m_SpacerFrame.Show(false); }
		}
	}
}
