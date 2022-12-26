#define UNICODE

#include <windows.h>
#include <iostream>
#include <string>

static HWND sHwnd;
static COLORREF redColor=RGB(255,0,0);
static COLORREF blueColor=RGB(0,0,255);
static COLORREF greenColor=RGB(0,255,0);
static int xRes = 1200; 
static int yRes = 720; 

void SetWindowHandle(HWND hwnd)
{
    sHwnd=hwnd;
}

void setPixel(int x,int y,COLORREF& color=redColor)
{
    if(sHwnd==NULL)
    {
        MessageBox(NULL, L"sHwnd was not initialized !", L"Error",MB_OK|MB_ICONERROR);
        exit(0);
    }
    HDC hdc=GetDC(sHwnd);
    SetPixel(hdc,x,y,color);
    ReleaseDC(sHwnd,hdc);
    return;
}

void render()
{
    for(int x = 0; x < xRes; x++) {
        for(int y = 0; y < yRes; y++) {
            COLORREF pixelColor = RGB(x%256, y%256, 0);
            setPixel(x, y, pixelColor);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch(message)
    {
    case WM_PAINT:
        SetWindowHandle(hwnd);
        render();
        break;
    case WM_CLOSE: // Failure to call DefWindowProc
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break; // Failure to call DefWindowProc //
    }
    return DefWindowProc(hwnd,message,wParam,lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("Test");
    WNDCLASS wndclass;
    wndclass.style         = CS_HREDRAW|CS_VREDRAW ;
    wndclass.lpfnWndProc   = WndProc ;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = hInstance ;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
    wndclass.lpszMenuName  = NULL ;
    wndclass.lpszClassName = szAppName ;
    
    // Register the window
    if(!RegisterClass(& wndclass))
    {
        MessageBox(NULL, L"Registering the class failed", L"Error",MB_OK|MB_ICONERROR);
        exit(0);
    }
    
    // CreateWindow
    HWND hwnd=CreateWindow(szAppName,L"cpp raytracer",
                           WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           NULL,
                           NULL,
                           hInstance,
                           NULL);
    if(!hwnd)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error",MB_OK);
        exit(0);
    }
    
    // ShowWindow and UpdateWindow
    ShowWindow(hwnd,iCmdShow);
    UpdateWindow(hwnd);
    
    // Message Loop
    MSG msg;
    while(GetMessage(& msg,NULL,0,0))
    {
        TranslateMessage(& msg);
        DispatchMessage(& msg);
    }
    
    return 0;
}