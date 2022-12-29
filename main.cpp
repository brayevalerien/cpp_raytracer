#define UNICODE

#include <windows.h>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;


static HWND sHwnd;
static COLORREF defaultColor = RGB(255, 0, 0); // will be used the default color for a pixel if not specified
static int xRes = 1200; // image resolution in pixels
static int yRes = 720;
static float projPlaneHeight = 2880;
static float projPlaneWidth = 4800;
static float projPlaneDistance = 1; // distance between the projection plane and the camera


class Sphere{
    public:
        tuple<float, float, float> center;
        float radius;
        COLORREF color;
    
        Sphere(tuple<float, float, float> center, float radius, COLORREF color) {
            this->center = center;
            this->radius = radius;
            this->color = color;
        }    
};

class Scene { // a scene that contains objects and a projection plane (viewport)
    public:
        // Projection plane properties
        float projPlaneWidth;
        float projPlaneHeight;
        float projPlaneDistance;
        vector<Sphere> spheres; // contains all spheres in the scene

        Scene(float projPlaneWidth, float projPlaneHeight, float projPlaneDistance, vector<Sphere> spheres) {
            this->projPlaneWidth = projPlaneWidth;
            this->projPlaneHeight = projPlaneHeight;
            this->projPlaneDistance = projPlaneDistance;
            this->spheres = spheres;
        }

        Scene getDefaultScene() { // returns a default scene with three spheres
            return Scene(1, 1, 1,
                         {
                            Sphere(make_tuple(0, -1, 3), 1.0, RGB(255, 0, 0)),
                            Sphere(make_tuple(2, 0, 4), 1, RGB(0, 0, 255)),
                            Sphere(make_tuple(-2, 0, 4), 1, RGB(0, 255, 0))
                         });
        }
};

void SetWindowHandle(HWND hwnd) {
    sHwnd=hwnd;
}

void setPixel(int x,int y, const COLORREF& color=defaultColor) {
    if(sHwnd==NULL) {
        MessageBox(NULL, L"sHwnd was not initialized !", L"Error",MB_OK|MB_ICONERROR);
        exit(0);
    }
    HDC hdc=GetDC(sHwnd);
    SetPixel(hdc,x,y,color);
    ReleaseDC(sHwnd,hdc);
    return;
}

tuple<float, float, float> canvasToViewport(int canvasX, int canvasY) {
    /**
     * Convert a pixel position in the canvas into a 3D viewport position
     * 
     * @param canvasX The x coordinate in the canvas
     * @param canvasY The y coordinate in the canvas
     * @return a 3-upple of coordinates in the viewport
    */
    float vpX = canvasX * projPlaneWidth/xRes;
    float vpY = canvasY * projPlaneHeight/yRes;
    float vpZ = projPlaneDistance;
    return make_tuple(vpX, vpY, vpZ);
}

tuple<int, int, int> viewportColor(float vpX, float vpY, float vpZ) {
    float r, g, b;
    // TODO write the code that computes the difuse color at the viewport position 
    return make_tuple(r, g, b);
}

const COLORREF pixelColor(int x, int y) {
    // the following two lines get the 3D positon of the pixel x, y in the viewport
    float vpX, vpY, vpZ;
    tie(vpX, vpY, vpZ) = canvasToViewport(x, y);
    // then get the color at the 3D point
    int r, g, b;
    tie(r, g, b) = viewportColor(vpX, vpY, vpZ);    
    return RGB(x%256, y%256, 0);
}


void render() {
    for(int x = 0; x < xRes; x++) {
        for(int y = 0; y < yRes; y++) {
            COLORREF color = pixelColor(x, y);
            setPixel(x, y, color);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam) {
    switch(message) {
    case WM_PAINT:
        SetWindowHandle(hwnd);
        render();
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
                        WS_POPUP, // fixed size, pined to the top left of the screen
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        xRes, // window size equals
                        yRes, // to the render resolution
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