package com.example.accalpha;

public final class KatsminDateTime {
    private short year, month, day, hour, minute, second;
    private boolean isValid;
    public boolean checkValid()
    {
        return isValid;
    }
    public KatsminDateTime(short y, short m, short d, short h, short mm, short s)
    {
        year = y; month = m; day = d; hour = h; minute = mm; second = s;
        isValid = true;
    }

}
