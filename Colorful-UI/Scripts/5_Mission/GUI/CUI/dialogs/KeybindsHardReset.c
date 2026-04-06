class colorfulKeybindsHardResetDialog extends UIScriptedMenu
{
	private Widget		m_Shader;
	private ButtonWidget m_bYes;
	private ButtonWidget m_bNo;
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/gui/layouts/dialogs/cui.keybinds_hard.dialog.layout");
		m_bYes = ButtonWidget.Cast( layoutRoot.FindAnyWidget("bYes") );
		m_bNo  = ButtonWidget.Cast( layoutRoot.FindAnyWidget("bNo") );
		m_Shader = layoutRoot.FindAnyWidget("Colorful_Shader");
		
		cuiElmnt.proBtnCB(m_bYes, "YES", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "ResetDefaults");
		cuiElmnt.proBtnCB(m_bNo,  "NO",  colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Cancel");

		Widget m_Separator = layoutRoot.FindAnyWidget("SeparatorPanel");
		Widget m_TopShader = layoutRoot.FindAnyWidget("TopShader");
		Widget m_BottomShader = layoutRoot.FindAnyWidget("BottomShader");
		
		if (m_Separator) m_Separator.SetColor(colorScheme.Separator());
		if (m_TopShader) m_TopShader.SetColor(colorScheme.TopShader());
		if (m_BottomShader) m_BottomShader.SetColor(colorScheme.BottomShader());

		return layoutRoot;
	}

	override void Update(float timeslice)
	{
		super.Update( timeslice );
		if (GetUApi().GetInputByID(UAUIBack).LocalPress())
		{
			Cancel();
		}
	}

	void ResetDefaults()
	{
		KeybindingsMenu keybindingsMenu = KeybindingsMenu.Cast(GetParentMenu());
		if (keybindingsMenu)
		{
			keybindingsMenu.PerformSetToDefaultsExt(1); // 1 = MODAL_RESULT_DEFAULT_ALL
		}
		GetGame().GetUIManager().Back();
	}
	
	void Cancel()
	{
		GetGame().GetUIManager().Back();
	}
}
