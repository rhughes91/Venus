#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>

#include "vector.h"
#include "graphics.h"
#include "machine.h"
#include "ui.h"

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

// DirectionalLight (struct): holds data needed to render a directional light
struct DirectionalLight
{
    Vector3 direction;
    Color color;
    float strength;
};


// RENDERING COMPONENTS

// Camera (struct): allows for the scene to be rendered from a certain perspective
struct Camera
{
    float speed;
    float projectionLevel = 1.0f;

    Color backgroundColor;
    Vector3 front, up;
    mat4x4 view, projection;

    Camera() {}
    Camera(float speed__, Color color__, Vector3 front__, Vector3 up__);
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

// Billboard (struct): forces the 'target' to rotate towards the active Camera
struct Billboard
{
    Entity target;
};

// Model (struct): holds an entity's Material and Mesh data which allows it to be rendered
struct Model
{
    Color color;
    Material material;
    Mesh data;
    uint32_t texture;

    Model(const Color& color__ = color::WHITE, const Material& material__ = Material(), const Mesh &data__ = Mesh(), uint32_t texture__ = 0) : color(color__), material(material__), data(data__), texture(texture__) {}

    // uses the Material's shader of this Model
    void draw();
};

// Rect (struct): screen-space equivalent of Transform :: holds position, rotation, scale, and alignment of an entity
struct Rect
{
    Alignment alignment;
    Vector2 position;
    Vector2 scale;
    Quaternion rotation;

    Rect() {}
    Rect(Alignment alignment__, Vector2 position__ = vec3::zero, Vector2 scale__ = vec3::one, Quaternion rotation__ = Quaternion()) : alignment(alignment__), position(position__), scale(scale__), rotation(rotation__)
    {
        refresh();
    }

    void setAlignment(const Alignment& alignment__)
    {
        alignment = alignment__;
        refresh();
    }
    void setScale(const Vector2& scale__)
    {
        scale = scale__;
        refresh();
    }

    // checks whether the provided position 'vec' is within the bounds of the Rect
    bool contains(const Vector2& vec)
    {
        Vector2 pos = relativePosition();
        return pos.x - scale.x/2 <= vec.x && pos.x + scale.x/2 >= vec.x && pos.y - scale.y/2 <= vec.y && pos.y + scale.y/2 >= vec.y;
    }

    // determines this Rect's 'position' relative to its 'alignment'
    Vector2 relativePosition();

    private:
        Vector2 relativeOrigin;

        // refreshes the relative origin of the Rect
        void refresh()
        {
            relativeOrigin = Vector2(alignment.hAlignment-1, alignment.vAlignment-1);
        }
};

// Button (struct): triggers its 'trigger' function when its attached Rect component is clicked on
struct Button
{
    void(*trigger)(Entity entity);

    Button(void(*trigger__)(Entity entity) = [](Entity entity){}) : trigger(trigger__) {}
};

// TextBox (struct): holds texts within the bounds of its attached Rect component (INCOMPLETE: TEXT RENDERING NOT YET IMPLEMENTED)
struct TextBox
{
    Alignment alignment;
    std::string text;
};

// Animation (struct): holds the data and logic needed to switch between images at a certain 'frameRate'
struct Animation
{
    std::vector<uint32_t> frames;
    int frame = 0;
    int frameRate = 1;

    Animation() {}
    Animation(const std::vector<uint32_t>& frames__)
    {
        frames = frames__;
    }

    Animation(const std::vector<uint32_t>& frames__, int32_t frameRate__)
    {
        frames = frames__;
        frameRate = frameRate__;
    }

    // loops 'frame' between 0 and the size of 'frames' and returns the image data at that index
    uint32_t step();

    private:
        int32_t frameCount = 0;
};

// acts as layered StateMachine :: layer one is AnimationState, layer two is Animation
using AnimationState = StateMachine<Animation>;
using Animator = StateMachine<AnimationState>;

// anim (namespace): holds basic StateMachine transition functions for Animations
namespace anim
{
    // resets 'animation' to frame zero on transition
    void reset(Animation& animation);

    // attempts to maintain frame index from 'last' animation on transition :: otherwise, frame index is set to zero
    void keep(Animation& current, Animation& last);
};

// PHYSICS COMPONENTS


// struct Box
// { 
//     Vector3 min, max;
//     Box(const Vector3 &vmin, const Vector3 &vmax) 
//     { 
//         min = vmin;
//         max = vmax;
//     } 
//     Vector3 bounds[2];
    
//     bool CheckLineBox(Vector3 L1, Vector3 L2, Vector3& Hit)
//     {
//         if (L2.x < min.x && L1.x < min.x) return false;
//         if (L2.x > max.x && L1.x > max.x) return false;
//         if (L2.y < min.y && L1.y < min.y) return false;
//         if (L2.y > max.y && L1.y > max.y) return false;
//         if (L2.z < min.z && L1.z < min.z) return false;
//         if (L2.z > max.z && L1.z > max.z) return false;
//         if (L1.x > min.x && L1.x < max.x &&
//             L1.y > min.y && L1.y < max.y &&
//             L1.z > min.z && L1.z < max.z)
//         {
//             Hit = L1;
//             return true;
//         }
//         if ((GetIntersection(L1.x - min.x, L2.x - min.x, L1, L2, Hit) && InBox(Hit, min, max, 1))
//         || (GetIntersection(L1.y - min.y, L2.y - min.y, L1, L2, Hit) && InBox(Hit, min, max, 2))
//         || (GetIntersection(L1.z - min.z, L2.z - min.z, L1, L2, Hit) && InBox(Hit, min, max, 3))
//         || (GetIntersection(L1.x - max.x, L2.x - max.x, L1, L2, Hit) && InBox(Hit, min, max, 1))
//         || (GetIntersection(L1.y - max.y, L2.y - max.y, L1, L2, Hit) && InBox(Hit, min, max, 2))
//         || (GetIntersection(L1.z - max.z, L2.z - max.z, L1, L2, Hit) && InBox(Hit, min, max, 3)))
//             return true;

//         return false;
//     }

//     bool GetIntersection(float fDst1, float fDst2, Vector3 P1, Vector3 P2, Vector3& Hit)
//     {
//         if ((fDst1 * fDst2) > 0) return false;
//         if (fDst1 == fDst2) return false;
//         Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
//         return true;
//     }

//     bool InBox(Vector3 Hit, Vector3 min, Vector3 max, int Axis)
//     {
//         if (Axis == 1 && Hit.z >= min.z && Hit.z <= max.z && Hit.y >= min.y && Hit.y <= max.y) return true;
//         if (Axis == 2 && Hit.z >= min.z && Hit.z <= max.z && Hit.x >= min.x && Hit.x <= max.x) return true;
//         if (Axis == 3 && Hit.x >= min.x && Hit.x <= max.x && Hit.y >= min.y && Hit.y <= max.y) return true;
//         return false;
//     }

// };

// BoxCollider (struct): calls 'trigger' function whenever another BoxCollider intersects with this one :: otherwise, miss function is called
struct BoxCollider
{
    bool mobile;
    Vector3 scale, storedPosition;
    
    void (*trigger)(Entity, Entity, int);
    void (*miss)(Entity);

    BoxCollider(bool mobile__ = false, const Vector3& scale__ = 1) : mobile(mobile__), scale(scale__)
    {
        trigger = [](Entity entity, Entity target, int triggerd){};
        miss = [](Entity entity){};
    }

    void setTrigger(void (*trigger__)(Entity, Entity, int))
    {
        trigger = trigger__;
    }

    void setMiss(void (*miss__)(Entity))
    {
        miss = miss__;
    }
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

    bool colliding()
    {
        return collisions[0] || collisions[1] || collisions[2] || collisions[3];
    }
    void resetCollisions()
    {
        collisions[0] = collisions[1] = collisions[2] = collisions[3] = false;
    }

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

// physics (namespace): allows for collision handling with a BoxCollider
namespace physics
{
    enum Direction {UP, RIGHT, DOWN, LEFT};
    void collisionHandler(Entity entity, Entity collision, int triggered);
    void collisionMiss(Entity entity);
}

// object (namespace: see structure.h): extension methods for object namespace
namespace object
{
    Vector3 brightness(int32_t value);
}

#endif