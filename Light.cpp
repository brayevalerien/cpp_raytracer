#include <math.h>
#include <string>

// custom file:
#include <Vector3.cpp>


class Light {
    /**
     * A class used to describe lights in a scene and some method to interact
     * with the light
     * There are three type of lights supported:
     *      - ambient: a world light that illuminates any point in the scene.
     *      - point: all rays of this kind of lights come from a single point.
     *      - sun: all rays of this kind of lights are parallel.
    */
    public:
        string type; // a light can either be "ambient", "point" or "sun"
        float intensity; // between 0 and 1, represents how bright the light is
        Vector3 position; // position of the light (ignored for ambient and sun)
        Vector3 direction; // direction of the light (ignored for point and sun)

    Light() {} // default constructor
    Light(string _type, float _intensity, Vector3 _position=Vector3(0, 0, 0), Vector3 _direction=Vector3(0, 0, 0)) {
        this->type = _type;
        this->intensity = _intensity;
        this->position = _position;
        this->direction = _direction;
    }

    
};