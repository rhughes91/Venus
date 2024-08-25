#pragma once

#include <array>
#include <set>

#include "color.h"
#include "vector.h"
#include "structure.h"

// UTILITY COMPONENTS

// Timer (struct): allows for a code to be run at regular intervals :: emulates infrastructure of the "fixedUpdate" System function
struct Timer
{
    float interval, timer, iterations;

    Timer() : interval(0.02f) {}
    Timer(float interval__) : interval(interval__) {}

    void begin()
    {
        float currentFrame = interval;
        lastFrame = currentFrame;
    }
    void update(float delta, float max = std::numeric_limits<float>::infinity());
    void reset()
    {
        timer = timer-interval;
    }

    bool set()
    {
        return timer > interval;
    }

    private:
        float lastFrame;
};

// Buffer (struct): maintains a boolean variable as true for a set amount of time
struct Buffer
{
    int time;
    bool primed;
    int frameCount;

    Buffer() : time(0), primed(false){}
    Buffer(int length) : time(length), primed(false){}
    void prime()
    {
        reset();
        primed = true;
    }
    void update()
    {
        if(frameCount >= time)
        {
            reset();
        }
        else
        {
            frameCount+=primed;
        }
    }
    void reset()
    {
        primed = false;
        frameCount = 0;
    }
};


// LIGHT COMPONENTS

// PointLight (struct): holds data needed to render a point light
struct PointLight
{
    Color color;
    float strength;
    float constant, linear, quadratic;

    PointLight() {}
    PointLight(const Color& color__, float strength__, const Vector3& values) : color(color__), strength(strength__), constant(values.x), linear(values.y), quadratic(values.z) {}
};

// SpotLight (struct): holds data needed to render a spotlight
struct SpotLight
{
    Vector3 direction;
    Color color;
    float strength;
    float constant, linear, quadratic;
    float outerCutOff, cutoff;

    SpotLight() {}
    SpotLight(const Vector3& direction__, const Color& color__, float strength__, const Vector3& values, float outerCutOff__, float cutoff__) : direction(direction__), color(color__), strength(strength__), constant(values.x), linear(values.y), quadratic(values.z), outerCutOff(outerCutOff__), cutoff(cutoff__) {}
};


// RENDERING COMPONENTS

// Transform (struct): holds data about an entity's position, rotation, and scale
struct Transform
{
    Vector3 position, storedPosition;
    Vector3 scale;
    Quaternion rotation, lastRotation;

    Transform() {}
    Transform(const Vector3& position__) : Transform(position__, vec3::one, Quaternion()) {}
    Transform(const Vector3& position__, const Vector3& scale__) : Transform(position__, scale__, Quaternion()) {}
    Transform(const Vector3& position__, const Quaternion& rotation__) : Transform(position__, vec3::one, rotation__) {}
    Transform(const Vector3& position__, const Vector3& scale__, const Quaternion& rotation__) : position(position__), storedPosition(position__), scale(scale__), rotation(rotation__), lastRotation(rotation__) {}
};

// Camera (struct): allows for the scene to be rendered from a certain perspective
struct Camera
{
    Color backgroundColor;
    Vector3 front, up;
    mat4x4 view, projection;
    float nearDistance, farDistance, fov;

    Camera() {}
    Camera(Color color__, float aspect__, Vector3 front__ = vec3::forward, Vector3 up__ = vec3::up, float near__ = 0.01f, float far__ = 200.f, float fov___ = 45);

    Frustum getFrustum(const Vector3& position, float aspect);
};


// PHYSICS COMPONENTS

// Physics2D (struct): handles basic 2D physics operations at 60 FPS :: all functions should be called in the "update" function for responsiveness and consistency
struct Physics2D
{
    float mass;
    Vector3 force, impulse, velocity, drag, terminal;
    bool dragDirection = true;

    Vector3 delta, lastDelta;
    bool collisions[4];

    Timer time;
    float maxDeltaTime = 1;

    Physics2D(float mass__ = 1, const Vector3& drag__ = 1, const Vector3& terminal__ = 0) : mass(mass__), drag(drag__), terminal(terminal__) {time = Timer(1.0/60.0); dragDirection = true;}

    void setRuntimeInterval(float interval)
    {
        time.interval = interval;
    }
    float runtimeInterval()
    {
        return time.interval;
    }

    bool colliding() const
    {
        return collisions[0] || collisions[1] || collisions[2] || collisions[3];
    }
    void resetCollisions()
    {
        collisions[0] = collisions[1] = collisions[2] = collisions[3] = false;
    }
    std::string collisionsToString() const;

    void resetVelocity()
    {
        velocity = 0;
    }
    void resetForce()
    {
        force = 0;
    }
    void resetImpulse()
    {
        impulse = 0;
    }

    void addForce(const Vector3& newForce)
    {
        force += newForce;
    }
    void addImpulse(const Vector3& newForce)
    {
        impulse += newForce;
    }

    void teleport(const Vector3& newPosition)
    {
        delta = lastDelta = newPosition;
    }

    Vector3 acceleration()
    {
        return force/mass;
    }
};

// BoxCollider (struct): calls 'trigger' function whenever another BoxCollider intersects with this one :: otherwise, miss function is called
struct BoxCollider
{
    bool mobile = false;
    Vector3 scale, offset;

    bool enter, exit;

    BoxCollider(const Vector3& scale__ = 1, const Vector3& offset__ = 0) : scale(scale__), offset(offset__) {}
};

// 
struct SquareCollider
{
    bool mobile = false;
    Vector3 scale, offset, storedPosition;

    bool enter, exit;

    SquareCollider(const Vector3& scale__ = 1, const Vector3& offset__ = 0) : scale(scale__), offset(offset__) {}
};

// 
struct AABB2D
{

};

// 
struct AABB : AABB2D
{

};

// Billboard (struct): forces the 'target' to rotate towards the active Camera
struct Billboard
{
    uint32_t target;
    Vector3 limit = Vector3(1, 1, 1);
};


// physics (namespace): allows for collision handling with a BoxCollider
namespace physics
{
    enum Direction {UP, RIGHT, DOWN, LEFT};
    void collisionTrigger(uint32_t entity, uint32_t collision, bool edge, int triggered, object::ecs& container);
    void collisionMiss(uint32_t entity, object::ecs& container);
}

//
namespace object
{
    Vector3 brightness(int32_t value);
}