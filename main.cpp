#define UNICODE

#include <windows.h>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <limits>
#include <math.h>

using namespace std;


static HWND sHwnd;
static COLORREF defaultColor = RGB(255, 0, 0); // will be used the default color for a pixel if not specified
static COLORREF backgroundColor = RGB(0, 0, 0);
static int xRes = 500; // image resolution in pixels
static int yRes = 500;


class Vector3{ // stores the coordinates of a Vector3 in 3D space
    public:
        float x, y, z; // Vector3 coordinates

        Vector3() {} // default constructor

        Vector3(float x, float y, float z){
            this->x = x;
            this->y = y;
            this->z = z;
        }

        Vector3 operator + (Vector3 B) {
            return Vector3(this->x + B.x, this->y + B.y, this->z + B.z);
        }

        Vector3 operator - (Vector3 B) {
            return Vector3(B.x - this->x, B.y - this->y, B.z - this->z);
        }

        static float distance(Vector3 A, Vector3 B) { // computes the distance between A and B
            return sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2) + pow(B.z - A.z, 2));
        }

        static float dot(Vector3 A, Vector3 B) { // computes the dot product between A and B
            return A.x * B.x + A.y * B.y + A.z * B.z;
        }

        string toString() {
            return "(" + to_string(this->x) + ", " + to_string(this->y) + ", " + to_string(this->z) + ")";
        }
};

class Sphere{ // a sphere that has a geometry (center, radius) and a color
    public:
        Vector3 center;
        float radius;
        COLORREF color;

        Sphere() {} // default Sphere constructor
    
        Sphere(Vector3 center, float radius, COLORREF color) {
            this->center = center;
            this->radius = radius;
            this->color = color;
        }

        tuple<float, float> intersectRay(Vector3 origin, Vector3 target) {
            /**
             * Compute the intersection Vector3(s) between this and the ray that goes
             * from origin and pass by target
             * 
             * @param origin The origin of the ray
             * @param target The target of the ray
             * @return a couple of floats (t1, t2) containing the intersection Vector3(s)
             *         (returns (infinity, infinity) if there is no intersection)
            */
            Vector3 CO = origin - this->center; // vector from the sphere to the origin
            float a = Vector3::dot(target, target);
            float b = 2 * Vector3::dot(CO, target);
            float c = Vector3::dot(CO, CO) - pow(this->radius, 2);
            float discriminant = pow(b, 2) - 4*a*c;
            if (discriminant < 0) { // case where there is no intersection
                return make_tuple(numeric_limits<float>::infinity(), numeric_limits<float>::infinity());
            }
            float t1, t2;
            t1 = (-b - sqrt(discriminant)) / (2*a); 
            t2 = (-b + sqrt(discriminant)) / (2*a); 
            return make_tuple(t1, t2);
        }

        string toString() {
            return "Center: " + this->center.toString() + ",   radius: " + to_string(this->radius);
        }
};

class Scene { // a scene that contains objects and a projection plane (viewport)
    public:
        Vector3 cameraPos;
        float projPlaneWidth;
        float projPlaneHeight;
        float projPlaneDistance; // controls the inverse camera fov
        vector<Sphere> spheres; // contains all spheres in the scene

        Scene() {} // default Scene constructor

        Scene(Vector3 cameraPos, float projPlaneWidth, float projPlaneHeight, float projPlaneDistance, vector<Sphere> spheres) {
            this->cameraPos = cameraPos;
            this->projPlaneWidth = projPlaneWidth;
            this->projPlaneHeight = projPlaneHeight;
            this->projPlaneDistance = -projPlaneDistance; // needs to be negative for some reason ¯\_(ツ)_/¯
            this->spheres = spheres;
        }

        static Scene getDefaultScene() { // returns a default scene with three spheres
            return Scene(Vector3(0, 0, 0), // camera position
                         1, 1, 1, // projection plane properties
                         { // vector of spheres in the scene
                            Sphere(Vector3(3, 0, 9), 1, RGB(255, 0, 0)),
                            Sphere(Vector3(0, 0, 9), 1, RGB(0, 255, 0)),
                            Sphere(Vector3(-3, 0, 9), 1, RGB(0, 0, 255)),
                            Sphere(Vector3(0, -10001, 0), 10000, RGB(150, 150, 150)) // ground
                         });
        }
};

void SetWindowHandle(HWND hwnd) {
    sHwnd=hwnd;
}

Vector3 screenToProjPlane(Scene scene, int screenX, int screenY) {
    /**
     * Convert a pixel position in the canvas into a 3D viewport position in the projection
     * plane
     * 
     * @param scene The scene that needs to be rendered
     * @param screenX The x coordinate in the canvas
     * @param screenY The y coordinate in the canvas
     * @return a 3-upple of coordinates in the viewport (projection plane in 3D space)
    */
    float vpX = screenX * scene.projPlaneWidth/xRes - scene.projPlaneWidth/2;
    float vpY = screenY * scene.projPlaneHeight/yRes - scene.projPlaneHeight/2;
    float vpZ = scene.projPlaneDistance;
    return Vector3(vpX, vpY, vpZ);
}

COLORREF traceRay(Scene scene, Vector3 origin, Vector3 target, float t_min, float t_max) {
    /**
     * Compute the ray that goes from origin to target and retuns the color of the closest hitpoint with
     * distance betwen t_min and t_max
     * 
     * @param scene The scene to trace the ray in
     * @param origin The ray origin
     * @param target The ray target (where the ray is headed)
     * @param t_min The minimum distance of a hit Vector3
     * @param t_max The maximum distance of a hit Vector3
    */
    float closestDist = numeric_limits<float>::infinity();
    int closestSphereIndex = -1;
    for (int i = 0; i < scene.spheres.size(); i++) {
        float t1, t2;
        tie(t1, t2) = scene.spheres[i].intersectRay(origin, target);
        if (t_min <= t1 && t1 <= t_max && t1 < closestDist) { // found a better valid hitpoint
            closestSphereIndex = i;
            closestDist = t1;
        }
        if (t_min <= t2 && t2 <= t_max && t2 < closestDist) { // found a better valid hitpoint
            closestSphereIndex = i;
            closestDist = t2;
        }
    }
    if (closestSphereIndex == -1) { // case where the ray did not intersect any sphere
        return backgroundColor;
    }
    return scene.spheres[closestSphereIndex].color;
}

COLORREF viewportColor(Scene scene, Vector3 vpPos) {
    Vector3 cameraPos = scene.cameraPos;
    COLORREF color = traceRay(scene,
                              cameraPos, vpPos,
                              1, numeric_limits<float>::infinity());
    return color;
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
    Vector3 viewportPos = screenToProjPlane(scene, x, y);
    COLORREF color = viewportColor(scene, viewportPos);
    return color; // placeholder, replace with color when finished
}


void render(Scene scene) {
    /**
     * Renders a scene (draws every single pixel)
     * 
     * @param scene The scene to render
    */
    for(int x = 0; x < xRes; x++) {
        for(int y = 0; y < yRes; y++) {
            setPixel(x, y, defaultColor); // indicates that the pixel is currently being rendered
            COLORREF color = pixelColor(scene, x, y);
            setPixel(x, y, color);
        }
    }
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