#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include "resource.h"
#include "PNG_file.h"

LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
HINSTANCE h_hInst;
LPCTSTR lpszClass = TEXT("PNG Steganography - AA23");
HWND Dlg;
HWND EditDlg;
HWND CallDlg=NULL;
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS wc;
	g_hInst = hInstance;



	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = lpszClass;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);



	hWnd = CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,(HMENU)NULL,hInstance,NULL);

	DialogBoxA(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DialogProc); // 잠금 다이얼로그

	while(GetMessage(&Message,NULL,0,0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hRadioEncode, hRadioDecode;
	HWND hCheckEncryption;
	HWND hEditMessage, hEditEncryption;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch(wParam)
		{
		case IDC_BUTTON_ORIGINAL:
			{
				OPENFILENAME ofn;
				char szFileName[MAX_PATH] = {NULL};

				ZeroMemory(&ofn,sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.lpstrFilter = "Image File (.png)\0*.png\0";
				ofn.lpstrFile = szFileName;
				ofn.nMaxFile = MAX_PATH;

				if(GetOpenFileName(&ofn) == TRUE)
				{
					char ext[_MAX_EXT] = {NULL};
					_splitpath(szFileName,NULL,NULL,NULL,ext);

					if(strncmp(ext,".png",strlen(".png")) != NULL)
					{
						MessageBoxA(hwndDlg,"Only .png Available","Oops!",MB_OK);
					}
					else
					{
						SetDlgItemText(hwndDlg,IDC_EDIT_ORIGINAL,szFileName);
					}
				}
				RemoveProp(GetParent(hwndDlg),"OFN");
				break;
			}
		case IDC_BUTTON_OUTPUT:
			{
				OPENFILENAME ofn;
				char szFileName[MAX_PATH] = {NULL};

				ZeroMemory(&ofn,sizeof(OPENFILENAME));
				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.lpstrFilter = "Image File (.png)\0*.png\0";
				ofn.lpstrFile = szFileName;
				ofn.nMaxFile = MAX_PATH;

				if(GetOpenFileName(&ofn) == TRUE)
				{
					char ext[_MAX_EXT] = {NULL};
					_splitpath(szFileName,NULL,NULL,NULL,ext);

					if(!ext)
					{
						strncat(szFileName,".png",strlen(".png"));
					}
					else if(strncmp(ext,".png",strlen(".png")) != NULL)
					{
						strncat(szFileName,".png",strlen(".png"));
					}

					SetDlgItemText(hwndDlg,IDC_EDIT_OUTPUT,szFileName);
				}
				RemoveProp(GetParent(hwndDlg),"OFN");
				break;
			}
		case IDC_RADIO_ENCODE:
			{
				hRadioEncode = GetDlgItem(hwndDlg,IDC_RADIO_ENCODE);
				hEditMessage = GetDlgItem(hwndDlg,IDC_EDIT_STEGO_DATA);

				if(SendMessage(hRadioEncode,BM_GETCHECK,0,0) == BST_CHECKED)
					EnableWindow(hEditMessage,TRUE);
				break;
			}
		case IDC_RADIO_DECODE:
			{
				hRadioDecode = GetDlgItem(hwndDlg,IDC_RADIO_DECODE);
				hEditMessage = GetDlgItem(hwndDlg,IDC_EDIT_STEGO_DATA);

				if(SendMessage(hRadioDecode,BM_GETCHECK,0,0) == BST_CHECKED)
					EnableWindow(hEditMessage,FALSE);
				break;
			}
		case IDC_CHECK_ENCRYPT_KEY:
			{
				hCheckEncryption = GetDlgItem(hwndDlg,IDC_CHECK_ENCRYPT_KEY);
				hEditEncryption = GetDlgItem(hwndDlg,IDC_EDIT_ENCRYPTION_KEY);

				if(SendMessage(hCheckEncryption,BM_GETCHECK,0,0) == BST_CHECKED)
				{
					SendMessage(hEditEncryption,EM_SETREADONLY,FALSE,FALSE);
				}
				else if(SendMessage(hCheckEncryption,BM_GETCHECK,0,0) == BST_UNCHECKED)
				{
					SetDlgItemText(hwndDlg,IDC_EDIT_ENCRYPTION_KEY,NULL);
					SendMessage(hEditEncryption,EM_SETREADONLY,TRUE,FALSE);
				}
				break;
			}

		case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
		}
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		return 1;
	case WM_CREATE:
		return 0;
	}
}