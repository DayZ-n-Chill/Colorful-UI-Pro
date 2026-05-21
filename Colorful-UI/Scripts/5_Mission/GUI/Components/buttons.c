// NOTE TO ALL:  THE CUI ELEMENETS AND WRAPPER CONCEPTS ARE OBSOLETE. THIS CODE IS HERE FOR REFERENCE ONLY.
//  I have begun to think my concept for this wrapper was a bad idea. 4.0 has removed it as it just causes issues here and there and I dont want to keep fixing it.  
// However, I have a much better and more stable solution already in place for 4.0. 
class CUIButtonHandler : ScriptedWidgetEventHandler
{
    Class                m_Owner;          // Menu instance that registered this handler; used by cuiElmnt.CleanupForOwner()
    private ButtonWidget m_Button;
    private TextWidget   m_TextWidget;
    private ImageWidget  m_ImageWidget;
    private int          m_TextColor;
    private int          m_HoverColor;
    private string       m_ClickAction;
    private Class        m_TargetClass;
    private string       m_CallbackMethod;
    private string       m_ServerIP;
    private int          m_ServerPort;

    private bool         m_IconOnly = false;
    private int          m_IconImageIndex = -1;
    private bool         m_SolidBg = false;

    void CUIButtonHandler(ButtonWidget button, TextWidget textWidget, ImageWidget imageWidget, int textColor, int hoverColor, string clickAction, Class targetClass, string callbackMethod, string serverIP, int serverPort)
    {
        m_Button         = button;
        m_TextWidget     = textWidget;
        m_ImageWidget    = imageWidget;
        m_TextColor      = textColor;
        m_HoverColor     = hoverColor;
        m_ClickAction    = clickAction;
        m_TargetClass    = targetClass;
        m_CallbackMethod = callbackMethod;
        m_ServerIP       = serverIP;
        m_ServerPort     = serverPort;

        if (m_Button) m_Button.SetHandler(this);
        ApplyBaseStyles();
    }

    void Dispose()
    {
        if (m_Button) m_Button.SetHandler(null);
        m_Button = null;
        m_TextWidget = null;
        m_ImageWidget = null;
    }

    void SetIconOnly(bool isIconOnly, int imageIdx)
    {
        m_IconOnly = isIconOnly;
        m_IconImageIndex = imageIdx;
        ApplyBaseStyles();
    }

    void SetSolidBg(bool solidOn)
    {
        m_SolidBg = solidOn;
        ApplyBaseStyles();
    }

    private void ApplyBaseStyles()
    {
        if (!m_Button) return;
        
        if ((m_Button.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER) return;

        if (m_IconOnly)
        {
            if (m_TextWidget) m_TextWidget.Show(false);
            m_Button.SetText("");

            if (m_ImageWidget)
            {
                if (m_IconImageIndex >= 0) m_ImageWidget.SetImage(m_IconImageIndex);
                m_ImageWidget.Show(true);
                m_ImageWidget.SetColor(m_TextColor);
            }

            m_Button.SetColor(UIColor.Transparent());
            return;
        }

        if (m_SolidBg)
        {
            if (m_TextWidget) m_TextWidget.SetColor(colorScheme.BtnText());
            else m_Button.SetTextColor(colorScheme.BtnText());

            if (m_ImageWidget)
            {
                if (m_IconImageIndex >= 0) m_ImageWidget.SetImage(m_IconImageIndex);
                m_ImageWidget.SetColor(colorScheme.BtnText());
            }

            m_Button.SetColor(m_TextColor);
            return;
        }

        if (!m_TextWidget && !m_ImageWidget)
        {
            m_Button.SetTextColor(m_TextColor);
            return;
        }

        if (m_TextWidget) m_TextWidget.SetColor(m_TextColor);
        else m_Button.SetTextColor(m_TextColor);

        if (m_ImageWidget)
        {
            m_ImageWidget.SetColor(m_TextColor);
            if (m_IconImageIndex >= 0) m_ImageWidget.SetImage(m_IconImageIndex);
        }

        m_Button.SetColor(UIColor.Transparent());
    }

    private void ApplyHoverStyles()
    {
        if (!m_Button) return;

        if ((m_Button.GetFlags() & WidgetFlags.IGNOREPOINTER) == WidgetFlags.IGNOREPOINTER) return;

        if (m_IconOnly)
        {
            if (m_ImageWidget) m_ImageWidget.SetColor(m_HoverColor);
            m_Button.SetColor(UIColor.Transparent());
            return;
        }

        if (m_SolidBg)
        {
            m_Button.SetColor(m_HoverColor);
            return;
        }

        if (!m_TextWidget && !m_ImageWidget)
        {
            m_Button.SetColor(m_HoverColor);
            return;
        }

        if (m_TextWidget) m_TextWidget.SetColor(m_HoverColor);
        if (m_ImageWidget) m_ImageWidget.SetColor(m_HoverColor);

        m_Button.SetColor(UIColor.Transparent());
    }

    override bool OnMouseEnter(Widget w, int x, int y)
    {
        if (w != m_Button) return false;
        ApplyHoverStyles();
        return true;
    }

    override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
    {
        if (w != m_Button) return false;
        ApplyBaseStyles();
        return true;
    }

    override bool OnFocus(Widget w, int x, int y)
    {
        if (w != m_Button) return false;
        ApplyHoverStyles();
        return true;
    }

    override bool OnFocusLost(Widget w, int x, int y)
    {
        if (w != m_Button) return false;
        ApplyBaseStyles();
        return true;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w != m_Button) return false;

        if (m_ClickAction != "")
        {
            GetGame().OpenURL(m_ClickAction);
            return true;
        }

        // Defer the callback to the next GUI tick. Calling m_CallbackMethod
        // synchronously here is a use-after-free hazard: methods like
        // MainMenu.OpenSettings or OptionsMenu.Back destroy the current
        // menu, which fires CleanupForOwner, which deletes THIS handler
        // mid-OnClick. When OnClick returns, the engine's event dispatcher
        // does a vtable touch on the freed handler -> ACCESS_VIOLATION at
        // call qword [rax+0x88]. Posting via CallLater(0) lets the engine
        // finish its event loop on a still-live handler before menu
        // teardown begins.
        if (m_TargetClass && m_CallbackMethod != "")
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.InvokeCallback, 0, false);
            return true;
        }

        if (m_ServerIP != "" && m_ServerPort > 0)
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(this.DoDirectConnect, 100, false);
            return true;
        }

        return true;
    }

    void InvokeCallback()
    {
        if (m_TargetClass && m_CallbackMethod != "")
            GetGame().GameScript.CallFunction(m_TargetClass, m_CallbackMethod, null, 0);
    }

    override bool OnMouseButtonUp(Widget w, int x, int y, int button)
    {
        return false;
    }

    void DoDirectConnect()
    {
        if (m_ServerIP != "" && m_ServerPort > 0)
        {
            GetGame().GetUIManager().CloseAll();
            DayZGame game = DayZGame.Cast(GetGame());
            if (game) {
                game.ConnectFromJoin(m_ServerIP, m_ServerPort);
            }
        }
    }
}
// NOTE: THese are not the final elements.  This was just a quick shit way of getting thing together for a release candadite. 
// I schlocked it together and will be putting something more appropriate after the release candadite.  
class cuiElmnt
{
    static ref array<ref CUIButtonHandler> s_Handlers = new array<ref CUIButtonHandler>();

    // Cleanup ALL handlers. Last-resort full purge — prefer CleanupForOwner.
    static void Cleanup()
    {
        for (int i = 0; i < s_Handlers.Count(); i++)
        {
            if (s_Handlers[i]) s_Handlers[i].Dispose();
            delete s_Handlers[i];
        }
        s_Handlers.Clear();
    }

    // Dispose every handler tagged with `owner`. Call from each menu's destructor:
    //   void ~MyMenu() { cuiElmnt.CleanupForOwner(this); }
    //
    // IMPORTANT: We Dispose() (clears widget binding so no more events route
    // to this handler) but DO NOT Remove() from s_Handlers. Removing drops
    // the strong ref and frees the handler synchronously — and the engine
    // may still be inside a callback frame on that handler (OnClick that
    // posted CallLater(InvokeCallback), or a CallLater dispatch in flight).
    // Freeing it mid-dispatch causes ACCESS_VIOLATION on the return vtable
    // touch. Keeping it in s_Handlers leaks a small object per closed menu
    // but eliminates the UAF crash.
    static void CleanupForOwner(Class owner)
    {
        if (!owner) return;
        for (int i = 0; i < s_Handlers.Count(); i++)
        {
            if (s_Handlers[i] && s_Handlers[i].m_Owner == owner)
                s_Handlers[i].Dispose();
        }
    }

    protected static void GetParts(ButtonWidget button, out TextWidget label, out ImageWidget icon)
    {
        label = null; icon = null;
        if (!button) return;
        label = TextWidget.Cast(button.FindAnyWidget(button.GetName() + "_label"));
        icon  = ImageWidget.Cast(button.FindAnyWidget(button.GetName() + "_img"));
        if (!icon) icon = ImageWidget.Cast(button.FindAnyWidget(button.GetName() + "_image"));
    }

    static void proSolidBtn(Class owner, ButtonWidget button, string text, int bgColor, int hoverBgColor, string clickAction)
    {
        if (!button) return;

        button.SetText(text);
        TextWidget label; ImageWidget icon; GetParts(button, label, icon);
        if (label) { label.SetText(text); button.SetText(""); }

        CUIButtonHandler h = new CUIButtonHandler(button, label, icon, bgColor, hoverBgColor, clickAction, null, "", "", 0);
        h.SetSolidBg(true);
        h.m_Owner = owner;
        s_Handlers.Insert(h);
    }

    static void proBtnURL(Class owner, ButtonWidget button, string text, int textColor, int hoverColor, string clickAction)
    {
        if (!button) return;

        button.SetText(text);
        TextWidget label; ImageWidget icon; GetParts(button, label, icon);
        if (label) { label.SetText(text); button.SetText(""); }
        if (icon) icon.SetColor(hoverColor);

        CUIButtonHandler h = new CUIButtonHandler(button, label, NULL, textColor, hoverColor, clickAction, null, "", "", 0);
        h.m_Owner = owner;
        s_Handlers.Insert(h);
    }

    static void proBtnDC(Class owner, ButtonWidget button, string text, int textColor, int hoverColor, string serverIP, int serverPort)
    {
        if (!button) return;

        button.SetText(text);
        TextWidget label; ImageWidget icon; GetParts(button, label, icon);
        if (label) { label.SetText(text); button.SetText(""); }
        if (icon) icon.SetColor(colorScheme.Icons());

        CUIButtonHandler h = new CUIButtonHandler(button, label, NULL, textColor, hoverColor, "", null, "", serverIP, serverPort);
        h.m_Owner = owner;
        s_Handlers.Insert(h);
    }

    static void proIconBtn(Class owner, ButtonWidget button, int iconImageIndex, int iconColor, int hoverColor, string clickAction)
    {
        if (!button) return;

        button.SetText("");
        TextWidget label; ImageWidget icon; GetParts(button, label, icon);
        if (label) label.Show(false);

        CUIButtonHandler h = new CUIButtonHandler(button, NULL, icon, iconColor, hoverColor, clickAction, null, "", "", 0);
        h.SetIconOnly(true, iconImageIndex);
        if (icon) icon.SetColor(iconColor);

        h.m_Owner = owner;
        s_Handlers.Insert(h);
    }

    static void proBtnCB(Class owner, ButtonWidget button, string text, int textColor, int hoverColor, Class targetClass, string callbackMethod)
    {
        if (!button) return;

        button.SetText(text);
        TextWidget label; ImageWidget icon; GetParts(button, label, icon);
        if (label) { label.SetText(text); button.SetText(""); }

        ImageWidget handlerIcon = icon;
        if (icon && text != "")
        {
            icon.SetColor(hoverColor);
            handlerIcon = NULL; // Match proBtnURL: Handler handles text, script handles icon color
        }

        CUIButtonHandler h = new CUIButtonHandler(button, label, handlerIcon, textColor, hoverColor, "", targetClass, callbackMethod, "", 0);

        if (text == "" && icon)
        {
            h.SetIconOnly(true, -1);
        }

        h.m_Owner = owner;
        s_Handlers.Insert(h);
    }
}
