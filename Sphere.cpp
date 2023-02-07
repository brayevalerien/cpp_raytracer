#include <math.h>
#include <string>
#include <limits>
#include <tuple>
#include <windows.h>

// custom files:
#include <Vector3.cpp>
#include <Object.cpp>

class Sphere: Object {
    /**
     * Sphere primitive.
    */
    public:
        float radius; // sphere radius

        Sphere() {} // default constructor
        Sphere(Vector3 _position, float _radius, COLORREF _color) {
            this->position = _position;
            this->radius = _radius;
            this->color = _color;
        }

        tuple<float, float> intersectRay(Vector3 origin, Vector3 dir, float distMin, float distMax) {
            /**
             * Finds the two intersection points of the sphere with the ray comming from origin, in direction
             * dir and contained between distMin and distMax. If the ray lands on the boundary of the object 
             * and thus only intersect the object once, both hitpoints will be equal.
             * This function returns the distances of the hitpoints. If the ray does not intersect the object,
             * it returns (+infinity, +infinity).
             * 
             * @param origin The origin of the considered ray
             * @param dir The direction in wich the ray is shot in
             * @param distMin The minimum distance between origin and a hitpoint for it to be considered
             * @param distMax The maximum distance between origin and a hitpoint for it to be considered
             * @return The distances of the two hipoints between the ray and the object. (+infinity, +infinity) if the ray does not intersect the object. 
            */
            Vector3 CO = origin - this->position;

            float a = pow(dir.length(), 2); 
            float b = 2 * Vector3::dot(CO, dir); 
            float c = pow(CO.length(), 2) - pow(this->radius, 2); 

            float discriminant = b*b - 4*a*c;
            if (discriminant < 0) { // no instersection found
                return make_tuple(numeric_limits<float>::infinity(), numeric_limits<float>::infinity());       
            }
            return make_tuple((-b+sqrt(discriminant))/(2*a), (-b-sqrt(discriminant))/(2*a));            
        }

        string toString() {
            /**
             * Returns a ready-to-print description of the sphere.
            */
            return "Sphere center: " + this->position.toString() + "   radius: " + to_string(this->radius);
        }
};