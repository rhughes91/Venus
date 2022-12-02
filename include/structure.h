#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <array>
#include <bitset>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>

#include "vector.h"

using Entity = uint32_t;
using Path = std::pair<std::string, std::string>;

class Object;

// operators (namespace): used for data structures that may require sorting
namespace operators
{
    struct less
    {
        bool operator() (int a, int b) const
        {
            return a < b;
        }
    };
}


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
    double deltaTime = 0.0f;
    double lastDeltaTime = 0.0f;
    double lastFrame = 0.0f;
    double averageFrameRate = 0.0f;
    double timer = 0;
    double runtime = 0;

    Time() {}

    // updates the "fixedUpdate" timer, interpolates deltaTime, and tracks the average framerate
    void update();
    void beginTimer();
    void resetTimer(double interval)
    {
        timer = timer-interval;
    }
};


using ComponentType = uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

/**
 * Entity Component System: System for compiling various data structures into and integer value called an 'Entity'
 * Key Classes: EntityManager, ComponentManager, SystemManager
 * Credit: https://austinmorlan.com/posts/entity_component_system/
 */

// ENTITY MANAGEMENT
const uint16_t MAX_ENTITIES = 100;

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
        void insertData(Entity entity, T component)
        {
            size_t newIndex = m_size;
            m_entityToIndexMap[entity] = newIndex;
            m_indexToEntityMap[newIndex] = entity;
            m_componentArray[newIndex] = component;
            
            m_size++;
        }

        // removes the index data of 'entity' and the component that belongs to it
        void removeData(Entity entity)
        {
            size_t removeIndex = m_entityToIndexMap[entity];
            size_t lastIndex = m_size-1;
            m_componentArray[removeIndex] = m_componentArray[lastIndex];

            Entity lastEntity = m_indexToEntityMap[lastIndex];
            m_entityToIndexMap[lastEntity] = removeIndex;
            m_indexToEntityMap[removeIndex] = lastEntity;

            m_entityToIndexMap.erase(entity);
            m_indexToEntityMap.erase(lastIndex);

            m_size--;
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
        size_t m_size;
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

        // returns the id of the given structure 'T' as assigned by "registerComponent"
        template<typename T>
        ComponentType getComponentType()
        {
            const char *typeName = typeid(T).name();
            return m_componentTypes[typeName];
        }

        // adds 'entity' to ComponentArray of type 'T'
        template<typename T>
        void addComponent(Entity entity, T component)
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
            const char *typeName = typeid(T).name();
            return std::static_pointer_cast<ComponentArray<T>>(m_componentArrays.at(typeName));
        }
};


// SYSTEM MANAGEMENT

// System (class): controls how and entity is able to access its components using six key methods (load, destroy, update, update, lateUpdate, render, fixedUpdate) :: entity insertion is dependant on its Signature
class System
{
    public:
        std::set<Entity, operators::less> m_entities;

        System()
        {
            p_onInsert = [](Entity entity, std::set<Entity, operators::less> &entities)
            {
                if(entity)
                    entities.insert(entity);
            };
            p_onRemove = [](Entity entity, std::set<Entity, operators::less> &entities)
            {
                if(entity)
                    entities.erase(entity);
            };
        };

        // function that is run when an entity is inserted into this system
        void insert(Entity entity) {p_onInsert(entity, m_entities);}   
        void setInsertion(void (*insertion)(Entity, std::set<Entity, operators::less> &))
        {
            p_onInsert = insertion;
        }     

        // protocol for when an entity is removed from the ECS :: entity data is erased if it found
        void remove(Entity entity) {p_onRemove(entity, m_entities);}
        void setRemoval(void (*removal)(Entity, std::set<Entity, operators::less> &))
        {
            p_onRemove = removal;
        }

        // function that is run before the primary game loop begins
        void load(System &system) {p_load(system);} 
        void setLoad(void (*load__)(System &))
        {
            p_load = load__;
        }      
        void reload()
        {
            load(*this);
        }

        void start(System &system) {p_start(system);} 
        void setStart(void (*start__)(System &))
        {
            p_start = start__;
        }      

        // function that is run after the primary game loop begins
        void destroy(System &system) {p_destroy(system);} 
        void setDestroy(void (*destroy__)(System &))
        {
            p_destroy = destroy__;
        }

        // first function that is run inside the primary game loop every frame
        void update(System &system) {p_update(system);}
        void setUpdate(void (*update__)(System &))
        {
            p_update = update__;
        }

        // second function that is run inside the primary game loop every frame
        void lateUpdate(System &system) {p_lateUpdate(system);} 
        void setLateUpdate(void (*lateUpdate__)(System &))
        {
            p_lateUpdate = lateUpdate__;
        }

        // third function that is run inside the primary game loop every frame
        void render(System &system) {p_render(system);} 
        void setRender(void (*render__)(System &))
        {
            p_render = render__;
        }

        // function that is run every 20 millisecond inside the primary game loop (runs before "update" on correct frames)
        void fixedUpdate(System &system) {p_fixedUpdate(system);}  
        void setFixedUpdate(void (*fixedUpdate__)(System &))
        {
            p_fixedUpdate = fixedUpdate__;
        }

        std::set<Entity, operators::less> entities()
        {
            return m_entities;
        }

        // the first entity stored in every System :: this entity is automatically assigned to be the System's global data throughtout its lifetime
        Entity id()
        {
            return *m_entities.begin();
        }

        template<typename T>
        T& data();

        private:
            void (*p_onInsert)(Entity, std::set<Entity, operators::less> &);
            void (*p_onRemove)(Entity, std::set<Entity, operators::less> &);

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
        // registers values of type 'T' as valid systems, and adds a component of type 'T' to a global entity :: this allows for variables to be stored statically between entities
        template<typename T>
        std::shared_ptr<T> registerSystem();

        // registers values of type 'T' as valid systems
        template<typename T>
        std::shared_ptr<T> registerClosedSystem()
        {
            auto system = std::make_shared<T>();
            m_systems.insert({typeid(T).name(), system});
            return system;
        }

        // returns whether the given type 'T' is a valid system type
        template<typename T>
        bool containsSystem()
        {
            return m_systems.find(typeid(T).name()) != m_systems.end();
        }

        // returns a pointer to the System of type 'T'
        template<typename T>
        std::shared_ptr<T> getSystem()
        {
            return std::static_pointer_cast<T>(m_systems[typeid(T).name()]);
        }

        // sets the System of type 'T''s Signature to 'signature' :: the System Signature is what determines what components an entity needs to be inserted into a System
        template<typename T>
        void setSignature(Signature signature)
        {
            m_signatures.insert({typeid(T).name(), signature});
        }

        // sets the System with the typename 'typeName''s Signature to 'signature'
        void setSignature(const char *typeName, Signature signature)
        {
            m_signatures.insert({typeName, signature});
        }

        // protocol for when an entity is removed from the ECS :: entity data is erased if it found
        void entityDestroyed(Entity entity)
        {
            for(auto const &pair : m_systems)
            {
                pair.second -> remove(entity);
            }
        }

        // updates which Systems an entity belongs to when one of its components is either added or removed
        void entitySignatureChanged(Entity entity, Signature signature)
        {
            for(auto const &pair : m_systems)
            {
                auto const &system = pair.second;
                auto const &systemSignature = m_signatures.at(pair.first);
                
                if((signature & systemSignature) == systemSignature)
                {
                    system -> insert(entity);
                }
                else
                {
                    system -> remove(entity);
                }
            }
        }


        // key methods for all systems

        void load()
        {
            for(auto &system : m_systems)
            {
                system.second -> load(*system.second);
            }
        }
        void start()
        {
            for(auto &system : m_systems)
            {
                system.second -> start(*system.second);
            }
        }
        void destroy()
        {
            for(auto &system : m_systems)
            {
                system.second -> destroy(*system.second);
            }
        }

        void render()
        {
            for(auto &system : m_systems)
            {
                system.second -> render(*system.second);
            }
        }
        void update()
        {
            for(auto &system : m_systems)
            {
                system.second -> update(*system.second);
            }
        }
        void lateUpdate()
        {
            for(auto &system : m_systems)
            {
                system.second -> lateUpdate(*system.second);
            }
        }
        void fixedUpdate()
        {
            for(auto &system : m_systems)
            {
                system.second -> fixedUpdate(*system.second);
            }
        }

    private:
        std::unordered_map<const char *, Signature> m_signatures{};
        std::unordered_map<const char *, std::shared_ptr<System>> m_systems{};
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
            return m_entities.at(name);
        }

        // protocol for when an entity is removed from the ECS :: entity data is erased if it found
        void destroyEntity(Entity entity)
        {
            return m_entityManager -> destroyEntity(entity);
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
        void addGlobalComponent(Entity entity, T component)
        {
            m_componentManager -> addComponent(entity, component);
        }

        template<typename T>
        T& addComponent(Entity entity, T component)
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
        ComponentType getComponentType()
        {
            return m_componentManager -> getComponentType<T>();
        }

        // SYSTEM
        template<typename T>
        std::shared_ptr<T> registerSystem()
        {
            return m_systemManager -> registerSystem<T>();
        }

        template<typename T>
        std::shared_ptr<T> registerClosedSystem()
        {
            return m_systemManager -> registerClosedSystem<T>();
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

        template<typename T>
        void setSystemSignature(Signature signature)
        {
            m_systemManager -> setSignature<T>(signature);
        }

        void setSystemSignature(const char *typeName, Signature signature)
        {
            m_systemManager -> setSignature(typeName, signature);
        }

    private:
        std::unique_ptr<EntityManager> m_entityManager;
        std::unique_ptr<ComponentManager> m_componentManager;
        std::unique_ptr<SystemManager> m_systemManager;
};


extern ObjectManager g_manager;
extern Time g_time;

// SYSTEMS: base Systems for the engine :: static variables are stored in these structures

struct Graphics : System {};
struct UIManager : System {};
struct CollisionHandler : System {};
struct PhysicsHandler : System {};
struct MovementHandler : System {};
struct SpotLightHandler : System {};
struct BillboardHandler : System {};
struct CameraManager : System {};
struct AnimationManager : System {};
struct ButtonManager : System {};


template<typename T>
std::shared_ptr<T> SystemManager::registerSystem()
{
    auto system = std::make_shared<T>();
    m_systems.insert({typeid(T).name(), system});

    g_manager.registerComponent<T>();
    g_manager.addGlobalComponent<T>(system -> id(), T());
    return system;
}

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
        void initialize(const char *name, std::shared_ptr<Script> rootPointer)
        {
            id = name;
            root = rootPointer;
        }

        // requires 'T' to be added to an entity so that it can use this Script
        template<typename T>
        void addRequirement()
        {
            signature.set(g_manager.getComponentType<T>());
            g_manager.setSystemSignature(id, signature);
        }


        // standard key method accessors for a System

        void load(void (*load__)(System &))
        {
            root -> setLoad(load__);
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
        const char *id;
        std::shared_ptr<Script> root;
        Signature signature;
};

// Object (class): Entity wrapper :: allows for adding and removing components, cloning, and provides a basic child:parent hierarchy
class Object
{
    public:
        std::string name;
        Entity data;

        Object() {}

        Object(const std::string& name__)
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
            for(Object child : m_children)
            {
                Object clone = child.clone();
                copy.addChild(clone);
            }
            return copy;
        }

        template<typename T>
        T& addComponent(T component)
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

        bool operator <(const Object& obj) const
        {
            return data < obj.data;
        }

    private:
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

// Button (struct): triggers its 'trigger' function when its attached Rect component is clicked on
struct Button
{
    void(*trigger)(Entity entity);

    Button(void(*trigger__)(Entity entity) = [](Entity entity){}) : trigger(trigger__) {}
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
    T& addComponent(Entity data, T component)
    {
        return g_manager.addComponent<T>(data, component);
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
    std::shared_ptr<T> getSystem()
    {
        return g_manager.getSystem<T>();
    }

    template<typename T>
    std::shared_ptr<T> registerSystem()
    {
        return g_manager.registerSystem<T>();
    }

    template<typename T>
    std::shared_ptr<T> containsSystem()
    {
        return g_manager.containsSystem<T>();
    }

    template<typename T>
    T& initializeScript()
    {
        if(g_manager.containsSystem<T>())
        {
            return object::find("g_global_event_runner").getComponent<T>();
        }
        else
        {
            g_manager.registerComponent<T>();
            T &script = object::find("g_global_event_runner").addComponent<T>();
            
            script.initialize(typeid(T).name(), g_manager.registerClosedSystem<T>());
            ((Script)script).addRequirement<T>();
        
            return script;
        }
    }
};

// physics (namespace): allows for collision handling with a BoxCollider
namespace physics
{
    enum Direction {UP, RIGHT, DOWN, LEFT};
    void collisionHandler(Entity entity, Entity collision, int triggered);
    void collisionMiss(Entity entity);
}

#endif