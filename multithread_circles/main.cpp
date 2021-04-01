#include <windows.h> 
#include "resource.h"
#include <vector>
#include <ctime>


#define TIMER  1
#define MAX_THREADS 12


using namespace std;

HINSTANCE hInst; 
LPCTSTR szWindowClass = "QWERTY";
LPCTSTR szTitle = "First program";
RECT rt;


static STARTUPINFO si;
static PROCESS_INFORMATION pi;

char buffer[100];
int precol = 0;

struct Object
{
	int Dx;
	int Dy;
	RECT rect;
};

DWORD   dwThreadIdArray[MAX_THREADS];
HANDLE  hThreadArray[MAX_THREADS];

vector<Object> *vect;



ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI MyThreadFunction(LPVOID lpParam);

auto fun = WndProc;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	vect = new vector<Object>(MAX_THREADS);


	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}
	// Цикл обробки повідомлень
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	delete vect;
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; 
	wcex.lpfnWndProc = (WNDPROC)WndProc; 
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; 
	wcex.hIcon = LoadIcon(NULL, IDI_HAND); 
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 1); 
	wcex.lpszMenuName = NULL; 
	wcex.lpszClassName = szWindowClass; 
	wcex.hIconSm = NULL;
	return RegisterClassEx(&wcex); 
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1));
	hInst = hInstance; 
	hWnd = CreateWindow(szWindowClass, 
		szTitle, 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		NULL, 
		hMenu, 
		hInstance, 
		NULL); 

	if (!hWnd) 
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow); 
	UpdateWindow(hWnd); 
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HPEN hPen;
	HBRUSH hBrush;
	int a = rand() % 4;
	int xCoord = 0;
	int yCoord = 0;

	static unsigned int thread_count = 0;

	switch (message)
	{
	case WM_CREATE: 
		SetTimer(hWnd, TIMER, 60, NULL);

		break;
	case WM_PAINT: 

		hdc = BeginPaint(hWnd, &ps);
		GetClientRect(hWnd, &rt); 

		hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
		hBrush = CreateSolidBrush(RGB(255, 255, 255));
		SelectObject(hdc, hBrush);
		SelectObject(hdc, hPen);
		
		for (auto& elem : *vect)
		{
			Ellipse(hdc, elem.rect.left, elem.rect.top, elem.rect.right, elem.rect.bottom);
		}
		EndPaint(hWnd, &ps); 
		break;


	case WM_TIMER:
		if (wParam == TIMER)
		{
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_COMMAND:

		DWORD process_status;

		switch (LOWORD(wParam))
		{
		case ID_CREATE_PROCESS:

			GetExitCodeProcess(pi.hProcess, &process_status);
			if (process_status == STILL_ACTIVE) 
			{
				MessageBox(hWnd, "Process already started", "Warning", MB_OK | MB_ICONWARNING);
			}
			else 
			{
				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));
				if (!CreateProcess(TEXT("C:\\Windows\\notepad.exe"), NULL, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
				{
					MessageBox(hWnd, "Smth went wrong in starting process", "Warning", MB_OK | MB_ICONWARNING);
				}

			}
			
			break;
		case ID_DELETE_PROCESS:

			GetExitCodeProcess(pi.hProcess, &process_status);

			if (process_status == STILL_ACTIVE)
			{
				TerminateProcess(pi.hProcess, 0);
			}
			else
			{
				MessageBox(hWnd, "Process already deleted", "Warning", MB_OK | MB_ICONWARNING);
			}
			break;
		}
		break;

	case WM_CHAR:
		GetClientRect(hWnd, &rt); 

		if (thread_count >= MAX_THREADS)
		{
			MessageBox(hWnd, "Limit of threads", "Warning", MB_OK | MB_ICONWARNING);
			break;
		}

		Object tmp;

		tmp.Dx = 0;
		tmp.Dy = 0;

		switch (a)
		{
		case 0:
			tmp.Dy = -1;
			break;
		case 1:
			tmp.Dy = 1;
			break;
		case 2:
			tmp.Dx = -1;
			break;
		case 3:
			tmp.Dx = 1;
			break;
		}

		xCoord = (rand() % (rt.right - rt.left - 50));
		yCoord = (rand() % (rt.bottom - rt.top - 50));

		SetRect(&tmp.rect, xCoord, yCoord, xCoord + 50, yCoord + 50);



		(*vect)[thread_count] = tmp;
		
		hThreadArray[thread_count] = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			MyThreadFunction,       // thread function name
			&((*vect)[thread_count]),          // argument to thread function 
			0,                      // use default creation flags 
			&dwThreadIdArray[thread_count]);   // returns the thread identifier 

		thread_count++;

		InvalidateRect(hWnd, NULL, TRUE);
		
		break;

	case WM_DESTROY: 
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	Object* circle = (Object*)lpParam;
	while (true)
	{
		Sleep(1000/60);
		circle->rect.left += circle->Dx;
		circle->rect.right += circle->Dx;
		circle->rect.top += circle->Dy;
		circle->rect.bottom += circle->Dy;

		if (circle->rect.left < rt.left || circle->rect.right > rt.right)
		{
			circle->Dx *= -1;
			circle->rect.right += (circle->Dx * 2);
			circle->rect.left += (circle->Dx * 2);
		}
		else if (circle->rect.top < rt.top || circle->rect.bottom > rt.bottom)
		{
			circle->Dy *= -1;
			circle->rect.top += (circle->Dy * 2);
			circle->rect.bottom += (circle->Dy * 2);
		}
	}
	return 0;
}
