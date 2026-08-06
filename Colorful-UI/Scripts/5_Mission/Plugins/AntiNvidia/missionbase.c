modded class MissionBase
{
    override UIScriptedMenu CreateScriptedMenu(int id)
    {
        if (id == MENU_ANVI_CAPTCHA)
        {
            UIScriptedMenu menu = new ANVICaptchaMenu;
            menu.SetID(id);
            return menu;
        }
        return super.CreateScriptedMenu(id);
    }
}