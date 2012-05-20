/////////////////////////////////////////////////////////////////////////
// gedmanage.cpp : Defines the entry point for the application.
/////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gedmanage.h"
#include "gedcompare.h"
#include <windows.h>
#include <Commdlg.h>
#define MAX_LOADSTRING 100

extern int gedCompare(void);
extern int gedMerge(void);

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

/////////////////////////////////////////////////////////////////////////
//BOOL SaveFile(HWND hEdit, LPSTR pszFileName)
/////////////////////////////////////////////////////////////////////////

BOOL SaveFile(HWND hEdit, LPSTR pszFileName)
{
   HANDLE hFile;
   BOOL bSuccess = FALSE;

   hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
   if(hFile != INVALID_HANDLE_VALUE)
   {
      DWORD dwTextLength;
      dwTextLength = GetWindowTextLength(hEdit);
      if(dwTextLength > 0)// No need to bother if there's no text.
      {
         LPSTR pszText;
         pszText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
         if(pszText != NULL)
         {
            if(GetWindowText(hEdit, pszText, dwTextLength + 1))
            {
               DWORD dwWritten;
               if(WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
                  bSuccess = TRUE;
            }
            GlobalFree(pszText);
         }
      }
      CloseHandle(hFile);
   }
   return bSuccess;
}

/////////////////////////////////////////////////////////////////////////
//BOOL LoadFile(HWND hEdit, LPSTR pszFileName)
/////////////////////////////////////////////////////////////////////////

BOOL LoadFile(HWND hEdit, LPSTR pszFileName)
{
   HANDLE hFile;
   BOOL bSuccess = FALSE;

   hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
   if(hFile != INVALID_HANDLE_VALUE)
   {
      DWORD dwFileSize;
      dwFileSize = GetFileSize(hFile, NULL);
      if(dwFileSize != 0xFFFFFFFF)
      {
         LPSTR pszFileText;
         pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
         if(pszFileText != NULL)
         {
            DWORD dwRead;
            if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
            {
               pszFileText[dwFileSize] = 0; // Null terminator
               if(SetWindowText(hEdit, pszFileText))
                  bSuccess = TRUE; // It worked!
            }
            GlobalFree(pszFileText);
         }
      }
      CloseHandle(hFile);
   }
   return bSuccess;
}

/////////////////////////////////////////////////////////////////////////
//BOOL DoFileOpenSave(HWND hwnd, BOOL bSave)
/////////////////////////////////////////////////////////////////////////

BOOL DoFileOpenSave(HWND hwnd, BOOL bSave)
{
   OPENFILENAME ofn1;
   OPENFILENAME ofn2;
   char szFileName1[MAX_PATH];
   char szFileName2[MAX_PATH];

   ZeroMemory(&ofn1, sizeof(ofn1));
   szFileName1[0] = 0;
   ZeroMemory(&ofn2, sizeof(ofn2));
   szFileName2[0] = 0;

   ofn1.lStructSize = sizeof(ofn1);
   ofn1.hwndOwner = hwnd;
   ofn1.lpstrFilter = "GEDcom Files (*.ged)\0*.ged\0All Files (*.*)\0*.*\0\0";
   ofn1.lpstrFile = szFileName1;
   ofn1.nMaxFile = MAX_PATH;
   ofn1.lpstrDefExt = "ged";
   {
      ofn1.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
      if(GetOpenFileName(&ofn1))
      {
         if(!LoadFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName1))
         {
            MessageBox(hwnd, "Load of file 1 failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
            return FALSE;
         }
      }
   }
   ofn2.lStructSize = sizeof(ofn2);
   ofn2.hwndOwner = hwnd;
   ofn2.lpstrFilter = "GEDcom Files (*.ged)\0*.ged\0All Files (*.*)\0*.*\0\0";
   ofn2.lpstrFile = szFileName2;
   ofn2.nMaxFile = MAX_PATH;
   ofn2.lpstrDefExt = "ged";
   {
      ofn2.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
      if(GetOpenFileName(&ofn2))
      {
         if(!LoadFile(GetDlgItem(hwnd, IDC_MAIN_TEXT), szFileName2))
         {
            MessageBox(hwnd, "Load of file 2 failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
            return FALSE;
         }
      }
   }
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////
//int APIENTRY _tWinMain
/////////////////////////////////////////////////////////////////////////

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GEDMANAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GEDMANAGE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


/////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
/////////////////////////////////////////////////////////////////////////

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GEDMANAGE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GEDMANAGE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

/////////////////////////////////////////////////////////////////////////
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
/////////////////////////////////////////////////////////////////////////

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

/////////////////////////////////////////////////////////////////////////
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
/////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int retVal;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_COMPARE:
//			MessageBox(hWnd, "Compare called","Compare",MB_OK);
			retVal = gedCompare();
			if (retVal == NOERR)
				MessageBox(hWnd, "File Compare completed","Success...",MB_OK);
			else if (retVal == ERRORLOADINGGED1)
				MessageBox(hWnd, "Error loading file 1","ERROR",MB_OK);
			else if (retVal == ERRORLOADINGGED2)
				MessageBox(hWnd, "Error loading file 2","ERROR",MB_OK);
			else if (retVal == MATCH1STERR)
				MessageBox(hWnd, "Error matching first person between the two trees","ERROR",MB_OK);
			else if (retVal == OPENBRANCHFOUND)
				MessageBox(hWnd, "Open branch was found","ERROR",MB_OK);
			else if (retVal == NOHEADER1)
				MessageBox(hWnd, "File 1 did not have a header on the first line","ERROR",MB_OK);
			else if (retVal == NOHEADER2)
				MessageBox(hWnd, "File 2 did not have a header on the first line","ERROR",MB_OK);
			else
				MessageBox(hWnd, "Unknown error","ERROR",MB_OK);
			break;
		case IDM_MERGE:
			MessageBox(hWnd, "Merge called","Merge",MB_OK);
			retVal = gedMerge();
			if (retVal != NOERR)
				MessageBox(hWnd, "Merge error","ERROR",MB_OK);
			break;
		case IDM_SELECT_FILES:
			MessageBox(hWnd, "Select Files","Select files",MB_OK);
			DoFileOpenSave(hWnd, 0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
/////////////////////////////////////////////////////////////////////////

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
