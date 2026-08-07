
class CfgPatches
{
	class ColorfulUI_Scripts
	{
        requiredVersion = 0.1;
		// The breadth of this list is load-bearing: script compile order
		// follows the addon dependency graph, and a UI mod must compile AFTER
		// other mods so its modded TabberUI.AddTab is the one that builds
		// injected tabs (that's where the tab font comes from). All entries
		// are vanilla game addons — no mod dependencies.
		requiredAddons[] =
		{
			"DZ_Data","DZ_Scripts","DZ_Sounds_Effects","ColorfulUI_GUI",
			"DZ_Characters","DZ_Characters_Headgear","DZ_Characters_Tops","DZ_Characters_Vests",
			"DZ_Characters_Pants","DZ_Characters_Belts","DZ_Characters_Backpacks","DZ_Characters_Heads",
			"DZ_Characters_Zombies","DZ_Gear_Optics","DZ_Gear_Tools","DZ_Radio","DZ_Gear_Food",
			"DZ_Gear_Medical","DZ_Gear_Containers","DZ_Gear_Consumables","DZ_Gear_Cooking",
			"DZ_Gear_Navigation","DZ_Gear_Drinks","DZ_Gear_Camping","DZ_Gear_Crafting",
			"DZ_Animals","DZ_AI","DZ_Weapons_Melee","DZ_Weapons_Projectiles","DZ_Weapons_Magazines",
			"DZ_Weapons_Firearms","DZ_Weapons_Explosives","DZ_Weapons_Ammunition","DZ_Weapons_Shotguns",
			"DZ_Weapons_Archery","DZ_Weapons_Optics","DZ_Weapons_Supports","DZ_Weapons_Muzzles",
			"DZ_Weapons_Attachments_Data","DZ_Weapons_Archery_Crossbow","DZ_Pistols",
			"DZ_Structures","DZ_Vehicles_Parts","DZ_Vehicles_Wheeled","DZ_Sounds_Weapons"
		};
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
