// // Referecnes ----------------------------------------------------
// // This modded class is origianlly located in the DayZGame.c file. 
// // It has been moved here for ease of use, & organization
// modded class LoadingScreen
// {
//     protected ImageWidget m_Background;
//     protected ImageWidget m_tShader;
//     protected ImageWidget m_bShader;
//     protected ImageWidget m_tipIcon;
//     protected TextWidget m_loadingMsg;

//     void LoadingScreen(DayZGame game)
//     {
//         m_DayZGame = game;
//         // Use the CUI 2.0 custom layout 
//         m_WidgetRoot = game.GetLoadingWorkspace().CreateWidgets("Colorful-UI/gui/layouts/cui.loading.layout");
        
//         // CUI 2.0 Elements
//         Class.CastTo(m_tShader, m_WidgetRoot.FindAnyWidget("TopShader"));
//         Class.CastTo(m_bShader, m_WidgetRoot.FindAnyWidget("BottomShader"));
//         Class.CastTo(m_loadingMsg, m_WidgetRoot.FindAnyWidget("LoadingMsg"));
        
//         // Load BG and Progress Bar
//         m_Background = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));
//         m_ProgressLoading = ProgressBarWidget.Cast(m_WidgetRoot.FindAnyWidget("LoadingBar"));
//         ProgressAsync.SetProgressData(m_ProgressLoading);
//         ProgressAsync.SetUserData(m_Background);
//         m_Background.Show(true);
        
//         // Theme the elements. 
//         m_ProgressLoading.SetColor(colorScheme.Loadingbar());
//         m_tShader.SetColor(colorScheme.TopShader());
//         m_bShader.SetColor(colorScheme.BottomShader());
//         m_loadingMsg.SetColor(colorScheme.LoadingMsg());
        
//         // I plan to make this timed and show some silly loading comments from the community.
//         m_loadingMsg.SetText("GAME IS LOADING!");
//     }

//     override void SetTitle(string title)
//     {
//         // This function is intentionally left blank because it is no longer used.
//     }

//     override void Show()
//     {
//         // Use Random Background Images
//         m_Background = ImageWidget.Cast(m_WidgetRoot.FindAnyWidget("ImageBackground"));        
//         m_Background.LoadImageFile(0, GetRandomBackground()); 
//     }
// };
