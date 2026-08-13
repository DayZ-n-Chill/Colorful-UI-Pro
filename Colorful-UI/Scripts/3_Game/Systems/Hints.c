// UiHintPanel / UiHintPanelLoading — CUI hint panels.
// Vanilla source: P:\scripts\3_game\gui\hints\uihintpanel.c

modded class HintPage
{
	private string m_ImageSet;
	private string m_ImageName;

	string GetImageSet()  { return m_ImageSet; }
	string GetImageName() { return m_ImageName; }
}

modded class UiHintPanel extends ScriptedWidgetEventHandler
{
	protected const string m_DataPath = "Colorful-UI/Scripts/Data/hints.json";
	protected string m_RootPath       = "Colorful-UI/GUI/layouts/loading/hints/cui.ingamehints.layout";

	protected static bool s_HintImageHidden;

	protected ButtonWidget m_HideImageBtn;
	protected TextWidget   m_HideImageBtnLabel;

	override protected void BuildLayout(Widget parent_widget)
	{
		super.BuildLayout(parent_widget);
		if (!m_RootFrame) return;

		ImageWidget leftIcon  = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LeftImage"));
		ImageWidget rightIcon = ImageWidget.Cast(m_RootFrame.FindAnyWidget("RightImage"));
		if (leftIcon)  leftIcon.SetColor(colorScheme.PrimaryText());
		if (rightIcon) rightIcon.SetColor(colorScheme.PrimaryText());

		m_HideImageBtn      = ButtonWidget.Cast(m_RootFrame.FindAnyWidget("HideImageBtn"));
		m_HideImageBtnLabel = TextWidget.Cast(m_RootFrame.FindAnyWidget("HideImageBtn_label"));
		if (m_HideImageBtnLabel) m_HideImageBtnLabel.SetColor(colorScheme.PrimaryText());

		ApplyHintImageState();
	}

	void ToggleHintImage()
	{
		s_HintImageHidden = !s_HintImageHidden;
		ApplyHintImageState();
	}

	protected void ApplyHintImageState()
	{
		if (m_UiHintImage) m_UiHintImage.Show(!s_HintImageHidden);

		if (m_HideImageBtnLabel)
		{
			if (s_HintImageHidden) m_HideImageBtnLabel.SetText("Show Image");
			else                   m_HideImageBtnLabel.SetText("Hide Image");
		}
	}

	override protected void SetHintImage()
	{
		if (!m_UiHintImage)
			return;

		HintPage page = m_ContentList.Get(m_PageIndex);
		string image_path;

		if (UseImagesets && page)
		{
			string set_name = page.GetImageSet();
			string img_name = page.GetImageName();
			if (set_name != "" && img_name != "")
				image_path = "set:" + set_name + " image:" + img_name;
		}

		if (image_path == "" && page)
			image_path = page.GetImagePath();

		if (image_path != "")
		{
			m_UiHintImage.Show(true);
			m_UiHintImage.LoadImageFile(0, image_path);
		}
		else
		{
			m_UiHintImage.Show(false);
		}

		if (s_HintImageHidden) m_UiHintImage.Show(false);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (button == MouseState.LEFT && m_HideImageBtn && w == m_HideImageBtn)
		{
			ToggleHintImage();
			return true;
		}
		return super.OnClick(w, x, y, button);
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w == m_HideImageBtn && m_HideImageBtnLabel)
		{
			m_HideImageBtnLabel.SetColor(colorScheme.ButtonHover());
			return true;
		}

		ImageWidget icon;
		if (w == m_UiLeftButton)
			icon = ImageWidget.Cast(w.FindAnyWidget("LeftImage"));
		else if (w == m_UiRightButton)
			icon = ImageWidget.Cast(w.FindAnyWidget("RightImage"));
		if (icon) icon.SetColor(colorScheme.ButtonHover());

		return super.OnMouseEnter(w, x, y);
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w == m_HideImageBtn && m_HideImageBtnLabel)
		{
			m_HideImageBtnLabel.SetColor(colorScheme.PrimaryText());
			return true;
		}

		ImageWidget icon;
		if (w == m_UiLeftButton)
			icon = ImageWidget.Cast(w.FindAnyWidget("LeftImage"));
		else if (w == m_UiRightButton)
			icon = ImageWidget.Cast(w.FindAnyWidget("RightImage"));
		if (icon) icon.SetColor(colorScheme.PrimaryText());

		return super.OnMouseLeave(w, enterW, x, y);
	}
}

modded class UiHintPanelLoading extends UiHintPanel
{
	protected ImageWidget m_TipLineL;
	protected ImageWidget m_TipLineR;
	protected ImageWidget m_TopShader;
	protected ImageWidget m_BottomShader;
	protected ImageWidget m_Icon;
	protected VideoWidget m_Video;

	void ~UiHintPanelLoading()
	{
		if (m_Video)
		{
			m_Video.Stop();
			m_Video.Unload();
		}
	}

	override void Init(DayZGame game)
	{
		m_RootPath = "Colorful-UI/GUI/layouts/loading/hints/cui.loadinghints.layout";

		if (!game)
		{
			return;
		}

		m_Game = game;
		LoadContentList();
		if (m_ContentList)
		{
			BuildLayout(m_ParentWidget);
			RandomizePageIndex();
			PopulateLayout();
			StartSlideshow();
		}
	}

	override protected void BuildLayout(Widget parent_widget)
	{
		m_RootFrame = m_Game.GetWorkspace().CreateWidgets(m_RootPath, parent_widget);

		#ifndef WORKBENCH
			if (LoadVideo) {
				Class.CastTo(m_Video, m_RootFrame.FindAnyWidget("LoadingVid"));
				if (m_Video && m_Video.GetState() == VideoState.NONE)
				{
					if (!FileExist("$saves:" + m_LoadingVideo))
						CopyFile("Colorful-UI/GUI/video/" + m_LoadingVideo, "$saves:" + m_LoadingVideo);
					m_Video.Load("$saves:" + m_LoadingVideo, true);
					m_Video.Play();
				}
			}
		#endif

		if (m_RootFrame)
		{
			m_Icon              = ImageWidget.Cast(m_RootFrame.FindAnyWidget("hintIcon"));
			m_TipLineL          = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LinesImageLeft"));
			m_TipLineR          = ImageWidget.Cast(m_RootFrame.FindAnyWidget("LinesImageRight"));
			m_TopShader         = ImageWidget.Cast(m_RootFrame.FindAnyWidget("TopShader"));
			m_BottomShader      = ImageWidget.Cast(m_RootFrame.FindAnyWidget("BottomShader"));
			m_SpacerFrame       = m_RootFrame.FindAnyWidget("Hints");
			m_UiLeftButton      = ButtonWidget.Cast(m_RootFrame.FindAnyWidget("LeftButton"));
			m_UiRightButton     = ButtonWidget.Cast(m_RootFrame.FindAnyWidget("RightButton"));
			m_UiHeadlineLabel   = TextWidget.Cast(m_RootFrame.FindAnyWidget("HeadlineLabel"));
			m_UiDescLabel       = RichTextWidget.Cast(m_RootFrame.FindAnyWidget("HintDescLabel"));
			m_UiHintImage       = ImageWidget.Cast(m_RootFrame.FindAnyWidget("HintImage"));
			m_UiPageingLabel    = TextWidget.Cast(m_RootFrame.FindAnyWidget("PageInfoLabel"));

			if (m_BottomShader)    m_BottomShader.SetColor(colorScheme.BottomShader());
			if (m_TopShader)       m_TopShader.SetColor(colorScheme.TopShader());
			if (m_Icon)            m_Icon.SetColor(colorScheme.Icons());
			if (m_TipLineL)        m_TipLineL.SetColor(colorScheme.TipLine());
			if (m_TipLineR)        m_TipLineR.SetColor(colorScheme.TipLine());
			if (m_UiHeadlineLabel) m_UiHeadlineLabel.SetColor(colorScheme.TipHeader());
			if (m_UiDescLabel)     m_UiDescLabel.SetColor(colorScheme.TipText());

			m_RootFrame.SetHandler(this);

			if (NoHints && m_SpacerFrame)
				m_SpacerFrame.Show(false);
		}
	}
}
