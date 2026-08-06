// CuiBackgroundVideo
// -----------------------------------------------------------------------------
// Reusable full-screen background video. Used by MainMenu / OptionsMenu (any
// menu that wants a looping background video) WITHOUT embedding VideoWidget
// inside the menu's own layoutRoot — that pattern crashes the engine.
//
//   m_BgVideo = new CuiBackgroundVideo("Colorful-UI/GUI/video/CUI_Video.mov");
//   // ...
//   delete m_BgVideo;          // or drop the ref; destructor Stop+Unload+Unlink
//
// Why this exists:
//
// Embedding a VideoWidget inside a UIScriptedMenu's layoutRoot triggers a
// reproducible ACCESS_VIOLATION (`mov rax,[rcx]; call [rax+0x88]` in the
// native video module) when the menu transitions to another menu. WMF
// decodes on a background thread; when EnterScriptedMenu starts tearing
// down the source menu's widget tree, the WMF thread keeps pushing frames
// to the widget while its vtable is being freed on the main thread. No
// script-side hook (OnHide, OnVisibilityChanged, destructor) fires early
// enough or synchronously enough to coordinate the shutdown.
//
// Mirrors CuiBackdrop's pattern: parent the widget at the workspace root
// (no parent arg to CreateWidgets), so the widget's lifetime is owned by
// this instance — not by any menu's layoutRoot. The video widget stays
// alive while we have a ref. When ~CuiBackgroundVideo fires, the widget is
// still allocated (workspace hasn't unlinked it), so Stop + Unload + Unlink
// runs cleanly. The WMF thread can't race because we hold a strong ref
// throughout the menu's lifetime.
//
// Layout (`Colorful-UI/GUI/layouts/components/cui.bgVideo.layout`):
//   - FrameWidget root at priority 1 (low) so any menu drawn above renders on top.
//   - Inner VideoWidget BgVideo, fullscreen (size 1 1), fixaspect outside,
//     ignorepointer 1 (never blocks input).
//
// Vanilla reference: P:\scripts\5_mission\gui\newui\mainmenu\mainmenuvideo.c
// uses Unload via OnVisibilityChanged because vanilla's MainMenuVideo is a
// dedicated UIScriptedMenu — its widget tree dies cleanly on Back(). Our
// pattern achieves the same widget-lifetime isolation with less ceremony.

class CuiBackgroundVideo
{
    // Singleton — the workspace-rooted widget persists across menu transitions
    // so MainMenu → Credits → Options never goes through a black frame between
    // teardown of one menu's video and load of the next. Use Ensure() to start
    // it; nothing destroys it, it lives until game exit.
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

    // Idempotent: returns the singleton, creating it on first call. If a
    // different videoPath is requested later, the existing widget is reused
    // (only the loaded source is swapped). Menus call this in Init and just
    // forget — never null-out the static, never delete.
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
