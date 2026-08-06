// CuiBackgroundVideo — looping full-screen background video for menus.
//
//   CuiBackgroundVideo.Ensure("Colorful-UI/GUI/video/CUI_Video.mov");
//
// IMPORTANT: never place a VideoWidget inside a menu's own layout. Video
// decoding runs on its own thread and keeps writing to the widget while the
// engine destroys the menu, which crashes the game when menus change. This
// component keeps the video outside every menu so that cannot happen.
//
// Layout: Colorful-UI/GUI/layouts/components/cui.bgVideo.layout — lowest
// draw priority so menus render over it, and it never blocks clicks.

class CuiBackgroundVideo
{
    // One shared instance that outlives individual menus, so moving between
    // Main Menu, Options and Credits never flashes a black frame. Start it
    // with Ensure(); it runs until the game closes.
    static ref CuiBackgroundVideo s_Instance;

    protected Widget       m_Root;
    protected VideoWidget  m_Video;
    protected string       m_LoadedPath;

    void CuiBackgroundVideo(string videoPath, bool looping = true)
    {
        m_Root = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/components/cui.bgVideo.layout");
        if (!m_Root) return;

        m_Video = VideoWidget.Cast(m_Root.FindAnyWidget("BgVideo"));
        if (m_Video)
        {
            m_Video.Load(videoPath, looping);
            m_Video.Play();
            m_LoadedPath = videoPath;
        }
    }

    void ~CuiBackgroundVideo()
    {
        if (m_Video)
        {
            m_Video.Stop();
            m_Video.Unload();
            m_Video = null;
        }

        if (m_Root)
        {
            m_Root.Unlink();
            m_Root = null;
        }
    }

    // Safe to call every time a menu opens: starts the video on first use,
    // and afterwards only swaps the file if a different one is asked for.
    static CuiBackgroundVideo Ensure(string videoPath, bool looping = true)
    {
        if (!s_Instance || !s_Instance.IsValid())
            s_Instance = new CuiBackgroundVideo(videoPath, looping);
        else if (s_Instance.m_LoadedPath != videoPath && s_Instance.m_Video)
        {
            // Swap source without tearing down the widget.
            s_Instance.m_Video.Stop();
            s_Instance.m_Video.Unload();
            s_Instance.m_Video.Load(videoPath, looping);
            s_Instance.m_Video.Play();
            s_Instance.m_LoadedPath = videoPath;
        }
        return s_Instance;
    }

    Widget       GetRoot()      { return m_Root; }
    VideoWidget  GetVideo()     { return m_Video; }
    bool         IsValid()      { return m_Root != null; }
}
