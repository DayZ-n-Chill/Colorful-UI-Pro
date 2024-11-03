// COLOR PALETTES ---------------------------------------------------------
// Use these coloring functions to mix and match the theme of your choice.
class UIColor
{
	// Note: ------------------------------------------------------------------
	// Many of these colors are included for convenience and may not be necessary 
	// for your current use. Please feel free to omit any that you don't need.

	// Brand Specific 
	static int BrandColor()           { return ARGB(255, 255, 204, 102); }   
	static int AccentColor()          { return ARGB(255, 100, 35, 35); }  
	static int TertiaryColor()        { return ARGB(255, 204, 255, 204); }

	// GreyScale
	static int White()                { return ARGB(255, 255, 255, 255); };
	static int Grey()                 { return ARGB(255, 130, 130, 130); };
	static int Black()                { return ARGB(255, 0, 0, 0); };
	static int Transparent()          { return ARGB(0, 0, 0, 0); };
	static int SemiTransparentWhite() { return ARGB(120, 255, 255, 255); };
	
	// R.O.Y.G.B.I.V
	static int Red()    			  { return ARGB(255, 173, 35, 35); };
	static int Orange() 			  { return ARGB(255, 215, 153, 19); };
	static int Yellow() 			  { return ARGB(255, 255, 204, 102); };
	static int Green()  			  { return ARGB(255, 51, 204, 51); };
	static int Blue()   			  { return ARGB(255, 51, 102, 102); };
	static int Indigo() 			  { return ARGB(255, 102, 51, 204); };
	static int Violet() 			  { return ARGB(255, 204, 51, 204); };
	
	// Social Network Colors 
	static int facebook()              { return ARGB(255, 59, 89, 152); };
	static int instagram()             { return ARGB(255, 193, 42, 163); };
	static int linkedin()              { return ARGB(255, 0, 119, 181); };
	static int pinterest()             { return ARGB(255, 189, 8, 28); };
	static int snapchat()              { return ARGB(255, 255, 252, 0); };
	static int tiktok()                { return ARGB(255, 1, 1, 1); };
	static int whatsapp()              { return ARGB(255, 37, 211, 102); };
	static int tumblr()                { return ARGB(255, 54, 70, 93); };
	static int flickr()                { return ARGB(255, 244, 0, 131); };
	static int vimeo()                 { return ARGB(255, 26, 183, 234); };
	static int skype()                 { return ARGB(255, 0, 175, 240); };
	static int slack()                 { return ARGB(255, 74, 21, 75); };
	static int medium()                { return ARGB(255, 2, 184, 117); };
	static int quora()                 { return ARGB(255, 170, 34, 0); };
	static int dribbble()              { return ARGB(255, 234, 76, 137); };
	static int soundcloud()            { return ARGB(255, 255, 85, 0); };
	static int spotify()               { return ARGB(255, 30, 215, 96); };
	static int twitch()                { return ARGB(255, 145, 70, 255); };
	static int telegram()              { return ARGB(255, 0, 136, 204); };
	static int mastodon()              { return ARGB(255, 99, 100, 255); };

	// Custom Colors 
	// Use to add your custom colors and use them where you want.
	static int cuiTeal()             { return ARGB(255, 102, 153, 153); };
	static int cuiBlue()             { return ARGB(255, 1, 5, 74); };
	static int cuiDarkBlue()         { return ARGB(155, 0, 0, 32); };
	static int cuiSubtleRed()        { return ARGB(255, 100, 35, 35); };
	static int cuiBrightRed()        { return ARGB(255, 152, 0, 0); };
}
