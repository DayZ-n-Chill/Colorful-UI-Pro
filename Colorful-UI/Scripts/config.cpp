
class CfgPatches
{
	class ColorfulUI_Scripts
	{
        requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Sounds_Effects","ColorfulUI_GUI"};
	};
};

class CfgAddons
{
	class PreloadBanks{};
	class PreloadAddons
	{
		class DayZ
		{
			list[] = {"DZ_Data","DZ_Scripts","DZ_UI","DZ_UIFonts","DZ_Sounds_Effects","ColorfulUI_GUI","ColorfulUI_Scripts"};
		};
	};
};

class CfgMods 
{
	class ColorfulUI
	{
		name = "Colorful-UI";
		dir = "Colorful-UI";
		creditsJson = "Colorful-UI/Scripts/Credits.json";
		inputs = "Colorful-UI/Scripts/Inputs.xml";
		type = "mod";

		dependencies[] =
		{
			"Game", "World", "Mission"
		};
		class defs
		{
			class imageSets			  {files[]= {"Colorful-UI/GUI/imagesets/CUI.imageset","Colorful-UI/GUI/imagesets/Backgrounds.imageset","Colorful-UI/GUI/imagesets/Branding.imageset"};};
			class gameScriptModule 	  {files[]= {"Colorful-UI/Scripts/3_Game"};};
			class worldScriptModule	  {files[]= {"Colorful-UI/Scripts/4_World"};};
			class missionScriptModule {files[]= {"Colorful-UI/Scripts/5_Mission"};};
		};
	};	
};

class CfgSoundSets
{
	class CUI_Music_Menu_SoundSet
	{
		soundShaders[] = {"DZNC_Music_Menu_SoundShader"};
		volumeFactor = 1;
		frequencyFactor = 1;
		spatial = 0;
	};
};	

class CfgSoundShaders
{
	class DZNC_Music_Menu_SoundShader
	{
		// To use multiple songs and override the vanilla main menu music,
		// Just add additional lines following the format above. 
		// Providing a single song will override all time-of-day variations.
		// The music will play in a random order.
		samples[] =
		{
			{"\Colorful-UI\GUI\sounds\MainMenu\Battle", 1},
			{"\Colorful-UI\GUI\sounds\MainMenu\Cello", 1},
			{"\Colorful-UI\GUI\sounds\MainMenu\Cinematic", 1},
			{"\Colorful-UI\GUI\sounds\MainMenu\Emotional_Piano", 1},
			{"\Colorful-UI\GUI\sounds\MainMenu\Piano", 1},
			{"\Colorful-UI\GUI\sounds\MainMenu\Horror_Suspense", 1},
			{"\Colorful-UI\GUI\sounds\MainMenu\Guitar_Reverb", 1}
		};
		volume = 0.8;
	};
};
