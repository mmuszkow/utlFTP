#include "stdinc.h"
#include "string_utils.h"

wstring urlencode(const wstring &c)
{
    wstring escaped=L"";
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (L'0' <= c[i] && c[i] <= L'9') ||//0-9
             (L'a' <= c[i] && c[i] <= L'z') ||//abc...xyz
             (L'A' <= c[i] && c[i] <= L'Z') || //ABC...XYZ
             (c[i]==L'~' || c[i]==L'!' || c[i]==L'*' || 
			 c[i]==L'(' || c[i]==L')' || c[i]==L'\'' || c[i] == L'.')
        )
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append(L"%");
            escaped.append(char2hex(c[i]));//converts char 255 to string "ff"
        }
    }
    return escaped;
}

wstring char2hex( wchar_t dec )
{
    wchar_t buff[8];
	wsprintf(buff,L"%X",dec);
    return wstring(buff);
}

wstring niceBytes(float val)
{
	wstringstream stream;
	stream.precision(2);
	if(val > (1024<<20))
		stream << fixed << val/(1024<<20) << L"GB";
	else if(val > (1024<<10))
		stream << fixed << val/(1024<<10) << L"MB";
	else if(val > 1024)
		stream << fixed << val/1024 << L"KB";
	else
		stream << val << L"B";
	return stream.str();
}

wstring extractFilename(const wstring& s)
{
	int i = s.size();
	while(i-->0)
	{
		if(s[i]==L'\\' || s[i]==L'/')
			break;
	}
	i++;
	return s.substr(i);
}
