// MissionServer — Anti-NVIDIA plugin startup banner.
// Vanilla source: P:\scripts\5_mission\mission\missionserver.c

modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        Print("========================================================================================================================");
        Print("");
        Print("Running Anti-NVIDIA Inspector in MAIN MENU mode (Colorful UI Plugin).");
        Print("");
        Print("========================================================================================================================");
    }
}
