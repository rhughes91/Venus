#include "glad/glad.h"

#include "component.h"
#include "input.h"
#include "machine.h"
#include "setup.h"
#include "structure.h"
#include "ui.h"

ObjectManager g_manager;

void object::load()
{
    g_manager.load();
}
void object::start()
{
    g_manager.start();
}
void object::destroy()
{
    g_manager.destroy();
}
void object::update()
{
    g_manager.update();
}
void object::lateUpdate()
{
    g_manager.lateUpdate();
}
void object::fixedUpdate()
{
    g_manager.fixedUpdate();
}

Camera::Camera(Color color__, Vector3 front__, Vector3 up__) : backgroundColor(color__), front(front__), up(up__)
{
    projection = mat4::inter(math::radians(45.0f), 2.5f, window::aspectRatioInv(), 0.01f, 200.0f, 1.0f);
}

bool Rect::contains(const Vector2& vec)
{
    Vector2 pos = relativePosition();
    Vector2 target = vec - Vector2((window::width() - window::height())/window::height() * pos.x, 0);
    float angle = rotation.euler().x*2 - M_PI;

    Vector2 ul = pos - scale * 0.5f, br = pos + scale * 0.5f;
    return math::quadPointIntersect(Quad(vec2::rotatedAround(ul, pos, angle), vec2::rotatedAround(Vector2(br.x, ul.y), pos, angle), vec2::rotatedAround(br, pos, angle), vec2::rotatedAround(Vector2(ul.x, br.y), pos, angle)), target);
}
Vector2 Rect::relativePosition()
{
    return (position + relativeOrigin - scale * relativeOrigin * Vector2(window::aspectRatio(), 1) * 0.5f);
}

Object object::find(std::string name)
{
    return Object(name, g_manager.getEntity(name));
}
Vector3 object::brightness(int32_t value)
{
    switch(value)
    {
        case 1: return Vector3(1.0f, 0.7f, 1.8f);
        case 2: return Vector3(1.0f, 0.35f, 0.44f);
        case 3: return Vector3(1.0f, 0.22f, 0.20f);
        case 4: return Vector3(1.0f, 0.14f, 0.07f);
        case 5: return Vector3(1.0f, 0.09f, 0.032f);
        case 6: return Vector3(1.0f, 0.07f, 0.017f);
        case 7: return Vector3(1.0f, 0.045f, 0.0075f);
        case 8: return Vector3(1.0f, 0.027f, 0.0028f);
        case 9: return Vector3(1.0f, 0.022f, .0019f);
        case 10: return Vector3(1.0f, 0.014f, 0.0007f);
        case 11: return Vector3(1.0f, 0.0014f, 0.000007f);
    };
    return 0;
}

// cannot interpret MUTE, DECR_VOLUME, INCR_VOLUME, CALCULATOR, GLOBAL_2, GLOBAL_3, and GLOBAL_4
void InputManager::initialize(int end)
{
    for(uint32_t i = 0; i <= end; i++)
    {
        inputs.insert({i, Key{i}});
    }
}
void InputManager::initialize(int start, int end)
{
    for(uint32_t i = start; i <= end; i++)
    {
        inputs.insert({i, Key{i}});
    }
}
void InputManager::parse(int32_t input, bool pressed)
{
    Key& key = inputs[input];
    if(pressed && !key.held)
    {
        key.pressed = key.held = true;
        key.released = false;

        heldKeys.insert(key);
    }
    else if(!pressed && key.held)
    {
        key.released = true;
        key.held = false;
    }
}
void InputManager::refresh()
{
    for(Key key : heldKeys)
    {
        inputs[key.data].pressed = inputs[key.data].released = false;
        if(!key.held)
        {
            heldKeys.erase(key);
        }
    }
}

void Timer::update(float max)
{
    timer += math::clamp(event::delta(), 0.f, max);
}

uint32_t Animation::step()
{
    if(frameCount != frameRate-1)
    {
        frameCount++;
    }
    else
    {
        frameCount = 0;
        if(frames.size() == 0)
            return texture::get("default.png");
        frame = (frame + 1) % frames.size();
    }
    return frames[frame];
}
void anim::reset(Animation& animation)
{
    animation.frame = 0;
}
void anim::keep(Animation& current, Animation& last)
{
    current.frame = (last.frame < current.frames.size() ? last.frame:0);
}

std::string Physics2D::collisionsToString() const
{
    return "[UP : " + std::to_string(collisions[physics::UP]) + ", DOWN : " + std::to_string(collisions[physics::DOWN]) + ", LEFT : " + std::to_string(collisions[physics::LEFT]) + ", RIGHT : " + std::to_string(collisions[physics::RIGHT]);
}
void physics::collisionTrigger(Entity entity, Entity collision, bool edge, int triggered)
{    
    Physics2D& physics = object::getComponent<Physics2D>(entity);
    if(!triggered)
        physics.resetCollisions();

    if(edge)
        return;

    Transform& transform = object::getComponent<Transform>(entity);
    BoxCollider& collider = object::getComponent<BoxCollider>(entity);
    Model& model = object::getComponent<Model>(entity);

    Vector3 boxDim = model.data.dimensions * transform.scale * collider.scale * 0.5f;

    BoxCollider& collider2 = object::getComponent<BoxCollider>(collision);
    Transform& transform2 = object::getComponent<Transform>(collision);
    Model& model2 = object::getComponent<Model>(collision);

    Vector3 position2 = transform2.position;
    Vector3 boxDim2 = model2.data.dimensions * transform2.scale * collider2.scale * 0.5f;
    Vector3 delta = (transform.position + collider.offset - collider.storedPosition) - 2*(position2 - collider2.storedPosition);

    float precision = 6;

    bool above = math::roundTo(collider.storedPosition.y - boxDim.y, precision) >= math::roundTo(collider2.storedPosition.y + boxDim2.y, precision), below = math::roundTo(collider.storedPosition.y + boxDim.y, precision) <= math::roundTo(collider2.storedPosition.y - boxDim2.y, precision);
    bool right = math::roundTo(collider.storedPosition.x - boxDim.x, precision) >= math::roundTo(collider2.storedPosition.x + boxDim2.x, precision), left = math::roundTo(collider.storedPosition.x + boxDim.x, precision) <= math::roundTo(collider2.storedPosition.x - boxDim2.x, precision);
    
    bool vertical = above || below;
    bool horizontal = right || left;

    if(!horizontal && delta.y < 0)
    {
        physics.velocity.y = 0;
        physics.collisions[physics::DOWN] = true;
        transform.position.y = position2.y - collider.offset.y + (boxDim2.y + boxDim.y);
    }
    else if(!horizontal && delta.y > 0)
    {
        physics.velocity.y = 0;
        physics.collisions[physics::UP] = true;
        transform.position.y = position2.y - collider.offset.y - (boxDim2.y + boxDim.y);
    }
    else if(!vertical && delta.x < 0)
    {
        physics.velocity.x = 0;
        physics.collisions[physics::LEFT] = true;
        transform.position.x = position2.x - collider.offset.x + (boxDim2.x + boxDim.x);
    }
    else if(!vertical && delta.x > 0)
    {
        physics.velocity.x = 0;
        physics.collisions[physics::RIGHT] = true;
        transform.position.x = position2.x - collider.offset.x - (boxDim2.x + boxDim.x);
    }
}
void physics::collisionMiss(Entity entity)
{
    object::getComponent<Physics2D>(entity).resetCollisions();
}

ObjectManager::ObjectManager()
{
    if(g_manager.m_entities.size() == 0)
    {
        m_entityManager = std::make_unique<EntityManager>();
        m_componentManager = std::make_unique<ComponentManager>();
        m_systemManager = std::make_unique<SystemManager>();

        registerComponent<Animator>();
        registerComponent<Billboard>();
        registerComponent<BoxCollider>();
        registerComponent<Button>();
        registerComponent<Camera>();
        registerComponent<Model>();
        registerComponent<Object>();
        registerComponent<Physics2D>();
        registerComponent<Rect>();
        registerComponent<SpotLight>();
        registerComponent<Transform>();

        Object global("g_global_event_runner");

        std::shared_ptr<System> system;
        
        system = registerSystem<AnimationManager>();
        system -> setFixedUpdate([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                object::getComponent<Model>(entity).texture = object::getComponent<Animator>(entity).transition().transition().step();
            }
        });

        system = registerSystem<BillboardHandler>();
        system -> setLateUpdate([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                Transform &transform = object::getComponent<Transform>(entity);
                transform.rotation = Quaternion(object::getComponent<Camera>(object::getComponent<Billboard>(entity).target).view).normalized().inverted();
            }
        });

        system = registerSystem<ButtonManager>();
        system -> setUpdate([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                Rect& rect = object::getComponent<Rect>(entity);
                Button& button = object::getComponent<Button>(entity);
                if(mouse::pressed(mouse::LEFT) && rect.contains(window::cursorScreenPosition()))
                {
                    button.trigger(entity);
                }
                else
                {
                    for(auto key : button.keyInputs)
                    {
                        if(key::pressed((key::KeyCode)key))
                        {
                            button.trigger(entity);
                        }
                    }

                    for(auto click : button.buttonInputs)
                    {
                        if(mouse::pressed((mouse::ButtonCode)click))
                        {
                            button.trigger(entity);
                        }
                    }
                }
            }
        });

        system = registerSystem<CameraManager>(10);
        void (*cameraFunction)(System &) = []
        (System &system)
        {
            for(const auto& entity : system.m_entities)
            {
                Camera& camera = object::getComponent<Camera>(entity);
                camera.view = mat4::lookAt(object::getComponent<Transform>(entity).position, -camera.front, vec3::up);

                if(window::resolutionUpdated())
                {
                    camera.projection = mat4::inter(math::radians(45.0f), 2.5f, window::aspectRatioInv(), 0.01f, 200.0f, 1);
                }
            }
        };
        system -> setLoad(cameraFunction);
        system -> setLateUpdate(cameraFunction);

        system = registerSystem<CollisionManager>(1);
        system -> setLoad([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                BoxCollider& collider = object::getComponent<BoxCollider>(entity);
                collider.storedPosition = object::getComponent<Transform>(entity).position + collider.offset;
            }
        });
        system -> setUpdate([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                BoxCollider& collider = object::getComponent<BoxCollider>(entity);
                Transform& transform = object::getComponent<Transform>(entity);
                bool triggered = false;
                if(collider.mobile)
                {
                    Model& model = object::getComponent<Model>(entity);

                    Vector3 position = transform.position + collider.offset;
                    Vector3 boxDim = (model.data.dimensions * transform.scale * collider.scale + vec3::abs(position - collider.storedPosition)) * 0.5f;

                    for(auto const &compare : system.m_entities)
                    {
                        if(compare == entity)
                            continue;

                        BoxCollider& collider2 = object::getComponent<BoxCollider>(compare);
                        Transform& transform2 = object::getComponent<Transform>(compare);
                        Model& model2 = object::getComponent<Model>(compare);

                        Vector3 position2 = transform2.position + collider2.offset;
                        Vector3 boxDim2 = model2.data.dimensions * transform2.scale * collider2.scale * 0.5f;

                        int precision = 6;
                        Vector3 positive = vec3::roundTo(position + boxDim, precision), negative = vec3::roundTo(position - boxDim, precision);
                        Vector3 positive2 = vec3::roundTo(position2 + boxDim2, precision), negative2 = vec3::roundTo(position2 - boxDim2, precision);
                        if(positive.x >= negative2.x && negative.x <= positive2.x && positive.y >= negative2.y && negative.y <= positive2.y && positive.z >= negative2.z && negative.z <= positive2.z)
                        {                         
                            bool edge = (positive.x == negative2.x || negative.x == positive2.x || positive.y == negative2.y || negative.y == positive2.y);
                            collider.trigger(entity, compare, edge, triggered);
                            collider2.trigger(compare, entity, edge, triggered);
                            triggered = true;
                        }
                    }
                    if(!triggered)
                    {
                        collider.miss(entity);
                    }
                }
                collider.storedPosition = transform.position + collider.offset;
            }
        });

        system = registerSystem<Graphics>();
        system -> setRender([]
        (System &system)
        {
            Camera& camera = object::getComponent<Camera>(window::camera());
            Transform& cameraTransform = object::getComponent<Transform>(window::camera());
            
            for(auto const &entity : system.m_entities)
            {
                Model& model = object::getComponent<Model>(entity);
                model.material.run(object::getComponent<Transform>(entity), model, camera, cameraTransform);
                model.draw();
            }
        });

        system = registerSystem<PhysicsManager>();
        system -> setStart([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                Physics2D& physics = object::getComponent<Physics2D>(entity);
                physics.delta = physics.lastDelta = object::getComponent<Transform>(entity).position;
                physics.time.begin();
                physics.resetCollisions();
            }
        });
        system -> setUpdate([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                Vector3& position = object::getComponent<Transform>(entity).position;
                Physics2D& physics = object::getComponent<Physics2D>(entity);

                physics.time.update(physics.maxDeltaTime);
                float time = physics.time.interval;

                while(physics.time.timer > physics.time.interval)
                {
                    Vector2 acceleration = physics.acceleration();
                    Vector3 deltaStep = vec3::roundTo(physics.lastDelta - physics.delta, 4);
                    if((physics.collisions[physics::LEFT] && deltaStep.x >= 0) || (physics.collisions[physics::RIGHT] && deltaStep.x <= 0))
                    {
                        physics.delta.x = position.x;
                    }
                    if((physics.collisions[physics::DOWN] && deltaStep.y >= 0) || (physics.collisions[physics::UP] && deltaStep.y <= 0))
                    {
                        physics.delta.y = position.y;
                    }
                    
                    physics.lastDelta = physics.delta;

                    if((physics.collisions[physics::UP] || physics.collisions[physics::DOWN]) && math::sign0(physics.force.x) != math::sign0(physics.velocity.x))
                    {
                        physics.velocity.x = physics.velocity.x / (1 + physics.drag.x * time);
                    }
                    // else if((physics.collisions[physics::LEFT] || physics.collisions[physics::RIGHT]) && math::sign0(physics.force.y) != math::sign0(physics.velocity.y))
                    // {
                    //     std::cout << "bskjn" << std::endl;
                    //     physics.velocity.y = physics.velocity.y / (1 + physics.drag.y * time);
                    // }

                    if(physics.terminal != 0)
                    {
                        physics.velocity = vec2::min(vec2::abs(physics.velocity + acceleration * time), physics.terminal) * vec2::sign(physics.velocity + acceleration * time);
                    }
                    else
                    {
                        physics.velocity += acceleration * time;
                    }
                    
                    physics.velocity += physics.impulse;

                    physics.delta += Vector3(physics.velocity * time + acceleration * 0.5f * time * time, 0);
                    physics.time.reset();
                    physics.resetImpulse();
                }

                position = vec3::lerp(physics.lastDelta, physics.delta, physics.time.timer / physics.time.interval);
                physics.resetForce();
            }
        });

        system = registerSystem<SpotLightManager>();
        system -> setUpdate([]
        (System &system)
        {
            Shader& shader = shader::get("obj_shader");
            shader.use();
            shader.setInt("totalSpotLights", system.m_entities.size());

            uint32_t iterator = 0;
            for(const auto& entity : system.m_entities)
            {
                Transform& transform = object::getComponent<Transform>(entity);
                SpotLight& light = object::getComponent<SpotLight>(entity);

                std::string name = "spotLights[" + std::to_string(iterator) + "]";
                shader.setVec3(name + ".position", transform.position);
                shader.setVec3(name + ".direction", light.direction);
                shader.setVec4(name + ".color", light.color);
                shader.setFloat(name + ".strength", light.strength);
                
                shader.setFloat(name + ".constant", light.constant);
                shader.setFloat(name + ".linear", light.linear);
                shader.setFloat(name + ".quadratic", light.quadratic);

                shader.setFloat(name + ".cutOff", light.cutoff);
                shader.setFloat(name + ".outerCutOff", light.outerCutOff);
                iterator++;
            }
        });

        system = registerSystem<UIManager>(1);
        system -> setRender([]
        (System &system)
        {
            glDisable(GL_DEPTH_TEST);

            for(auto const &entity :system.m_entities)
            {
                Rect& transform = object::getComponent<Rect>(entity);
                Model& model = object::getComponent<Model>(entity);

                Shader& shader = model.material.shader;
                shader.use();
                
                shader.setFloat("aspect", window::aspectRatio());
                shader.setVec2("position", transform.relativePosition());
                shader.setVec2("scale", transform.scale);
                shader.setMat4("model", (mat4x4(1) * (mat4x4)transform.rotation).matrix, false);
                shader.setVec4("objColor", model.color);

                model.draw();
            }
            glEnable(GL_DEPTH_TEST);
        });

        Signature signature;
        signature.set(getComponentType<Transform>());
        signature.set(getComponentType<SpotLight>());
        setSystemSignature<SpotLightManager>(signature);

        signature.reset();
        signature.set(getComponentType<Model>());
        signature.set(getComponentType<Animator>());
        setSystemSignature<AnimationManager>(signature);

        signature.reset();
        signature.set(getComponentType<Rect>());
        signature.set(getComponentType<Button>());
        setSystemSignature<ButtonManager>(signature);

        signature.reset();
        signature.set(getComponentType<Transform>());
        signature.set(getComponentType<Model>());
        setSystemSignature<Graphics>(signature);

        signature.reset();
        signature.set(getComponentType<Physics2D>());
        signature.set(getComponentType<Transform>());
        setSystemSignature<PhysicsManager>(signature);

        signature.reset();
        signature.set(getComponentType<Transform>());
        signature.set(getComponentType<Model>());
        signature.set(getComponentType<BoxCollider>());
        setSystemSignature<CollisionManager>(signature);

        signature.reset();
        signature.set(getComponentType<Rect>());
        signature.set(getComponentType<Model>());
        setSystemSignature<UIManager>(signature);

        signature.reset();
        signature.set(getComponentType<Camera>());
        signature.set(getComponentType<Transform>());
        setSystemSignature<CameraManager>(signature);

        signature.reset();
        signature.set(getComponentType<Billboard>());
        signature.set(getComponentType<Transform>());
        setSystemSignature<BillboardHandler>(signature);
    }
}