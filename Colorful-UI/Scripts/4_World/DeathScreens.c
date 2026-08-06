modded class DayZPlayerImplement
{
    override void ShowDeadScreen(bool show, float duration)
    {
        #ifndef NO_GUI
        if (show && IsPlayerSelected())
        {
            GetGame().GetUIManager().ScreenFadeIn(duration, "", FadeColors.BLACK, FadeColors.WHITE);
            ShowGameOverOverlay = true;
            // EnterScriptedMenu is a no-op when called synchronously from the
            // death simulation — keep retrying on the GUI queue until it's open
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(OpenGameOverMenu, 250, true);
        }
        else
        {
            GetGame().GetUIManager().ScreenFadeOut(0);
            ShowGameOverOverlay = false;
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(OpenGameOverMenu);
        };

        // Cancel any prior pending fade-stop so consecutive deaths don't stack timers.
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(StopDeathDarkeningEffect);
        if (duration > 0)
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StopDeathDarkeningEffect, duration * 1000, false);
        }
        else
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(StopDeathDarkeningEffect);
        };
        #endif
    };

    void OpenGameOverMenu()
    {
        UIScriptedMenu menu = GetGame().GetUIManager().GetMenu();
        if (menu && menu.GetID() == MENU_INGAME)
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(OpenGameOverMenu);
            return;
        }
        GetGame().GetUIManager().EnterScriptedMenu(MENU_INGAME, null);
    };
};
