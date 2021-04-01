#include <windows.h>; // підключення бібліотеки з функціями API
#include "resource.h"
#include <string>
#include <ctime>

using namespace std;

// Глобальні змінні:
HINSTANCE hInst; //Дескриптор програми
LPCTSTR szWindowClass = "QWERTY";
LPCTSTR szTitle = "First programm";

#define BUFFERSIZE 2048
#define COPY_FILENAME_BUFFER 16

LPDWORD byte_num;
string lastOpenedFile = "";
CHAR readBuffer[BUFFERSIZE];

// Попередній опис функцій

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK createFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

// Основна програма
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	srand(time(0));
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
	wcex.hbrBackground = GetSysColorBrush(COLOR_WINDOW); //установка фону
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
	HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1));
	hInst = hInstance; //зберігає дескриптор додатка в змінній hInst
	hWnd = CreateWindow(szWindowClass, // ім’я класу вікна
		szTitle, // назва програми
		WS_OVERLAPPEDWINDOW, // стиль вікна
		CW_USEDEFAULT, // положення по Х
		CW_USEDEFAULT, // положення по Y
		600, // розмір по Х
		420, // розмір по Y
		NULL, // дескриптор батьківського вікна
		hMenu, // дескриптор меню вікна
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
	RECT rt;
	OPENFILENAME file;
	HANDLE hFile;
	CHAR buffer[BUFFERSIZE];
	string copyname;

	switch (message)
	{
	case WM_CREATE: //Повідомлення приходить при створенні вікна
		break;
	case WM_PAINT: //Перемалювати вікно
		hdc = BeginPaint(hWnd, &ps); //Почати графічний вивід
		GetClientRect(hWnd, &rt); //Область вікна для малювання

		TextOut(hdc, 10, 16, buffer, wsprintf(buffer, readBuffer));

		EndPaint(hWnd, &ps); //Закінчити графічний вивід
		break;
	case WM_INITMENU:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_CREATE:
			//file = CreateFile(name.c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			//if (file == INVALID_HANDLE_VALUE)
			//{
			//	MessageBox(NULL, "Poshel Nahui", "otsosi", MB_OK);
			//}
			//CloseHandle(file);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)createFile);
			break;
		case ID_FILE_OPEN:
			OPENFILENAME ofn;       // common dialog box structure
			char szFile[256];       // buffer for file name

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
			// use the contents of szFile to initialize itself.
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box. 

			switch (GetOpenFileName(&ofn))
			{
			case FALSE:
				MessageBox(hWnd, "OPENFILENAME Error", "Error", MB_ICONERROR);
				break;
			case TRUE:

				hFile = CreateFile(
					ofn.lpstrFile,
					GENERIC_READ,
					0,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

				if (hFile == INVALID_HANDLE_VALUE)
				{
					MessageBox(hWnd, "Unable to open the file!", "Error", MB_ICONERROR);
					break;
				}

				lastOpenedFile = ofn.lpstrFile;

				if (!ReadFile(hFile, readBuffer, BUFFERSIZE, byte_num, NULL))
				{
					MessageBox(hWnd, "Unable to read the file!", "Error", MB_ICONERROR);
					break;
				}
				else
				{ InvalidateRect(hWnd, NULL, TRUE); }

				CloseHandle(hFile);
				break;
			}

			break;
		case ID_FILE_SAVE:

			hFile = CreateFile(
				"readonly.txt",
				GENERIC_READ,
				0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_READONLY,
				NULL);

			if (hFile == INVALID_HANDLE_VALUE)
			{
				MessageBox(hWnd, "Can't create a file!", "ERROR", MB_ICONERROR | MB_OK);
				break;
			}
			CloseHandle(hFile);
			break;
		case ID_FILE_COPY:
			if (lastOpenedFile.length() <= 0)
			{ 
				MessageBox(hWnd, "Last opened file name is blank", "ERROR", MB_ICONERROR | MB_OK);
				break;
			}
			copyname = lastOpenedFile + "-copy.txt";
			if (!CopyFile((LPCSTR)lastOpenedFile.c_str(), (LPCSTR)copyname.c_str(), FALSE))
			{
				MessageBox(hWnd, "Can't create copy!", "ERROR", MB_ICONERROR | MB_OK);
				break;
			}
		}
		break;
	case WM_DESTROY: //Завершення роботи
		PostQuitMessage(0);
		break;
	default:
		//Обробка повідомлень, які не оброблені користувачем
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


LRESULT CALLBACK createFile(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	CHAR filename[256] = { 0 };
	HWND hEdit;
	HANDLE file;
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDOK:
			hEdit = GetDlgItem(hDlg, IDC_EDIT1);
			

			SendMessage(hEdit, WM_GETTEXT, (WPARAM)255, (LPARAM)filename);

			file = CreateFile(
				(LPCSTR)filename,
				GENERIC_READ,
				0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			if (file == INVALID_HANDLE_VALUE)
			{
				MessageBox(hDlg, "Unable to create file \n", "ERROR",  MB_ICONERROR | MB_OK);
				return TRUE;
			}

			CloseHandle(file);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;

		}
		break;
	}
	return FALSE;
}
