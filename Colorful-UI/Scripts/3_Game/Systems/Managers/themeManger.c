// UI THEME ---------------------------------------------------------------
// Adjust these settings to align with your desired theme and aesthetics.
class colorScheme 
{
	// Base Typography 
	static int PrimaryText()         { return UIColor.White(); }           // Primary text color for main body content
	static int SecondaryText()       { return UIColor.Grey(); }            // Color for secondary text, e.g., subtitles or info
	static int TextHover()           { return UIColor.BrandColor(); }              // Text color for hover effects, creates interactivity

	// Global UI Elements 
	static int ButtonHover()         { return UIColor.BrandColor(); }              // Button color on hover, provides feedback
	static int LogOutTimer()         { return UIColor.BrandColor(); }              // Timer color for logout warnings or countdowns
	static int Separator()           { return UIColor.BrandColor(); }             // Divider color to segment UI sections
	static int TabHover()            { return UIColor.BrandColor(); }              // Tab color on hover, indicates active navigation
	static int Loadingbar()          { return UIColor.BrandColor(); }             // Loading bar color for visual indication

	// Shader Colors 
	static int TopShader()           { return UIColor.CUI2Teal(); }        // Top shader on layouts for added depth
	static int BottomShader()        { return UIColor.CUI2Blue(); }        // Bottom shader on layouts for added depth

	// Loading Screen
	static int TipIcon()             { return UIColor.BrandColor(); }              // Icon color for loading tips, aligns with bra   nd
	static int TipHeader()           { return UIColor.BrandColor(); }              // Header color for tips, separates from body t   ext
	static int TipLine()             { return UIColor.BrandColor(); }              // Divider line color in tips, adds structure   
	static int TipText()             { return UIColor.PrimaryText(); }             // Main text color for tips, ensures readabilit   y
	static int LoadingMsg()          { return UIColor.TertiaryColor(); }           // Color for loading messages, subtle but disti   nct

	// Main Menu
	static int NavIcon()             { return UIColor.BrandColor(); }              // Navigation icon color, promotes brand consistency
	static int SurvivorBox()         { return UIColor.CUI2DarkBlue(); }    // Background color for "Survivor" box, adds separation
	static int StatsBox()            { return UIColor.CUI2DarkBlue(); }    // Background color for "Stats" box, harmonizes with SurvivorBox

	// Options Page 
	static int OptionHeaders()       { return UIColor.TextHover(); }               // Header color in options, aligns with hover effect
	static int OptionInputColors()   { return UIColor.TextHover(); }               // Input fields color, consistent with hover
	static int OptionSliderColors()  { return UIColor.TextHover(); }              // Slider color for a unified interactive feel
	static int OptionCaretColors()   { return UIColor.TextHover(); }               // Caret color for dropdowns, harmonizes with inputs
	static int OptionBGHover()       { return UIColor.TextHover(); }               // Background color for hovered options, reinforces interactivity
}
