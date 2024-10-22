// UI THEME ---------------------------------------------------------------
// Adjust these settings to align with your desired theme and aesthetics.
class colorScheme 
{
	// Brand Specific (Set your primary & secondary colors)
	static int BrandColor() 		  { return UIColor.Yellow(); };
	static int AccentColor()  		  { return UIColor.CUI2SubtleRed(); };
	static int TertiaryColor()  	  { return ARGB(255, 204, 255, 204); };

	// Base Typography 
	static int PrimaryText()          { return UIColor.White(); }; 	
	static int TextHover()            { return BrandColor(); }; 	
	static int SecondaryText()        { return UIColor.Grey(); }; 

	// Shader Colors 
	static int TopShader()       	  { return UIColor.CUI2Teal(); };
	static int BottomShader()    	  { return UIColor.CUI2Blue(); };

	// Global UI Elements 
	static int ButtonHover()     	  { return BrandColor(); };	
	static int LogOutTimer()     	  { return BrandColor(); };	
	static int Separator()       	  { return AccentColor(); };
	static int TabHover()        	  { return BrandColor(); };
	static int Loadingbar()      	  { return AccentColor(); };

	// Loading Screen
	static int TipIcon()         	  { return BrandColor(); };
	static int TipHeader()       	  { return BrandColor(); };
	static int TipLine()         	  { return BrandColor(); };
	static int TipText()       	 	  { return PrimaryText(); };
	static int LoadingMsg()       	  { return TertiaryColor(); };
	
	// Main Menu
	static int NavIcon()         	  { return BrandColor(); };
	static int SurvivorBox()       	  { return UIColor.CUI2DarkBlue(); };
	static int StatsBox()     	 	  { return UIColor.CUI2DarkBlue(); };
	
	// Options Page 
	static int OptionHeaders()		  { return TextHover(); };
	static int OptionInputColors()    { return TextHover(); };
	static int OptionSliderColors()   { return TextHover(); };
	static int OptionCaretColors()    { return TextHover(); };
	static int OptionBGHover()		  { return TextHover(); };
}
