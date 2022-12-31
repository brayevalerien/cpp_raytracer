#define UNICODE

#include <windows.h>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <limits>

using namespace std;


static HWND sHwnd;
static COLORREF defaultColor = RGB(255, 0, 0); // will be used the default color for a pixel if not specified
static COLORREF backgroundColor = RGB(0, 0, 0);
static int xRes = 1200; // image resolution in pixels
static int yRes = 720;
static float projPlaneHeight = 2880;
static float projPlaneWidth = 4800;
static float projPlaneDistance = 1; // distance between the projection plane and the camera


class Point{ // stores the coordinates of a point in 3D space
    public:
        float x, y, z; // point coordinates

        Point() {} // default constructor

        Point(float x, float y, float z){
            this->x = x;
            this->y = y;
            this->z = z;
        }
};

class Sphere{ // a sphere that has a geometry (center, radius) and a color
    public:
        Point center;
        float radius;
        COLORREF color;

        Sphere() {} // default Sphere constructor
    
        Sphere(Point center, float radius, COLORREF color) {
            this->center = center;
            this->radius = radius;
            this->color = color;
        }    
};

class Scene { // a scene that contains objects and a projection plane (viewport)
    public:
        Point cameraPos;
        float projPlaneWidth;
        float projPlaneHeight;
        float projPlaneDistance; // controls the inverse camera fov
        vector<Sphere> spheres; // contains all spheres in the scene

        Scene() {} // default Scene constructor

        Scene(Point cameraPos, float projPlaneWidth, float projPlaneHeight, float projPlaneDistance, vector<Sphere> spheres) {
            this->cameraPos = cameraPos;
            this->projPlaneWidth = projPlaneWidth;
            this->projPlaneHeight = projPlaneHeight;
            this->projPlaneDistance = projPlaneDistance;
            this->spheres = spheres;
        }

        static Scene getDefaultScene() { // returns a default scene with three spheres
            return Scene(Point(0, 0, 0), // camera position
                         9, 6, 1, // projection plane properties
                         { // vector of spheres in the scene
                            Sphere(Point(0, -1, 3), 1.0, RGB(255, 0, 0)),
                            Sphere(Point(2, 0, 4), 1, RGB(0, 0, 255)),
                            Sphere(Point(-2, 0, 4), 1, RGB(0, 255, 0))
                         });
        }
};

void SetWindowHandle(HWND hwnd) {
    sHwnd=hwnd;
}


Point screenToProjPlane(Scene scene, int screenX, int screenY) {
    /**
     * Convert a pixel position in the canvas into a 3D viewport position in the projection
     * plane
     * 
     * @param scene The scene that needs to be rendered
     * @param screenX The x coordinate in the canvas
     * @param screenY The y coordinate in the canvas
     * @return a 3-upple of coordinates in the viewport (projection plane in 3D space)
    */
    float vpX = screenX * scene.projPlaneWidth/xRes;
    float vpY = screenY * scene.projPlaneHeight/yRes;
    float vpZ = scene.projPlaneDistance;
    return Point(vpX, vpY, vpZ);
}

COLORREF traceRay(Scene scene, Point origin, Point target, float t_min, float t_max) {
    /**
     * Compute the ray that goes from origin to target and retuns the color of the closest hitpoint with
     * distance betwen t_min and t_max
     * 
     * @param scene The scene to trace the ray in
     * @param origin The ray origin
     * @param target The ray target (where the ray is headed)
     * @param t_min The minimum distance of a hit point
     * @param t_max The maximum distance of a hit point
    */
    float closestDist = numeric_limits<float>::infinity();
    Sphere closestSphere;

    // TODO: translate the following pseudocode
    // for sphere in scene.spheres {
    //     t1, t2 = IntersectRaySphere(O, D, sphere)
    //     if t1 in [t_min, t_max] and t1 < closest_t {
    //         closest_t = t1
    //         closest_sphere = sphere
    //     }
    //     if t2 in [t_min, t_max] and t2 < closest_t {
    //         closest_t = t2
    //         closest_sphere = sphere
    //     }
    // }
    // if closest_sphere == NULL {
    //    ❶return BACKGROUND_COLOR
    // }
    // return closest_sphere.color

    for (Sphere sphere: scene.spheres) {
        
    }
}

COLORREF viewportColor(Scene scene, Point vpPos) {
    float r, g, b;
    Point cameraPos = scene.cameraPos;
    COLORREF color = traceRay(scene,
                              cameraPos, vpPos,
                              1, numeric_limits<float>::infinity());
    return RGB(r, g, b);
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

const COLORREF pixelColor(Scene scene, int x, int y) {
    /**
     * Computes the color of a single pixel in the final image
     * 
     * @param scene The scene that needs to be rendered
     * @param x The pixel x coordinate in the final image
     * @param y The pixel y coordinate in the final image
     * @return The color of the pixel at position x, y in the final image 
    */
    Point viewportPos = screenToProjPlane(scene, x, y);
    COLORREF color = viewportColor(scene, viewportPos);
    return RGB(x%256, y%256, 0); // placeholder, replace with color when finished
}


void render(Scene scene) {
    /**
     * Renders a scene (draws every single pixel)
     * 
     * @param scene The scene to render
    */
    for(int x = 0; x < xRes; x++) {
        for(int y = 0; y < yRes; y++) {
            COLORREF color = pixelColor(scene, x, y);
            setPixel(x, y, color);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam) {
    switch(message) {
    case WM_PAINT:
        SetWindowHandle(hwnd);
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