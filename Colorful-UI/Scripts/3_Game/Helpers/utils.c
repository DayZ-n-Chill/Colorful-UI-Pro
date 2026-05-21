bool allInvalid = true;

void CheckSocials(Widget w, string url)
{
    if (!w) return;
    
    if (url == "#" || url == "" || url == "test" || url == "/test")
    {
        w.Show(false);
    }
    else
    {
        w.Show(true);
        allInvalid = false;
    }
}

void CheckURL(Widget w, string url)
{
    if (!w) return;
    
    if (url == "#" || url == "" || url == "test" || url == "/test")
    {
        w.Show(false);
    }
    else
    {
        w.Show(true);
    }
}
