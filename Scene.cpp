#include <math.h>
#include <string>
#include <vector>
#include <tuple>

// custom files:
#include <Vector3.cpp>
#include <Object.cpp>
#include <Sphere.cpp>
#include <Light.cpp>


class Scene {
    /**
     * Describe an entire scene, containing a camera, a projection plane, objects and
     * lights.
    */
    public:
        Vector3 cameraPos; // position of the camera
        tuple<float, float> projPlaneDimensions; // (width, height) of the projection plane
        float projPlaneDistance; // distance from the camera to the projection plane
        vector<Object> objects; // objects contained in the scene
        vector<Light> lights; // lights contained in the scene

        Scene() {} // default constructor

        Scene(Vector3 _cameraPos, tuple<float, float> _projPlaneDimensions, float _projPlaneDistance, vector<Object> _objects, vector<Light> _lights) {
            this->cameraPos = _cameraPos;
            this->projPlaneDimensions = _projPlaneDimensions;
            this->projPlaneDistance = -_projPlaneDistance; // needs to be negative for some reason ¯\_(ツ)_/¯
            this->objects = _objects;
            this->lights = _lights;
        }

        static Scene getDefaultScene() { // returns default scene with three spheres and a ground
            // the whole scene is hardcoded
            return Scene(Vector3(0, 0, 0), // camera position
                         make_tuple(1, 1), 1, // projection plane properties
                         { // vector of spheres in the scene
                            Sphere(Vector3(3, 0, 9), 1, RGB(255, 0, 0)),
                            Sphere(Vector3(0, 0, 9), 1, RGB(0, 255, 0)),
                            Sphere(Vector3(-3, 0, 9), 1, RGB(0, 0, 255)),
                            Sphere(Vector3(0, -50001, 0), 50000, RGB(150, 150, 150)) // ground
                         }, { // vector of lights in the scene
                            Light("ambient", .1, Vector3(0, 0, 0), Vector3(0, 0, 0)),
                            Light("point", 1, Vector3(0, 2, 7), Vector3(0, 0, 0)),
                            // Light("directional", 1, Vector3(0, 0, 0), Vector3(-1, -1, 2))
                         });
        }
};