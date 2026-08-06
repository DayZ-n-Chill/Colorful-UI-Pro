// CUI_ErrorTestScreen
// -----------------------------------------------------------------------------
// Debug harness: one button per error/dialog the vanilla ErrorModuleHandler
// system can produce (see ErrorTestData.c). Booted instead of the normal main
// menu when Settings.c's ErrorTestScreen flag is true (MainMenu.c, top of
// Init()). Every button throws its error through the real public API -
// ErrorModuleHandler.ThrowError(ErrorCategory category, int code,
// string additionalInfo = "") - P:\scripts\3_game\global\errormodulehandler\
// errormodulehandler.c:43 - so whatever dialog results (native ShowDialog or
// this mod's CuiDialog reskin) is the genuine article, not a mock.
//
// Buttons are generated dynamically from CUI_ErrorTestData's table (~185
// entries) rather than hand-placed in the layout: cui.errortest.layout only
// supplies the screen chrome + a ScrollWidget/WrapSpacer content container,
// and cui.errortest.row.layout / cui.errortest.header.layout are single-widget
// prototypes cloned per entry via GetGame().GetWorkspace().CreateWidgets(path,
// parent) - the same pattern vanilla's own CreditsDepartmentSection uses for
// dynamic list rows (P:\scripts\5_mission\gui\newui\credits\elements\
// creditsdepartmentelement.c:65-77).
//
// Handler choice: cuiElmnt.proBtnCB (Components/buttons.c) is NOT used for the
// per-error buttons. Its InvokeCallback dispatches via
// GetGame().GameScript.CallFunction(target, methodName, null, 0) - no
// arguments - so it only works when N buttons all call the same fixed,
// pre-existing method name. Here every button needs a distinct (category,
// code) pair carried alongside it, and there's no way to synthesize ~185
// uniquely-named callback methods. A plain ScriptedWidgetEventHandler that
// stores category+code as instance fields (one instance per button, set via
// ButtonWidget.SetHandler - the same primitive CUIButtonHandler itself uses
// under proBtnCB) fits a data-driven list directly. The Back button, which
// needs no parameters, still uses proBtnCB like the rest of this mod's chrome.
class CUI_ErrorTestButtonHandler : ScriptedWidgetEventHandler
{
	protected ButtonWidget m_Button;
	protected ErrorCategory m_Category;
	protected int m_Code;
	protected MainMenu m_Owner;

	void CUI_ErrorTestButtonHandler(ButtonWidget button, ErrorCategory category, int code, MainMenu owner)
	{
		m_Button = button;
		m_Category = category;
		m_Code = code;
		m_Owner = owner;
		if (m_Button) m_Button.SetHandler(this);
	}

	// Mirrors CUIButtonHandler.Dispose() (Components/buttons.c) - detach from
	// the widget before the widget tree is torn down.
	void Dispose()
	{
		if (m_Button) m_Button.SetHandler(null);
		m_Button = null;
		m_Owner = null;
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w != m_Button) return false;

		ErrorModuleHandler.ThrowError(m_Category, m_Code);

		// ClientKicked-category throws are captured by ErrorDialog.c's modded
		// DialogueErrorProperties.HandleError() into CuiPendingError instead of
		// firing the native dialog immediately (see that file - it only fires
		// for ClientKicked entries with no UIScriptedMenu handler, which is all
		// of them). Normally that pending error is only flushed on the NEXT
		// MainMenu.Init() (CheckPendingCuiError, called after this screen's
		// early-return branch, so it wouldn't run automatically while this
		// screen is up). Flush it here instead, one GUI tick later, through the
		// public wrapper MainMenu.CUI_ErrorTest_FlushPendingError() so the
		// operator sees the actual CuiDialog result immediately. For every
		// other category (ConnectErrorClient/Server/Script, BIOSError -1) this
		// call is a harmless no-op - CuiPendingError.s_Pending stays false
		// because ErrorDialog.c's override never touches those categories.
		if (m_Owner)
			GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(m_Owner.CUI_ErrorTest_FlushPendingError, 0, false);

		return true;
	}
}

class CUI_ErrorTestScreen
{
	protected Widget m_Root;
	protected ref array<ref CUI_ErrorTestButtonHandler> m_Handlers = new array<ref CUI_ErrorTestButtonHandler>();

	Widget Build(MainMenu owner)
	{
		m_Root = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.errortest.layout");
		if (!m_Root) return null;

		Widget separator = m_Root.FindAnyWidget("ErrorTestSeparator");
		if (separator) separator.SetColor(colorScheme.Separator());

		WrapSpacerWidget content = WrapSpacerWidget.Cast(m_Root.FindAnyWidget("ErrorTestContent"));
		if (content) BuildEntries(content, owner);

		ButtonWidget backBtn = ButtonWidget.Cast(m_Root.FindAnyWidget("BackBtn"));
		cuiElmnt.proBtnCB(owner, backBtn, "Back to Main Menu", colorScheme.PrimaryText(), colorScheme.ButtonHover(), owner, "CUI_BackToMainMenu");

		return m_Root;
	}

	protected void BuildEntries(WrapSpacerWidget content, MainMenu owner)
	{
		array<ref CUI_ErrorTestEntry> entries;
		CUI_ErrorTestData.GetEntries(entries);
		if (!entries) return;

		bool first = true;
		ErrorCategory lastCategory;

		foreach (CUI_ErrorTestEntry entry : entries)
		{
			if (first || entry.m_Category != lastCategory)
			{
				Widget headerRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.errortest.header.layout", content);
				TextWidget header = TextWidget.Cast(headerRoot);
				if (header)
				{
					// EnumTools.EnumToString (P:\scripts\1_core\proto\enconvert.c:614),
					// not m_Category.ToString(): an enum variable is int-backed, so
					// ToString() would print "5" instead of "ClientKicked" even if the
					// compiler accepts the member call on an enum-typed value.
					header.SetText(EnumTools.EnumToString(ErrorCategory, entry.m_Category));
					header.SetColor(colorScheme.BrandColor());
				}
				lastCategory = entry.m_Category;
				first = false;
			}

			Widget rowRoot = GetGame().GetWorkspace().CreateWidgets("Colorful-UI/GUI/layouts/dialogs/cui.errortest.row.layout", content);
			ButtonWidget btn = ButtonWidget.Cast(rowRoot);
			if (!btn) continue;

			btn.SetText(entry.m_Label);
			btn.SetColor(colorScheme.BtnSolidBG());
			btn.SetTextColor(colorScheme.BtnText());

			CUI_ErrorTestButtonHandler handler = new CUI_ErrorTestButtonHandler(btn, entry.m_Category, entry.m_Code, owner);
			m_Handlers.Insert(handler);
		}

		content.Update();
	}

	// Called from MainMenu.~MainMenu(). Detaches every dynamically-created
	// button handler before the widget tree goes away.
	void Cleanup()
	{
		if (!m_Handlers) return;

		for (int i = 0; i < m_Handlers.Count(); i++)
		{
			if (m_Handlers[i]) m_Handlers[i].Dispose();
		}
		m_Handlers.Clear();
	}
}
