modded class DynamicMusicPlayerRegistry
{
    // CUI fully replaces the vanilla menu-music array. Do NOT add super.RegisterTracksMenu()
    // here — that would re-register vanilla tracks alongside ours. Replacement is intentional.
    protected override void RegisterTracksMenu()
	{
		m_TracksMenu = new array<ref DynamicMusicTrackData>();

		RegisterTrackMenu("CUI_Music_Menu_SoundSet", true);
	}
}