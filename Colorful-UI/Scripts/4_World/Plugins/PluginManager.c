modded class PluginManager
{
	override void Init()
	{
		super.Init();

		// Force register the bugged vanilla debug plugin on Diag builds
		// so that PluginItemDiagnostic stops throwing "is not Registred" errors.
		#ifdef DIAG_DEVELOPER
		RegisterPlugin("PluginConfigDebugProfile", true, true, true);
		#endif
	}
}
