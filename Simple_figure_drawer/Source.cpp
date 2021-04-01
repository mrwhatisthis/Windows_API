#include <windows.h> // підключення бібліотеки з функціями API
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// Глобальні змінні:
HINSTANCE hInst; //Дескриптор програми
LPCTSTR szWindowClass = " QWERTY";
LPCTSTR szTitle = "Program";
// Попередній опис функцій
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


enum class figure_type { LINE, RECTANGLE, ELLIPSE } fig_type;
HWND line, rect, ellipse, vscroll, hscroll = NULL;
SCROLLINFO hSi, vSi;
bool isMouseDown = false;
static int hScrollPos = 0, vScrollPos = 0;

struct Figure
{
	RECT size;
	figure_type type;
};
vector<Figure> figureContainer;


// Основна програма
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	// Реєстрація класу вікна
	MyRegisterClass(hInstance);
	// Створення вікна програми
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
	return msg.wParam;
}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW; //стиль вікна
	wcex.lpfnWndProc = (WNDPROC)WndProc; //віконна процедура
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance; //дескриптор програми
	wcex.hIcon = LoadIcon(NULL, IDI_HAND); //визначення іконки
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW); //визначення курсору
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 1); //установка фону
	wcex.lpszMenuName = NULL; //визначення меню
	wcex.lpszClassName = szWindowClass; //ім’я класу
	wcex.hIconSm = NULL;
	return RegisterClassEx(&wcex); //реєстрація класу вікна
}
// FUNCTION: InitInstance (HANDLE, int)
// Створює вікно програми і зберігає дескриптор програми в змінній hInst
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	hInst = hInstance; //зберігає дескриптор додатка в змінній hInst
	hWnd = CreateWindow(szWindowClass, // ім’я класу вікна
		szTitle, // назва програми
		WS_OVERLAPPEDWINDOW, // стиль вікна
		CW_USEDEFAULT, // положення по Х
		CW_USEDEFAULT, // положення по Y
		CW_USEDEFAULT, // розмір по Х
		CW_USEDEFAULT, // розмір по Y
		NULL, // дескриптор батьківського вікна
		NULL, // дескриптор меню вікна
		hInstance, // дескриптор програми
		NULL); // параметри створення.
	if (!hWnd) //Якщо вікно не творилось, функція повертає FALSE
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow); //Показати вікно
	UpdateWindow(hWnd); //Оновити вікно
	return TRUE;
}
// FUNCTION: WndProc (HWND, unsigned, WORD, LONG)
// Віконна процедура. Приймає і обробляє всі повідомлення, що приходять в додаток
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	Figure add;
	RECT rt;
	HPEN hPen;
	int delta;
	int direction;
	switch (message)
	{
	case WM_CREATE: //Повідомлення приходить при створенні вікна
		line = CreateWindow("button", "Line", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
			5, 5, 100, 20, hWnd, (HMENU)"ID_LINE", hInst, NULL);
		rect = CreateWindow("button", "Rect", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
			5, 30, 100, 20, hWnd, (HMENU)"ID_RECT", hInst, NULL);
		ellipse = CreateWindow("button", "Elipse", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
			5, 55, 100, 20, hWnd, (HMENU)"ID_ELIPSE", hInst, NULL);

		GetClientRect(hWnd, &rt); //Область вікна для малювання

		hscroll = CreateWindow("scrollbar", "", WS_CHILD | WS_VISIBLE | SBS_HORZ,
			rt.left, rt.bottom - 30, rt.right, 30, hWnd, (HMENU)1, hInst, NULL);
		vscroll = CreateWindow("scrollbar", "", WS_CHILD | WS_VISIBLE | SBS_VERT,
			rt.right - 30, rt.top, 30, rt.bottom, hWnd, (HMENU)1, hInst, NULL);


		hSi.cbSize = vSi.cbSize = sizeof(hSi);
		hSi.fMask = vSi.fMask = SIF_ALL;
		hSi.nMin = vSi.nMin = 0;
		hSi.nMax = vSi.nMax = 100;
		hSi.nPage = vSi.nPage = 20;
		hSi.nPos = vSi.nPos = 0;

		SetScrollInfo(hscroll, SB_CTL, &hSi, TRUE);
		SetScrollInfo(vscroll, SB_CTL, &vSi, TRUE);

		break;
	case WM_PAINT: //Перемалювати вікно
		hdc = BeginPaint(hWnd, &ps); //Почати графічний вивід
		GetClientRect(hWnd, &rt); //Область вікна для малювання

		hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		SelectObject(hdc, hPen);

		for (auto elem : figureContainer)
		{
			switch (elem.type)
			{
			case figure_type::LINE:
				MoveToEx(hdc, elem.size.left, elem.size.top, (LPPOINT)NULL);
				LineTo(hdc, elem.size.right, elem.size.bottom);
				break;

			case figure_type::ELLIPSE:
				Ellipse(hdc, elem.size.left, elem.size.top, elem.size.right, elem.size.bottom);
				break;

			case figure_type::RECTANGLE:
				Rectangle(hdc, elem.size.left, elem.size.top, elem.size.right, elem.size.bottom);
				break;
			}
		}
		EndPaint(hWnd, &ps); //Закінчити графічний вивід
		break;

	case WM_DESTROY: //Завершення роботи
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
		isMouseDown = true;
		add.size.right = add.size.left = LOWORD(lParam);
		add.size.top = add.size.bottom = HIWORD(lParam);
		add.type = fig_type;
		figureContainer.push_back(add);
		break;
	case WM_MOUSEMOVE:
		if (isMouseDown)
		{
			figureContainer[figureContainer.size() - 1].size.right = LOWORD(lParam);
			figureContainer[figureContainer.size() - 1].size.bottom = HIWORD(lParam);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_LBUTTONUP:
		isMouseDown = false;
		break;

	case WM_COMMAND:
		if ((HWND)lParam == line)
		{
			//MessageBox(NULL, "Line", "Draw line", MB_OK);
			fig_type = figure_type::LINE;
		}
		else if ((HWND)lParam == rect)
		{
			//MessageBox(NULL, "Rect", "Draw rect", MB_OK);
			fig_type = figure_type::RECTANGLE;
		}
		else if ((HWND)lParam == ellipse)
		{
			//MessageBox(NULL, "Ellipse", "Draw elipse", MB_OK);
			fig_type = figure_type::ELLIPSE;
		}
		break;

	case WM_HSCROLL:
		GetClientRect(hWnd, &rt);
		hSi.cbSize = sizeof(hSi);
		GetScrollInfo(hscroll, SB_CTL, &hSi);
		direction = HIWORD(wParam) - hSi.nPos;
		delta = ((rt.right - rt.left) / 100);
		//hScrollPos = HIWORD(wParam);
		hSi.nPos = hScrollPos;
		for (auto& element : figureContainer)
		{
			element.size.left -= delta * direction;
			element.size.right -= delta * direction;
		}
		SetScrollInfo(hscroll, SB_CTL, &hSi, TRUE);
		hScrollPos = HIWORD(wParam);
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_VSCROLL:
		GetClientRect(hWnd, &rt);
		vSi.cbSize = sizeof(vSi);
		GetScrollInfo(vscroll, SB_CTL, &vSi);
		direction = HIWORD(wParam) - vSi.nPos;
		delta = ((rt.bottom - rt.top) / 100);
		vSi.nPos = vScrollPos;
		for (auto& element : figureContainer)
		{
			element.size.bottom -= delta * direction;
			element.size.top -= delta * direction;
		}
		SetScrollInfo(vscroll, SB_CTL, &vSi, TRUE);
		vScrollPos = HIWORD(wParam);
		InvalidateRect(hWnd, NULL, TRUE);
	default:
		//Обробка повідомлень, які не оброблені користувачем
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}