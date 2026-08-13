// CUI_ErrorTestScreen — debug screen that raises every game error on demand.

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
