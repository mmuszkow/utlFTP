#pragma once

#include <string>
using namespace std;

wstring urlencode(const wstring &c);
wstring char2hex(wchar_t dec);
wstring extractFilename(const wstring& s);
wstring niceBytes(float val);
