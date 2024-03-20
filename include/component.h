#ifndef COMPONENT_H
#define COMPONENT_H

#include <set>

#include "graphics.h"
#include "machine.h"
#include "vector.h"

// Key (struct): stores whether the stored key data (corresponding to GLFW key values) is being pressed, held, or released
struct Key
{
    uint32_t data;
    bool pressed, held, released;

    operator std::string()
    {
        return "Pressed: "+std::to_string(pressed) + ", Held: "+std::to_string(held) + ", Released: "+std::to_string(released);
    }
    bool operator <(const Key& key) const
    {
        return data < key.data;
    }
};

// InputManager (struct): stores keys and regulates whether they are being pressed, held, or released
struct InputManager
{
    uint8_t heldKey = 0;

    std::unordered_map<uint32_t, Key> inputs;
    std::set<Key> heldKeys;

    void initialize(int end);                // initialized key values from 0 to 'end'
    void initialize(int start, int end);     // initialized key values from 'start' to 'end'
    void parse(int32_t input, bool pressed); // determines whether the input is being pressed, held, or released
    void refresh();                          // clears pressed and released keys after one frame, and erases held keys if they are released
};

// Time (struct): holds all the timing data that happens between frames :: controls when "fixedUpdate" is run
struct Time
{
    bool frozen;
    int32_t advanceKey = '\\';

    double deltaTime = 0.0f;
    double lastDeltaTime = 0.0f;
    double lastFrame = 0.0f;
    double timer = 0;
    double runtime = 0;

    std::array<double, 10> framerates;

    Time();

    // updates the "fixedUpdate" timer, interpolates deltaTime, and tracks the average framerate
    void update();
    void beginTimer();
    void resetTimer(double interval)
    {
        timer = timer-interval;
    }

    private:
        int32_t framerateIndex = 0;
};

namespace event
{
    float delta();
    float framerate();
    float time();

    void freezeTime(bool freeze);
}


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

// Camera (struct): allows for the scene to be rendered from a certain perspective
struct Camera
{
    Color backgroundColor;
    Vector3 front, up;
    mat4x4 view, projection;
    float nearDistance, farDistance;

    Camera() {}
    Camera(Color color__, Vector3 front__, Vector3 up__, float near__ = 0.01f, float far__ = 200.f);
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
    Transform(const Vector3& position__, const Quaternion& rotation__) : Transform(position__, vec3::one, rotation__) {}
    Transform(const Vector3& position__, const Vector3& scale__, const Quaternion& rotation__) : position(position__), scale(scale__), rotation(rotation__) {}
};

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


struct Animation;

namespace anim
{
    void texture(const Animation& animation, void *render, uint32_t frame, size_t size);
    void uv(const Animation& animation, void *render, uint32_t frame, size_t size);
};

// Animation (struct): holds the data and logic needed to switch between images at a certain 'frameRate'
struct Animation
{
    std::vector<Texture> frames;
    int frame = 0;
    int frameRate = 1;
    bool loop = true;

    Animation() {}
    Animation(const std::vector<Texture>& frames__, void (*animationAction__)(const Animation&, void *, uint32_t, size_t) = anim::texture, bool loop__ = true)
    {
        frames = frames__;
        animationAction = animationAction__;
        loop = loop__;
    }

    Animation(const std::vector<Texture>& frames__, int32_t frameRate__, void (*animationAction__)(const Animation&, void *, uint32_t, size_t) = anim::texture, bool loop__ = true)
    {
        frames = frames__;
        frameRate = frameRate__;
        animationAction = animationAction__;
        loop = loop__;
    }

    void setAnimate(void (*animationAction__)(const Animation& animation, void *render, uint32_t frame, size_t size), bool loop__ = true)
    {
        animationAction = animationAction__;
        loop = loop__;
    }

    // loops 'frame' between 0 and the size of 'frames' and returns the image data at that index
    uint32_t step();
    void animate(void *render, uint32_t frame, size_t size)
    {
        animationAction(*this, render, frame, size);
    }

    private:
        int32_t frameCount = 0;
        void (*animationAction)(const Animation& animation, void *render, uint32_t frame, size_t size);
};

// anim (namespace): holds basic StateMachine transition functions for Animations
namespace anim
{
    // resets 'animation' to frame zero on update
    void reset(Animation& animation);

    // resets 'animation' to frame zero on update
    bool restart(Animation& current, Animation& last);

    // attempts to maintain frame index from 'last' animation on transition :: otherwise, frame index is set to zero
    bool keep(Animation& current, Animation& last);
};

// acts as layered StateMachine :: layer one is AnimationState, layer two is Animation
using AnimationState = StateMachine<Animation>;
using Animator = StateMachine<AnimationState>;

// PHYSICS COMPONENTS

// BoxCollider (struct): calls 'trigger' function whenever another BoxCollider intersects with this one :: otherwise, miss function is called
struct BoxCollider
{
    bool mobile = false;
    Vector3 scale, offset, storedPosition;

    bool enter, exit;

    BoxCollider(const Vector3& scale__ = 1, const Vector3& offset__ = 0) : scale(scale__), offset(offset__) {}
};

struct SquareCollider
{
    bool mobile = false;
    Vector3 scale, offset, storedPosition;

    bool enter, exit;

    SquareCollider(const Vector3& scale__ = 1, const Vector3& offset__ = 0) : scale(scale__), offset(offset__) {}
};

struct AABB2D
{

};

struct AABB
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
    void collisionTrigger(uint32_t entity, uint32_t collision, bool edge, int triggered);
    void collisionMiss(uint32_t entity);
}

namespace object
{
    Vector3 brightness(int32_t value);
}

#endif