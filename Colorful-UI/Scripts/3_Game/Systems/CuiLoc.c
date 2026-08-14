// CuiLoc — script-side resolver for the CUI_ stringtable keys.
//
// Why this exists: Colorful-UI/languagecore/stringtable.csv resolves correctly at game boot
// (the engine's boot-time PBO scan merges it into the string registry for whatever language
// is active then), but after the player changes language from the in-game Options menu, every
// "#CUI_..." widget text renders as the raw key and never recovers without a restart. Verified
// against vanilla source: P:\scripts\5_mission\gui\newui\options\optionsmenugame.c:251-261 only
// calls mission.ResetGUI() on a language change, and P:\scripts\5_mission\mission\missiongameplay.c:
// 1199-1203 shows ResetGUI() only rebuilds the inventory widget tree. There is no script-level
// stringtable reload API anywhere in P:\scripts — whatever native pass re-resolves "#KEY" widgets
// after a switch pulls from a source set that does not include mod-provided stringtables. Re-calling
// Widget.TranslateString()/SetText("#CUI_x") after a switch queries that same broken registry, so
// it cannot fix this either.
//
// Fix: read our own CSV at runtime, resolve CUI_ keys ourselves, and never depend on the engine's
// "#KEY" mechanism for them. Every SetText call site that used to pass a raw "#CUI_..." key should
// call CuiLoc.Get("CUI_...") instead — see MainMenu.c, Loading.c, Hints.c.
//
// ---------------------------------------------------------------------------------------------
// HOW THE CURRENT LANGUAGE IS DETERMINED
//
// The first version of this file assumed DayZGame.GetCurrentDisplayLanguageIdx()
// (P:\scripts\3_game\dayzgame.c:3826-3838) returned an index that lined up 1:1 with this CSV's
// column order. It does not, and it cannot:
//
//   * That function returns ListOptionsAccess.GetIndex() for AT_OPTIONS_LANGUAGE — an index into
//     the engine's *selectable language list*, whose ordering is never exposed to script.
//   * That list is shorter than this CSV. Vanilla's own P:\languagecore\stringtable.csv carries
//     13 language columns but only 12 "options_language_XX" name rows (DE, FR, IT, EN, RU, ES,
//     CZ, CHT, PL, CHS, JA, PTBR). There is no options_language_HU, so Hungarian is a translation
//     column with no matching entry in the option list. 12 items can never index 13 columns.
//   * A fresh "new GameOptions()" is not guaranteed to have been seeded this early either
//     (P:\scripts\3_game\dayzgame.c:1105-1106 calls Initialize() once during DeferredInit), so the
//     index can legitimately be 0 or -1 before the game has finished starting.
//
// So we stop guessing at indices. Two mechanisms, tried in order:
//
//   1. ENGINE PROBE (primary). Ask the engine to translate a well-known vanilla key and see which
//      language comes back. This is a direct measurement of the language the engine is actually
//      rendering in right now, so it is immune to option-list ordering, to a stale option index,
//      and to the boot-order problem above. Vanilla keys still resolve correctly after a language
//      switch — that is the whole premise of this bug — so the probe stays accurate exactly when
//      we need it. "#main_menu_play" (P:\languagecore\stringtable.csv:516) is used because its
//      value is distinct in every selectable language.
//
//   2. OPTION ITEM TEXT (fallback). ListOptionsAccess.GetItemText() is the only language *identity*
//      exposed to script anywhere in P:\scripts — there is no language code or name getter on CGame,
//      no profile option, nothing in 1_core/2_gamelib. We match whatever it hands back (the
//      "options_language_XX" key form, the native name form such as "Deutsch" / "日本語", or a bare
//      two-letter code) against an explicit alias table.
//
// If both fail we fall back to English and log why. The "[CuiLoc]" diagnostic block dumps the probe
// result, the raw option index and every GetItemText() value so the mapping can be fixed from ground
// truth in the client script log instead of by guesswork. It is emitted once per distinct probe
// result, so a language change produces a second block rather than being silent.
class CuiLoc
{
	protected static const string CSV_PATH = "Colorful-UI/languagecore/stringtable.csv";

	// Vanilla key used to fingerprint the engine's current language. Values at
	// P:\languagecore\stringtable.csv:516 — unique across every selectable language.
	protected static const string PROBE_KEY = "#main_menu_play";

	// Column indices into a parsed row, counted from the "english" column: the "Language" (key)
	// column and the "original" column are dropped when the table is parsed, so english is 0.
	static const int COL_ENGLISH     = 0;
	static const int COL_CZECH       = 1;
	static const int COL_GERMAN      = 2;
	static const int COL_RUSSIAN     = 3;
	static const int COL_POLISH      = 4;
	static const int COL_HUNGARIAN   = 5;
	static const int COL_ITALIAN     = 6;
	static const int COL_SPANISH     = 7;
	static const int COL_FRENCH      = 8;
	static const int COL_CHINESE     = 9;	// traditional
	static const int COL_JAPANESE    = 10;
	static const int COL_PORTUGUESE  = 11;
	static const int COL_CHINESESIMP = 12;

	// key -> array of per-language strings, in CSV column order starting at "english".
	protected static ref map<string, ref array<string>> s_Table;

	// engine-translated PROBE_KEY -> column
	protected static ref map<string, int> s_ProbeMap;

	// language option item text (lowercased where ASCII) -> column
	protected static ref map<string, int> s_NameMap;

	// Last probe result we emitted a diagnostic block for. Seeded to a value the engine can never
	// return so the very first resolve always logs, and re-logs on every subsequent language change.
	protected static string s_LoggedProbe = "<never>";

	// Set once the language option has actually been reachable during a diagnostic dump. Until then
	// we keep re-logging, otherwise a first Get() from the boot loading screen (before options are
	// seeded) would burn the only dump on a block that has no item[] list in it.
	protected static bool s_OptionsDumped;

	protected static int s_LoadedRows;
	protected static int s_LoadedColumns;

	// Resolves a bare key (no leading "#") against the current display language.
	// A key that is not in our table is returned unchanged, which means "#some_vanilla_key" passed
	// through here still reaches the widget as "#some_vanilla_key" and gets resolved by the engine
	// as normal. Hints.c relies on that pass-through for hint headline/description keys.
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

		// Some columns can legitimately be blank in a hand-edited CSV; never show an empty label.
		if (text == "" && COL_ENGLISH < values.Count())
			text = values.Get(COL_ENGLISH);

		if (text == "")
			return key;

		return text;
	}

	// Column for the language the engine is rendering in right now, or COL_ENGLISH if unknown.
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

		// Re-emits on every language change, so the log carries a block for the boot language and
		// another for whatever the player switched to. Also keeps retrying while the language option
		// is still out of reach, so we never lose the item[] dump to an early boot-time call.
		if (probeText != s_LoggedProbe || !s_OptionsDumped)
		{
			s_LoggedProbe = probeText;
			LogDiagnostics(probeText, col, how);
		}

		if (col < 0)
			return COL_ENGLISH;

		return col;
	}

	// --- mechanism 1: ask the engine what language it is speaking -------------------------------

	protected static int ColumnForProbe(string probeText)
	{
		// A failed engine lookup echoes the key back, with or without the leading '#'.
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

		// Values of "#main_menu_play" per column, from P:\languagecore\stringtable.csv:516.
		// english and hungarian share "PLAY", but hungarian is not selectable in the options
		// menu (there is no options_language_HU row), so "PLAY" unambiguously means english.
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

	// --- mechanism 2: match the language option's own label -------------------------------------

	protected static int ColumnFromLanguageOption()
	{
		if (!g_Game)
			return -1;

		// GameOptions is a Managed engine object; keep it alive for the whole read rather than
		// handing the ListOptionsAccess back out of this scope.
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

	// Accepts any of the shapes GetItemText() might return: "#options_language_DE",
	// "options_language_DE", "Deutsch", "DE", "german".
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

		// ToLower() only folds ASCII bytes, which is exactly what we want here: it normalises
		// "options_language_DE" and "English" while leaving multi-byte native names intact.
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

		// Key forms and native names come from the options_language_XX rows in
		// P:\languagecore\stringtable.csv:938-968. Plain language names and ISO-ish codes are
		// included because we do not know which shape the engine hands back.
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

		// No options_language_HU row exists in vanilla, so this is unreachable through the option
		// list today. Kept so the column is addressable if BI ever ships Hungarian.
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

	// Registers an alias under both its literal spelling and its ASCII-lowercased spelling, so
	// ColumnForName() can match either without us hand-maintaining two lists.
	protected static void AddName(string alias, int col)
	{
		s_NameMap.Set(alias, col);

		string lowered = alias;
		lowered.ToLower();
		s_NameMap.Set(lowered, col);
	}

	// --- diagnostics ----------------------------------------------------------------------------

	// Dumps everything needed to fix the mapping from the client script log
	// (G:\Colorful-UI-Pro\.server\!ClientDiagLogs). Emitted once per distinct probe result, so the
	// log gets one block for the boot language and a fresh one after every in-game language change.
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

	// --- csv loading ----------------------------------------------------------------------------

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
				continue; // header row
			}

			if (line == "" || line == "\r")
				continue;

			array<string> fields = ParseCsvLine(line);
			if (fields.Count() < 3)
				continue; // need at least key + original + one language column

			string key = fields.Get(0);
			array<string> values = new array<string>();
			for (int i = 2; i < fields.Count(); i++) // 0 = key, 1 = original (skipped)
				values.Insert(fields.Get(i));

			s_Table.Set(key, values);

			s_LoadedRows++;
			if (values.Count() > s_LoadedColumns)
				s_LoadedColumns = values.Count();
		}

		CloseFile(f);

		Print("[CuiLoc] loaded " + s_LoadedRows.ToString() + " keys from " + CSV_PATH + " (" + s_LoadedColumns.ToString() + " language columns)");
	}

	// Parses one quoted-CSV row of the shape: "field1","field2",...,"fieldN",
	// Every field in this stringtable is always quoted and fields never contain embedded
	// quote characters, so a plain split on `","` is sufficient — no quote-escaping needed.
	// Note this is deliberately byte-based: Substring()/IndexOfFrom() both index bytes
	// (P:\scripts\1_core\proto\enstring.c has separate ...Utf8 variants), so mixing them is
	// consistent and multi-byte translations survive intact.
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
