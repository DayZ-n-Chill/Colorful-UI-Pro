// class CUIDialogMenu : UIScriptedMenu
// {
//     protected TextWidget     m_DialogTitle;
//     protected TextWidget     m_DialogBody;
//     protected ButtonWidget   m_ButtonOK;
//     protected ButtonWidget   m_ButtonCancel;

//     override Widget Init()
//     {
//         // Load the layout for this dialog
//         layoutRoot = GetGame().GetWorkspace().CreateWidgets("MyModScripts/layouts/MyDialog.layout", null);

//         // Find widgets
//         m_DialogTitle  = TextWidget.Cast(layoutRoot.FindAnyWidget("DialogTitle"));
//         m_DialogBody   = TextWidget.Cast(layoutRoot.FindAnyWidget("DialogBody"));
//         m_ButtonOK     = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonOK"));
//         m_ButtonCancel = ButtonWidget.Cast(layoutRoot.FindAnyWidget("ButtonCancel"));

//         return layoutRoot;
//     }

//     void SetTitle(string title)
//     {
//         if (m_DialogTitle)
//             m_DialogTitle.SetText(title);
//     }

//     void SetBody(string body)
//     {
//         if (m_DialogBody)
//             m_DialogBody.SetText(body);
//     }

//     // Accessors to the button widgets for your cuiElmnt to handle
//     ButtonWidget GetButtonOK()
//     {
//         return m_ButtonOK;
//     }

//     ButtonWidget GetButtonCancel()
//     {
//         return m_ButtonCancel;
//     }

//     // You could also override OnClick(...) if you want local handling
//     override bool OnClick(Widget w, int x, int y, int button)
//     {
//         // If you want direct logic here, do so.
//         // Otherwise, you can handle clicks externally using your handler approach.
//         return super.OnClick(w, x, y, button);
//     }
// }
