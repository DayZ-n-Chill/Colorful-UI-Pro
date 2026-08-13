// DynamicMusicPlayerRegistry — replaces the vanilla menu music list.
// Vanilla source: P:\scripts\3_game\systems\dynamicmusicplayer\dynamicmusicplayerregistry.c

modded class DynamicMusicPlayerRegistry
{
    protected override void RegisterTracksMenu()
	{
		m_TracksMenu = new array<ref DynamicMusicTrackData>();

		RegisterTrackMenu("CUI_Music_Menu_SoundSet", true);
	}
}
