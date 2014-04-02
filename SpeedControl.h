#pragma once

#include <windows.h>

namespace utlFTP
{
	namespace SpeedCtrl
	{
		static const wchar_t CLASS_NAME[] = L"TransferSpeedCtrl";
		static const UINT WM_NEWVAL = RegisterWindowMessage(L"TransferSpeedCtrlNewVal");
		static const HPEN LIME_PEN = CreatePen(PS_SOLID, 1, RGB(0,255,0));
		static const HPEN YELLOW_PEN = CreatePen(PS_DOT, 1, RGB(255,255,0));
		static const int MULTIPLIER = 5;
		static const int MARGIN_H = 10;
		static const int MARGIN_V = 45;

		struct chartInfo
		{
			float* values;
			unsigned int width;
			unsigned int height;
			float min;
			float max;
		};

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			PAINTSTRUCT ps;
			HDC hdc;
			unsigned int i;
			wchar_t label_buff[32];
			if(uMsg==WM_CREATE)
			{
				RECT r;
				GetWindowRect(hWnd, &r);
				chartInfo* info = new chartInfo;
				info->width = ((r.right-r.left)>>MULTIPLIER)+1;
				info->height = r.bottom-r.top-(MARGIN_H<<1);
				info->values = new float[info->width];
				for(i=0;i<info->width;i++)
					info->values[i] = 0;
				info->min = 1024000.0f;
				info->max = 0.0f;
				SetProp(hWnd,L"INFO",info);
				return 0;
			}
			if(uMsg==WM_PAINT)
			{
				chartInfo* info = reinterpret_cast<chartInfo*>(GetProp(hWnd,L"INFO"));
				if(!info)
					return -1;

				hdc = BeginPaint(hWnd, &ps);

				SelectObject(hdc,GetStockObject(DEFAULT_GUI_FONT));				
				SetBkColor(hdc, RGB(0,0,0));
				SetTextColor(hdc, RGB(255,255,0));
				//SetTextAlign(hdc,VTA_CENTER);

				for(i=0;i<4;i++)
				{
					SelectObject(hdc, GetStockObject(NULL_PEN));
					LineTo(hdc,MARGIN_V,MARGIN_H+(info->height>>1)*i+1);
					SelectObject(hdc, YELLOW_PEN);
					LineTo(hdc,(info->width<<MULTIPLIER)-1,MARGIN_H+(info->height>>1)*i+1);
				}
				
				if(info->max > info->min)
				{
					swprintf(label_buff,32,L"%.0fKB/s",info->min/1024.0f);
					TextOut(hdc,0,info->height+4,label_buff,wcslen(label_buff));
					swprintf(label_buff,32,L"%.0fKB/s",(info->max-((info->max-info->min)/2.0f))/1024.0f);
					TextOut(hdc,0,(info->height>>1)+4,label_buff,wcslen(label_buff));
					swprintf(label_buff,32,L"%.0fKB/s",info->max/1024.0f);
					TextOut(hdc,0,6,label_buff,wcslen(label_buff));

					SelectObject(hdc, LIME_PEN);

					POINT* p = new POINT[info->width];
					for(i=0;i<info->width;i++)
					{
						p[i].x = i << MULTIPLIER;
						p[i].y = static_cast<LONG>(MARGIN_H+(1 - (info->values[i]-info->min) / static_cast<float>(info->max-info->min)) * info->height);
					}
					Polyline(hdc,p,info->width);
					delete [] p;
				}
				EndPaint(hWnd, &ps);
				return 0;
			}
			if(uMsg==WM_NEWVAL)
			{
				chartInfo* info = reinterpret_cast<chartInfo*>(GetProp(hWnd,L"INFO"));
				if(!info)
					return -1;

				/*if(info->val[0] == info->max || info->val[0] == info->min)
				{
					float tmp_max = min(info->val[1],info->max);
					float tmp_min = max(info->val[1],info->min);
					info->max = tmp_max;
					info->min = tmp_min;
				}*/

				for(i=0;i<info->width-1;i++)
					info->values[i] = info->values[i+1];

				float val = static_cast<float>(wParam);
				if(val < info->min)
					info->min = val;
				if(val > info->max)
					info->max = val;
				if(info->min == info->max)
					info->max += 10240.0f;

				info->values[info->width-1] = val;

				InvalidateRect(hWnd, NULL, true);
				return 0;
			}
			if(uMsg == WM_DESTROY)
			{
				chartInfo* info = reinterpret_cast<chartInfo*>(GetProp(hWnd,L"INFO"));
				if(info)
				{
					delete [] info->values;
					delete info;
				}
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

		static bool registerCtrl(HINSTANCE hInst)
		{
			WNDCLASS wndclass;

			wndclass.style = CS_HREDRAW | CS_VREDRAW; 
			wndclass.lpfnWndProc = WndProc; 
			wndclass.cbClsExtra = 0; 
			wndclass.cbWndExtra = 0; 
			wndclass.hInstance = hInst; 
			wndclass.hIcon = 0; 
			wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); 
			wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
			wndclass.lpszMenuName = NULL; 
			wndclass.lpszClassName = CLASS_NAME;

			return (RegisterClass(&wndclass) != 0); 
		}
	};
};