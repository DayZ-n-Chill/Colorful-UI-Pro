modded class OptionsMenu extends UIScriptedMenu
{
	private Widget m_Separator, m_shader, m_TopShader, m_BottomShader, m_MenuDivider, m_LoadingBar;
	protected VideoWidget m_MenuVid;

	private bool IsMainMenuContext()
	{
		Mission m = GetGame().GetMission();
		return m && m.IsInherited(MissionMainMenu);
	}
		
	override Widget Init()
	{
		m_Options = new GameOptions();
		
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/options/cui.options_menu.layout", null);
		
		layoutRoot.FindAnyWidget("Tabber").GetScript(m_Tabber);
		
		m_Details  = layoutRoot.FindAnyWidget("settings_details");
		m_Version  = TextWidget.Cast(layoutRoot.FindAnyWidget("version"));
		
		m_GameTab     = new OptionsMenuGame(layoutRoot.FindAnyWidget("Tab_0"), m_Details, m_Options, this);
		m_SoundsTab   = new OptionsMenuSounds(layoutRoot.FindAnyWidget("Tab_1"), m_Details, m_Options, this);
		m_VideoTab    = new OptionsMenuVideo(layoutRoot.FindAnyWidget("Tab_2"), m_Details, m_Options, this);
		m_ControlsTab = new OptionsMenuControls(layoutRoot.FindAnyWidget("Tab_3"), m_Details, m_Options, this);

		m_Apply    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("apply"));
		m_Back     = ButtonWidget.Cast(layoutRoot.FindAnyWidget("back"));
		m_Reset    = ButtonWidget.Cast(layoutRoot.FindAnyWidget("reset"));
		m_Defaults = ButtonWidget.Cast(layoutRoot.FindAnyWidget("defaults"));
		
		m_TopShader    = layoutRoot.FindAnyWidget("TopShader");
		m_BottomShader = layoutRoot.FindAnyWidget("BottomShader");
		m_MenuDivider  = layoutRoot.FindAnyWidget("MenuDivider");
		m_LoadingBar   = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("LoadingBar"));
		
		if (m_LoadingBar) m_LoadingBar.SetColor(colorScheme.Loadingbar());
		
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Back), "Back", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Back");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Defaults), "Defaults", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "PerformSetToDefaults");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Apply), "Apply", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Apply");
		cuiElmnt.proBtnCB(this, ButtonWidget.Cast(m_Reset), "Reset", colorScheme.PrimaryText(), colorScheme.ButtonHover(), this, "Reset");
		
		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());
		m_MenuDivider.SetColor(colorScheme.Separator());

		m_ModalLock = false;
		m_CanApplyOrReset = false;
		SetFocus(null);
			
		OnChanged();
		Class.CastTo(m_shader, layoutRoot.FindAnyWidget("Colorful_Shader"));
		m_Separator = layoutRoot.FindAnyWidget("colorful_separator");
		
		#ifdef WORKBENCH
		#else
			// Only show options video when opened from MAIN MENU, never from in-game pause/options.
			if (EnableOptionsVideo && IsMainMenuContext())
			{
				Class.CastTo(m_MenuVid, layoutRoot.FindAnyWidget("MenuVideo"));
				if (m_MenuVid)
				{
					if (!FileExist("$saves:" + m_OptionsMenuVideo))
						CopyFile("Colorful-UI/GUI/video/" + m_OptionsMenuVideo, "$saves:" + m_OptionsMenuVideo);
					m_MenuVid.Load("$saves:" + m_OptionsMenuVideo, true);
					m_MenuVid.Play();
				}
			}
		#endif

		return layoutRoot;
	}

	// // vanilla color helpers (unchanged)
	override void ColorDisable(Widget w)
	{
		SetFocus(null);
		w.SetAlpha(0.5);
		if (w)
		{
			Widget label = w.FindAnyWidget(w.GetName() + "_label");
			if (label && label.IsInherited(TextWidget))
			{
				TextWidget.Cast(label).SetColor(colorScheme.DisabledText());
			}
		}
	}


	// Vanilla OptionsMenu.Reset() and PerformSetToDefaults() revert immediately
	// with no confirmation. Wrap each with a CuiDialog so the user gets a
	// chance to back out — Confirm fires the vanilla revert via super.
	override void Reset()
	{
		CuiDialog.Show(
			"Reset Settings",
			"Revert all settings to their saved values? Any unsaved changes will be lost.",
			true, this, "DoReset", "");
	}

	void DoReset()
	{
		super.Reset();
	}

	override void PerformSetToDefaults()
	{
		CuiDialog.Show(
			"Reset to Defaults",
			"Reset all settings to factory defaults? This will overwrite your saved configuration.",
			true, this, "DoSetToDefaults", "");
	}

	void DoSetToDefaults()
	{
		super.PerformSetToDefaults();
	}

	void ~OptionsMenu()
	{
		cuiElmnt.CleanupForOwner(this);
	}
}
