modded class InGameMenu extends UIScriptedMenu
{
	// private Widget m_Separator;
	protected ImageWidget m_TopShader, m_BottomShader, m_MenuDivider;
	protected ButtonWidget m_PrioQ, m_Website, m_Discord, m_Twitter, m_Youtube, m_Reddit, m_Facebook;	
	float m_TimerSlice;
	private Widget m_GameOverScreen;
	private ImageWidget m_GameOverScreenImage;
	

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/inGame/cui.ingame.layout");
		m_HintPanel					= new UiHintPanel(layoutRoot.FindAnyWidget( "hint_frame" ));
			
		m_ContinueButton			= layoutRoot.FindAnyWidget( "ContinueBtn" );
		m_ExitButton				= layoutRoot.FindAnyWidget( "ExitBtn" );
		m_RestartButton				= layoutRoot.FindAnyWidget( "RestartBtn" );
		m_RespawnButton 			= layoutRoot.FindAnyWidget( "respawn_button" );
		m_RestartDeadRandomButton	= layoutRoot.FindAnyWidget( "respawn_button_random" );
		m_RestartDeadCustomButton	= layoutRoot.FindAnyWidget( "respawn_button_custom" );
		m_OptionsButton				= layoutRoot.FindAnyWidget( "OptionsBtn" );
		m_PrioQ						= layoutRoot.FindAnyWidget( "QueueBtn" );
		m_Website					= layoutRoot.FindAnyWidget( "WebsiteBtn" );
		m_MenuDivider				= layoutRoot.FindAnyWidget( "MenuDivider" );
		m_Discord 					= layoutRoot.FindAnyWidget( "DiscordBtn" );
		m_Twitter 					= layoutRoot.FindAnyWidget( "TwitterBtn" );
		m_Youtube 					= layoutRoot.FindAnyWidget( "YoutubeBtn" );
		m_Reddit 					= layoutRoot.FindAnyWidget( "RedditBtn" );
		m_Facebook 					= layoutRoot.FindAnyWidget( "FacebookBtn" );

		m_TopShader 			    = layoutRoot.FindAnyWidget( "TopShader" );
		m_BottomShader 				= layoutRoot.FindAnyWidget( "BottomShader" );

		m_MenuDivider.SetColor(colorScheme.Separator());
		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());

		HudShow( false );
		
		cuiElmnt.proBtn(m_ContinueButton, "Continue", colorScheme.PrimaryText(), colorScheme.ButtonHover());
		cuiElmnt.proBtn(m_OptionsButton, "Options", colorScheme.PrimaryText(), colorScheme.ButtonHover());
		cuiElmnt.proBtn(m_ExitButton, "Exit", colorScheme.PrimaryText(), colorScheme.ButtonHover());
		cuiElmnt.proBtn(m_RestartButton, "Restart", colorScheme.PrimaryText(), colorScheme.ButtonHover());
		cuiElmnt.proBtn(m_RestartDeadRandomButton, "Random Respawn", colorScheme.PrimaryText(), colorScheme.ButtonHover());
		cuiElmnt.proBtn(m_RestartDeadCustomButton, "Custom Respawn", colorScheme.PrimaryText(), colorScheme.ButtonHover());
		
		// cuiElmnt.proBtn(
		// 	m_RespawnButton,
		// 	"Respawn",
		// 	colorScheme.PrimaryText(),
		// 	colorScheme.ButtonHover(),
		// 	OnClick_Respawn(),
		// );
		
		cuiElmnt.proBtn(
			m_PrioQ,
			"Priority Queue",
			colorScheme.PrimaryText(),
			colorScheme.ButtonHover(),
			MenuURLS.urlPriorityQ,
		);
	
		cuiElmnt.proBtn(
			m_Website,
			"Visit Website",
			colorScheme.PrimaryText(),
			UIColor.Facebook(),
			MenuURLS.urlFacebook,
		);

		cuiElmnt.proBtn(
			m_Discord,
			"Discord",
			colorScheme.PrimaryText(),
			UIColor.Discord(),
			MenuURLS.urlDiscord,
		);

		cuiElmnt.proBtn(
			m_Twitter,
			"Twitter",
			colorScheme.PrimaryText(),
			UIColor.Twitter(),
			MenuURLS.urlTwitter,
		);	

		cuiElmnt.proBtn(
			m_Youtube,
			"Youtube",
			colorScheme.PrimaryText(),
			UIColor.YouTube(),
			MenuURLS.urlYoutube,
		);

		cuiElmnt.proBtn(
			m_Reddit,
			"Reddit",
			colorScheme.PrimaryText(),
			UIColor.Reddit(),
			MenuURLS.urlReddit,
		);

		cuiElmnt.proBtn(
			m_Facebook,
			"Facebook",
			colorScheme.PrimaryText(),
			UIColor.Facebook(),
			MenuURLS.urlFacebook,
		);

		return layoutRoot;
	}
	
	override void OnShow() {
	    super.OnShow();
	}
}
