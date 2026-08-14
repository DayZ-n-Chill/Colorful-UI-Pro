// CuiLoc — script-side resolver for the CUI_ stringtable keys.
// Vanilla source: P:\languagecore\stringtable.csv

class CuiLoc
{
	protected static const string CSV_PATH = "Colorful-UI/languagecore/stringtable.csv";
	protected static const string PROBE_KEY = "#main_menu_play";

	static const int COL_ENGLISH     = 0;
	static const int COL_CZECH       = 1;
	static const int COL_GERMAN      = 2;
	static const int COL_RUSSIAN     = 3;
	static const int COL_POLISH      = 4;
	static const int COL_HUNGARIAN   = 5;
	static const int COL_ITALIAN     = 6;
	static const int COL_SPANISH     = 7;
	static const int COL_FRENCH      = 8;
	static const int COL_CHINESE     = 9;
	static const int COL_JAPANESE    = 10;
	static const int COL_PORTUGUESE  = 11;
	static const int COL_CHINESESIMP = 12;

	protected static ref map<string, ref array<string>> s_Table;
	protected static ref map<string, int> s_ProbeMap;
	protected static ref map<string, int> s_NameMap;
	protected static string s_LoggedProbe = "<never>";
	protected static bool s_OptionsDumped;
	protected static int s_LoadedRows;
	protected static int s_LoadedColumns;

	static string Get(string key)
	{
		EnsureLoaded();

		array<string> values;
		if (!s_Table || !s_Table.Find(key, values) || !values || values.Count() == 0)
			return key;

		int col = CurrentLangColumn();
		if (col < 0 || col >= values.Count())
			col = COL_ENGLISH;

		string text = values.Get(col);

		if (text == "" && COL_ENGLISH < values.Count())
			text = values.Get(COL_ENGLISH);

		if (text == "")
			return key;

		return text;
	}

	static int CurrentLangColumn()
	{
		string probeText = Widget.TranslateString(PROBE_KEY);
		probeText = probeText.Trim();

		string how = "engine probe";
		int col = ColumnForProbe(probeText);

		if (col < 0)
		{
			how = "language option item text";
			col = ColumnFromLanguageOption();
		}

		if (col < 0)
			how = "FAILED - defaulted to english";

		if (probeText != s_LoggedProbe || !s_OptionsDumped)
		{
			s_LoggedProbe = probeText;
			LogDiagnostics(probeText, col, how);
		}

		if (col < 0)
			return COL_ENGLISH;

		return col;
	}

	protected static int ColumnForProbe(string probeText)
	{
		if (probeText == "" || probeText == PROBE_KEY || probeText == "main_menu_play")
			return -1;

		EnsureProbeMap();

		int col;
		if (s_ProbeMap.Find(probeText, col))
			return col;

		return -1;
	}

	protected static void EnsureProbeMap()
	{
		if (s_ProbeMap)
			return;

		s_ProbeMap = new map<string, int>();

		s_ProbeMap.Set("PLAY",     COL_ENGLISH);
		s_ProbeMap.Set("HRÁT",     COL_CZECH);
		s_ProbeMap.Set("SPIELEN",  COL_GERMAN);
		s_ProbeMap.Set("ИГРАТЬ",   COL_RUSSIAN);
		s_ProbeMap.Set("GRAJ",     COL_POLISH);
		s_ProbeMap.Set("GIOCA",    COL_ITALIAN);
		s_ProbeMap.Set("JUGAR",    COL_SPANISH);
		s_ProbeMap.Set("JOUER",    COL_FRENCH);
		s_ProbeMap.Set("遊玩",      COL_CHINESE);
		s_ProbeMap.Set("プレイ",     COL_JAPANESE);
		s_ProbeMap.Set("JOGAR",    COL_PORTUGUESE);
		s_ProbeMap.Set("开始游戏",   COL_CHINESESIMP);
	}

	protected static int ColumnFromLanguageOption()
	{
		if (!g_Game)
			return -1;

		GameOptions options = new GameOptions();
		ListOptionsAccess langOption = ListOptionsAccess.Cast(options.GetOptionByType(OptionAccessType.AT_OPTIONS_LANGUAGE));
		if (!langOption)
			return -1;

		int idx = langOption.GetIndex();
		if (idx < 0 || idx >= langOption.GetItemsCount())
			return -1;

		string itemText;
		langOption.GetItemText(idx, itemText);

		return ColumnForName(itemText);
	}

	protected static int ColumnForName(string itemText)
	{
		string text = itemText;
		text = text.Trim();

		if (text.Length() > 0 && text.Substring(0, 1) == "#")
			text = text.Substring(1, text.Length() - 1);

		if (text == "")
			return -1;

		EnsureNameMap();

		int col;
		if (s_NameMap.Find(text, col))
			return col;

		string lowered = text;
		lowered.ToLower();
		if (s_NameMap.Find(lowered, col))
			return col;

		return -1;
	}

	protected static void EnsureNameMap()
	{
		if (s_NameMap)
			return;

		s_NameMap = new map<string, int>();

		AddName("options_language_EN",   COL_ENGLISH);
		AddName("English",               COL_ENGLISH);
		AddName("EN",                    COL_ENGLISH);

		AddName("options_language_CZ",   COL_CZECH);
		AddName("Čeština",               COL_CZECH);
		AddName("Czech",                 COL_CZECH);
		AddName("CZ",                    COL_CZECH);
		AddName("CS",                    COL_CZECH);

		AddName("options_language_DE",   COL_GERMAN);
		AddName("Deutsch",               COL_GERMAN);
		AddName("German",                COL_GERMAN);
		AddName("DE",                    COL_GERMAN);

		AddName("options_language_RU",   COL_RUSSIAN);
		AddName("Русский",               COL_RUSSIAN);
		AddName("Russian",               COL_RUSSIAN);
		AddName("RU",                    COL_RUSSIAN);

		AddName("options_language_PL",   COL_POLISH);
		AddName("Polski",                COL_POLISH);
		AddName("Polish",                COL_POLISH);
		AddName("PL",                    COL_POLISH);

		AddName("options_language_HU",   COL_HUNGARIAN);
		AddName("Magyar",                COL_HUNGARIAN);
		AddName("Hungarian",             COL_HUNGARIAN);
		AddName("HU",                    COL_HUNGARIAN);

		AddName("options_language_IT",   COL_ITALIAN);
		AddName("Italiano",              COL_ITALIAN);
		AddName("Italian",               COL_ITALIAN);
		AddName("IT",                    COL_ITALIAN);

		AddName("options_language_ES",   COL_SPANISH);
		AddName("Español",               COL_SPANISH);
		AddName("Spanish",               COL_SPANISH);
		AddName("ES",                    COL_SPANISH);

		AddName("options_language_FR",   COL_FRENCH);
		AddName("Français",              COL_FRENCH);
		AddName("French",                COL_FRENCH);
		AddName("FR",                    COL_FRENCH);

		AddName("options_language_CHT",  COL_CHINESE);
		AddName("繁體中文",                COL_CHINESE);
		AddName("Chinese",               COL_CHINESE);
		AddName("CHT",                   COL_CHINESE);

		AddName("options_language_JA",   COL_JAPANESE);
		AddName("日本語",                  COL_JAPANESE);
		AddName("Japanese",              COL_JAPANESE);
		AddName("JA",                    COL_JAPANESE);
		AddName("JP",                    COL_JAPANESE);

		AddName("options_language_PTBR", COL_PORTUGUESE);
		AddName("Português do Brasil",   COL_PORTUGUESE);
		AddName("Português",             COL_PORTUGUESE);
		AddName("Portuguese",            COL_PORTUGUESE);
		AddName("PTBR",                  COL_PORTUGUESE);
		AddName("PT",                    COL_PORTUGUESE);

		AddName("options_language_CHS",  COL_CHINESESIMP);
		AddName("简体中文",                COL_CHINESESIMP);
		AddName("CHS",                   COL_CHINESESIMP);
	}

	protected static void AddName(string alias, int col)
	{
		s_NameMap.Set(alias, col);

		string lowered = alias;
		lowered.ToLower();
		s_NameMap.Set(lowered, col);
	}

	protected static void LogDiagnostics(string probeText, int resolvedCol, string how)
	{
		Print("[CuiLoc] ---- language diagnostics ----");
		Print("[CuiLoc] csv rows loaded = " + s_LoadedRows.ToString() + ", language columns per row = " + s_LoadedColumns.ToString());
		Print("[CuiLoc] probe Widget.TranslateString(\"" + PROBE_KEY + "\") = '" + probeText + "'");

		if (g_Game)
		{
			GameOptions options = new GameOptions();
			ListOptionsAccess langOption = ListOptionsAccess.Cast(options.GetOptionByType(OptionAccessType.AT_OPTIONS_LANGUAGE));

			if (langOption)
			{
				s_OptionsDumped = true;

				int idx = langOption.GetIndex();
				int count = langOption.GetItemsCount();

				Print("[CuiLoc] GetCurrentDisplayLanguageIdx() = " + g_Game.GetCurrentDisplayLanguageIdx().ToString());
				Print("[CuiLoc] language option GetIndex() = " + idx.ToString() + ", GetItemsCount() = " + count.ToString() + ", GetDefaultIndex() = " + langOption.GetDefaultIndex().ToString());

				string item;
				for (int i = 0; i < count; i++)
				{
					langOption.GetItemText(i, item);
					Print("[CuiLoc]   item[" + i.ToString() + "] = '" + item + "'");
				}
			}
			else
			{
				Print("[CuiLoc] AT_OPTIONS_LANGUAGE option not reachable (called before options init?)");
			}
		}
		else
		{
			Print("[CuiLoc] g_Game is null");
		}

		Print("[CuiLoc] resolved column = " + resolvedCol.ToString() + " via " + how);
		Print("[CuiLoc] ---- end language diagnostics ----");
	}

	protected static void EnsureLoaded()
	{
		if (s_Table)
			return;

		s_Table = new map<string, ref array<string>>();

		if (!FileExist(CSV_PATH))
		{
			Print("[CuiLoc] stringtable not found at " + CSV_PATH);
			return;
		}

		FileHandle f = OpenFile(CSV_PATH, FileMode.READ);
		if (f == 0)
		{
			Print("[CuiLoc] could not open " + CSV_PATH);
			return;
		}

		string line;
		bool firstLine = true;
		while (FGets(f, line) >= 0)
		{
			if (firstLine)
			{
				firstLine = false;
				continue;
			}

			if (line == "" || line == "\r")
				continue;

			array<string> fields = ParseCsvLine(line);
			if (fields.Count() < 3)
				continue;

			string key = fields.Get(0);
			array<string> values = new array<string>();
			for (int i = 2; i < fields.Count(); i++)
				values.Insert(fields.Get(i));

			s_Table.Set(key, values);

			s_LoadedRows++;
			if (values.Count() > s_LoadedColumns)
				s_LoadedColumns = values.Count();
		}

		CloseFile(f);

		Print("[CuiLoc] loaded " + s_LoadedRows.ToString() + " keys from " + CSV_PATH + " (" + s_LoadedColumns.ToString() + " language columns)");
	}

	protected static array<string> ParseCsvLine(string rawLine)
	{
		string line = rawLine;
		line.Replace("\r", "");

		if (line.Length() > 0 && line.Substring(line.Length() - 1, 1) == ",")
			line = line.Substring(0, line.Length() - 1);

		if (line.Length() >= 2 && line.Substring(0, 1) == "\"" && line.Substring(line.Length() - 1, 1) == "\"")
			line = line.Substring(1, line.Length() - 2);

		array<string> fields = new array<string>();
		int start = 0;
		int idx = line.IndexOfFrom(start, "\",\"");
		while (idx != -1)
		{
			fields.Insert(line.Substring(start, idx - start));
			start = idx + 3;
			idx = line.IndexOfFrom(start, "\",\"");
		}
		fields.Insert(line.Substring(start, line.Length() - start));

		return fields;
	}
}
