package com.example.accalpha;

public class KatsminTools {
    public static String TrimString(String s)
    {
        String ret = "";
        for(int i = 0; i < s.length(); i++)
        {
            if(s.charAt(i) != ' ')
            {
                ret += s.charAt(i);
            }
        }
        return ret;
    }
    public static String RemoveNewLine(String s)
    {
        if(s.charAt(s.length()-1) == '\n')
        {
            return s.substring(0,s.length()-2);
        }
        return s;
    }
}

