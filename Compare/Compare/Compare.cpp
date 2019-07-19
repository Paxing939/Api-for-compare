#include "framework.h"
#include "Commdlg.h"
#include "Compare.h"
#include "opencv2/opencv.hpp"
#include <string>
#pragma warning(disable : 4996)
using namespace cv;
using std::string;

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
int CtoWCoding(char* CStr, wchar_t* WStr);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_COMPARE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_COMPARE));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_COMPARE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_COMPARE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


char szFile[200] = "D:\\Work_25\\ТЕСТОВОЕ ЗАДАНИЕ\\ImjProc3\\Milex-2019_Moment3.jpg";
char szFile1[200] = "D:\\Work_25\\ТЕСТОВОЕ ЗАДАНИЕ\\ImjProc3\\Milex-2019_Moment3.jpg";
wchar_t szWCFile[200];
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hDC, hCompatibleDC, hCompatibleDC1;
	PAINTSTRUCT PaintStruct;
	HANDLE hBitmap, hOldBitmap, hBitmap1, hOldBitmap1;
	RECT Rect;
	BITMAP Bitmap;
	static int nHDif = 0, nVDif = 0, nHPos = 0, nVPos = 0;
	int wmId = 0, wmEvent;
	OPENFILENAME Ofn;
	char str[200];
	char szFileTitle[20] = "\0";
	char szFilter[] = "Образ (*.bmp)\0*.bmp\0";
	char szDefExt[] = ".bmp";
	int StrLen;
	bool use_mask = false;
	Mat img, templ, mask, result, img_display, part;
	bool method_accepts_mask;
	const char* image_window = "Source Image";
	const char* result_window = "Result window";
	int match_method = 0;
	double minVal; double maxVal; Point minLoc; Point maxLoc; Point matchLoc;
	int result_cols = 0;
	int result_rows = 0;
	int sum = 0, counter = 0;
	string fileName;
	switch (message) {
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_OPEN:
			memset(&Ofn, 0, sizeof(OPENFILENAME));
			Ofn.lStructSize = sizeof(OPENFILENAME);
			Ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
			Ofn.hwndOwner = hWnd;
			Ofn.lpstrDefExt = szDefExt;
			Ofn.lpstrFilter = szFilter;
			Ofn.nFilterIndex = 1;
			Ofn.lpstrFile = szFile;
			Ofn.nMaxFile = sizeof(szFile);
			Ofn.lpstrFileTitle = szFileTitle;
			Ofn.nMaxFileTitle = sizeof(szFileTitle);
			Ofn.lpstrInitialDir = NULL;
			if (!GetOpenFileName(&Ofn)) return 0;
			wsprintf(str, "Образ - [%s]", szFileTitle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_FIND_MATCH:
			templ = imread(szFile);
			for (int i = 2; i <= 30; i++)
			{
				fileName = "D:\\base\\GE-";
				fileName += std::to_string(i);
				fileName += ".bmp";
				img = imread(fileName);
				if (img.empty() || templ.empty() || (use_mask && mask.empty()) && false)
				{
					MessageBox(hWnd, "Can't read one of the images", "Error", 0);
					return -1;
				}
				img.copyTo(img_display);
				result_cols = img.cols - templ.cols + 1;
				result_rows = img.rows - templ.rows + 1;
				result.create(result_rows, result_cols, CV_32FC1);
				method_accepts_mask = (TM_SQDIFF == match_method || match_method == TM_CCORR_NORMED);
				matchTemplate(img, templ, result, 0);
				if (result.empty())
					continue;

				normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());
				minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
				if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
				{
					matchLoc = minLoc;
				}
				else
				{
					matchLoc = maxLoc;
				}
				img_display.copyTo(img);
				part = img_display({ matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows) });
				absdiff(part, templ, part);
				for (int j = 75; j < 100; j++)
					for (int k = 75; k < 100; k++)
					{
						sum += part.at<Vec3b>(j, k)[0];
						if (part.at<Vec3b>(j, k)[0] == 0)
							counter++;
					}
				if (sum < 8000)
					break;
				sum = 0;
				counter = 0;
			}
			rectangle(img, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
			imwrite("D:\\g.bmp", img);
			strcpy(szFile1, "D:\\g.bmp");
			//strcpy(szFile, fileName.c_str());
			InvalidateRect(hWnd, NULL, TRUE);
			break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		/* получить контекст устройства */
		hDC = BeginPaint(hWnd, &PaintStruct);
		/* загрузить bitmap который будет отображаться в окне из файла */
		hBitmap = LoadImage(NULL, szFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hBitmap1 = LoadImage(NULL, szFile1, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		/*if (!hBitmap) {
			MessageBox(NULL, "File not found!", "Error", MB_OK);
			return 0;
		}*/
		/* получть размерность изображения */
		GetObject(hBitmap, sizeof(BITMAP), &Bitmap);
		/* создать совместимый с контекстом окна контекст в памяти */
		hCompatibleDC = CreateCompatibleDC(hDC);
		hCompatibleDC1 = CreateCompatibleDC(hDC);
		/* делаем загруженный битмап текущим в совместимом контексте */
		hOldBitmap = SelectObject(hCompatibleDC, hBitmap);
		hOldBitmap = SelectObject(hCompatibleDC1, hBitmap1);
		/* определить размер рабочей области окна */
		GetClientRect(hWnd, &Rect);
		/* версия без прокрутки
		* копировать битмап с совместимого на основной контекст устройства с масштабированием
		StretchBlt(hDC, 0, 0, Rect.right, Rect.bottom, hCompatibleDC, 0, 0, Bitmap.bmWidth,
			Bitmap.bmHeight, SRCCOPY);
		*/
		/* версия с прокруткой */
		BitBlt(hDC, 0, 0, Rect.right, Rect.bottom, hCompatibleDC, nHPos, nVPos, SRCCOPY);
		BitBlt(hDC, 1000, 0, Rect.right, Rect.bottom, hCompatibleDC1, nHPos, nVPos, SRCCOPY);
		if ((nHDif = (Bitmap.bmWidth - Rect.right)) > 0)
			SetScrollRange(hWnd, SB_HORZ, 0, nHDif, TRUE);
		else
			SetScrollRange(hWnd, SB_HORZ, 0, 0, TRUE);
		if ((nVDif = (Bitmap.bmHeight - Rect.bottom)) > 0)
			SetScrollRange(hWnd, SB_VERT, 0, nVDif, TRUE);
		else
			SetScrollRange(hWnd, SB_VERT, 0, 0, TRUE);
		/* вновь делаем старый битмап текущим */
		SelectObject(hCompatibleDC, hOldBitmap);
		/* удалить загруженный битмап */
		DeleteObject(hBitmap);
		/* удалить совместимый контекст */
		DeleteDC(hCompatibleDC);
		/* освободить основной контекст, завершая перерисовку рабочей области окна */
		EndPaint(hWnd, &PaintStruct);
		return 0;
	case WM_VSCROLL:
		switch (LOWORD(wParam)) {
		case SB_LINEDOWN:
			if (nVPos < nVDif)
				++nVPos;
			break;
		case SB_LINEUP:
			if (nVPos > 0)
				--nVPos;
			break;
		case SB_THUMBTRACK:
			nVPos = HIWORD(wParam);
			break;
		}
		SetScrollPos(hWnd, SB_VERT, nVPos, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_HSCROLL:
		switch (LOWORD(wParam)) {
		case SB_LINEDOWN:
			if (nHPos - nHDif)
				++nHPos;
			break;
		case SB_LINEUP:
			if (nHPos > 0)
				--nHPos;
			break;
		case SB_THUMBTRACK:
			nHPos = HIWORD(wParam);
			break;
		}
		SetScrollPos(hWnd, SB_HORZ, nHPos, TRUE);
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Обработчик сообщений для окна "О программе".
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

int CtoWCoding(char* CStr, wchar_t* WStr)
{
	int i;
	for (i = 0; i < 200 && CStr[i] != 0; i++)
	{
		WStr[i] = ((wchar_t)CStr[i]) & 0xff;
		if (WStr[i] > L'z') WStr[i] += 848;
	}
	WStr[i] = 0;
	return i;
}