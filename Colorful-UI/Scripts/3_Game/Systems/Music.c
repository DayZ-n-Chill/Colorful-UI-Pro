// This is really a fix more than a system, but it's here for now.
// Sets the Music Menu SoundSet which is controlled in the config.cpp file
modded class DynamicMusicPlayerRegistry
{
    protected override void RegisterTracksMenu()
	{
		m_TracksMenu = new array<ref DynamicMusicTrackData>();
		RegisterTrackMenu("Music_Menu_SoundSet");
	}
}
