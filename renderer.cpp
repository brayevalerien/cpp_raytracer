#define UNICODE

#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <tuple>
#include <limits>
#include <windows.h>

// custom files:
#include <Vector3.cpp> // for 3D vectors
#include <Object.cpp> // for Objects and primitives (Sphere, Box, Triangle...)
#include <Scene.cpp> // for 3D scenes (containing objects, lights, a camera...)

using namespace std;

// global variables definition
static HWND sHwnd; // window used for rendering
static int RESOLUTION_X = 500; // finale image
static int RESOLUTION_Y = 500; // resolution in pixels
static COLORREF DEFAULT_COLOR = RGB(255, 0, 0);
static COLORREF BACKGROUND_COLOR = RGB(0, 0, 0);


void SetWindowHandle(HWND hwnd) {
    sHwnd=hwnd;
}

void setPixel(int x,int y, const COLORREF& color=DEFAULT_COLOR) {
    if(sHwnd==NULL) {
        MessageBox(NULL, L"sHwnd was not initialized !", L"Error",MB_OK|MB_ICONERROR);
        exit(0);
    }
    HDC hdc=GetDC(sHwnd);
    SetPixel(hdc,x,y,color);
    ReleaseDC(sHwnd,hdc);
    return;
}


void render(Scene: scene) {
    /**
     * TODO
    */
   cout << "DONE" << endl;
}



LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam) {
    switch(message) {
    case WM_PAINT:
        SetWindowHandle(hwnd);
        std::cout << "Starting rendering process..." << std::endl;
        render(Scene::getDefaultScene());
        cout << "Rendering complete." << endl;
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow) {
    static TCHAR szAppName[] = TEXT("3D Renderer");
    WNDCLASS wndclass;
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = WndProc;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;
    
    // Register the window
    if(!RegisterClass(& wndclass)) {
        MessageBox(NULL, L"Registering the class failed", L"Error",MB_OK|MB_ICONERROR);
        exit(0);
    }
    
    // CreateWindow
    HWND hwnd=CreateWindow(szAppName,L"cpp raytracer",
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        RESOLUTION_X, // window size equals
                        RESOLUTION_Y, // to the render resolution
                        NULL,
                        NULL,
                        hInstance,
                        NULL);
    if(!hwnd) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error",MB_OK);
        exit(0);
    }
    
    // ShowWindow and UpdateWindow
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);
    
    // Message Loop
    MSG msg;
    while(GetMessage(& msg, NULL, 0, 0)) {
        TranslateMessage(& msg);
        DispatchMessage(& msg);
    }

    return 0;
}