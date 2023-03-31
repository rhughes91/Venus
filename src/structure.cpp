#include "glad/glad.h"

#include "component.h"
#include "input.h"
#include "machine.h"
#include "setup.h"
#include "structure.h"
#include "ui.h"

ObjectManager g_manager;

Camera::Camera(Color color__, Vector3 front__, Vector3 up__, float near__, float far__) : backgroundColor(color__), front(front__), up(up__), nearDistance(near__), farDistance(far__)
{
    projection = mat4::per(math::radians(45.0f), window::aspectRatioInv(), nearDistance, farDistance);
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

Text::Text(const Font& font__, const std::string& text__, const Color& color__, float scale__, const Alignment& alignment__, const Rect& bounds__, const Shader& shader__) : font(font__), text(text__), color(color__), scale(scale__), alignment(alignment__), bounds(bounds__), shader(shader__)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}
void Text::refresh()
{
    relativeOrigin = Vector2(alignment.horizontal-1, alignment.vertical-1);
    points.clear();

    switch(alignment.horizontal)
    {
        case alignment::LEFT:
            leftAligned();
        break;
        case alignment::CENTER:
            centerAligned();
        break;
        case alignment::RIGHT:
            rightAligned();
        break;
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Vector2), &points[0], GL_STATIC_DRAW);
    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector2), (void*)0);
}
void Text::leftAligned()
{
    float spaceWidth = font.characters[' '].scale.x * 0.025f;

    struct LetterInfo
    {
        float width = 0;
        int pointIndex = 0, lineIndex;

        LetterInfo(float width__, int pointIndex__, int lineIndex__) : width(width__), pointIndex(pointIndex__), lineIndex(lineIndex__) {}
    };
    std::vector<LetterInfo> info;
    int lastLine = 0, lastIndex;

    float offsetY = 0;
    std::vector<std::pair<int, float>> offsetX = {{0, 0}};
    int32_t offsetXIndex = 0;
    for(int l = 0; l<text.size(); l++)
    {
        if(text[l] == '\b')
        {
            LetterInfo last = info[info.size()-1];
            lastIndex = last.pointIndex;
            for(int b = points.size()-1; b>=last.pointIndex; b--)
            {
                points.pop_back();
            }

            if(last.lineIndex != offsetXIndex)
            {
                offsetX.pop_back();
                lastLine = offsetXIndex = last.lineIndex;
                offsetY += font.maxScale.y * 0.025f;
            }
            else
            {
                offsetX[offsetXIndex].second -= last.width;
            }
            info.pop_back();
            continue;
        }

        lastLine = offsetXIndex;
        lastIndex = points.size();

        switch(text[l])
        {
            case ' ':
            {
                CharacterTTF under = font.characters[' '];
                offsetX[offsetXIndex].second += (under.scale.x + under.rsb) * 0.025f;
            }
            continue;
            case '\n':              
                offsetY -= font.maxScale.y * 0.025f;
                offsetX.push_back({0, 0});
                offsetXIndex++;
                info.push_back({0, lastIndex, lastLine});
            continue;
            case '\r':
                if(l != text.size()-1 && text[l+1] == '\n')
                    l++;
                offsetY -= font.maxScale.y * 0.025f;
                offsetX.push_back({0, 0});
                offsetXIndex++;
                info.push_back({0, lastIndex, lastLine});
            continue;
            case '\t':
                float mod = math::modf(offsetX[offsetXIndex].second, spaceWidth * 4);
                offsetX[offsetXIndex].second += -mod + spaceWidth * 4;
                info.push_back({-mod + spaceWidth * 4, lastIndex, lastLine});
            continue;
        }

        CharacterTTF letter = font.characters[text[l]];
        int size;
        if(size = letter.points.size())
        {
            if((offsetX[offsetXIndex].second + (letter.scale.x) * 0.025f) * scale > bounds.scale.x)
            {
                switch(newLineSetting)
                {
                    case text::LETTER:
                        offsetY -= font.maxScale.y * 0.025f;
                        offsetX.push_back({0, 0});
                        offsetXIndex++;
                    break;
                    case text::WORD:
                        offsetY -= font.maxScale.y * 0.025f;
                        offsetX.push_back({0, 0});
                        offsetXIndex++;
                    break;
                }
            }

            int contourIndex = 0, endConnection = 0;
            float partition = 0.01f;
            points.push_back(letter.points[0].position + Vector2(offsetX[offsetXIndex].second, offsetY));
            for(int i=0; i<size; i++)
            {
                if(i != letter.contourEnds[contourIndex]+1)
                {
                    points.push_back(letter.points[i].position + Vector2(offsetX[offsetXIndex].second, offsetY));
                }
                else
                {
                    points.push_back(letter.points[endConnection].position + Vector2(offsetX[offsetXIndex].second, offsetY));
                    contourIndex = (contourIndex = letter.contourEnds.size()-1 ? contourIndex+1 : 0);
                    endConnection = i;
                }
                points.push_back(letter.points[i].position + Vector2(offsetX[offsetXIndex].second, offsetY));
            }
            float width = (letter.scale.x + letter.rsb) * 0.025f;

            points.push_back(letter.points[endConnection].position + Vector2(offsetX[offsetXIndex].second, offsetY));
            offsetX[offsetXIndex].second += width;
            info.push_back({width, lastIndex, lastLine});
        }
    }

    height = (offsetXIndex+1) * font.maxScale.y;
}
void Text::centerAligned()
{
    leftAligned();
}
void Text::rightAligned()
{
    leftAligned();
}

ObjectManager::ObjectManager()
{
    if(g_manager.m_entities.size() == 0)
    {
        m_entityManager = std::make_unique<EntityManager>();
        m_componentManager = std::make_unique<ComponentManager>();
        m_systemManager = std::make_unique<SystemManager>();

        registerComponent<Model>();
        registerComponent<Object>();
        registerComponent<Transform>();

        Object global("g_global_event_runner");

        auto& animations = object::initializeScript<AnimationManager, Animator>(15);
        animations.addRequirement<Model>();
        animations.fixedUpdate([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                object::getComponent<Model>(entity).texture = object::getComponent<Animator>(entity).transition().transition().step();
            }
        });

        auto& billboards = object::initializeScript<BillboardManager, Billboard>(18);
        billboards.addRequirement<Transform>();
        billboards.lateUpdate([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                Transform& transform = object::getComponent<Transform>(entity);
                transform.rotation = Quaternion(mat4::lookAt(transform.position, (object::getComponent<Transform>(object::getComponent<Billboard>(entity).target).position - transform.position).normalized(), vec3::up)).inverted();
            }
        });

        auto& buttons = object::initializeScript<ButtonManager, Button>(33);
        buttons.addRequirement<Rect>();
        buttons.update([]
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

        auto& cameras = object::initializeScript<CameraManager, Camera>(21);
        cameras.addRequirement<Transform>();
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
        cameras.load(cameraFunction);
        cameras.lateUpdate(cameraFunction);

        auto& collisions = object::initializeScript<CollisionManager, BoxCollider>(24);
        collisions.addRequirement<Transform>();
        collisions.addRequirement<Model>();
        collisions.load([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                BoxCollider& collider = object::getComponent<BoxCollider>(entity);
                collider.storedPosition = object::getComponent<Transform>(entity).position + collider.offset;
            }
        });
        collisions.update([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                BoxCollider& collider = object::getComponent<BoxCollider>(entity);
                Transform& transform = object::getComponent<Transform>(entity);
                bool triggered = false;
                if(object::getComponent<Object>(entity).mobile)
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
        collisions.render([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                BoxCollider& collider = object::getComponent<BoxCollider>(entity);
                if(collider.wireframeEnabled())
                {
                    Transform& transform = object::getComponent<Transform>(entity);
                    collider.frame(transform);
                }
            }
        });

        auto& graphics = object::initializeScript<GraphicsManager, Transform>(9);
        graphics.addRequirement<Model>();
        graphics.insert([]
        (Entity entity, std::vector<Entity>& entities)
        {
            if(entity && std::find(entities.begin(), entities.end(), entity) == entities.end())
            {
                entities.push_back(entity);
                std::sort(entities.begin(), entities.end(), []
                (Entity one, Entity two)
                {
                    return object::getComponent<Model>(one).color.a < object::getComponent<Model>(two).color.a;
                });
            }
        });
        graphics.start([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                object::getComponent<Model>(entity).data.refresh();
            }
        });
        graphics.render([]
        (System &system)
        {
            Camera& camera = object::getComponent<Camera>(window::camera());
            Transform& cameraTransform = object::getComponent<Transform>(window::camera());
            
            for(auto const &entity : system.m_entities)
            {
                object::getComponent<Model>(entity).render(entity, camera, cameraTransform);
            }
        });

        auto& meshes = object::initializeScript<MeshManager, MeshAddon>(12);
        meshes.addRequirement<Model>();
        meshes.start([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                object::getComponent<Model>(entity).data.append(object::getComponent<Transform>(entity), object::getComponent<MeshAddon>(entity).additions);
            }
        });

        auto& physics = object::initializeScript<PhysicsManager, Physics2D>(30);
        physics.addRequirement<Transform>();
        physics.start([]
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
        physics.update([]
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

        auto& pointlights = object::initializeScript<PointLightManager, PointLight>(27);
        pointlights.addRequirement<Transform>();
        pointlights.update([]
        (System &system)
        {
            Shader& shader = shader::get("object_shader");
            shader.use();
            shader.setInt("totalPointLights", system.m_entities.size());

            uint32_t iterator = 0;
            for(const auto& entity : system.m_entities)
            {
                Transform& transform = object::getComponent<Transform>(entity);
                PointLight& light = object::getComponent<PointLight>(entity);

                std::string name = "pointLights[" + std::to_string(iterator) + "]";
                shader.setVec3(name + ".position", transform.position);
                shader.setVec4(name + ".color", light.color);
                shader.setFloat(name + ".strength", light.strength);
                
                shader.setFloat(name + ".constant", light.constant);
                shader.setFloat(name + ".linear", light.linear);
                shader.setFloat(name + ".quadratic", light.quadratic);
                iterator++;
            }
        });

        auto& spotlights = object::initializeScript<SpotLightManager, SpotLight>(9);
        spotlights.addRequirement<Transform>();
        spotlights.update([]
        (System &system)
        {
            Shader& shader = shader::get("object_shader");
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

        auto& uis = object::initializeScript<UIManager, Rect>(6);
        uis.addRequirement<Model>();
        uis.start([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                object::getComponent<Model>(entity).data.refresh();
            }
        });
        uis.render([]
        (System &system)
        {
            Camera& camera = object::getComponent<Camera>(window::camera());
            Transform& cameraTransform = object::getComponent<Transform>(window::camera());

            glDisable(GL_DEPTH_TEST);
            for(auto const &entity :system.m_entities)
            {
                object::getComponent<Model>(entity).render(entity, camera, cameraTransform);
            }
            glEnable(GL_DEPTH_TEST);
        });

        auto& texts = object::initializeScript<TextManager, Text>(3);
        texts.addRequirement<Rect>();
        texts.start([]
        (System &system)
        {
            for(auto const &entity : system.m_entities)
            {
                object::getComponent<Text>(entity).refresh();
            }
        });
        texts.render([]
        (System &system)
        {
            glDisable(GL_DEPTH_TEST);
            
            for(auto const &entity : system.m_entities)
            {
                Rect& transform = object::getComponent<Rect>(entity);
                Text& text = object::getComponent<Text>(entity);

                Vector2 offset;
                Alignment alignment = text.getAlignment();
                switch(alignment.vertical)
                {
                    case alignment::TOP:
                        offset.y += -text.getCeiling();
                    break;
                    case alignment::MIDDLE:
                        offset.y += -text.getCeiling()/2 - text.getHeight()/2;
                    break;
                    case alignment::BOTTOM: break;
                }
                switch(alignment.horizontal)
                {
                    case alignment::LEFT:
                        offset += -transform.scale * Vector2(window::aspectRatio(), -1) * 0.5f;
                    break;
                    case alignment::CENTER: break;
                    case alignment::RIGHT:
                        offset += transform.scale * Vector2(window::aspectRatio(), 1) * 0.5f;
                    break;
                }

                text.render(transform.relativePosition() + offset);
            }

            glEnable(GL_DEPTH_TEST);
        });
    }
}