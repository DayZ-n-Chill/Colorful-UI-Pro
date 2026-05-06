class colorfulExitDialog extends UIScriptedMenu
{
	protected Widget				m_Separator;
	protected ButtonWidget		 	m_bYes;
	protected ButtonWidget			m_bNo;
	protected Widget 				m_Text;
	protected Widget 				m_Caption;

	private Widget m_TopShader;
	private Widget m_BottomShader;
	
	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.Exit.dialog.layout");
		
		m_Separator					= layoutRoot.FindAnyWidget("SeparatorPanel");
		m_bYes						= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bYes"));
		m_bNo						= ButtonWidget.Cast(layoutRoot.FindAnyWidget("bNo"));
		m_Text						= layoutRoot.FindAnyWidget("Text");
		m_Caption					= layoutRoot.FindAnyWidget("Caption");

		m_TopShader 			    = layoutRoot.FindAnyWidget("TopShader");
		m_BottomShader 			    = layoutRoot.FindAnyWidget("BottomShader");
		
		if (m_Separator) m_Separator.SetColor(colorScheme.Separator());
		if (m_TopShader) m_TopShader.SetColor(colorScheme.TopShader());
		if (m_BottomShader) m_BottomShader.SetColor(colorScheme.BottomShader());
		
		cuiElmnt.proBtnCB(m_bYes, "#message_dialog_yes", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Exit");
		cuiElmnt.proBtnCB(m_bNo, "#message_dialog_no", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Cancel");

		return layoutRoot;
	}

	void Exit()
	{
		Mission m = GetGame().GetMission();
		if (m && m.IsInherited(MissionMainMenu))
		{
			// The elusive NULL pointer: Vanilla DayZ's LoadingScreen tries to set text on 'm_ProgressText'
			// during the RequestExit shutdown sequence. Since Colorful-UI removed this widget, 
			// it throws a VM exception. This exception aborts the script cleanly turning off the video,
			// which cascades into the C++ DirectX Access Violation!
			DayZGame game = DayZGame.Cast(GetGame());
			if (game && game.m_loading && !game.m_loading.m_ProgressText)
			{
				if (game.m_loading.m_WidgetRoot)
				{
					// Bind it to a known TextWidget to safely absorb the .SetText("") call
					game.m_loading.m_ProgressText = TextWidget.Cast(game.m_loading.m_WidgetRoot.FindAnyWidget("LoadingMsg"));
				}
			}

			UIScriptedMenu mainMenu = GetGame().GetUIManager().FindMenu(MENU_MAIN);
			if (mainMenu)
			{
				Widget root = mainMenu.GetLayoutRoot();
				if (root)
				{
					VideoWidget video = VideoWidget.Cast(root.FindAnyWidget("MenuVideo"));
					if (video) 
					{
						video.Stop();
						video.Unlink();
					}
				}
			}
			
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(g_Game.RequestExit, IDC_MAIN_QUIT);
		}
		else
		{
			UIMenuPanel parentMenu = this.GetParentMenu();
			GetGame().GetUIManager().Back();
			GetGame().GetUIManager().EnterScriptedMenu(MENU_LOGOUT, parentMenu);
		}
	}

	void Cancel()
	{
		// Calling .Back() blindly crashes the video rendering thread when the UIManager falls back
		// directly to MENU_MAIN instead of the previously active overlaid menu.
		// Simply hiding/closing THIS dialog leaves the underlying UI layout stack and video running untouched.
		UIMenuPanel parentMenu = this.GetParentMenu();
		if (parentMenu)
			GetGame().GetUIManager().Back();
		else
			this.Close();
	}
};
