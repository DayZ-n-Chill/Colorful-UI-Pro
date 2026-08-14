// CuiBackgroundVideo — looping full-screen background video for menus.

class CuiBackgroundVideo
{
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

    static CuiBackgroundVideo Ensure(string videoPath, bool looping = true)
    {
        if (!s_Instance || !s_Instance.IsValid())
            s_Instance = new CuiBackgroundVideo(videoPath, looping);
        else if (s_Instance.m_LoadedPath != videoPath && s_Instance.m_Video)
        {
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
