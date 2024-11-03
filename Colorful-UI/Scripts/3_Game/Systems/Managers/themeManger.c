// UI THEME ---------------------------------------------------------------
// Adjust these settings to align with your desired theme and aesthetics.
class colorScheme 
{
	// Brand Specific 
	static int BrandColor()           { return ARGB(255, 255, 204, 102); }   
	static int AccentColor()          { return ARGB(255, 100, 35, 35); }  
	static int TertiaryColor()        { return ARGB(255, 204, 255, 204); }

	// Base Typography 
	static int PrimaryText()         { return UIColor.White(); }           // Primary text color for main body content
	static int SecondaryText()       { return UIColor.Grey(); }            // Color for secondary text, e.g., subtitles or info
	static int TextHover()           { return PrimaryText(); }              // Text color for hover effects, creates interactivity

	// Global UI Elements 
	static int ButtonHover()         { return BrandColor(); }              // Button color on hover, provides feedback
	static int LogOutTimer()         { return BrandColor(); }              // Timer color for logout warnings or countdowns
	static int Separator()           { return BrandColor(); }             // Divider color to segment UI sections
	static int TabHover()            { return BrandColor(); }              // Tab color on hover, indicates active navigation
	static int Loadingbar()          { return BrandColor(); }             // Loading bar color for visual indication

	// Shader Colors 
	static int TopShader()           { return UIColor.cuiTeal(); }        // Top shader on layouts for added depth
	static int BottomShader()        { return UIColor.cuiBlue(); }        // Bottom shader on layouts for added depth

	// Loading Screen
	static int TipText()             { return PrimaryText(); }             // Main text color for tips, ensures readabilit   y
	static int LoadingMsg()          { return TertiaryColor(); }           // Color for loading messages, subtle but disti   nct
	static int TipIcon()             { return BrandColor(); }              // Icon color for loading tips, aligns with bra   nd
	static int TipHeader()           { return BrandColor(); }              // Header color for tips, separates from body t   ext
	static int TipLine()             { return BrandColor(); }              // Divider line color in tips, adds structure   

	// Main Menu
	static int NavIcon()             { return BrandColor(); }              // Navigation icon color, promotes brand consistency
	static int SurvivorBox()         { return UIColor.cuiDarkBlue(); }       // Background color for "Survivor" box, adds separation
	static int StatsBox()            { return UIColor.cuiDarkBlue(); }       // Background color for "Stats" box, harmonizes with SurvivorBox

	// Options Page 
	static int OptionHeaders()       { return BrandColor(); }               // Header color in options, aligns with hover effect
	static int OptionInputColors()   { return BrandColor(); }               // Input fields color, consistent with hover
	static int OptionSliderColors()  { return BrandColor(); }              // Slider color for a unified interactive feel
	static int OptionCaretColors()   { return BrandColor(); }               // Caret color for dropdowns, harmonizes with inputs
	static int OptionBGHover()       { return BrandColor(); }               // Background color for hovered options, reinforces interactivity
}
