modded class UiHintPanel extends ScriptedWidgetEventHandler
{	
	protected const string m_DataPath = "Colorful-UI/Scripts/data/hints.json";
	protected string m_RootPath       = "Colorful-UI/Gui/layouts/loading/hints/cui.ingamehints.layout";	
}

modded class UiHintPanelLoading extends UiHintPanel
{
 	protected ImageWidget cui_TipLineL;
	protected ImageWidget cui_TipLineR;

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
        protected ImageWidget cui_TopShader;
	    protected ImageWidget cui_BottomShader;
        protected ImageWidget cui_Icon;
        VideoWidget cui_Video;

		m_RootFrame = m_Game.GetWorkspace().CreateWidgets( m_RootPath, parent_widget );

        #ifdef WORKBENCH
            // Skip the video code entirely.
            // This allows Workbench to open without shitting iteslef.
        #else
            if (LoadVideo) {
                Class.CastTo(cui_Video, m_RootFrame.FindAnyWidget("LoadingVid"));
                CopyFile("Colorful-UI/GUI/video/LoadingVid.mp4", "$saves:LoadingVid.mp4");
                cui_Video.Load("$saves:LoadingVid.mp4", true);
                cui_Video.Play();
            }
        #endif

		if (m_RootFrame)
		{
            cui_Icon            = ImageWidget.Cast(m_RootFrame.FindAnyWidget("hintIcon"));	
		    cui_TipLineL        = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LinesImageLeft"));
            cui_TipLineR        = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LinesImageRight"));
            cui_TopShader       = ImageWidget.Cast(m_RootFrame.FindAnyWidget("TopShader"));
		    cui_BottomShader    = ImageWidget.Cast(m_RootFrame.FindAnyWidget("BottomShader"));
            m_SpacerFrame		= m_RootFrame.FindAnyWidget("Hints");	
			m_UiLeftButton		= ButtonWidget.Cast(m_RootFrame.FindAnyWidget("LeftButton"));		
			m_UiRightButton		= ButtonWidget.Cast(m_RootFrame.FindAnyWidget("RightButton"));
			m_UiHeadlineLabel	= TextWidget.Cast(m_RootFrame.FindAnyWidget("HeadlineLabel"));	
			m_UiDescLabel		= RichTextWidget.Cast(m_RootFrame.FindAnyWidget("HintDescLabel"));
			m_UiHintImage		= ImageWidget.Cast(m_RootFrame.FindAnyWidget("HintImage"));
			m_UiPageingLabel	= TextWidget.Cast(m_RootFrame.FindAnyWidget("PageInfoLabel"));
            
		    cui_BottomShader.SetColor(colorScheme.BottomShader());
            cui_TopShader.SetColor(colorScheme.TopShader());
            cui_Icon.SetColor(colorScheme.Icons());
            cui_TipLineL.SetColor(colorScheme.TipLine());
            cui_TipLineR.SetColor(colorScheme.TipLine());
            m_UiHeadlineLabel.SetColor(colorScheme.TipHeader());
            m_UiDescLabel.SetColor(colorScheme.TipText());
			
            m_RootFrame.SetHandler(this);

            if (NoHints) { m_SpacerFrame.Show(false); }
		}
	}
}
