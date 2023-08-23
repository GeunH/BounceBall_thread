#include <windows.h>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void DoCreateMain(HWND hWnd);
void DoButtonMain(HWND hWnd);
void DoPaintMain(HWND hWnd);
void OnTimer(HWND hWnd);

HANDLE hMutex;

#define R 20
#define BOUNCE 1

HBITMAP hBit;
HINSTANCE g_hInst;
HWND hWnd, hWnd2;
int i = 0;
HANDLE hThread;
LPCTSTR lpszClass1 = TEXT("Bounce3");

RECT crt;
int x, y;
int xv, yv;
int flag = 0;
int btnflag = 0;
int turn_flag = 0;


typedef struct trans_struct {
    int x;
    int y;
    int xv;
    int yv;
}data;

void moveBall();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {

    HWND hWnd;
    MSG Message1;
    WNDCLASS WndClass;
    g_hInst = hInstance;

    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hInstance = hInstance;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.lpszClassName = lpszClass1;
    WndClass.lpszMenuName = NULL;
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&WndClass);

    hWnd = CreateWindow(lpszClass1, lpszClass1, WS_BORDER | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 600,
        NULL, (HMENU)NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);

    while (GetMessage(&Message1, 0, 0, 0)) {
        TranslateMessage(&Message1);
        DispatchMessage(&Message1);
    }
    return (int)Message1.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    COPYDATASTRUCT* pcds;
    switch (iMessage) {
    case WM_COPYDATA:
        pcds = lParam;
        data Data;
        x = ((data*)pcds->lpData)->x;
        y = ((data*)pcds->lpData)->y;
        xv = ((data*)pcds->lpData)->xv;
        yv = ((data*)pcds->lpData)->yv;
        turn_flag = 1;
        break;
    case WM_CREATE:
        DoCreateMain(hWnd);
        break;
    case WM_PAINT:
        DoPaintMain(hWnd);
        break;
    case WM_LBUTTONDOWN:
        DoButtonMain(hWnd);
        break;
    case WM_TIMER:
        OnTimer(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        KillTimer(hWnd, 0);
        CloseHandle(hThread);
        return 0;

    }
    return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

DWORD WINAPI ThreadMovingBall(LPVOID arg)
{
    Sleep(3000);
    while (1)
    {
        WaitForSingleObject(hMutex, INFINITE);
        Sleep(30);
        if (turn_flag == 1)
        {
            moveBall();  //공의 움직임
        }
        ReleaseMutex(hMutex);
    }
    return 0;
}

void DoCreateMain(HWND hWnd)
{
    GetClientRect(hWnd, &crt);

    x = 100;   //공의 x 좌표 초기값
    y = 250;//공의 y 좌표 초기값

    xv = (rand() % 4) + 5; //x벡터 속도
    yv = (rand() % 4) + 5; //y벡터 속도

    HANDLE hThreadMovingBall = CreateThread(NULL, 0, ThreadMovingBall, (PVOID)hWnd, 0, NULL);
    CloseHandle(hThreadMovingBall);
}

void DoPaintMain(HWND hWnd)
{
    HDC hdc, hMemDC;
    PAINTSTRUCT ps;
    HBITMAP OldBit;

    hdc = BeginPaint(hWnd, &ps);

    hMemDC = CreateCompatibleDC(hdc);
    OldBit = (HBITMAP)SelectObject(hMemDC, hBit);
    BitBlt(hdc, 0, 0, crt.right, crt.bottom, hMemDC, 0, 0, SRCCOPY);

    SelectObject(hMemDC, OldBit);
    DeleteDC(hMemDC);
    EndPaint(hWnd, &ps);

    return;
}

void DoButtonMain(HWND hWnd)
{
    if (btnflag == 0) {
        MessageBox(hWnd, TEXT("3번 프로그램이 실행되었습니다."), TEXT("OK"), MB_OK);
        SetTimer(hWnd, 1, 25, NULL);
    }
    btnflag = 1;
}


void OnTimer(HWND hWnd)
{
    if (turn_flag == 1)
    {
        TCHAR str[123];
        HDC hdc, hMemDC;
        HBITMAP OldBit;
        HPEN hPen, OldPen;
        HBRUSH hBrush, OldBrush;

        hdc = GetDC(hWnd);
        GetClientRect(hWnd, &crt);
        if (hBit == NULL)
        {
            hBit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
        }
        hMemDC = CreateCompatibleDC(hdc);
        OldBit = (HBITMAP)SelectObject(hMemDC, hBit);

        FillRect(hMemDC, &crt, GetSysColorBrush(COLOR_WINDOW));

        hPen = CreatePen(PS_INSIDEFRAME, 2, RGB(0, 0, 0));
        OldPen = (HPEN)SelectObject(hMemDC, hPen);
        hBrush = CreateSolidBrush(RGB(255, 120, 0));
        OldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
        Ellipse(hMemDC, x - R, y - R, x + R, y + R);    //공그리기


        hBrush = CreateSolidBrush(RGB(255, 255, 0));
        OldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

        DeleteObject(SelectObject(hMemDC, OldPen));
        DeleteObject(SelectObject(hMemDC, OldBrush));

        wsprintf(str, TEXT("%d, %d, %d, %d"), x, y, xv, yv);
        TextOut(hMemDC, 100, 50, str, lstrlen(str));

        SelectObject(hMemDC, OldBit);
        DeleteDC(hMemDC);
        ReleaseDC(hWnd, hdc);
        InvalidateRect(hWnd, NULL, FALSE);
    }
}

void moveBall()
{
    if (y <= R || y >= crt.bottom - R) {
        yv *= -(BOUNCE);
        if (y <= R) y = R;
        if (y >= crt.bottom - R) y = crt.bottom - R;
    }

    if (x <= -R && turn_flag == 1)
    {
        COPYDATASTRUCT pcds;
        data Data = { crt.right -  1,y+yv, xv, yv };
        pcds.dwData = &Data;
        pcds.cbData = sizeof(Data);
        pcds.lpData = (LPVOID)&Data;
        HWND* hWMd = FindWindow(NULL, L"Bounce2");
        SendMessage(hWMd, WM_COPYDATA, NULL, (LPARAM)&pcds);

        turn_flag = 0;

        x += R / 2;
        xv *= (-BOUNCE);
    }

    else if (x >= crt.right - R)
    {
        x -= R / 2;
        xv *= (-BOUNCE);
    }
    x += (int)xv;
    y += (int)yv;
}