modded class InGameMenu extends UIScriptedMenu
{
	private Widget m_Separator;

	private Widget cui_Discord;
	private Widget cui_Twitter;
	private Widget cui_Youtube;
	private Widget cui_Reddit;
	private Widget cui_Facebook;
	private Widget m_MenuDivider;
	
	private Widget m_TopShader;
	private Widget m_BottomShader;

	private Widget m_GameOverScreen;
	private ImageWidget m_GameOverScreenImage;
	float m_TimerSlice;

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/menus/inGame/cui.ingame.layout");
			
		m_ContinueButton			= layoutRoot.FindAnyWidget( "ContinueBtn" );
		m_ExitButton				= layoutRoot.FindAnyWidget( "ExitBtn" );
		m_MenuDivider				= layoutRoot.FindAnyWidget( "MenuDivider" );
		m_RestartButton				= layoutRoot.FindAnyWidget( "RestartBtn" );
		m_RespawnButton 			= layoutRoot.FindAnyWidget( "respawn_button" );
		m_RestartDeadRandomButton	= layoutRoot.FindAnyWidget( "respawn_button_random" );
		m_RestartDeadCustomButton	= layoutRoot.FindAnyWidget( "respawn_button_custom" );
		m_OptionsButton				= layoutRoot.FindAnyWidget( "OptionsBtn" );
		

		m_HintPanel					= new UiHintPanel(layoutRoot.FindAnyWidget( "hint_frame" ));
		
		// m_ServerInfoPanel 			= layoutRoot.FindAnyWidget( "server_info" );
		// m_ServerIP 					= TextWidget.Cast( layoutRoot.FindAnyWidget( "server_ip" ) );
		// m_ServerPort 				= TextWidget.Cast( layoutRoot.FindAnyWidget( "server_port" ) );
		// m_ServerName 				= TextWidget.Cast( layoutRoot.FindAnyWidget( "server_name" ) );
		// m_FavoriteImage 			= layoutRoot.FindAnyWidget( "favorite_image" );
		// m_UnfavoriteImage 			= layoutRoot.FindAnyWidget( "unfavorite_image" );
		// m_CopyInfoButton 			= layoutRoot.FindAnyWidget( "copy_button" );
		// m_Separator 				= layoutRoot.FindAnyWidget( "actionItems_separator" );
	
		cui_Discord 				= layoutRoot.FindAnyWidget( "DiscordBtn" );
		cui_Twitter 				= layoutRoot.FindAnyWidget( "TwitterBtn" );
		cui_Youtube 				= layoutRoot.FindAnyWidget( "YoutubeBtn" );
		cui_Reddit 					= layoutRoot.FindAnyWidget( "RedditBtn" );
		cui_Facebook 				= layoutRoot.FindAnyWidget( "FacebookBtn" );

		m_TopShader 			    = layoutRoot.FindAnyWidget( "TopShader" );
		m_BottomShader 			    = layoutRoot.FindAnyWidget( "BottomShader" );

		m_MenuDivider.SetColor(colorScheme.Separator());
		m_TopShader.SetColor(colorScheme.TopShader());
		m_BottomShader.SetColor(colorScheme.BottomShader());

		HudShow( false );


		m_GameOverScreen = Widget.Cast(layoutRoot.FindAnyWidget("GameOverScreen"));
		m_GameOverScreen.SetAlpha(0);
		m_GameOverScreen.Show(false);
		m_GameOverScreenImage = ImageWidget.Cast(m_GameOverScreen.FindAnyWidget("GameOverScreenImage"));
		m_GameOverScreenImage.LoadImageFile(0, GameOverScreens.GetRandomElement());
		m_GameOverScreenImage.SetAlpha(0);

		cuiElmnt.proBtn(cui_Discord, "Discord", colorScheme.PrimaryText(), UIColor.Discord());
		cuiElmnt.proBtn(cui_Twitter, "Twitter", colorScheme.PrimaryText(), UIColor.Twitter());
		cuiElmnt.proBtn(cui_Youtube, "Youtube", colorScheme.PrimaryText(), UIColor.YouTube());
		cuiElmnt.proBtn(cui_Reddit, "Reddit", colorScheme.PrimaryText(), UIColor.Reddit());
		cuiElmnt.proBtn(cui_Facebook, "Facebook", colorScheme.PrimaryText(), UIColor.Facebook());

		return layoutRoot;
	}


	// override void Update(float timeslice)
	// {
	// 	if (ShowDeadScreen)
	// 	{
	// 		m_TimerSlice += timeslice;
	// 		if (m_TimerSlice >= 0.01)
	// 		{
	// 			GameOverMan(timeslice);
	// 			m_TimerSlice = 0;
	// 		}
	// 	}
	// 	super.Update(timeslice);
	// };

	// void GameOverMan(float timeslice)
	// {
	// 	if (m_GameOverScreenImage.GetAlpha() < 1)
	// 	{
	// 		m_GameOverScreen.Show(true);

	// 		float newAlpha = Math.Min(m_GameOverScreen.GetAlpha() + (1.5 * timeslice), 1);
	// 		m_GameOverScreen.SetAlpha(newAlpha);

	// 		if (newAlpha > 0.5)
	// 		{
	// 			m_GameOverScreenImage.SetAlpha(Math.Min(m_GameOverScreenImage.GetAlpha() + (1.25 * timeslice), 1));
	// 		}
	// 	}
	// }

	// override bool OnClick(Widget w, int x, int y, int button)
	// {
	// 	if ( w == m_ContinueButton )
	// 	{
	// 		OnClick_Continue();
	// 		return true;
	// 	}
	// 	else if ( w == m_RestartButton )
	// 	{
	// 		OnClick_Restart();
	// 		return true;
	// 	}
	// 	else if ( w == m_RespawnButton )
	// 	{
	// 		OnClick_Respawn();
	// 		return true;
	// 	}
	// 	else if ( w == m_OptionsButton )
	// 	{
	// 		OnClick_Options();
	// 		return true;
	// 	}
	// 	else if ( w == m_ExitButton )
	// 	{
	// 		OnClick_Exit();
	// 		return true;
	// 	}
	// 	else if (button == MouseState.LEFT && w == cui_Discord)
	// 	{
	// 		GetGame().OpenURL(MenuURLS.urlDiscord);
	// 		return true;
	// 	}
	// 	else if (button == MouseState.LEFT && w == cui_Twitter)
	// 	{
	// 		GetGame().OpenURL(MenuURLS.urlTwitter);
	// 		return false;
	// 	}
	// 	else if (button == MouseState.LEFT && w == cui_Reddit)
	// 	{
	// 		GetGame().OpenURL(MenuURLS.urlReddit);
	// 		return false;
	// 	}
	// 	else if (button == MouseState.LEFT && w == cui_Youtube)
	// 	{
	// 		GetGame().OpenURL(MenuURLS.urlYoutube);
	// 		return false;
	// 	}	
	// 	else if (button == MouseState.LEFT && w == cui_Facebook)
	// 	{
	// 		GetGame().OpenURL(MenuURLS.urlFacebook);
	// 		return false;
	// 	}

	// 	return false;
	// }

	
	override void OnShow() {
	    super.OnShow();
	}
}
