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
        
        Vector3 operator * (float scalar) {
            return Vector3(this->x * scalar, this->y * scalar, this->z * scalar);
        }

        Vector3 operator / (float scalar) {
            return Vector3(this->x / scalar, this->y / scalar, this->z / scalar);
        }

        bool operator == (Vector3 B) {
            return this->x == B.x && this->y == B.y && this->z == B.z;
        }

        static float distance(Vector3 A, Vector3 B) { // computes the distance between A and B
            return sqrt(pow(B.x - A.x, 2) + pow(B.y - A.y, 2) + pow(B.z - A.z, 2));
        }

        static float dot(Vector3 A, Vector3 B) { // computes the dot product between A and B
            return A.x * B.x + A.y * B.y + A.z * B.z;
        }

        static float norm(Vector3 A) {
            return sqrt(pow(A.x, 2) + pow(A.y, 2) + pow(A.z, 2));
        }

        static Vector3 normalize(Vector3 A) {
            return A/Vector3::norm(A);
        }

        string toString() {
            return "(" + to_string(this->x) + ", " + to_string(this->y) + ", " + to_string(this->z) + ")";
        }
};

class Light {
    public:
        string type;
        float intensity;
        Vector3 position, direction;

        Light() {} // default constructor

        Light(string type, float intensity, Vector3 position, Vector3 direction) {
            this->type = type;
            this->intensity = intensity;
            this->position = position;
            this->direction = direction;
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

        tuple<float, float, Vector3, Vector3, Vector3, Vector3> intersectRay(Vector3 origin, Vector3 target) {
            /**
             * Compute the intersection Vector3(s) between this and the ray that goes
             * from origin and pass by target
             * 
             * @param origin The origin of the ray
             * @param target The target of the ray
             * @return a tupple (t1, t2, H1, H2; N1, N2) of a couple of floats containing the intersection
             *         (returns (infinity, infinity) if there is no intersection), a couple of Vector3
             *         that are the hipoints respectivly at t1 and t2 and finally a couple of Vector3
             *         that are the normals of the hitpoints (0 if no intersection)
            */
            Vector3 CO = origin - this->center; // vector from the sphere to the origin
            Vector3 rayDir = Vector3::normalize(origin - target); // direction of the ray
            float a = Vector3::dot(rayDir, rayDir);
            float b = 2 * Vector3::dot(CO, rayDir);
            float c = Vector3::dot(CO, CO) - pow(this->radius, 2);
            float discriminant = pow(b, 2) - 4*a*c;
            if (discriminant < 0) { // case where there is no intersection
                return make_tuple(numeric_limits<float>::infinity(), numeric_limits<float>::infinity(), Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
            }
            float t1 = (-b + sqrt(discriminant)) / (2*a);
            float t2 = (-b - sqrt(discriminant)) / (2*a);
            Vector3 H1 = rayDir * t1; // hitpoint 1
            Vector3 H2 = rayDir * t2; // hitpoint 2
            Vector3 N1 = Vector3::normalize(H1 - this->center); // normal at H1
            Vector3 N2 = Vector3::normalize(H2 - this->center); // normal at H2
            return make_tuple(t1, t2, H1, H2, N1, N2);
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
        vector<Light> lights; // contains all lights in the scene

        Scene() {} // default Scene constructor

        Scene(Vector3 cameraPos, float projPlaneWidth, float projPlaneHeight, float projPlaneDistance, vector<Sphere> spheres, vector<Light> lights) {
            this->cameraPos = cameraPos;
            this->projPlaneWidth = projPlaneWidth;
            this->projPlaneHeight = projPlaneHeight;
            this->projPlaneDistance = -projPlaneDistance; // needs to be negative for some reason ??\_(???)_/??
            this->spheres = spheres;
            this->lights = lights;
        }

        static Scene getDefaultScene() { // returns default scene with three spheres and a ground
            // the whole scene is hardcoded
            return Scene(Vector3(0, 0, 0), // camera position
                         1, 1, 1, // projection plane properties
                         { // vector of spheres in the scene
                            Sphere(Vector3(3, 0, 9), 1, RGB(255, 0, 0)),
                            Sphere(Vector3(0, 0, 9), 1, RGB(0, 255, 0)),
                            Sphere(Vector3(-3, 0, 9), 1, RGB(0, 0, 255)),
                            Sphere(Vector3(0, -10001, 0), 10000, RGB(150, 150, 150)) // ground
                         }, { // vector of lights in the scene
                            Light("ambient", .1, Vector3(0, 0, 0), Vector3(0, 0, 0)),
                            Light("point", 1, Vector3(0, 2, 7), Vector3(0, 0, 0)),
                            // Light("directional", 1, Vector3(0, 0, 0), Vector3(-1, -1, 2))
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

tuple<float, Sphere> closestIntersection(Scene scene, Vector3 origin, Vector3 target, float t_min, float t_max) {
       /**
    * Find the closest intersection between the ray comming from origin to target and restricted
    * between t_min and t_max with the objects in the scene
   */
    float tRes = numeric_limits<float>::infinity();
    Sphere sphereRes;
    for (Sphere sphere : scene.spheres) {
        float t1, t2;
        Vector3 _1, _2, _3, _4; // dummy variables
        tie(t1, t2, _1, _2, _3, _4) = sphere.intersectRay(origin, target);
        if (t_min<=t1 && t1<=t_max && t1<tRes) { // a closer intersection is found
            // update the results
            tRes = t1;
            sphereRes = sphere;
        }
        if (t_min<=t2 && t2<=t_max && t2<=tRes) { // a closer intersection is found
            // update the results
            tRes = t2;
            sphereRes = sphere;
        }
    }
    return make_tuple(tRes, sphereRes);
}

tuple<COLORREF, Vector3, Vector3> traceRay(Scene scene, Vector3 origin, Vector3 target, float t_min, float t_max) {
    /**
     * Compute the ray that goes from origin to target and returns informations about the closest hitpoint with its
     * distance betwen t_min and t_max
     * 
     * @param scene The scene to trace the ray in
     * @param origin The ray origin
     * @param target The ray target (where the ray is headed)
     * @param t_min The minimum distance of a hit Vector3
     * @param t_max The maximum distance of a hit Vector3
     * @return A tuple containing the color, the position and the normal of the closest hitpoint of the ray 
    */
    float closestDist = numeric_limits<float>::infinity();
    Vector3 hitPos, hitNormal;
    int closestSphereIndex = -1;
    for (int i = 0; i < scene.spheres.size(); i++) {
        float t1, t2; // distance of the hitpoints (infinity if no intersection)
        Vector3 H1, H2; // hitpoints positions (0 if no intersection)
        Vector3 N1, N2; // normal at the hitpoints (0 if no intersection)
        tie(t1, t2, H1, H2, N1, N2) = scene.spheres[i].intersectRay(origin, target);
        if (t_min <= t1 && t1 <= t_max && t1 < closestDist) { // found a better valid hitpoint
            closestSphereIndex = i;
            closestDist = t1;
            hitPos = H1;
            hitNormal = N1;
        }
        if (t_min <= t2 && t2 <= t_max && t2 < closestDist) { // found a better valid hitpoint
            closestSphereIndex = i;
            closestDist = t2;
            hitPos = H2;
            hitNormal = N2;
        }
    }
    if (closestSphereIndex == -1) { // case where the ray did not intersect any sphere
        return make_tuple(backgroundColor, Vector3(0, 0, 0), Vector3(0, 0, 0));
    }
    return make_tuple(scene.spheres[closestSphereIndex].color, hitPos, hitNormal);;
}

// bool isLightObstructed(Scene scene, Light light, Vector3 position) {
//     /**
//      * Checks if the light is obstructed from position and in the scene (if there is an object
//      * between the light and position)
//     */
//     if (light.type == "ambient") {
//         return false; // ambient light cannot be obstructed 
//     }
//     Vector3 lightPos;
//     if (light.type == "point") {
//         lightPos = light.position;
//     }
//     if (light.type == "directional") {
//         lightPos = Vector3(0, 0, 0) - light.direction/Vector3::norm(light.direction) * 100000000; // virtual position of the light
//     }
//     // trace the ray from the position to the light and check if an object obstructing the (light) ray
//     COLORREF _;
//     Vector3 hitPos, hitNormal; // both equal 0 iff the light is not obstructed
//     float epsilon = 0.000000000001; // margin for ray collision
//     tie(_, hitPos, hitNormal) = traceRay(scene,
//                                          position, lightPos,
//                                          epsilon, Vector3::distance(position, lightPos)-epsilon);
//     if (Vector3::norm(hitNormal) <= 0.0000001) {
//         // std::cout << "Light not obstructed!!!!" << std::endl; // debug
//         return false; // light is not obstructed
//     }
//     else {
//         std::cout << "hitPos: " << hitPos.toString() << "   hitNormal: " << hitNormal.toString() << std::endl;
//         return true;
//     }
// }

bool isLightObstructed(Scene scene, Light light, Vector3 position) {
    /**
     * Checks if the light is obstructed from position and in the scene (if there is an object
     * between the light and position)
    */
    if (light.type == "ambient") {
        return false; // ambient light cannot be obstructed 
    }
    Vector3 lightPos;
    if (light.type == "point") {
        lightPos = light.position;
    }
    if (light.type == "directional") {
        lightPos = Vector3(0, 0, 0) - Vector3::normalize(light.direction) * 100000000; // virtual position of the light
    }
    // trace the ray from the position to the light and check if an object obstructing the (light) ray
    float tShadow;
    Sphere sphereShadow;
    tie(tShadow, sphereShadow) = closestIntersection(scene, position, lightPos, 0.1, 10000000);
    // tie(tShadow, sphereShadow) = closestIntersection(scene, position, light.position, .00000001, numeric_limits<float>::infinity());
    if (tShadow < 1000000000000) { // this condition means the ray has found an object
        std::cout << "Light obstructed by: " << sphereShadow.toString() << std::endl;
        return true; // light is obstructed
    }
    else {
        return false;
    }
}


float lightIntensity (Scene scene, Vector3 position, Vector3 normal) { 
    /**
     * TODO: write the documentation for this function
    */
    float intensity = 0;
    for (int i = 0; i < scene.lights.size(); i++) {
        Light light = scene.lights[i];
        // if (!isLightObstructed(scene, light, position)) { // this condition is responsible for the shadows
        if (true) { // debug
            // if the light is not obstructed, handle the three types of light differently
            if (light.type == "ambient") {
                intensity += light.intensity;
            }
            else {
                Vector3 lightDir;
                if (light.type == "point") {
                    lightDir = light.position - position;
                }
                if (light.type == "directional") {
                    lightDir = light.direction;
                }
                lightDir = Vector3::normalize(lightDir);
                float NdotDir = Vector3::dot(normal, lightDir);
                if (NdotDir > 0) { // compute diffuse lighting
                    intensity += light.intensity * NdotDir/(Vector3::norm(normal) * Vector3::norm(lightDir));
                }
            }
        }
    }
    return max((float) 0, min(intensity, (float) 1)); // clamp intensity between 0 and 1
}

COLORREF viewportColor(Scene scene, Vector3 vpPos) {
    /**
     * TODO: write the docstring for this function
    */
    Vector3 cameraPos = scene.cameraPos;
    COLORREF color; // color of the closest solid point
    Vector3 hitPos, normal; // position and the normal of the geometry at the hitpoint
    tie(color, hitPos, normal) = traceRay(scene,
                              cameraPos, vpPos,
                              1, numeric_limits<float>::infinity());
    float intensity = lightIntensity(scene, hitPos, normal);
    return RGB(GetRValue(color) * intensity, GetGValue(color) * intensity, GetBValue(color) * intensity);
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