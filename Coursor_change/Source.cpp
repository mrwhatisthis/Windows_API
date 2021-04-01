#include <Windows.h> 
#include "resource.h"

#define X_WINDOW_SIZE 400
#define Y_WINDOW_SIZE 250



HINSTANCE hInst; 
LPCTSTR szWindowClass = "Demian Stetsyk";
LPCTSTR szTitle = "First program";


ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK dialogHandler(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK userInputDialogHandler(HWND, UINT, WPARAM, LPARAM);


int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	MSG msg;

	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_DBLCLKS; 
	wcex.lpfnWndProc = (WNDPROC)WndProc; 
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 
	wcex.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	wcex.hCursor = LoadCursor(NULL, IDC_WAIT); 
	wcex.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH); 
	wcex.lpszMenuName = NULL; 
	wcex.lpszClassName = szWindowClass; 
	wcex.hIconSm = NULL;
	return RegisterClassEx(&wcex); 
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; 
	HMENU menu;
	menu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1));
	hWnd = CreateWindow(szWindowClass, 
		szTitle, 
		WS_MINIMIZEBOX | WS_SYSMENU | WS_CAPTION | WS_VSCROLL, 
		(GetSystemMetrics(SM_CXSCREEN) - X_WINDOW_SIZE) / 2, 
		(GetSystemMetrics(SM_CYSCREEN) - Y_WINDOW_SIZE) / 2, 
		X_WINDOW_SIZE, 
		Y_WINDOW_SIZE, 
		NULL, 
		menu, 
		hInstance, 
		NULL);
	if (!hWnd) 
	{
		return FALSE;
	}

	ShowWindow(hWnd, SW_MINIMIZE); 
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rt;
	int act;
	switch (message)
	{
	case WM_CREATE: 
		break;

	case WM_LBUTTONDBLCLK:
		MessageBox(hWnd, "Text", "Check", MB_OK);
		break;

	case WM_PAINT: 
		hdc = BeginPaint(hWnd, &ps); 
		GetClientRect(hWnd, &rt); 
		DrawText(hdc, "Hello world" , -1, &rt, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hWnd, &ps);
		break;
	case WM_CLOSE:
		act = MessageBox(NULL, "Are you shure?", "Exit", MB_YESNO);
		switch (act)
		{
		case IDYES:
			PostQuitMessage(0);
			break;
		case IDNO:
			break;
		}
		break;
	case WM_DESTROY: 

		PostQuitMessage(0);
		break;

	case WM_INITMENU:
		break;

	case WM_COMMAND:
		HCURSOR hC;
		HMENU newMenu;
		switch(LOWORD(wParam))
		{
		case IDC_CURSOR1:
			hC = LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR1));
			SetClassLongA(hWnd, GCL_HCURSOR, (LONG)hC);
			break;

		case IDC_CURSOR2:
			hC = LoadCursor(hInst, MAKEINTRESOURCE(IDC_CURSOR2));
			SetClassLongA(hWnd, GCL_HCURSOR, (LONG)hC);
			break;

		case IDC_POINTER:
			hC = LoadCursor(hInst, MAKEINTRESOURCE(IDC_POINTER));
			SetClassLongA(hWnd, GCL_HCURSOR, (LONG)hC);
			break;

		case ID_I40004:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)dialogHandler);
			break;
		case ID_INFORMATION_ADDITIONALDIALOG:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, (DLGPROC)userInputDialogHandler);
			break;
		case ID_MORE_SWICHMENU:
			newMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU2));
			SetMenu(hWnd, newMenu);
			break;
		}
		break;
	default:
		
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK dialogHandler(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int act;
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDOK:
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			case IDCLOSE:
				act = MessageBox(NULL, "Are you shure?", "Exit", MB_YESNO);
				switch (act)
				{
				case IDYES:
					PostQuitMessage(0);
					return TRUE;
				case IDNO:
					EndDialog(hDlg, LOWORD(wParam));
					return TRUE;
				}
				return TRUE;
			case IDC_BUTTON3:
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
	}
	return FALSE;
}

LRESULT CALLBACK userInputDialogHandler(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hEdit1;
	CHAR s_text_1[256] = { 0 };
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
			SendMessage(hEdit1, WM_GETTEXT, (WPARAM)255, (LPARAM)s_text_1);
			MessageBox(NULL, (LPCWSTR)s_text_1, "Input", MB_OK);
			return TRUE;
		case IDCLOSE:
			MessageBox(NULL, "Window is closing.", "Allert", MB_OK);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		
		return TRUE;
	}
	return FALSE;
}