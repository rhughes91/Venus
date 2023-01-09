#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <string>

#define M_PI 3.14159265358979323846
#include <math.h>
#include <vector>

struct Vector2;
struct Vector4;
struct Quaternion;
struct Quad;

// math (namespace): namespace that contains basic float math functions
namespace math
{
    inline float abs(float value)                               // returns the absolute value of the given 'value'
    {
        return std::sqrt(value*value);
    }
    inline float clamp(float value, float low, float high)      // returns the given 'value' clamped between the provided 'low' and 'high' values
    {
        if(value < low) return low;
        else if(value > high) return high;
        return value;
    }
    inline float degrees(float radian)                          // returns the given 'value' (assumed to be in radians) as a degree
    {
        return radian * (180/M_PI);
    }
    inline float lerp(float value1, float value2, float weight) // returns 'value1' linearly interpolated towards 'value2' by 'weight'
    {
        return value1 * (1 - weight) + value2 * weight;
    }
    inline float modf(float value, float divisor)               // returns the remainder of the given 'value' divided by 'divisor'
    {
        if(divisor == 0) return value;
        while(abs(value) > abs(divisor))
        {
            value -= divisor * abs(value)/value;
        }
        return value;
        
    }
    inline float radians(float degree)                          // returns the given 'value' (assumed to be in degrees) as a radian
    {
        return degree * (M_PI/180);
    }
    inline float roundTo(float value, int32_t precision)        // returns the given 'value' rounded to 'precision' decimal places
    {
        return std::round(value * std::pow(10, precision)) / std::pow(10, precision);
    }
    inline int32_t sign(float value)                            // returns the sign of the given 'value' as an integer, where zero is equal to one (eg. 10 = 1, -10 = -1, 0 = 1)
    {
        return value == 0 ? 1:abs(value)/value;
    }
    inline int32_t sign0(float value)                           // returns the sign of the given 'value' as an integer, where zero is equal to zero (eg. 10 = 1, -10 = -1, 0 = 0)
    {
        return value == 0 ? 0:abs(value)/value;
    }

    float triArea(const Vector2& p1, const Vector2& p2, const Vector2& p3);
    float quadArea(const Quad& quad);
    
    bool lineLineIntersect(const Vector2& p1, const Vector2& p2, const Vector2& q1, const Vector2& q2);
    bool quadPointIntersect(const Quad& quad, const Vector2& point);
}

// struct fixed
// {

// };

// Vector2 (structure): structure that holds two variables (x, y) and allows two dimensional vector operations
struct Vector2
{
    public:
        float x, y;
    
    Vector2() : x(0), y(0){}
    Vector2(float value) : x(value), y(value) {}
    Vector2(float x__, float y__) : x(x__), y(y__){}

    operator std::string()
    {
        return std::to_string(x) + ", " + std::to_string(y);
    }
    
    float length() const                  // returns the magnitude of this Vector2
    {
        return std::sqrt(x*x+y*y);
    }
    float length2() const                 // returns the squared magnitude of this Vector2 :: faster than "Vector2::length" and useful for comparisons with other Vector2s
    {
        return (x*x+y*y);
    }
    float cross(const Vector2 &vec) const // returns the z-coordinate that results from taking the cross product of this Vector2 and the provided Vector2 'vec'
    {
        return x*vec.y - y*vec.x;
    }
    float dot(const Vector2 &vec) const   // returns the dot product between this Vector2 and the provided Vector2 'vec'
    {
        return x*vec.x + y*vec.y;
    }
    Vector2 normalized() const            // returns the direction of this Vector2 (magnitude becomes one)
    {
        float length = Vector2::length();
        return length == 0 ? Vector2():Vector2(x/length, y/length);
    }

    void normalize() // changes this Vector2 into a direction vector (magnitude becomes one)
    {
        float length = Vector2::length();
        if(length != 0)
        {
            x = x/length;
            y = y/length;
        }
    }
};

// Vector2 (operators): standard operators for two dimensional vectors
inline Vector2 operator +(const Vector2 &vec1, const Vector2 &vec2) {return Vector2(vec1.x+vec2.x, vec1.y+vec2.y);}
inline Vector2 operator -(const Vector2 &vec1, const Vector2 &vec2) {return Vector2(vec1.x-vec2.x, vec1.y-vec2.y);}
inline Vector2 operator *(const Vector2 &vec1, const Vector2 &vec2) {return Vector2(vec1.x*vec2.x, vec1.y*vec2.y);} // all multiplication operator overloads simply multiply each of the provided Vector2s components with each other
inline Vector2 operator /(const Vector2 &vec1, const Vector2 &vec2) {return Vector2(vec1.x/vec2.x, vec1.y/vec2.y);} // all division operator overloads simply divide each of the provided Vector2s components by each other
inline Vector2 operator +(const Vector2 &vec, const float &num) {return Vector2(vec.x+num, vec.y+num);}
inline Vector2 operator -(const Vector2 &vec, const float &num) {return Vector2(vec.x-num, vec.y-num);}
inline Vector2 operator *(const Vector2 &vec, const float &num) {return Vector2(vec.x*num, vec.y*num);}
inline Vector2 operator /(const Vector2 &vec, const float &num) {return Vector2(vec.x/num, vec.y/num);}
inline Vector2 operator -(const Vector2 &vec) {return Vector2(-vec.x, -vec.y);}
inline Vector2 &operator +=(Vector2 &vec1, const Vector2 &vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    return vec1;
}
inline Vector2 &operator -=(Vector2 &vec1, const Vector2 &vec2)
{
    vec1.x -= vec2.x;
    vec1.y -= vec2.y;
    return vec1;
}
inline Vector2 &operator *=(Vector2 &vec1, const Vector2 &vec2)
{
    vec1.x *= vec2.x;
    vec1.y *= vec2.y;
    return vec1;
}
inline Vector2 &operator /=(Vector2 &vec1, const Vector2 &vec2)
{
    vec1.x /= vec2.x;
    vec1.y /= vec2.y;
    return vec1;
}
inline bool operator ==(const Vector2 &vec1, const Vector2 &vec2)
{
    return (vec2.x == vec1.x && vec2.y == vec1.y);
}
inline bool operator !=(const Vector2 &vec1, const Vector2 &vec2)
{
    return (vec2.x != vec1.x || vec2.y != vec1.y);
}
inline bool operator <(const Vector2 &vec1, const Vector2 &vec2)
{
    return vec1.length2() < vec2.length2();
}
inline bool operator >(const Vector2 &vec1, const Vector2 &vec2)
{
    return vec1.length2() > vec2.length2();
}
inline bool operator <=(const Vector2 &vec1, const Vector2 &vec2)
{
    return vec1.length2() <= vec2.length2();
}
inline bool operator >=(const Vector2 &vec1, const Vector2 &vec2)
{
    return vec1.length2() >= vec2.length2();
}

// Vector3 (structure: extends Vector2) structure that holds three variables (x, y, z) and allows three dimensional vector operations
struct Vector3 : Vector2
{
    float z;
    Vector3() : Vector2(0, 0), z(0) {}
    Vector3(float value) : Vector2(value, value), z(value) {}
    Vector3(float x__, float y__) : Vector2(x__, y__), z(0) {}
    Vector3(float x__, float y__, float z__) : Vector2(x__, y__), z(z__) {}
    Vector3(const Vector2 &vector, float z__) : Vector3(vector.x, vector.y, z__) {}

    Vector3 xy() const // returns a new Vector3 with only the x and y components of this Vector3
    {
        return Vector3(x, y, 0);
    }
    Vector3 xz() const // returns a new Vector3 with only the x and z components of this Vector3
    {
        return Vector3(x, 0, z);
    }
    Vector3 yz() const // returns a new Vector3 with only the y and z components of this Vector3
    {
        return Vector3(0, y, z);
    }

    operator std::string() const
    {
        return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
    }

    float length() const                        // returns the magnitude of this Vector3
    {
        return std::sqrt(x*x+y*y+z*z);
    }
    float length2() const                       // returns the squared magnitude of this Vector3 :: faster than "Vector3::length" and useful for comparisons with other Vector3s
    {
        return (x*x+y*y+z*z);
    }
    float dot(const Vector3 &vec) const         // returns the dot product between this Vector3 and the provided Vector3 'vec'
    {
        return x*vec.x + y*vec.y + z*vec.z;
    }
    Vector3 cross(const Vector3 &vec) const     // returns the cross product between this Vector3 and the provided Vector3 'vec'
    {
        return Vector3(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
    }
    Vector3 normalized() const                  // returns the direction of this Vector3 (magnitude becomes one)
    {
        float length = Vector3::length();
        return length == 0 ? Vector3():Vector3(x/length, y/length, z/length);
    }
    
    void normalize()    // changes this Vector3 into a direction vector (magnitude becomes one)
    {
        float length = Vector3::length();
        if(length != 0)
        {
            x = x/length;
            y = y/length;
            z = z/length;
        }
    }
};

// Vector3 (operators): standard operators for three dimensional vectors
inline Vector3 operator +(const Vector3 &vec1, const Vector3 &vec2) {return Vector3(vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z);}
inline Vector3 operator -(const Vector3 &vec1, const Vector3 &vec2) {return Vector3(vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z);}
inline Vector3 operator *(const Vector3 &vec1, const Vector3 &vec2) {return Vector3(vec1.x*vec2.x, vec1.y*vec2.y, vec1.z*vec2.z);} // all multiplication operator overloads simply multiply each of the provided Vector2s components with each other
inline Vector3 operator /(const Vector3 &vec1, const Vector3 &vec2) {return Vector3(vec1.x/vec2.x, vec1.y/vec2.y, vec1.z/vec2.z);} // all division operator overloads simply divide each of the provided Vector2s components by each other
inline Vector3 operator +(const Vector3 &vec, float num) {return Vector3(vec.x+num, vec.y+num, vec.z+num);}
inline Vector3 operator -(const Vector3 &vec, float num) {return Vector3(vec.x-num, vec.y-num, vec.z-num);}
inline Vector3 operator *(const Vector3 &vec, float num) {return Vector3(vec.x*num, vec.y*num, vec.z*num);}
inline Vector3 operator /(const Vector3 &vec, float num) {return Vector3(vec.x/num, vec.y/num, vec.z/num);}
inline Vector3 operator -(const Vector3 &vec) {return Vector3(-vec.x, -vec.y, -vec.z);}
inline Vector3 &operator +=(Vector3 &vec1, const Vector3 &vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    vec1.z += vec2.z;
    return vec1;
}
inline Vector3 &operator -=(Vector3 &vec1, const Vector3 &vec2)
{
    vec1.x -= vec2.x;
    vec1.y -= vec2.y;
    vec1.z -= vec2.z;
    return vec1;
}
inline Vector3 &operator *=(Vector3 &vec1, const Vector3 &vec2)
{
    vec1.x *= vec2.x;
    vec1.y *= vec2.y;
    vec1.z *= vec2.z;
    return vec1;
}
inline Vector3 &operator /=(Vector3 &vec1, const Vector3 &vec2)
{
    vec1.x /= vec2.x;
    vec1.y /= vec2.y;
    vec1.z /= vec2.z;
    return vec1;
}
inline bool operator ==(const Vector3 &vec1, const Vector3 &vec2)
{
    return (vec2.x == vec1.x && vec2.y == vec1.y && vec2.z == vec1.z);
}
inline bool operator !=(const Vector3 &vec1, const Vector3 &vec2)
{
    return (vec2.x != vec1.x || vec2.y != vec1.y || vec2.z != vec1.z);
}
inline bool operator <(const Vector3 &vec1, const Vector3 &vec2)
{
    return vec1.length2() < vec2.length2();
}
inline bool operator >(const Vector3 &vec1, const Vector3 &vec2)
{
    return vec1.length2() > vec2.length2();
}
inline bool operator <=(const Vector3 &vec1, const Vector3 &vec2)
{
    return vec1.length2() <= vec2.length2();
}
inline bool operator >=(const Vector3 &vec1, const Vector3 &vec2)
{
    return vec1.length2() >= vec2.length2();
}

// Vector4 (structure: extends Vector3) structure that holds four variables (x, y, z, w) and allows four dimensional vector operations :: TODO: complete Vector4 methods
struct Vector4 : Vector3
{
    float w;

    Vector4() : Vector3(), w(0) {}
    Vector4(float x__, float y__, float z__, float w__) : Vector3(x__, y__, z__), w(w__) {}
};

// Vector4 (operators): standard operators for four dimensional vectors
inline Vector3 operator +(const Vector4 &vec1, const Vector4 &vec2) {return Vector4(vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z, vec1.w+vec2.w);}
inline Vector4 operator -(const Vector4 &vec1, const Vector4 &vec2) {return Vector4(vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z, vec1.w-vec2.w);}
inline Vector4 operator *(const Vector4 &vec1, const Vector4 &vec2) {return Vector4(vec1.x*vec2.x, vec1.y*vec2.y, vec1.z*vec2.z, vec1.w*vec2.w);} // all multiplication operator overloads simply multiply each of the provided Vector2s components with each other
inline Vector4 operator /(const Vector4 &vec1, const Vector4 &vec2) {return Vector4(vec1.x/vec2.x, vec1.y/vec2.y, vec1.z/vec2.z, vec1.w/vec2.w);} // all division operator overloads simply divide each of the provided Vector2s components by each other
inline Vector4 operator +(const Vector4 &vec, float num) {return Vector4(vec.x+num, vec.y+num, vec.z+num, vec.w+num);}
inline Vector4 operator -(const Vector4 &vec, float num) {return Vector4(vec.x-num, vec.y-num, vec.z-num, vec.w-num);}
inline Vector4 operator *(const Vector4 &vec, float num) {return Vector4(vec.x*num, vec.y*num, vec.z*num, vec.w*num);}
inline Vector4 operator /(const Vector4 &vec, float num) {return Vector4(vec.x/num, vec.y/num, vec.z/num, vec.w/num);}
inline Vector4 operator -(const Vector4 &vec) {return Vector4(-vec.x, -vec.y, -vec.z, -vec.w);}
inline Vector4 &operator +=(Vector4 &vec1, const Vector4 &vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    vec1.z += vec2.z;
    vec1.w += vec2.w;
    return vec1;
}
inline Vector4 &operator -=(Vector4 &vec1, const Vector4 &vec2)
{
    vec1.x -= vec2.x;
    vec1.y -= vec2.y;
    vec1.z -= vec2.z;
    vec1.w -= vec2.w;
    return vec1;
}
inline Vector4 &operator *=(Vector4 &vec1, const Vector4 &vec2)
{
    vec1.x *= vec2.x;
    vec1.y *= vec2.y;
    vec1.z *= vec2.z;
    vec1.w *= vec2.w;
    return vec1;
}
inline Vector4 &operator /=(Vector4 &vec1, const Vector4 &vec2)
{
    vec1.x /= vec2.x;
    vec1.y /= vec2.y;
    vec1.z /= vec2.z;
    vec1.w /= vec2.w;
    return vec1;
}
inline bool operator ==(const Vector4 &vec1, const Vector4 &vec2)
{
    return (vec2.x == vec1.x && vec2.y == vec1.y && vec2.z == vec1.z && vec2.w == vec1.w);
}
inline bool operator !=(const Vector4 &vec1, const Vector4 &vec2)
{
    return (vec2.x != vec1.x || vec2.y != vec1.y || vec2.z != vec1.z || vec2.w != vec1.w);
}
inline bool operator <(const Vector4 &vec1, const Vector4 &vec2)
{
    return vec1.length2() < vec2.length2();
}
inline bool operator >(const Vector4 &vec1, const Vector4 &vec2)
{
    return vec1.length2() > vec2.length2();
}
inline bool operator <=(const Vector4 &vec1, const Vector4 &vec2)
{
    return vec1.length2() <= vec2.length2();
}
inline bool operator >=(const Vector4 &vec1, const Vector4 &vec2)
{
    return vec1.length2() >= vec2.length2();
}

// Vector2I (structure) structure that holds two integer variables (x, y) and allows two dimensional vector operations
struct Vector2I
{
    public:
        int x, y;
    
    Vector2I() : x(0), y(0){}
    Vector2I(float x__, float y__) : x(x__), y(y__){}

    operator std::string() const
    {
        return std::to_string(x) + ", " + std::to_string(y);
    }
    operator Vector3() const
    {
        return Vector3((float)x, float(y), 0);
    }
    operator Vector2() const
    {
        return Vector2((float)x, float(y));
    }

    Vector2I abs() const                         // returns the "math::abs" of each of its components as a new Vector2I
    {
        return Vector2I(math::abs(x), math::abs(y));
    }
    Vector2I pow(float exponent) const           // returns the "std::pow" of each of its components as a new Vector2I
    {
        return Vector2I(std::pow(x, exponent), std::pow(y, exponent));
    }
    Vector2I sign() const                        // returns the "math::sign" of each of its components as a new Vector2I
    {
        return Vector2I(math::sign(x), math::sign(y));
    }
    Vector2I sign0() const                       // returns the "math::sign0" of each of its components as a new Vector2I
    {
        return Vector2I(math::sign0(x), math::sign0(y));
    }
    
    float length() const                    // returns the magnitude of this Vector2I
    {
        return std::sqrt(x*x+y*y);
    }
    float length2() const                   // returns the squared magnitude of this Vector2 :: faster than "Vector2I::length" and useful for comparisons with other Vector2s
    {
        return (x*x+y*y);
    }
    float cross(const Vector2I &vec) const  // returns the z-coordinate that results from taking the cross product of this Vector2I and the provided Vector2I 'vec'
    {
        return x*vec.y - y*vec.x;
    }
    float dot(const Vector2I &vec) const    // returns the dot product between this Vector2I and the provided Vector2I 'vec'
    {
        return x*vec.x + y*vec.y;
    }
    Vector2 normalized() const              // returns the direction of this Vector2I (magnitude becomes one, returns a Vector2)
    {
        float length = Vector2I::length();
        return length == 0 ? Vector2():Vector2(x/length, y/length);
    }
};

// Vector2 (operators): standard operators for two dimensional vectors
inline Vector2I operator +(const Vector2I &vec1, const Vector2I &vec2) {return Vector2I(vec1.x+vec2.x, vec1.y+vec2.y);}
inline Vector2I operator -(const Vector2I &vec1, const Vector2I &vec2) {return Vector2I(vec1.x-vec2.x, vec1.y-vec2.y);}
inline Vector2I operator *(const Vector2I &vec1, const Vector2 &vec2) {return Vector2I(vec1.x*vec2.x, vec1.y*vec2.y);} // all multiplication operator overloads simply multiply each of the provided Vector2s components with each other
inline Vector2I operator /(const Vector2I &vec1, const Vector2 &vec2) {return Vector2I(vec1.x/vec2.x, vec1.y/vec2.y);} // all division operator overloads simply divide each of the provided Vector2s components by each other
inline Vector2I operator +(const Vector2I &vec, const float &num) {return Vector2I(vec.x+num, vec.y+num);}
inline Vector2I operator -(const Vector2I &vec, const float &num) {return Vector2I(vec.x-num, vec.y-num);}
inline Vector2I operator *(const Vector2I &vec, const float &num) {return Vector2I(vec.x*num, vec.y*num);}
inline Vector2I operator /(const Vector2I &vec, const float &num) {return Vector2I(vec.x/num, vec.y/num);}
inline Vector2I operator -(const Vector2I &vec) {return Vector2I(-vec.x, -vec.y);}
inline Vector2I &operator +=(Vector2I &vec1, const Vector2I &vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    return vec1;
}
inline Vector2I &operator -=(Vector2I &vec1, const Vector2I &vec2)
{
    vec1.x -= vec2.x;
    vec1.y -= vec2.y;
    return vec1;
}
inline Vector2I &operator *=(Vector2I &vec1, const Vector2I &vec2)
{
    vec1.x *= vec2.x;
    vec1.y *= vec2.y;
    return vec1;
}
inline Vector2I &operator /=(Vector2I &vec1, const Vector2I &vec2)
{
    vec1.x /= vec2.x;
    vec1.y /= vec2.y;
    return vec1;
}
inline bool operator ==(const Vector2I &vec1, const Vector2I &vec2)
{
    return (vec2.x == vec1.x && vec2.y == vec1.y);
}
inline bool operator !=(const Vector2I &vec1, const Vector2I &vec2)
{
    return (vec2.x != vec1.x || vec2.y != vec1.y);
}
inline bool operator <(const Vector2I &vec1, const Vector2I &vec2)
{
    return vec1.length2() < vec2.length2();
}
inline bool operator >(const Vector2I &vec1, const Vector2I &vec2)
{
    return vec1.length2() > vec2.length2();
}
inline bool operator <=(const Vector2I &vec1, const Vector2I &vec2)
{
    return vec1.length2() <= vec2.length2();
}
inline bool operator >=(const Vector2I &vec1, const Vector2I &vec2)
{
    return vec1.length2() >= vec2.length2();
}

// Vector3I (structure: extends Vector2I) structure that holds three integer variables (x, y, z) and allows three dimensional vector operations
struct Vector3I : Vector2I
{
    int z, w;
    Vector3I() : Vector2I(0, 0), z(0), w(1) {}
    Vector3I(float x__, float y__, float z__) : Vector2I(x__, y__), z(z__), w(1) {}

    operator std::string() const
    {
        return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
    }

    Vector3I abs() const                         // returns the "math::abs" of each of its components as a new Vector3I
    {
        return Vector3I(math::abs(x), math::abs(y), math::abs(z));
    }
    Vector3I pow(float exponent) const           // returns the "std::pow" of each of its components as a new Vector3I
    {
        return Vector3I(std::pow(x, exponent), std::pow(y, exponent),std::pow(z, exponent));
    }
    Vector3I sign() const                        // returns the "math::sign" of each of its components as a new Vector3I
    {
        return Vector3I(math::sign(x), math::sign(y), math::sign(z));
    }
    Vector3I sign0() const                       // returns the "math::sign0" of each of its components as a new Vector3I
    {
        return Vector3I(math::sign0(x), math::sign0(y), math::sign0(y));
    }

    float length() const                        // returns the magnitude of this Vector3I
    {
        return std::sqrt(x*x+y*y+z*z);
    }
    float length2() const                       // returns the squared magnitude of this Vector3I :: faster than "Vector3I::length" and useful for comparisons with other Vector3Is
    {
        return (x*x+y*y+z*z);
    }
    float dot(const Vector3I &vec) const        // returns the dot product between this Vector3I and the provided Vector3I 'vec'
    {
        return x*vec.x + y*vec.y + z*vec.z;
    }
    Vector3I cross(const Vector3I &vec) const   // returns the cross product between this Vector3I and the provided Vector3I 'vec'
    {
        return Vector3I(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
    }
    Vector3 normalized() const                  // returns the direction of this Vector3 (magnitude becomes one)
    {
        float length = Vector3I::length();
        return length == 0 ? Vector3():Vector3(x/length, y/length, z/length);
    }
};

// Vector3I (operators): standard operators for three dimensional integer vectors
inline Vector3I operator +(const Vector3I &vec1, const Vector3I &vec2) {return Vector3I(vec1.x+vec2.x, vec1.y+vec2.y, vec1.z+vec2.z);}
inline Vector3I operator -(const Vector3I &vec1, const Vector3I &vec2) {return Vector3I(vec1.x-vec2.x, vec1.y-vec2.y, vec1.z-vec2.z);}
inline Vector3I operator *(const Vector3I &vec1, const Vector3I &vec2) {return Vector3I(vec1.x*vec2.x, vec1.y*vec2.y, vec1.z*vec2.z);} // all multiplication operator overloads simply multiply each of the provided Vector2s components with each other
inline Vector3I operator /(const Vector3I &vec1, const Vector3I &vec2) {return Vector3I(vec1.x/vec2.x, vec1.y/vec2.y, vec1.z/vec2.z);} // all division operator overloads simply divide each of the provided Vector2s components by each other
inline Vector3I operator +(const Vector3I &vec, float num) {return Vector3I(vec.x+num, vec.y+num, vec.z+num);}
inline Vector3I operator -(const Vector3I &vec, float num) {return Vector3I(vec.x-num, vec.y-num, vec.z-num);}
inline Vector3I operator *(const Vector3I &vec, float num) {return Vector3I(vec.x*num, vec.y*num, vec.z*num);}
inline Vector3I operator /(const Vector3I &vec, float num) {return Vector3I(vec.x/num, vec.y/num, vec.z/num);}
inline Vector3I operator -(const Vector3I &vec) {return Vector3I(-vec.x, -vec.y, -vec.z);}
inline Vector3I &operator +=(Vector3I &vec1, const Vector3I &vec2)
{
    vec1.x += vec2.x;
    vec1.y += vec2.y;
    vec1.z += vec2.z;
    return vec1;
}
inline Vector3I &operator -=(Vector3I &vec1, const Vector3I &vec2)
{
    vec1.x -= vec2.x;
    vec1.y -= vec2.y;
    vec1.z -= vec2.z;
    return vec1;
}
inline Vector3I &operator *=(Vector3I &vec1, const Vector3I &vec2)
{
    vec1.x *= vec2.x;
    vec1.y *= vec2.y;
    vec1.z *= vec2.z;
    return vec1;
}
inline Vector3I &operator /=(Vector3I &vec1, const Vector3I &vec2)
{
    vec1.x /= vec2.x;
    vec1.y /= vec2.y;
    vec1.z /= vec2.z;
    return vec1;
}
inline bool operator ==(const Vector3I &vec1, const Vector3I &vec2)
{
    return (vec2.x == vec1.x && vec2.y == vec1.y && vec2.z == vec1.z);
}
inline bool operator !=(const Vector3I &vec1, const Vector3I &vec2)
{
    return (vec2.x != vec1.x || vec2.y != vec1.y || vec2.z != vec1.z);
}
inline bool operator <(const Vector3I &vec1, const Vector3I &vec2)
{
    return vec1.length2() < vec2.length2();
}
inline bool operator >(const Vector3I &vec1, const Vector3I &vec2)
{
    return vec1.length2() > vec2.length2();
}
inline bool operator <=(const Vector3I &vec1, const Vector3I &vec2)
{
    return vec1.length2() <= vec2.length2();
}
inline bool operator >=(const Vector3I &vec1, const Vector3I &vec2)
{
    return vec1.length2() >= vec2.length2();
}

// mat4x4 (structure): structure that holds an array of 16 floats and allows 4 by 4 matrix operations
struct mat4x4
{
    float matrix[16];

    mat4x4()
    {
        for(int i=0; i<16; i++) matrix[i] = 0;
    }
    explicit mat4x4(float scale) // creates a scaled identity matrix by quantity 'scale'
    {
        for(int i=0; i<16; i++) matrix[i] = 0;
        matrix[0] = matrix[5] = matrix[10] = matrix[15] = scale;
    }

    mat4x4 inverted() const                        // returns the inverted form of this matrix :: https://semath.info/src/inverse-cofactor-ex4.html
    {
        mat4x4 inv;
        double det;
        int i;

        inv.matrix[0] = 
        matrix[5]  * matrix[10] * matrix[15] - 
        matrix[5]  * matrix[11] * matrix[14] - 
        matrix[9]  * matrix[6]  * matrix[15] + 
        matrix[9]  * matrix[7]  * matrix[14] +
        matrix[13] * matrix[6]  * matrix[11] - 
        matrix[13] * matrix[7]  * matrix[10];

        inv.matrix[4] = 
       -matrix[4]  * matrix[10] * matrix[15] + 
        matrix[4]  * matrix[11] * matrix[14] + 
        matrix[8]  * matrix[6]  * matrix[15] - 
        matrix[8]  * matrix[7]  * matrix[14] - 
        matrix[12] * matrix[6]  * matrix[11] + 
        matrix[12] * matrix[7]  * matrix[10];

        inv.matrix[8] = 
        matrix[4]  * matrix[9] * matrix[15] - 
        matrix[4]  * matrix[11] * matrix[13] - 
        matrix[8]  * matrix[5] * matrix[15] + 
        matrix[8]  * matrix[7] * matrix[13] + 
        matrix[12] * matrix[5] * matrix[11] - 
        matrix[12] * matrix[7] * matrix[9];

        inv.matrix[12] = 
       -matrix[4]  * matrix[9] * matrix[14] + 
        matrix[4]  * matrix[10] * matrix[13] +
        matrix[8]  * matrix[5] * matrix[14] - 
        matrix[8]  * matrix[6] * matrix[13] - 
        matrix[12] * matrix[5] * matrix[10] + 
        matrix[12] * matrix[6] * matrix[9];

        inv.matrix[1] = 
       -matrix[1]  * matrix[10] * matrix[15] + 
        matrix[1]  * matrix[11] * matrix[14] + 
        matrix[9]  * matrix[2] * matrix[15] - 
        matrix[9]  * matrix[3] * matrix[14] - 
        matrix[13] * matrix[2] * matrix[11] + 
        matrix[13] * matrix[3] * matrix[10];

        inv.matrix[5] = 
        matrix[0]  * matrix[10] * matrix[15] - 
        matrix[0]  * matrix[11] * matrix[14] - 
        matrix[8]  * matrix[2] * matrix[15] + 
        matrix[8]  * matrix[3] * matrix[14] + 
        matrix[12] * matrix[2] * matrix[11] - 
        matrix[12] * matrix[3] * matrix[10];

        inv.matrix[9] = 
       -matrix[0]  * matrix[9] * matrix[15] + 
        matrix[0]  * matrix[11] * matrix[13] + 
        matrix[8]  * matrix[1] * matrix[15] - 
        matrix[8]  * matrix[3] * matrix[13] - 
        matrix[12] * matrix[1] * matrix[11] + 
        matrix[12] * matrix[3] * matrix[9];

        inv.matrix[13] = 
        matrix[0]  * matrix[9] * matrix[14] - 
        matrix[0]  * matrix[10] * matrix[13] - 
        matrix[8]  * matrix[1] * matrix[14] + 
        matrix[8]  * matrix[2] * matrix[13] + 
        matrix[12] * matrix[1] * matrix[10] - 
        matrix[12] * matrix[2] * matrix[9];

        inv.matrix[2] = 
        matrix[1]  * matrix[6] * matrix[15] - 
        matrix[1]  * matrix[7] * matrix[14] - 
        matrix[5]  * matrix[2] * matrix[15] + 
        matrix[5]  * matrix[3] * matrix[14] + 
        matrix[13] * matrix[2] * matrix[7] - 
        matrix[13] * matrix[3] * matrix[6];

        inv.matrix[6] = 
       -matrix[0]  * matrix[6] * matrix[15] + 
        matrix[0]  * matrix[7] * matrix[14] + 
        matrix[4]  * matrix[2] * matrix[15] - 
        matrix[4]  * matrix[3] * matrix[14] - 
        matrix[12] * matrix[2] * matrix[7] + 
        matrix[12] * matrix[3] * matrix[6];

        inv.matrix[10] = 
        matrix[0]  * matrix[5] * matrix[15] - 
        matrix[0]  * matrix[7] * matrix[13] - 
        matrix[4]  * matrix[1] * matrix[15] + 
        matrix[4]  * matrix[3] * matrix[13] + 
        matrix[12] * matrix[1] * matrix[7] - 
        matrix[12] * matrix[3] * matrix[5];

        inv.matrix[14] =
       -matrix[0]  * matrix[5] * matrix[14] + 
        matrix[0]  * matrix[6] * matrix[13] + 
        matrix[4]  * matrix[1] * matrix[14] - 
        matrix[4]  * matrix[2] * matrix[13] - 
        matrix[12] * matrix[1] * matrix[6] + 
        matrix[12] * matrix[2] * matrix[5];

        inv.matrix[3] = 
       -matrix[1] * matrix[6] * matrix[11] + 
        matrix[1] * matrix[7] * matrix[10] + 
        matrix[5] * matrix[2] * matrix[11] - 
        matrix[5] * matrix[3] * matrix[10] - 
        matrix[9] * matrix[2] * matrix[7] + 
        matrix[9] * matrix[3] * matrix[6];

        inv.matrix[7] = 
        matrix[0] * matrix[6] * matrix[11] - 
        matrix[0] * matrix[7] * matrix[10] - 
        matrix[4] * matrix[2] * matrix[11] + 
        matrix[4] * matrix[3] * matrix[10] + 
        matrix[8] * matrix[2] * matrix[7] - 
        matrix[8] * matrix[3] * matrix[6];

        inv.matrix[11] = 
       -matrix[0] * matrix[5] * matrix[11] + 
        matrix[0] * matrix[7] * matrix[9] + 
        matrix[4] * matrix[1] * matrix[11] - 
        matrix[4] * matrix[3] * matrix[9] - 
        matrix[8] * matrix[1] * matrix[7] + 
        matrix[8] * matrix[3] * matrix[5];

        inv.matrix[15] = 
        matrix[0] * matrix[5] * matrix[10] - 
        matrix[0] * matrix[6] * matrix[9] - 
        matrix[4] * matrix[1] * matrix[10] + 
        matrix[4] * matrix[2] * matrix[9] + 
        matrix[8] * matrix[1] * matrix[6] - 
        matrix[8] * matrix[2] * matrix[5];

        det = matrix[0] * inv.matrix[0] + matrix[1] * inv.matrix[4] + matrix[2] * inv.matrix[8] + matrix[3] * inv.matrix[12];

        if (det == 0)
            return mat4x4(1);

        det = 1.0 / det;

        for (i = 0; i < 16; i++)
            inv.matrix[i] *= det;

        return inv;
    }
    mat4x4 scaled(const Vector3 &vector) const     // returns this matrix with its 0th, 5th, and 10th indices scaled by the x, y, and z of 'vector' :: multiplying a matrix by this will scale its size by 'vector'
    {
        mat4x4 result = *this;
        result.matrix[0] *= vector.x;
        result.matrix[5] *= vector.y;
        result.matrix[10] *= vector.z;
        return result;
    }
    mat4x4 to3x3() const                           // returns this matrix with its 3rd, 7th, 11th, 12th, 13th, and 14th elements set to zero
    {
        mat4x4 result = *this;
        result.matrix[3] = result.matrix[7] = result.matrix[11] = result.matrix[12]= result.matrix[13]= result.matrix[14] = 0;
        return result;
    }
    mat4x4 translated(const Vector3 &vector) const // returns this matrix with its 3rd, 7th, and 11th indices changed to the x, y, and z of 'vector' :: multiplying a matrix by this will translate its coordinates to 'vector' position
    {
        mat4x4 result = *this;
        result.matrix[3] = vector.x;
        result.matrix[7] = vector.y;
        result.matrix[11] = vector.z;

        return result;
    }
    mat4x4 transposed() const                      // returns this matrix with its columns and rows switched :: useful for graphics libraries which read matrix data differently
    {
        mat4x4 result = mat4x4(0);
        result.matrix[0] = matrix[0];
        result.matrix[1] = matrix[4];
        result.matrix[2] = matrix[8];
        result.matrix[3] = matrix[12];

        result.matrix[4] = matrix[1];
        result.matrix[5] = matrix[5];
        result.matrix[6] = matrix[9];
        result.matrix[7] = matrix[13];

        result.matrix[8] = matrix[2];
        result.matrix[9] = matrix[6];
        result.matrix[10] = matrix[10];
        result.matrix[11] = matrix[14];

        result.matrix[12] = matrix[3];
        result.matrix[13] = matrix[7];
        result.matrix[14] = matrix[11];
        result.matrix[15] = matrix[15];

        return result;
    }
    
    void scale(const Vector3 &vector)
    {
        matrix[0] *= vector.x;
        matrix[5] *= vector.y;
        matrix[10] *= vector.z;
    }
    void translate(const Vector3 &vector)
    {
        matrix[3] = vector.x;
        matrix[7] = vector.y;
        matrix[11] = vector.z;
    }

    mat4x4 rotated(const Quaternion &quat) const;  // returns this matrix rotated by the 'quat' via matrix multiplication
    void rotate(const Quaternion &quat);

    operator std::string() const
    {
        return
        "| " + std::to_string(matrix[0]) + ", " + std::to_string(matrix[1]) + ", " + std::to_string(matrix[2]) + ", " + std::to_string(matrix[3]) + " |\n" + 
        "| " +std::to_string(matrix[4]) + ", " + std::to_string(matrix[5]) + ", " + std::to_string(matrix[6]) + ", " + std::to_string(matrix[7]) + " |\n" + 
        "| " +std::to_string(matrix[8]) + ", " + std::to_string(matrix[9]) + ", " + std::to_string(matrix[10]) + ", " + std::to_string(matrix[11]) + " |\n" + 
        "| " +std::to_string(matrix[12]) + ", " + std::to_string(matrix[13]) + ", " + std::to_string(matrix[14]) + ", " + std::to_string(matrix[15]) + " |\n";
    }
};

// mat4x4 (operators): standard operators for 4 by 4 matrices
inline Vector4 operator *(const mat4x4 &mat, const Vector4 &vector)
{
    Vector4 result = Vector4();
    
    result.x = vector.x*mat.matrix[0] + vector.y*mat.matrix[1] + vector.z*mat.matrix[2] + vector.w*mat.matrix[3];
    result.y = vector.x*mat.matrix[4] + vector.y*mat.matrix[5] + vector.z*mat.matrix[6] + vector.w*mat.matrix[7];
    result.z = vector.x*mat.matrix[8] + vector.y*mat.matrix[9] + vector.z*mat.matrix[10] + vector.w*mat.matrix[11];
    result.w = vector.x*mat.matrix[12] + vector.y*mat.matrix[13] + vector.z*mat.matrix[14] + vector.w*mat.matrix[15];

    return result;
}
inline Vector3 operator *(const mat4x4 &mat, const Vector3 &vector)
{
    Vector3 result = Vector3();
    
    result.x = vector.x*mat.matrix[0] + vector.y*mat.matrix[1] + vector.z*mat.matrix[2] + mat.matrix[3];
    result.y = vector.x*mat.matrix[4] + vector.y*mat.matrix[5] + vector.z*mat.matrix[6] + mat.matrix[7];
    result.z = vector.x*mat.matrix[8] + vector.y*mat.matrix[9] + vector.z*mat.matrix[10] + mat.matrix[11];

    return result;
}
inline mat4x4 operator *(const mat4x4 &mat1, const mat4x4 &mat2)
{
    mat4x4 result(0);

    result.matrix[0] = mat1.matrix[0]*mat2.matrix[0] + mat1.matrix[1]*mat2.matrix[4] + mat1.matrix[2]*mat2.matrix[8] + mat1.matrix[3]*mat2.matrix[12];
    result.matrix[1] = mat1.matrix[0]*mat2.matrix[1] + mat1.matrix[1]*mat2.matrix[5] + mat1.matrix[2]*mat2.matrix[9] + mat1.matrix[3]*mat2.matrix[13];
    result.matrix[2] = mat1.matrix[0]*mat2.matrix[2] + mat1.matrix[1]*mat2.matrix[6] + mat1.matrix[2]*mat2.matrix[10] + mat1.matrix[3]*mat2.matrix[14];
    result.matrix[3] = mat1.matrix[0]*mat2.matrix[3] + mat1.matrix[1]*mat2.matrix[7] + mat1.matrix[2]*mat2.matrix[11] + mat1.matrix[3]*mat2.matrix[15];

    result.matrix[4] = mat1.matrix[4]*mat2.matrix[0] + mat1.matrix[5]*mat2.matrix[4] + mat1.matrix[6]*mat2.matrix[8] + mat1.matrix[7]*mat2.matrix[12];
    result.matrix[5] = mat1.matrix[4]*mat2.matrix[1] + mat1.matrix[5]*mat2.matrix[5] + mat1.matrix[6]*mat2.matrix[9] + mat1.matrix[7]*mat2.matrix[13];
    result.matrix[6] = mat1.matrix[4]*mat2.matrix[2] + mat1.matrix[5]*mat2.matrix[6] + mat1.matrix[6]*mat2.matrix[10] + mat1.matrix[7]*mat2.matrix[14];
    result.matrix[7] = mat1.matrix[4]*mat2.matrix[3] + mat1.matrix[5]*mat2.matrix[7] + mat1.matrix[6]*mat2.matrix[11] + mat1.matrix[7]*mat2.matrix[15];

    result.matrix[8] = mat1.matrix[8]*mat2.matrix[0] + mat1.matrix[9]*mat2.matrix[4] + mat1.matrix[10]*mat2.matrix[8] + mat1.matrix[11]*mat2.matrix[12];
    result.matrix[9] = mat1.matrix[8]*mat2.matrix[1] + mat1.matrix[9]*mat2.matrix[5] + mat1.matrix[10]*mat2.matrix[9] + mat1.matrix[11]*mat2.matrix[13];
    result.matrix[10] = mat1.matrix[8]*mat2.matrix[2] + mat1.matrix[9]*mat2.matrix[6] + mat1.matrix[10]*mat2.matrix[10] + mat1.matrix[11]*mat2.matrix[14];
    result.matrix[11] = mat1.matrix[8]*mat2.matrix[3] + mat1.matrix[9]*mat2.matrix[7] + mat1.matrix[10]*mat2.matrix[11] + mat1.matrix[11]*mat2.matrix[15];

    result.matrix[12] = mat1.matrix[12]*mat2.matrix[0] + mat1.matrix[13]*mat2.matrix[4] + mat1.matrix[14]*mat2.matrix[8] + mat1.matrix[15]*mat2.matrix[12];
    result.matrix[13] = mat1.matrix[12]*mat2.matrix[1] + mat1.matrix[13]*mat2.matrix[5] + mat1.matrix[14]*mat2.matrix[9] + mat1.matrix[15]*mat2.matrix[13];
    result.matrix[14] = mat1.matrix[12]*mat2.matrix[2] + mat1.matrix[13]*mat2.matrix[6] + mat1.matrix[14]*mat2.matrix[10] + mat1.matrix[15]*mat2.matrix[14];
    result.matrix[15] = mat1.matrix[12]*mat2.matrix[3] + mat1.matrix[13]*mat2.matrix[7] + mat1.matrix[14]*mat2.matrix[11] + mat1.matrix[15]*mat2.matrix[15];

    return result;
}
inline bool operator ==(const mat4x4 &mat1, const mat4x4 &mat2)
{
    for(int i=0; i<16; i++)
    {
        if(mat2.matrix[i] != mat1.matrix[i]) return false;
    }
    return true;
}
inline bool operator !=(const mat4x4 &mat1, const mat4x4 &mat2)
{
    return !(mat2 == mat1);
}
inline mat4x4 operator -(const mat4x4 &mat)
{
    mat4x4 result = mat;
    result.matrix[0] = -result.matrix[0];
    result.matrix[1] = -result.matrix[1];
    result.matrix[2] = -result.matrix[2];
    result.matrix[3] = -result.matrix[3];
    result.matrix[4] = -result.matrix[4];
    result.matrix[5] = -result.matrix[5];
    result.matrix[6] = -result.matrix[6];
    result.matrix[7] = -result.matrix[7];
    result.matrix[8] = -result.matrix[8];
    result.matrix[9] = -result.matrix[9];
    result.matrix[10] = -result.matrix[10];
    result.matrix[11] = -result.matrix[11];
    result.matrix[12] = -result.matrix[12];
    result.matrix[13] = -result.matrix[13];
    result.matrix[14] = -result.matrix[14];
    result.matrix[15] = -result.matrix[15];
    return result;
}

// Quaternion (structure): structure that holds four variables (q0, q1, q2, q3) and allows for basic Quaternion operations and conversions :: https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html
struct Quaternion
{
    float q0, q1, q2, q3;

    Quaternion() : q0(1), q1(0), q2(0), q3(0){}
    Quaternion(const mat4x4 &rotationMatrix)             // creates a rotation quaternion by converting a mat4x4 to a Quaterion :: Rotation Matrix Quaternion
    {
        const float *matrix = rotationMatrix.matrix;
        float trace = matrix[0] + matrix[5] + matrix[10]; 
        if( trace > 0 )
        {
            float s = 0.5f / sqrtf(trace+ 1.0f);
            q0 = 0.25f / s;
            q1 = ( matrix[9] - matrix[6] ) * s;
            q2 = ( matrix[2] - matrix[8] ) * s;
            q3 = ( matrix[4] - matrix[1] ) * s;
        } 
        else
        {
            if ( matrix[0] > matrix[5] && matrix[0] > matrix[19] )
            {
                float s = 2.0f * sqrtf( 1.0f + matrix[0] - matrix[5] - matrix[19]);
                q0 = (matrix[9] - matrix[6] ) / s;
                q1 = 0.25f * s;
                q2 = (matrix[1] + matrix[4] ) / s;
                q3 = (matrix[2] + matrix[8] ) / s;
            }
            else if (matrix[5] > matrix[10])
            {
                float s = 2.0f * sqrtf( 1.0f + matrix[5] - matrix[0] - matrix[10]);
                q0 = (matrix[2] - matrix[8] ) / s;
                q1 = (matrix[1] + matrix[4] ) / s;
                q2 = 0.25f * s;
                q3 = (matrix[6] + matrix[9] ) / s;
            }
            else
            {
                float s = 2.0f * sqrtf( 1.0f + matrix[10] - matrix[0] - matrix[5] );
                q0 = (matrix[4] - matrix[1] ) / s;
                q1 = (matrix[2] + matrix[8] ) / s;
                q2 = (matrix[6] + matrix[9] ) / s;
                q3 = 0.25f * s;
            }
        }
    }
    Quaternion(float theta, const Vector3 &direction)    // creates a rotation quaternion rotated 'theta' radians around the 'direction' axis :: Axis-Angle Quaternion
    {
        q0 = std::cos(theta/2);
        q1 = direction.x*std::sin(theta/2);
        q2 = direction.y*std::sin(theta/2);
        q3 = direction.z*std::sin(theta/2);
    }

    Vector3 imaginary() const // returns the imaginary components of a quaternion (q1, q2, q3) as a Vector3
    {
        return Vector3(q1, q2, q3);
    }
    Vector3 euler() const
    {
        float atanOne = std::atan2(q3, q0);
        float atanTwo = std::atan2(-q1, q2);
        return Vector3(atanOne - atanTwo, std::acos(2*(q0*q0 + q3*q3) - 1), atanOne + atanTwo);
    }

    Quaternion normalized() const // returns this quaternion altered to have a magnitude of one
    {
        Quaternion q;
        float length = std::sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
        q.q0 = q0/length;
        q.q1 = q1/length;
        q.q2 = q2/length;
        q.q3 = q3/length;
        return q;
    }
    Quaternion conjugate() const
    {
        Quaternion q = *this;
        q.q1 = -q1;
        q.q2 = -q2;
        q.q3 = -q3;
        return q;
    }
    Quaternion inverted() const
    {
        Quaternion conjugate = this->conjugate();
        return conjugate/(*this*conjugate);
    }

    Quaternion operator *(const Quaternion &rotation) const
    {
        Quaternion result = Quaternion();
        result.q0 = q0*rotation.q0 - q1*rotation.q1 - q2*rotation.q2 - q3*rotation.q3;
        result.q1 = q0*rotation.q1 + q1*rotation.q0 - q2*rotation.q3 + q3*rotation.q2;
        result.q2 = q0*rotation.q2 + q1*rotation.q3 + q2*rotation.q0 - q3*rotation.q1;
        result.q3 = q0*rotation.q3 - q1*rotation.q2 + q2*rotation.q1 + q3*rotation.q0;
        return result;
    }
    Quaternion operator /(const Quaternion &rotation) const
    {
        Quaternion result = Quaternion();
        float dividend = q0*q0 + q1*q1 + q2*q2 + q3*q3;
        result.q0 = (q0*rotation.q0 + q1*rotation.q1 + q2*rotation.q2 + q3*rotation.q3)/dividend;
        result.q1 = (q0*rotation.q1 + q1*rotation.q0 + q2*rotation.q3 + q3*rotation.q2)/dividend;
        result.q2 = (q0*rotation.q2 + q1*rotation.q3 + q2*rotation.q0 + q3*rotation.q1)/dividend;
        result.q3 = (q0*rotation.q3 + q1*rotation.q2 + q2*rotation.q1 + q3*rotation.q0)/dividend;
        return result;
    }
    Quaternion &operator *=(const Quaternion &rotation)
    {
        (*this) = (*this) * rotation;
        return *this;
    }
    bool operator ==(const Quaternion &quat) const
    {
        return q0 == quat.q0 && q1 == quat.q1 && q2 == quat.q2 && q3 == quat.q3;
    }
    operator Vector3() const
    {
        return Vector3(2*(q1*q3 - q0*q2), 2*(q2*q3 - q0*q1), 1 - 2*(q1*q1 + q2*q2));
    }
    operator mat4x4() const
    {
        mat4x4 rotation = mat4x4(1);

        rotation.matrix[0] = 1 - 2*q2*q2 - 2*q3*q3; // s2 e3
        rotation.matrix[1] = 2*q1*q2 - 2*q0*q3; // e1
        rotation.matrix[2] = 2*q1*q3 + 2*q0*q2; // s0 

        rotation.matrix[4] = 2*q1*q2 + 2*q0*q3; // e0
        rotation.matrix[5] = 1 - 2*q1*q1 - 2*q3*q3; //e2
        rotation.matrix[6] = 2*q2*q3 - 2*q0*q1;

        rotation.matrix[8] = 2*q1*q3 - 2*q0*q2; // s1
        rotation.matrix[9] = 2*q2*q3 + 2*q0*q1;
        rotation.matrix[10] = 1 - 2*q1*q1 - 2*q2*q2; // s3

        return rotation;
    }
    operator std::string() const
    {
        return std::to_string(q0) + ", " + std::to_string(q1) + ", " + std::to_string(q2) + ", " + std::to_string(q3);
    }

};

struct Quad
{
    Vector2 p1, p2, p3, p4;
    
    Quad() {};
    Quad(const Vector2& one, const Vector2& two, const Vector2& three, const Vector2& four) : p1(one), p2(two), p3(three), p4(four) {}
};

namespace vec2
{
    const Vector2 zero(0, 0);
    const Vector2 one(1, 1);
    const Vector2 left(-1, 0);
    const Vector2 right(1, 0);
    const Vector2 up(0, 1);
    const Vector2 down(0, -1);

    Vector2 abs(const Vector2 &vector);                                               // returns "math::abs" of each of its components as a new Vector2
    Vector2 degrees(const Vector2& vector);
    Vector2 lerp(const Vector2& vec1, const Vector2& vec2, float weight);             // returns 'vec1' linearly interpolated to 'vec2' by weight
    Vector2 min(const Vector2 &vec1, const Vector2 &vec2);                            // returns "std::max" of each of its components as a new Vector2
    Vector2 pow(const Vector2 &vector, float exponent);                               // returns "std::pow" of each of its components as a new Vector2
    Vector2 radians(const Vector2& vector);
    Vector2 rotatedAround(const Vector2& vector, const Vector2& origin, float theta);
    Vector2 sign(const Vector2 &vector);                                              // returns the "math::sign" of each of its components as a new Vector2
    Vector2 sign0(const Vector2 &vector);                                             // returns the "math::sign0" of each of its components as a new Vector2

    std::vector<Vector2> bezier(const std::vector<Vector2>& points, const std::vector<Vector2>& controls, float partition);
}
namespace vec3
{
    const Vector3 zero(0, 0, 0);
    const Vector3 one(1, 1, 1);
    const Vector3 left(-1, 0, 0);
    const Vector3 right(1, 0, 0);
    const Vector3 up(0, 1, 0);
    const Vector3 down(0, -1, 0);
    const Vector3 forward(0, 0, 1);
    const Vector3 back(0, 0, -1);

    Vector3 pow(const Vector3 &vector, float exponent);                                        // returns "std::pow" of each of its components as a new Vector3
    Vector3 abs(const Vector3 &vector);                                                        // returns "math::abs" of each of its components as a new Vector3
    Vector3 max(const Vector3 &vec1, const Vector3 &vec2);                                     // returns "std::max" of each of its components as a new Vector3
    Vector3 sign(const Vector3 &vector);                                                       // returns "math::sign" of each of its components as a new Vector3
    Vector3 sign0(const Vector3 &vector);                                                      // returns "math::sign0" of each of its components as a new Vector3
    Vector3 roundTo(const Vector3& vector, int32_t precision);                                 // returns "math::roundTo" of each of its components as a new Vector3
    Vector3 triSurface(const Vector3 &vec1, const Vector3 &vec2, const Vector3 &pThree);       // returns normal vector of the triangular area described by the provided position vectors
    Vector3 lerp(const Vector3& vec1, const Vector3& vec2, float weight);                      // returns 'vec1' linearly interpolated to 'vec2' by weight
    Vector3 slerp(const Vector3& vec1, const Vector3& vec2, float weight);                     // returns 'vec1' spherically interpolated to 'vec2' by weight (INCOMPLETE)
    bool inRange(const Vector3& vec1, const Vector3& vec2, float proximity);                   // returns whether 'vec1' is in proximity of 'vec2'
    float angle(const Vector3& vec1, const Vector3& vec2);                                     // returns the angle between 'vec1' and 'vec2'
    float signedAngle(const Vector3& vec1, const Vector3& vec2, const Vector3& up = vec3::up); // returns the angle between 'vec1' and 'vec2' with 'up' being considered the positive direction
}
namespace mat4
{
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix

    mat4x4 inter(float fov, float scale, float aspect, float clipNear, float clipFar, float weight); // creates a projection matrix interpolated between an orthogonal and perspective matrix by 'weight'
    mat4x4 ortho(float scale, float aspect, float clipNear, float clipFar);                          // creates an orthogonal projection matrix using the window proportions
    mat4x4 ortho(float left, float right, float bottom, float top, float zNear, float zFar);         // creates an orthogonal projection with provided proportions (INCOMPLETE)
    mat4x4 per(float fov, float aspect, float clipNear, float clipFar);                              // creates a perspective projection matrix
    mat4x4 lookAt(const Vector3 &position, const Vector3 &direction, const Vector3 &up);             // creates a view matrix representing a 'direcition' from a 'position' relative to 'up'
}

// defines hash functions for custom classes (Vector3)
namespace std
{
    template <>
    struct hash<Vector3>
    {
        std::size_t operator()(const Vector3& vector) const
        {
            return 
                ((std::hash<float>()(vector.x) ^
                (std::hash<float>()(vector.y) << 1)) >> 1) ^
                (std::hash<float>()(vector.z) << 1);
        }
    };

}
std::string to_string(const Vector3& vector);
std::ostream& operator<<(std::ostream& os, const Vector2& obj);
std::ostream& operator<<(std::ostream& os, const Vector3& obj);
std::ostream& operator<<(std::ostream& os, const Vector4& obj);
std::ostream& operator<<(std::ostream& os, const Quaternion& obj);
std::ostream& operator<<(std::ostream& os, const mat4x4& obj);
std::ostream& operator<<(std::ostream& os, const Quad& obj);

#endif