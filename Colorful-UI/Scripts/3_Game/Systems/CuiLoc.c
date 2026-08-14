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
class CuiLoc
{
	protected static const string CSV_PATH = "Colorful-UI/languagecore/stringtable.csv";

	// key -> array of per-language strings, in CSV column order starting at "english"
	// (i.e. the "Language" and "original" columns are skipped when building this array).
	protected static ref map<string, ref array<string>> s_Table;

	// Resolves a bare key (no leading "#") against the current display language.
	// Falls back to english (column 0) if the language index is out of range, and to the raw
	// key itself if the key isn't in the table at all, matching the engine's own "#KEY" fallback
	// behavior so a lookup miss is visually obvious instead of silently blank.
	static string Get(string key)
	{
		EnsureLoaded();

		array<string> values;
		if (!s_Table || !s_Table.Find(key, values) || !values || values.Count() == 0)
			return key;

		int col = CurrentLangColumn();
		if (col < 0 || col >= values.Count())
			col = 0;

		string text = values.Get(col);
		if (text == "")
			return key;

		return text;
	}

	// ASSUMPTION (unverified without launching the game — see report): the index returned by
	// GetCurrentDisplayLanguageIdx() matches this CSV's column order 1:1 (0 = english, 1 = czech,
	// 2 = german, 3 = russian, 4 = polish, 5 = hungarian, 6 = italian, 7 = spanish, 8 = french,
	// 9 = chinese, 10 = japanese, 11 = portuguese, 12 = chinesesimp), since this csv's header is
	// byte-identical to vanilla's own P:\languagecore\stringtable.csv and DayZ's language option
	// list is built from that same master ordering. If translations come out mismatched in
	// Workbench/live testing, dump ListOptionsAccess(AT_OPTIONS_LANGUAGE).GetItemText(i, text) for
	// each i and reorder the CSV columns (or add an explicit index remap here) to match.
	protected static int CurrentLangColumn()
	{
		if (!g_Game)
			return 0;

		int idx = g_Game.GetCurrentDisplayLanguageIdx();
		if (idx < 0)
			return 0;

		return idx;
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
		}

		CloseFile(f);
	}

	// Parses one quoted-CSV row of the shape: "field1","field2",...,"fieldN",
	// Every field in this stringtable is always quoted and fields never contain embedded
	// quote characters, so a plain split on `","` is sufficient — no quote-escaping needed.
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
