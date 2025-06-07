class CuiLogger
{
    protected static const string LOG_FILE = "$profile:cui_logger.log";

    static void Log(string message)
    {
        FileHandle handle = OpenFile(LOG_FILE, FileMode.APPEND);
        if (handle)
        {
            FPrintln(handle, Timestamp() + " - " + message);
            CloseFile(handle);
        }
    }

    protected static string Timestamp()
    {
        int year, month, day, hour, minute;

        if (GetGame() && GetGame().GetWorld())
        {
            GetGame().GetWorld().GetDate(year, month, day, hour, minute);
        }
        else
        {
            GetYear(year);
            GetMonth(month);
            GetDay(day);
            GetHour(hour);
            GetMinute(minute);
        }

        return string.Format("%1-%2-%3 %4:%5", year, month, day, hour, minute);
    }
}
