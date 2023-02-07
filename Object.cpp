#include <math.h>
#include <limits>
#include <string>
#include <tuple>
#include <windows.h>

// custom file:
#include <Vector3.cpp> // for 3D vectors


class Object {
    /**
     * A class used to describe any physical object in a Scene and
     * some methods to interact with an Object
    */
    public:
        Vector3 position; // center of the object
        COLORREF color; // color of the object surface

        Object() {} // default constructor
        Object(Vector3 _position, COLORREF _color) {
            this->position = _position;
            this->color = _color;
        }

        tuple<float, float> intersectRay(Vector3 origin, Vector3 dir, float distMin, float distMax) {
            /**
             * Finds the two intersection points of the object with the ray comming from origin, in direction
             * dir and contained between distMin and distMax (all objects are assumed to be convex and thus 
             * only have two intersection points). If the ray lands on the boundary of the object and thus
             * only intersect the object once, both hitpoints will be equal.
             * This function returns the distances of the hitpoints. If the ray does not intersect the object,
             * it returns (+infinity, +infinity).
             * 
             * @param origin The origin of the considered ray
             * @param dir The direction in wich the ray is shot in
             * @param distMin The minimum distance between origin and a hitpoint for it to be considered
             * @param distMax The maximum distance between origin and a hitpoint for it to be considered
             * @return The distances of the two hipoints between the ray and the object. (+infinity, +infinity) if the ray does not intersect the object. 
            */
            return make_tuple(numeric_limits<float>::infinity(), numeric_limits<float>::infinity());           
        }

        string toString() {
            /**
             * Returns a ready-to-print describing the object.
            */
            return "Object position: " + this->position.toString();
        }
};