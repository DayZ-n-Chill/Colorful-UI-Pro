modded class DayZPlayerImplement extends DayZPlayer
{
    override void ShowDeadScreen(bool show, float duration)
    {
        #ifndef NO_GUI
        if (show && IsPlayerSelected())
        {
            GetGame().GetUIManager().ScreenFadeIn(duration, "", FadeColors.BLACK, FadeColors.WHITE);
            GetGame().GetUIManager().EnterScriptedMenu(MENU_INGAME, null);
        }
        else
        {
            GetGame().GetUIManager().ScreenFadeOut(0);
        };

        // Cancel any prior pending fade-stop so consecutive deaths don't stack timers.
        GetGame().GetCallQueue(CALL_CATEGORY_GUI).Remove(StopDeathDarkeningEffect);
        if (duration > 0)
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(StopDeathDarkeningEffect, duration * 1000, false);
            ShowGameOverOverlay = true;
        }
        else
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GUI).Call(StopDeathDarkeningEffect);
            ShowGameOverOverlay = false;
        };
        #endif
    };
};
