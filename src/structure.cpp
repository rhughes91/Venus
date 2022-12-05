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

Camera::Camera(float speed__, Color color__, Vector3 front__, Vector3 up__) : speed(speed__), backgroundColor(color__), front(front__), up(up__)
{
    projection = mat4::inter(math::radians(45.0f), 2.5f, window::aspectRatioInv(), 0.01f, 200.0f, 1.0f);
}

bool Rect::contains(const Vector2& vec)
{
    Vector2 pos = relativePosition();
    Vector2 target = vec - Vector2((window::width() - window::height())/window::height() * pos.x, 0);

    return pos.x - scale.x/2 <= target.x && pos.x + scale.x/2 >= target.x && pos.y + scale.x/2 >= target.y && pos.y - scale.y/2 <= target.y;
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

void physics::collisionHandler(Entity entity, Entity collision, int triggered)
{
    Physics2D& physics = object::getComponent<Physics2D>(entity);
    if(!triggered)
    {
        physics.resetCollisions();
    }

    Transform& transform = object::getComponent<Transform>(entity);
    Transform& transform2 = object::getComponent<Transform>(collision);

    Model& model = object::getComponent<Model>(entity);
    Model& model2 = object::getComponent<Model>(collision);

    BoxCollider& collider = object::getComponent<BoxCollider>(entity);
    BoxCollider& collider2 = object::getComponent<BoxCollider>(collision);
    
    Vector3 sign = vec3::sign(transform.position - transform2.position);
    Vector3 depth = (transform.position-model.data.dimensions*transform.scale*collider.scale*0.5f*sign)-(transform2.position+model2.data.dimensions*transform2.scale*collider2.scale*0.5f*sign);
    Vector3 depthSign = vec3::sign0(depth);

    if(math::abs(depth.x) <= math::abs(depth.y))
    {
        transform.position -= Vector3(depth.x, 0, 0);
        if(math::sign0(math::roundTo(transform.position.x - collider.storedPosition.x, 6)) != -depthSign.x)
        {
            physics.velocity.x = 0;
        }

        if(depthSign.x <= 0)
        {
            physics.collisions[physics::LEFT] = true;
        }
        else
        {
            physics.collisions[physics::RIGHT] = true;
        }
    }
    else
    {
        transform.position -= Vector3(0, depth.y, 0);
        if(math::sign0(math::roundTo(transform.position.y - collider.storedPosition.y, 6)) != -depthSign.y)
        {
            physics.velocity.y = 0;
        }

        if(depthSign.y <= 0)
        {
            physics.collisions[physics::DOWN] = true;
        }
        else
        {
            physics.collisions[physics::UP] = true;
        }
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

        Object global("g_global_event_runner");

        registerComponent<Camera>();
        registerComponent<Transform>();
        registerComponent<Animator>();
        registerComponent<Model>();
        registerComponent<SpotLight>();
        registerComponent<Rect>();
        registerComponent<Button>();
        registerComponent<Physics2D>();
        registerComponent<BoxCollider>();
        registerComponent<Billboard>();

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
                if(mouse::pressed(mouse::LEFT) && rect.contains(window::cursorScreenPosition()))
                {
                    object::getComponent<Button>(entity).trigger(entity);
                }
            }
        });

        system = registerSystem<CameraManager>();
        system -> setLateUpdate([]
        (System &system)
        {
            for(const auto& entity : system.m_entities)
            {
                Camera& camera = object::getComponent<Camera>(entity);
                camera.view = mat4::lookAt(object::getComponent<Transform>(entity).position, -camera.front, vec3::up);

                if(window::resolutionUpdated())
                {
                    camera.projection = mat4::inter(math::radians(45.0f), 2.5f, window::aspectRatioInv(), 0.01f, 200.0f, camera.projectionLevel);
                }
            }
        });

        system = registerSystem<CollisionManager>();
        system -> setLateUpdate([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                BoxCollider& collider = object::getComponent<BoxCollider>(entity);
                if(collider.mobile)
                {
                    Transform& transform = object::getComponent<Transform>(entity);
                    Model& model = object::getComponent<Model>(entity);
                    Vector3 boxDim = model.data.dimensions * transform.scale * collider.scale * 0.5f;
                    Vector3 position = transform.position;
                    bool triggered = false;

                    for(auto const &compare : system.m_entities)
                    {
                        if(compare == entity)
                            continue;

                        BoxCollider& collider2 = object::getComponent<BoxCollider>(compare);
                        Transform& transform2 = object::getComponent<Transform>(compare);
                        Model& collisionModel = object::getComponent<Model>(compare);
                        Vector3 boxDim2 = collisionModel.data.dimensions * transform2.scale * collider2.scale * 0.5f;
                        Vector3 position2 = transform2.position, hit;
                        
                        // Box box = Box(position2-boxDim2, position2+boxDim2);
                        int precision = 6;
                        if
                        (
                            (math::roundTo(position.x + boxDim.x, precision) >= math::roundTo(position2.x - boxDim2.x, precision) && math::roundTo(position.x - boxDim.x, precision) <= math::roundTo(position2.x + boxDim2.x, precision) &&
                            math::roundTo(position.y + boxDim.y, precision) >= math::roundTo(position2.y - boxDim2.y, precision) && math::roundTo(position.y - boxDim.y, precision) <= math::roundTo(position2.y + boxDim2.y, precision) &&
                            math::roundTo(position.z + boxDim.z, precision) >= math::roundTo(position2.z - boxDim2.z, precision) && math::roundTo(position.z - boxDim.z, precision) <= math::roundTo(position2.z + boxDim2.z, precision))
                        )
                        {
                            collider.trigger(entity, compare, triggered);
                            collider2.trigger(compare, entity, triggered);
                            triggered = true;
                        }
                        // else if(box.CheckLineBox(collider.storedPosition, position, hit))
                        // {
                        //     std::cout << collider.storedPosition << " : " << position << std::endl;
                        // }
                    }
                    if(!triggered)
                    {
                        collider.miss(entity);
                    }
                }
            }
        });
        system -> setRender([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                object::getComponent<BoxCollider>(entity).storedPosition = object::getComponent<Transform>(entity).position;
            }
        });

        system = registerSystem<Graphics>();
        system -> setRender([]
        (System &system)
        {
            Camera& camera = object::getComponent<Camera>(window::camera());
            Transform& cameraTransform = object::getComponent<Transform>(window::camera());
            
            window::clearScreen(camera.backgroundColor);
            for(auto const &entity : system.m_entities)
            {
                Transform& transform = object::getComponent<Transform>(entity);
                Model& model = object::getComponent<Model>(entity);
                model.material.run(transform, model, camera, cameraTransform);
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
                    
                    if(physics.colliding() && math::sign0(physics.force.x) != math::sign0(physics.velocity.x))
                    {
                        physics.velocity.x = physics.velocity.x / (1 + physics.drag.x * time);
                    }

                    if(physics.terminal != 0)
                    {
                        physics.velocity.x = std::min(math::abs(physics.velocity.x + acceleration.x * time), (float)(physics.terminal.x)) * math::sign(physics.velocity.x + acceleration.x * time);
                        physics.velocity.y = std::min(math::abs(physics.velocity.y + acceleration.y * time), (float)(physics.terminal.y)) * math::sign(physics.velocity.y + acceleration.y * time);
                    }
                    else
                        physics.velocity += acceleration * time;
                    
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
        

        system = registerSystem<UIManager>();
        system -> setRender([]
        (System &system)
        {
            glDisable(GL_DEPTH_TEST);
            Vector2 aspectRatio = Vector2(window::aspectRatio(), 1);
            for(auto const &entity :system.m_entities)
            {
                Rect& transform = object::getComponent<Rect>(entity);
                Model& model = object::getComponent<Model>(entity);

                Shader shader = model.material.shader;
                shader.use();
                shader.setVec2("position", transform.relativePosition());
                shader.setVec2("scale", transform.scale * aspectRatio);
                shader.setMat4("model", (mat4x4(1) * (mat4x4)transform.rotation).matrix, true);

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