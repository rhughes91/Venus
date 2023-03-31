#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <algorithm>
#include <array>
#include <bitset>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>

#include "vector.h"
#include "setup.h"
#include "file_util.h"
#include "container.h"

using Entity = uint32_t;
using Path = std::pair<std::string, std::string>;

class Object;


// GLOBAL STRUCTURES

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
    int32_t advanceKey;

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


using ComponentType = uint8_t;
const ComponentType MAX_COMPONENTS = 64;

using Signature = std::bitset<MAX_COMPONENTS>;

/**
 * Entity Component System: System for compiling various data structures into and integer value called an 'Entity'
 * Key Classes: EntityManager, ComponentManager, SystemManager
 * Credit: https://austinmorlan.com/posts/entity_component_system/
 */

// ENTITY MANAGEMENT
const uint16_t MAX_ENTITIES = 200;

// EntityManager (class): regulates how Entities are created and destroyed, and attaches "Signature" bitset that tracks attached components
class EntityManager
{
    public:
        EntityManager()
        {
            for(Entity entity = 0; entity<MAX_ENTITIES; entity++)
            {
                m_availableEntities.push(entity);
            }
        }

        Entity createEntity()
        {
            Entity id = m_availableEntities.front();
            m_availableEntities.pop();
            m_livingEntityCount++;
            return id;
        }

        Entity copyEntity(Entity entity)
        {
            Entity id = m_availableEntities.front();
            m_availableEntities.pop();
            m_livingEntityCount++;
            m_signatures[id] = m_signatures[entity];

            return id;
        }

        void setSignature(Entity entity, Signature signature)
        {
            m_signatures[entity] = signature;
        }

        Signature getSignature(Entity entity)
        {
            return m_signatures[entity];
        }

        void destroyEntity(Entity entity)
        {
            m_signatures[entity].reset();

            m_availableEntities.push(entity);
            --m_livingEntityCount;
        }

    private:
        std::queue<Entity> m_availableEntities{};
        std::array<Signature, MAX_ENTITIES> m_signatures{};

        uint32_t m_livingEntityCount{};
};


// COMPONENT MANAGEMENT

// IComponentArray (class): acts as the foundation for "ComponentArray" :: holds generic methods and fields
class IComponentArray
{
    public:
        virtual ~IComponentArray() = default;
        virtual void entityDestroyed(Entity entity) = 0;
        virtual bool copyComponent(Entity original, Entity copy) = 0;

        bool contains(Entity entity)
        {
            return m_entityToIndexMap.find(entity) != m_entityToIndexMap.end();
        }

        std::unordered_map<size_t, Entity> m_indexToEntityMap;
        std::unordered_map<Entity, size_t> m_entityToIndexMap;
        
};

// ComponentArray (class: extends IComponentArray): functionally acts as an array of components (outside structures registered as components)
template<typename T>
class ComponentArray : public IComponentArray
{
    public:
        // stores the 'entity' as an index to its respective 'component'
        void insertData(Entity entity, const T& component)
        {
            m_entityToIndexMap[entity] = m_count;
            m_indexToEntityMap[m_count] = entity;
            m_componentArray[m_count] = component;
            
            m_count++;
        }

        // removes the index data of 'entity' and the component that belongs to it
        void removeData(Entity entity)
        {
            size_t removeIndex = m_entityToIndexMap[entity];
            size_t lastIndex = m_count-1;
            m_componentArray[removeIndex] = m_componentArray[lastIndex];

            Entity lastEntity = m_indexToEntityMap[lastIndex];
            m_entityToIndexMap[lastEntity] = removeIndex;
            m_indexToEntityMap[removeIndex] = lastEntity;

            m_entityToIndexMap.erase(entity);
            m_indexToEntityMap.erase(lastIndex);

            m_count--;
        }

        // returns the component T attached to 'entity'
        T& getData(Entity entity)
        {
            return m_componentArray[m_entityToIndexMap[entity]];
        }

        // protocol for when an entity is removed from the ECS :: entity data is erased if it found
        void entityDestroyed(Entity entity) override
        {
            if(m_entityToIndexMap.find(entity) != m_entityToIndexMap.end())
            {
                removeData(entity);
            }
        }

        bool containsComponent(Entity entity)
        {
            return contains(entity);
        }

        // attaches component attached to 'original' to 'copy' :: returns true if component is successfully found and copied; returns false otherwise
        bool copyComponent(Entity original, Entity copy) override
        {
            if(contains(original))
            {
                insertData(copy, m_componentArray[m_entityToIndexMap[original]]);
                return true;
            }
            return false;
        }

    private:
        std::array<T, MAX_ENTITIES> m_componentArray;
        size_t m_count;
        bool m_enabled;
};

// ComponentManager (class): acts as an array of ComponentArrays :: regulates the addition, removal, and retrieval of components from their respective ComponentArrays
class ComponentManager
{
    public:
        // copies all components from 'original' to 'copy'
        void copyComponents(Entity original, Entity copy)
        {
            for(auto& pair : m_componentArrays)
            {
                pair.second -> copyComponent(original, copy);
            }
        }

        // registers given structure 'T' as a valid component type by assigning it an id
        template<typename T>
        void registerComponent()
        {
            const char *typeName = typeid(T).name();

            m_componentTypes.insert({typeName, m_nextComponentType});
            m_componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

            m_nextComponentType++;
        }

        template <typename T>
        bool componentRegistered()
        {
            return m_componentTypes.count(typeid(T).name());
        }

        // returns the id of the given structure 'T' as assigned by "registerComponent"
        template<typename T>
        ComponentType getComponentType()
        {
            const char *typeName = typeid(T).name();
            return m_componentTypes[typeName];
        }

        // adds 'entity' to ComponentArray of type 'T'
        template<typename T>
        void addComponent(Entity entity, const T& component)
        {
            getComponentArray<T>() -> insertData(entity, component);
        }

        // remove 'entity' from ComponentArray of type 'T'
        template<typename T>
        void removeComponent(Entity entity)
        {
            getComponentArray<T>() -> removeData(entity);
        }

        // retrieves component from ComponentArray of type 'T' attached to the given 'entity'
        template<typename T>
        T& getComponent(Entity entity)
        {
            return getComponentArray<T>() -> getData(entity);
        }

        template<typename T>
        bool containsComponent(Entity entity)
        {
            return getComponentArray<T>() -> containsComponent(entity);
        }

        // protocol for when an entity is removed from the ECS :: entity data is erased if it found
        void entityDestroyed(Entity entity)
        {
            for(auto const &pair : m_componentArrays)
            {
                auto const &component = pair.second;
                component->entityDestroyed(entity);
            }
        }

    private:
        std::unordered_map<const char *, ComponentType> m_componentTypes{};
        std::unordered_map<const char *, std::shared_ptr<IComponentArray>> m_componentArrays{};

        ComponentType m_nextComponentType{};

        // returns generic IComponentArray of type T as specified ComponentArray
        template<typename T>
        std::shared_ptr<ComponentArray<T>> getComponentArray()
        {
            return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays.at(typeid(T).name()));
        }
};


// SYSTEM MANAGEMENT

// System (class): controls how and entity is able to access its components using six key methods (load, destroy, update, update, lateUpdate, render, fixedUpdate) :: entity insertion is dependant on its Signature
class System
{
    public:
        std::vector<Entity> m_entities;

        System()
        {
            p_onInsert = [](Entity entity, std::vector<Entity> &entities)
            {
                if(entity && std::find(entities.begin(), entities.end(), entity) == entities.end())
                {
                    entities.push_back(entity);
                    std::sort(entities.begin(), entities.end(), operators::less<Entity>);
                }
            };
            p_onRemove = [](Entity entity, std::vector<Entity> &entities)
            {
                if(entity)
                {
                    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
                }
            };
        };

        // function that is run when an entity is inserted into this system
        void insert(Entity entity) {p_onInsert(entity, m_entities);}   
        void setInsertion(void (*insertion)(Entity, std::vector<Entity> &))
        {
            p_onInsert = insertion;
        }     

        // protocol for when an entity is removed from the ECS :: entity data is erased if it found
        void remove(Entity entity) {p_onRemove(entity, m_entities);}
        void setRemoval(void (*removal)(Entity, std::vector<Entity> &))
        {
            p_onRemove = removal;
        }

        // function that is run before the primary game loop begins
        void load() {p_load(*this);} 
        void setLoad(void (*load__)(System &))
        {
            p_load = load__;
        }      
        void reload()
        {
            load();
        }

        void start() {p_start(*this);} 
        void setStart(void (*start__)(System &))
        {
            p_start = start__;
        }      

        // function that is run after the primary game loop ends
        void destroy() {p_destroy(*this);} 
        void setDestroy(void (*destroy__)(System &))
        {
            p_destroy = destroy__;
        }

        // first function that is run inside the primary game loop every frame
        void update() {p_update(*this);}
        void setUpdate(void (*update__)(System &))
        {
            p_update = update__;
        }

        // second function that is run inside the primary game loop every frame
        void lateUpdate() {p_lateUpdate(*this);} 
        void setLateUpdate(void (*lateUpdate__)(System &))
        {
            p_lateUpdate = lateUpdate__;
        }

        // third function that is run inside the primary game loop every frame
        void render() {p_render(*this);} 
        void setRender(void (*render__)(System &))
        {
            p_render = render__;
        }

        // function that is run every 20 millisecond inside the primary game loop (runs before "update" on correct frames)
        void fixedUpdate() {p_fixedUpdate(*this);}  
        void setFixedUpdate(void (*fixedUpdate__)(System &))
        {
            p_fixedUpdate = fixedUpdate__;
        }

        std::vector<Entity> entities()
        {
            return m_entities;
        }

        // the first entity stored in every System :: this entity is automatically assigned to be the System's global data throughtout its lifetime
        Entity id()
        {
            return 0;
        }

        void setName(const std::string& name__)
        {
            name = name__;
        }
        std::string getName()
        {
            return name;
        }

        template<typename T>
        T& data();

        private:
            std::string name;

            void (*p_onInsert)(Entity, std::vector<Entity> &);
            void (*p_onRemove)(Entity, std::vector<Entity> &);

            void (*p_load)(System &system) = [](System &){};
            void (*p_start)(System &system) = [](System &){};
            void (*p_destroy)(System &system) = [](System &){};

            void (*p_render)(System &system) = [](System &){};
            void (*p_update)(System &system) = [](System &){};
            void (*p_lateUpdate)(System &system) = [](System &){};
            void (*p_fixedUpdate)(System &system) = [](System &){};
};

// SystemManager (class): acts as a data structure for System objects :: controls when an entity is added to a system, and calls each system's key methods
class SystemManager
{
    public:
        // registers values of type 'T' as valid systems
        template<typename T>
        std::shared_ptr<T> registerSystem(int32_t priority = 0)
        {
            auto system = std::make_shared<T>();
            system -> setName(typeid(T).name());
            m_systems.push(system, priority);
            m_signatures[typeid(T).name()] = Signature();
            return system;
        }

        // returns whether the given type 'T' is a valid system type
        template<typename T>
        bool containsSystem()
        {
            for(const auto& system : m_systems)
            {
                if(typeid(T).name() == system.value -> getName())
                {
                    return true;
                }
            }
            return false;
        }

        // returns a pointer to the System of type 'T'
        template<typename T>
        std::shared_ptr<T> getSystem()
        {
            for(const auto& system : m_systems)
            {
                if(typeid(T).name() == system.value -> getName())
                {
                    return std::static_pointer_cast<T>(system.value);
                }
            }
            return std::shared_ptr<T>(nullptr);
        }

        // sets the System with the typename 'typeName''s Signature to 'signature'
        void setSignature(const char *typeName, Signature signature)
        {
            m_signatures.at(typeName) = signature;
        }

        // protocol for when an entity is removed from the ECS :: entity data is erased if it found
        void entityDestroyed(Entity entity)
        {
            for(const auto& system : m_systems)
            {
                system.value -> remove(entity);
            }
        }

        // updates which Systems an entity belongs to when one of its components is either added or removed
        void entitySignatureChanged(Entity entity, Signature signature)
        {
            for(const auto& system : m_systems)
            {
                auto const &systemSignature = m_signatures.at(system.value -> getName());
                
                if((signature & systemSignature) == systemSignature)
                {                    
                    system.value -> insert(entity);
                }
                else
                {
                    system.value -> remove(entity);
                }
            }
        }

        // key methods for all systems

        void load()
        {
            for(auto& system : m_systems)
            {
                system.value -> load();
            }
        }
        void start()
        {
            for(auto& system : m_systems)
            {
                system.value -> start();
            }
        }
        void destroy()
        {
            for(auto& system : m_systems)
            {
                system.value -> destroy();
            }
        }

        void render()
        {
            for(auto& system : m_systems)
            {
                system.value -> render();
            }
        }
        void update()
        {
            for(auto& system : m_systems)
            {
                system.value -> update();
            }
        }
        void lateUpdate()
        {
            for(auto& system : m_systems)
            {
                system.value -> lateUpdate();
            }
        }
        void fixedUpdate()
        {
            for(auto& system : m_systems)
            {
                system.value -> fixedUpdate();
            }
        }

    private:
        std::unordered_map<std::string, Signature> m_signatures{};
        ctr::priority_queue<std::shared_ptr<System>> m_systems{};
};

// ObjectManager (class): an intermediary class for EntityManager, ComponentManager, and SystemManager :: all methods in this class can be found in the other manager classes
class ObjectManager
{
    public:
        std::unordered_map<std::string, Entity> m_entities;

        ObjectManager();

        // ENTITY
        Entity createEntity(const std::string& name)
        {
            return m_entities[name] = m_entityManager -> createEntity();
        }

        Entity copyEntity(const std::string& name, const std::string& copyName)
        {
            Entity original = m_entities[name];
            Entity copy = m_entities[copyName] = m_entityManager -> copyEntity(original);
            m_componentManager -> copyComponents(original, copy);
            m_systemManager -> entitySignatureChanged(copy, m_entityManager -> getSignature(copy));
            return copy;
        }

        Entity getEntity(const std::string& name) const
        {
            m_entities.at(name);
            return m_entities.at(name);
        }

        // protocol for when an entity is removed from the ECS :: entity data is erased if it found
        void destroyEntity(Entity entity)
        {
            m_entityManager -> destroyEntity(entity);
            m_componentManager -> entityDestroyed(entity);
            m_systemManager -> entityDestroyed(entity);
        }

        void load()
        {
            m_systemManager -> load();
        }
        void start()
        {
            m_systemManager -> start();
        }
        void destroy()
        {
            m_systemManager -> destroy();
        }

        void render()
        {
            m_systemManager -> render();
        }
        void update()
        {
            m_systemManager -> update();
        }
        void lateUpdate()
        {
            m_systemManager -> lateUpdate();
        }
        void fixedUpdate()
        {
            m_systemManager -> fixedUpdate();
        }

        // COMPONENT
        template<typename T>
        void registerComponent()
        {
            m_componentManager -> registerComponent<T>();
        }

        template<typename T>
        bool componentRegistered()
        {
            return m_componentManager -> componentRegistered<T>();
        }

        template<typename T>
        T& addComponent(Entity entity, const T& component)
        {
            m_componentManager -> addComponent(entity, component);
            
            auto signature = m_entityManager -> getSignature(entity);
            signature.set(m_componentManager -> getComponentType<T>(), true);
            m_entityManager -> setSignature(entity, signature);
            m_systemManager -> entitySignatureChanged(entity, signature);
            
            return getComponent<T>(entity);
        }

        template<typename T>
        void removeComponent(Entity entity)
        {
            m_componentManager -> removeComponent<T>(entity);

            auto signature = m_entityManager -> getSignature(entity);
            signature.set(m_componentManager -> getComponentType<T>(), false);
            m_entityManager -> setSignature(entity, signature);

            m_systemManager -> entitySignatureChanged(entity, signature);
        }

        template<typename T>
        T& getComponent(Entity entity)
        {
            return (m_componentManager -> getComponent<T>(entity));
        }

        template<typename T>
        bool containsComponent(Entity entity)
        {
            return m_componentManager -> containsComponent<T>(entity);
        }

        template<typename T>
        ComponentType getComponentType()
        {
            return m_componentManager -> getComponentType<T>();
        }

        // SYSTEM

        template<typename T>
        std::shared_ptr<T> registerSystem(int32_t priority)
        {
            return m_systemManager -> registerSystem<T>(priority);
        }

        template<typename T>
        bool containsSystem()
        {
            return m_systemManager -> containsSystem<T>();
        }

        template<typename T>
        std::shared_ptr<T> getSystem()
        {
            return m_systemManager -> getSystem<T>();
        }

        void setSystemSignature(const char *typeName, Signature signature)
        {
            m_systemManager -> setSignature(typeName, signature);
        }

        std::unique_ptr<EntityManager> m_entityManager;
        std::unique_ptr<ComponentManager> m_componentManager;
        std::unique_ptr<SystemManager> m_systemManager;
};


extern ObjectManager g_manager;

template<typename T>
T& System::data()
{
    return g_manager.getComponent<T>(0);
}

// Script (struct: extends System): creates a Component:System hybrid that can be easily user defined
struct Script : System
{
    public:
        // stores the Script's typename as its 'id'
        void initialize(const char *name__, std::shared_ptr<Script> rootPointer)
        {
            setName(name__);
            root = rootPointer;
        }
        void initText();

        // requires 'T' to be added to an entity so that it can use this Script
        template<typename T>
        void addRequirement()
        {
            if(!g_manager.componentRegistered<T>())
            {
                g_manager.registerComponent<T>();
            }
            signature.set(g_manager.getComponentType<T>());
            g_manager.setSystemSignature(getName().c_str(), signature);
        }
        Signature getSignature()
        {
            return signature;
        }


        // standard key method accessors for a System
        void insert(void (*insert__)(Entity, std::vector<Entity> &))
        {
            root -> setInsertion(insert__);
        }
        void remove(void (*remove__)(Entity, std::vector<Entity> &))
        {
            root -> setRemoval(remove__);
        }

        void load(void (*load__)(System &))
        {
            root -> setLoad(load__);
        }
        void start(void (*start__)(System &))
        {
            root -> setStart(start__);
        }
        void destroy(void (*destroy__)(System &))
        {
            root -> setDestroy(destroy__);
        }
        void render(void (*render__)(System &))
        {
            root -> setRender(render__);
        }
        void update(void (*update__)(System &))
        {
            root -> setUpdate(update__);
        }
        void lateUpdate(void (*lateUpdate__)(System &))
        {
            root -> setLateUpdate(lateUpdate__);
        }
        void fixedUpdate(void (*fixedUpdate__)(System &))
        {
            root -> setFixedUpdate(fixedUpdate__);
        }

    private:
        std::shared_ptr<Script> root;
        Signature signature;
};

struct AnimationManager : Script {};
struct BillboardManager : Script {};
struct ButtonManager : Script {};
struct CameraManager : Script {};
struct CollisionManager : Script {};
struct GraphicsManager : Script {};
struct LineManager : Script {};
struct MeshManager : Script {};
struct PhysicsManager : Script {};
struct PointLightManager : Script {};
struct SpotLightManager : Script {};
struct TextManager : Script {};
struct UIManager : Script {};

// Object (class): Entity wrapper :: allows for adding and removing components, cloning, and provides a basic child:parent hierarchy
class Object
{
    public:
        std::string name;
        Entity data;
        bool mobile;

        Object() {}

        Object(const std::string& name__, bool mobile__ = true) : mobile(mobile__)
        {
            name = name__;
            if(g_manager.m_entities.find(name) != g_manager.m_entities.end())
            {
                int16_t start = 0;
                while(g_manager.m_entities.find(name + std::to_string(start)) != g_manager.m_entities.end())
                {
                    start++;
                }
                name += std::to_string(start);
            }
            data = g_manager.createEntity(name);
            g_manager.addComponent<Object>(data, *this);
        }

        Object(const std::string& name__, Entity data__) : name(name__), data(data__) {}

        void addChild(const Object& child)
        {
            if(!duplicateChild(child.data))
                m_children.insert(child);
            else
                std::cout << "ERROR :: Object has already been added to family tree." << std::endl;
        }

        std::set<Object> children()
        {
            return m_children;
        }

        Object clone()
        {
            std::string copyName = name;
            int16_t start = 0;
            while(g_manager.m_entities.find(copyName + std::to_string(start)) != g_manager.m_entities.end())
            {
                start++;
            }
            Object copy;
            copy.name = copyName + std::to_string(start);
            copy.data = g_manager.copyEntity(name, copy.name);
            copy.mobile = mobile;
            for(Object child : m_children)
            {
                Object clone = child.clone();
                copy.addChild(clone);
            }
            return copy;
        }

        void destroy()
        {
            g_manager.destroyEntity(data);
        }

        template<typename T>
        T& addComponent(const T& component)
        {
            return g_manager.addComponent<T>(data, component);
        }

        template<typename T>
        T& addComponent()
        {
            return g_manager.addComponent<T>(data, T());
        }

        template<typename T>
        void removeComponent()
        {
            g_manager.removeComponent<T>(data);
        }

        template<typename T>
        T& getComponent()
        {
            return g_manager.getComponent<T>(data);
        }

        template<typename T>
        bool containsComponent()
        {
            return g_manager.containsComponent<T>(data);
        }

        bool operator <(const Object& obj) const
        {
            return data < obj.data;
        }

        void setEnabled(bool enable)
        {
            m_enabled = enable;
        }
        bool enabled()
        {
            return m_enabled;
        }

    private:
        bool m_enabled;
        std::set<Object> m_children;

        bool duplicateChild(Entity id)
        {
            bool recursiveCheck;
            for(Object m_child : m_children)
            {
                if(m_child.data == id || (recursiveCheck = m_child.duplicateChild(id)))
                {
                    return true;
                }
            }
            return false;
        }
};

// Billboard (struct): forces the 'target' to rotate towards the active Camera
struct Billboard
{
    Entity target;
};


// PHYSICS COMPONENTS

// BoxCollider (struct): calls 'trigger' function whenever another BoxCollider intersects with this one :: otherwise, miss function is called
struct BoxCollider
{
    Vector3 scale, offset, storedPosition;
    
    void (*trigger)(Entity, Entity, bool, int);
    void (*miss)(Entity);

    BoxCollider(const Vector3& scale__ = 1, const Vector3& offset__ = 0, void (*trigger__)(Entity, Entity, bool, int) = [](Entity entity, Entity target, bool edge, int triggerd){}, void (*miss__)(Entity) = [](Entity entity){}) : scale(scale__), offset(offset__), trigger(trigger__), miss(miss__)
    {
        refreshWireframe();
    }

    void setTrigger(void (*trigger__)(Entity, Entity, bool, int))
    {
        trigger = trigger__;
    }

    void setMiss(void (*miss__)(Entity))
    {
        miss = miss__;
    }

    void adjustCollider(const MeshAddon& addon)
    {
        Vector2 xBounds, yBounds, zBounds;
        for(const auto& addition : addon.additions)
        {
            xBounds.x = std::min(xBounds.x, addition.position.x);
            xBounds.y = std::max(xBounds.y, addition.position.x);

            yBounds.x = std::min(yBounds.x, addition.position.y);
            yBounds.y = std::max(yBounds.y, addition.position.y);

            zBounds.x = std::min(zBounds.x, addition.position.z);
            zBounds.y = std::max(zBounds.y, addition.position.z);
        }

        offset = {(xBounds.x + xBounds.y)/2, (yBounds.x + yBounds.y)/2, (zBounds.x + zBounds.y)/2};
        scale = {math::abs(xBounds.x) + math::abs(xBounds.y) + 1, math::abs(yBounds.x) + math::abs(yBounds.y) + 1, math::abs(zBounds.x) + math::abs(zBounds.y) + 1};

        if(wireframe)
            refreshWireframe();
    }

    void enableWireframe(bool enabled)
    {
        if(wireframe = enabled)
        {
            refreshWireframe();
        }
    }

    bool wireframeEnabled()
    {
        return wireframe;
    }

    void frame(const Transform& transform)
    {

    }

    private:
        bool wireframe = false;
        std::vector<Vector3> wirePoints;

        void refreshWireframe()
        {
            wirePoints.clear();

            wirePoints.push_back(Vector3(-scale.x, -scale.y, scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(-scale.x, scale.y, scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(scale.x, scale.y, scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(scale.x, -scale.y, scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(-scale.x, -scale.y, scale.z)*0.5f + offset);

            wirePoints.push_back(Vector3(-scale.x, -scale.y, -scale.z)*0.5f + offset);

            wirePoints.push_back(Vector3(-scale.x, scale.y, -scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(-scale.x, scale.y, scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(-scale.x, scale.y, -scale.z)*0.5f + offset);

            wirePoints.push_back(Vector3(scale.x, scale.y, -scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(scale.x, scale.y, scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(scale.x, scale.y, -scale.z)*0.5f + offset);

            wirePoints.push_back(Vector3(scale.x, -scale.y, -scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(scale.x, scale.y, -scale.z)*0.5f + offset);
            wirePoints.push_back(Vector3(scale.x, -scale.y, -scale.z)*0.5f + offset);

            wirePoints.push_back(Vector3(-scale.x, -scale.y, -scale.z)*0.5f + offset);
        }
};

// object (namespace): allows the user to avoid accessing the global ObjectManager variable :: only way to initialize a Script
namespace object
{
    void load();
    void start();
    void destroy();
    void render();
    void update();
    void lateUpdate();
    void fixedUpdate();
    
    Object find(std::string name);
    Vector3 brightness(int32_t value);

    template<typename T>
    T& addComponent(Entity entity, const T& component)
    {
        g_manager.m_componentManager -> addComponent(entity, component);
            
        auto signature = g_manager.m_entityManager -> getSignature(entity);
        signature.set(g_manager.m_componentManager -> getComponentType<T>(), true);
        g_manager.m_entityManager -> setSignature(entity, signature);
        g_manager.m_systemManager -> entitySignatureChanged(entity, signature);
        
        return g_manager.getComponent<T>(entity);
    }

    template<typename T>
    T& getComponent(Entity data)
    {
        return g_manager.getComponent<T>(data);
    }

    template<typename T>
    void removeComponent(Entity data)
    {
        g_manager.removeComponent<T>(data);
    }

    template<typename T>
    bool containsComponent(Entity data)
    {
        return g_manager.containsComponent<T>(data);
    }

    template<typename T>
    bool componentRegistered()
    {
        return g_manager.componentRegistered<T>();
    }

    template<typename T>
    void enableComponent(Entity data, bool enable)
    {
        // object::getComponent<T>(data)
    }

    template<typename T>
    ComponentType getComponentType()
    {
        return g_manager.getComponentType<T>();
    }

    template<typename T>
    std::shared_ptr<T> getSystem()
    {
        return g_manager.getSystem<T>();
    }

    template<typename T>
    std::shared_ptr<T> containsSystem()
    {
        return g_manager.containsSystem<T>();
    }

    template<typename T>
    T& initializeScript(int32_t priority = 0)
    {
        if(g_manager.containsSystem<T>())
        {
            
            return object::find("g_global_event_runner").getComponent<T>();
        }
        else
        {
            g_manager.registerComponent<T>();

            T &script = object::find("g_global_event_runner").addComponent<T>();
            script.initialize(typeid(T).name(), g_manager.registerSystem<T>(priority));
            script.template addRequirement<T>();
        
            return script;
        }
    }

    template<typename T>
    T& initializeScript(bool recurseRequirement, int32_t priority = 0)
    {
        if(g_manager.containsSystem<T>())
        {
            return object::find("g_global_event_runner").getComponent<T>();
        }
        else
        {
            g_manager.registerComponent<T>();

            T &script = object::find("g_global_event_runner").addComponent<T>();
            script.initialize(typeid(T).name(), g_manager.registerSystem<T>(priority));
            if(recurseRequirement)
            {
                script.template addRequirement<T>();
            }
            return script;
        }
    }

    template<typename S, typename T>
    S& initializeScript(int32_t priority = 0)
    {
        if(g_manager.containsSystem<S>())
        {
            return object::find("g_global_event_runner").getComponent<S>();
        }
        else
        {
            g_manager.registerComponent<S>();

            S& script = object::find("g_global_event_runner").addComponent<S>();
            script.initialize(typeid(S).name(), g_manager.registerSystem<S>(priority));
            script.template addRequirement<T>();

            return script;
        }
    }
};

// physics (namespace): allows for collision handling with a BoxCollider
namespace physics
{
    enum Direction {UP, RIGHT, DOWN, LEFT};
    void collisionTrigger(Entity entity, Entity collision, bool edge, int triggered);
    void collisionMiss(Entity entity);
}

//
namespace shader
{
    void simple(Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform);
    void ui(Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform);
    void advanced(Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform, const Vector3& strength, int32_t shininess);
}

// Material (struct): holds the information necessary to render an entity to a complex Shader
struct Material
{
    Shader shader;

    Vector3 ambientStrength, diffuseStrength, specularStrength;
    Color specular;
    float shininess;

    // determines whether the object responds to point and spot lights
    bool useAdvancedLighting = true;
    void(*run)(Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform);

    Material(const Shader &shader__ = Shader()) : shader(shader__)
    {
        run = shader::simple;
    }

    Material(const Shader& shader__, void(*runFunction)(Entity entity, const Model& model, const Camera& camera, const Transform& cameraTransform)) : shader(shader__)
    {
        run = runFunction;
    }
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
    void render(Entity entity, const Camera& camera = Camera(), const Transform& cameraTransform = Transform())
    {
        material.run(entity, *this, camera, cameraTransform);
        data.draw(texture);
    }
};



#endif