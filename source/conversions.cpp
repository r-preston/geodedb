#include "headers/program.h"

JSValue JSConvert::to_js(std::string s)
{
    return JSValue(ToWebString(s));
}

JSValue JSConvert::to_js(bool b)
{
    return JSValue(b);
}

JSValue JSConvert::to_js(int i)
{
    return JSValue(i);
}

JSValue JSConvert::to_js(std::vector<std::string> v)
{
    JSArray jarr;
    for (unsigned int i = 0; i < v.size(); i++)
    {
        jarr.Push(JSValue(ToWebString(v[i])));
    }
    return JSValue(jarr);
}

JSValue JSConvert::to_js(std::vector<int> v)
{
    JSArray jarr;
    for (unsigned int i = 0; i < v.size(); i++)
    {
        jarr.Push(JSValue(v[i]));
    }
    return JSValue(jarr);
}

std::string JSConvert::to_string(JSValue j)
{
    return ToString(j.ToString());
}

bool JSConvert::to_bool(JSValue j)
{
    return j.ToBoolean();
}

int JSConvert::to_int(JSValue j)
{
    return j.ToInteger();
}

std::vector<std::string> JSConvert::to_vectorstring(JSValue j)
{
    JSArray jarray = j.ToArray();
    std::vector<std::string> v;

    for (unsigned int i = 0; i < jarray.size(); i++)
    {
        v.push_back(ToString(jarray[i].ToString()));
    }
    return v;
}

std::vector<std::wstring> JSConvert::to_vectorwstring(JSValue j)
{
    JSArray jarray = j.ToArray();
    std::vector<std::wstring> v;

    for (unsigned int i = 0; i < jarray.size(); i++)
    {
        std::wstring widestring;
        for (unsigned int j = 0; j < jarray[i].ToString().length(); j++)
        {
            wchar16 widechar = jarray[i].ToString().data()[j];
            widestring += (wchar_t)widechar;
        }
        v.push_back(widestring);
    }
    return v;
}

std::vector<int> JSConvert::to_vectorint(JSValue j)
{
    JSArray jarray = j.ToArray();
    std::vector<int> v;

    for (unsigned int i = 0; i < jarray.size(); i++)
    {
        v.push_back(jarray[i].ToInteger());
    }
    return v;
}

std::vector<search_result> JSConvert::to_vectorsearchresult(JSValue j)
{
    std::vector<search_result> result;
    JSArray j_array = j.ToArray();

    for (unsigned int i = 0; i < j_array.size(); i++)
    {
        search_result sres;
        JSObject j_obj = j_array[i].ToObject();

        sres.tablename      = to_string( j_obj.GetProperty( WSLit("tablename")) );
        sres.results        = to_vectorint( j_obj.GetProperty( WSLit("results")) );
        sres.active_item    = to_int( j_obj.GetProperty( WSLit("active_item")) );
        sres.sort_mask      = to_int( j_obj.GetProperty( WSLit("sort_mask")) );
        sres.sort_ascending = to_bool( j_obj.GetProperty( WSLit("sort_ascending")) );
        sres.search_or      = to_bool( j_obj.GetProperty( WSLit("search_or")) );
        sres.datecomp       = to_int( j_obj.GetProperty( WSLit("datecomp")) );
        sres.costcomp       = to_int( j_obj.GetProperty( WSLit("costcomp")) );
        sres.numcomp        = to_int( j_obj.GetProperty( WSLit("numcomp")) );

        result.push_back(sres);
    }

    return result;
}