#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>

#include "color.h"
#include "vector.h"

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
    void update(float max = std::numeric_limits<float>::infinity());
    void reset()
    {
        timer = timer-interval;
    }

    private:
        float lastFrame;
};

// Buffer (struct): maintains a boolean variable as true for a set amount of time
struct Buffer
{
    bool primed;
    int time, frameCount;

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
    SpotLight(const Vector3& direction__, const Color& color__, float strength__, float constant__, float linear__, float quadratic__, float outerCutOff__, float cutoff__) : direction(direction__), color(color__), strength(strength__), constant(constant__), linear(linear__), quadratic(quadratic__), outerCutOff(outerCutOff__), cutoff(cutoff__) {}
    SpotLight(const Vector3& direction__, const Color& color__, float strength__, const Vector3 values, float outerCutOff__, float cutoff__) : SpotLight(direction__, color__, strength__, values.x, values.y, values.z, outerCutOff__, cutoff__) {}
};


// RENDERING COMPONENTS

// Camera (struct): allows for the scene to be rendered from a certain perspective
struct Camera
{
    Color backgroundColor;
    Vector3 front, up;
    mat4x4 view, projection;

    Camera() {}
    Camera(Color color__, Vector3 front__, Vector3 up__);
};

// Transform (struct): holds data about an entity's position, rotation, and scale
struct Transform
{
    Vector3 position;
    Vector3 scale;
    Quaternion rotation;

    Transform() {}
    Transform(const Vector3& position__) : Transform(position__, vec3::one, Quaternion()) {}
    Transform(const Vector3& position__, const Vector3& scale__) : Transform(position__, scale__, Quaternion()) {}
    Transform(const Vector3& position__, const Vector3& scale__, const Quaternion& rotation__) : position(position__), scale(scale__), rotation(rotation__) {}
};

// Physics2D (struct): handles basic 2D physics operations at 60 FPS :: all functions should be called in the "update" function for responsiveness and consistency
struct Physics2D
{
    Vector2 force, impulse, velocity, drag, terminal;
    float mass;

    Vector3 delta, lastDelta;
    bool collisions[4];

    Timer time;
    float maxDeltaTime = 1;

    Physics2D(float mass__ = 1, const Vector2& drag__ = 1, const Vector2& terminal__ = 0) : mass(mass__), drag(drag__), terminal(terminal__) {time = Timer(1.0/60.0);}

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

    void addForce(const Vector2 newForce)
    {
        force += newForce;
    }
    void addImpulse(const Vector2 newForce)
    {
        impulse += newForce;
    }

    Vector2 acceleration()
    {
        return force/mass;
    }
};

#endif