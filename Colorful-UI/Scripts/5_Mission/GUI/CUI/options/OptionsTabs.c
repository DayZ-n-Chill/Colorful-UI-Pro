modded class OptionsMenuSounds extends ScriptedWidgetEventHandler
{
	override string GetLayoutName()
	{
	return "Colorful-UI/GUI/layouts/options/tabs/cui.sound_tab.layout";

    }
}

modded class OptionsMenuVideo extends ScriptedWidgetEventHandler
{
	override string GetLayoutName()
	{
		return "Colorful-UI/GUI/layouts/options/tabs/cui.video_tab.layout";
	}	
}

modded class OptionsMenuControls extends ScriptedWidgetEventHandler
{
	override string GetLayoutName()
	{
		return "Colorful-UI/GUI/layouts/options/tabs/cui.controls_tab.layout";
	}
}

modded class OptionsMenuGame extends ScriptedWidgetEventHandler
{
	override string GetLayoutName()
	{
		return "Colorful-UI/GUI/layouts/options/tabs/cui.game_tab.layout";
	}
}