#include <math.h>
#include <string>

using namespace std;

class Vector3 {
    /**
     * A class used to store vectors in 3D space, such as positions or directions.
     * It contains usefull basic linear algebra functions for manipulating vectors.
    */
    public:
        float x, y, z;

        Vector3() {}; // default constructor
        Vector3(float _x, float _y, float _z) {
            this->x = _x;
            this->y = _y;
            this->z = _z;
        }

        Vector3 operator + (Vector3 B) {
            return Vector3(this->x + B.x,
                           this->y + B.y,
                           this->z + B.z);
        }
        Vector3 operator - (Vector3 B) {
            return Vector3(this->x - B.x,
                           this->y - B.y,
                           this->z - B.z);
        }
        Vector3 operator * (float a) {
            return Vector3(this->x * a,
                           this->y * a,
                           this->z * a);
        }
        Vector3 operator / (float a) {
            return Vector3(this->x / a,
                           this->y / a,
                           this->z / a);
        }
        bool operator == (Vector3 B) {
            return this->x == B.x &&
                   this->y == B.y &&
                   this->z == B.z;
        }

        // Some usefull functions to do math with Vector3s
        static float distance(Vector3 A, Vector3 B) {
            /**
             * Computes the distance between two 3 dimensional vectors
             * 
             * @param A
             * @param B
             * @return the distance between A and B
            */
            return sqrt(pow(A.x - B.x, 2) + pow(A.y - B.y, 2) + pow(A.z - B.z, 2));
        }
        static float dot(Vector3 A, Vector3 B) {
            /**
             * Computes the dot product between two 3 dimensional vectors
             * 
             * @param A
             * @param B
             * @return A dot B
            */
            return A.x * B.x + A.y * B.y + A.z * B.z;
        }
        float length() {
            /**
             * Computes the length of the 3 dimensional vector
            */
           return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
        }
        static float length(Vector3 A) {
            /**
             * Computes the length of a 3 dimensional vector
             * 
             * @param A
             * @return the length of A (sqrt(x²+y²+z²))
            */
           return sqrt(pow(A.x, 2) + pow(A.y, 2) + pow(A.z, 2));
        }
        Vector3 normalize() {
            /**
             * Returns a Vector3 of same direction and of length 1
            */
           return *this / this->length();
        }
        static Vector3 normalize(Vector3 A) {
            /**
             * Returns a Vector 3 of same direction as the parameter
             * and of length 1
             * 
             * @param A
             * @return a unit vector with the same direction as A 
            */
           return A / A.length();
        }

        // Other utilitary functions
        string toString() {
            /**
             * Return a ready-to-display string describing the 3 dimensional vector,
             * formated as: "(x, y, z)"
            */
            return "(" + to_string(this->x) + ", " + to_string(this->y) + ", " + to_string(this->z) + ")";
        }
        static string toString(Vector3 A) {
            /**
             * Return a ready-to-display string describing a 3 dimensional vector,
             * formated as: "(x, y, z)"
             * 
             * @param A the Vector3 to print
             * @return a string describing A
            */
            return "(" + to_string(A.x) + ", " + to_string(A.y) + ", " + to_string(A.z) + ")";
        }
};