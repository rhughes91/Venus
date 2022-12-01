#include "vector.h"
#include "color.h"
#include <iomanip>

Vector2 vec2::sign(const Vector2 &vector)
{
    return Vector2(math::sign(vector.x), math::sign(vector.y));
}
Vector2 vec2::sign0(const Vector2 &vector)
{
    return Vector2(math::sign0(vector.x), math::sign0(vector.y));
}

Vector3 vec3::pow(const Vector3 &vector, float exponent)
{
    return Vector3(std::pow(vector.x, exponent), std::pow(vector.y, exponent), std::pow(vector.z, exponent));
}
Vector3 vec3::max(const Vector3 &vec1, const Vector3 &vec2)
{
    return Vector3(std::max(vec1.x, vec2.x), std::max(vec1.y, vec2.y), std::max(vec1.z, vec2.z));
}
Vector3 vec3::abs(const Vector3 &vector)
{
    return Vector3(math::abs(vector.x), math::abs(vector.y), math::abs(vector.z));
}
Vector3 vec3::sign(const Vector3 &vector)
{
    return Vector3(math::sign(vector.x), math::sign(vector.y), math::sign(vector.z));
}
Vector3 vec3::sign0(const Vector3 &vector)
{
    return Vector3(math::sign0(vector.x), math::sign0(vector.y), math::sign0(vector.z));
}
Vector3 vec3::roundTo(const Vector3& vector, int32_t precision)
{
    return Vector3(std::round(vector.x * std::pow(10, precision)) / std::pow(10, precision), std::round(vector.y * std::pow(10, precision)) / std::pow(10, precision), std::round(vector.z * std::pow(10, precision)) / std::pow(10, precision));
}
Vector3 vec3::triSurface(const Vector3 &vec1, const Vector3 &vec2, const Vector3 &pThree)
{
    Vector3 u = vec2 - vec1;
    Vector3 v = pThree - vec1;

    return Vector3((u.y*v.z - u.z*v.y) * math::sign(vec1.x), (u.z*v.x - u.x*v.z)* math::sign(vec1.y), (u.x*v.y - u.y*v.x) * math::sign(vec1.z));
}
Vector3 vec3::lerp(const Vector3& vec1, const Vector3& vec2, float weight)
{
    return vec1 + (vec2-vec1) * weight;
}
Vector3 vec3::slerp(const Vector3& vec1, const Vector3& vec2, float weight)
{
    float dot = vec1.dot(vec2);
    
    dot = math::clamp(dot, -1.0f, 1.0f);
    float theta = std::acos(dot)*weight;
    Vector3 RelativeVec = vec2 - vec1*dot;
    RelativeVec.normalize();
    
    return ((vec1*std::cos(theta)) + (RelativeVec*std::sin(theta)));
}
bool vec3::inRange(const Vector3& vec1, const Vector3& vec2, float proximity)
{
    return (vec1-vec2).length() < proximity;
}
float vec3::angle(const Vector3& vec1, const Vector3& vec2)
{
    return std::acos(math::clamp(vec1.dot(vec2) / (vec1.length() * vec2.length()), -1, 1));
}
float vec3::signedAngle(const Vector3& vec1, const Vector3& vec2, const Vector3& up)
{
    return vec3::angle(vec1, vec2) * math::sign(vec1.cross(vec2).dot(vec3::up));
}

std::string to_string(const Vector3& vector)
{
    return (std::string)vector;
}

std::ostream& operator<<(std::ostream& os, const Color& obj)
{
    os << std::setprecision(5) << std::fixed << "[" << obj.r << ", " << obj.g << ", " << obj.b << "]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const Vector2& obj)
{
    os << std::setprecision(5) << std::fixed << "[" << obj.x << ", " << obj.y << "]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const Vector3& obj)
{
    os << std::setprecision(5) << std::fixed << "[" << obj.x << ", " << obj.y << ", " << obj.z << "]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const Quaternion& obj)
{
    os << std::setprecision(5) << std::fixed << "[" << obj.q0 << ", " << obj.q1 << ", " << obj.q2 << ", " << obj.q3 << "]";
    return os;
}
std::ostream& operator<<(std::ostream& os, const mat4x4& obj)
{
    os << std::setprecision(5) << std::fixed << (std::string)obj;
    return os;
}

void mat4x4::rotate(const Quaternion &quat)
{
    *this = *this * ((mat4x4)quat);
}
mat4x4 mat4x4::rotated(const Quaternion &quat) const
{
    mat4x4 result = *this;
    return result * (mat4x4)quat;
}
mat4x4 mat4::inter(float fov, float scale, float aspect, float clipNear, float clipFar, float weight)
{
    float sIntensity = 1.5f, 
    sZero = 0.9f, 
    sOne = 0.8f;
    // float scaleWeight = 1 - std::pow(1 - weight, scaleIntensity);
    float scaleWeight = std::pow(1-std::pow(1-weight, sIntensity*sOne), 1/(sIntensity*sZero));

    int intensity = 1;
    weight = std::pow(weight, intensity) / (std::pow(weight, intensity) + std::pow(1-weight, intensity));

    scale = std::tan(fov/2)*scaleWeight + scale*(1-scaleWeight);
    mat4x4 projection = mat4x4(1);

    float scaleZ = -(clipFar+clipNear)*weight + -2*(1-weight);
    float matrix11 = -2*(clipFar*clipNear)*weight - (clipFar+clipNear)*(1-weight);

    projection.scale(Vector3(1/(scale*aspect), 1/scale, scaleZ/(clipFar-clipNear)));
    projection.matrix[11] = matrix11/(clipFar-clipNear);
    projection.matrix[14] = -weight;
    projection.matrix[15] = 1-weight;

    return projection;
}
mat4x4 mat4::ortho(float scale, float aspect, float clipNear, float clipFar)
{
    mat4x4 projection = mat4x4(1);

    projection.scale(Vector3(1/(scale*aspect), 1/scale, -2/(clipFar-clipNear)));
    projection.matrix[11] = -(clipFar+clipNear)/(clipFar-clipNear);

    return projection;
}
mat4x4 mat4::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    mat4x4 result(1);
	result.matrix[0] = 2 / (right - left);
    result.matrix[5] = 2 / (top - bottom);
	result.matrix[10] = 1 / (zFar - zNear);
	result.matrix[3] = - (right + left) / (right - left);
	result.matrix[7] = - (top + bottom) / (top - bottom);
	result.matrix[11] = - zNear / (zFar - zNear);
    return result;
}
mat4x4 mat4::per(float fov, float aspect, float clipNear, float clipFar)
{
    float scale = std::tan(fov/2);
    mat4x4 projection = mat4x4(1);

    projection.scale(Vector3(1/(scale*aspect), 1/scale, -(clipFar+clipNear)/(clipFar-clipNear)));
    projection.matrix[11] = -2*(clipFar*clipNear)/(clipFar-clipNear);
    projection.matrix[14] = -1;
    projection.matrix[15] = 0;

    return projection;
}
mat4x4 mat4::lookAt(const Vector3 &position, const Vector3 &direction, const Vector3 &cameraUp)
{
    Vector3 right = cameraUp.cross(direction).normalized();
    Vector3 up = direction.cross(right).normalized();

    mat4x4 result = mat4x4(1);
    result.matrix[0] = right.x;
    result.matrix[1] = right.y;
    result.matrix[2] = right.z;

    result.matrix[4] = up.x;
    result.matrix[5] = up.y;
    result.matrix[6] = up.z;

    result.matrix[8] = direction.x;
    result.matrix[9] = direction.y;
    result.matrix[10] = direction.z;

    result.matrix[3] = -right.dot(position);
    result.matrix[7] = -up.dot(position);
    result.matrix[11] = -direction.dot(position);

    result = result;
    return result;
}