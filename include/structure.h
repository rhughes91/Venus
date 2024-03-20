#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <cstring>
#include <string>
#include <typeinfo>
#include <stdint.h>
#include <vector>

// #define ECS_DEBUG_OFF

using entity = uint32_t; /** @brief Alias for a 32-bit unsigned value.*/

template<typename T>
struct Serialization
{
    static size_t (*length)(const T&); /** @brief A user-defined function to define the length of provided data T.*/
    static size_t (*serialize)(const T&, std::vector<uint8_t>&, size_t); /** A user-defined function to define the serialization of provided data T.*/
    static T (*deserialize)(std::vector<uint8_t>&, size_t); /** A user-defined functoin to define the deserialization of provided data T.*/
};

void printSerializationError(const std::string& );

template<typename T>
size_t defaultLengthSetter(const T&)
{
    printSerializationError("ERROR: " + std::string(typeid(T).name()) + std::string(" does not have a function to define serialization length.\n"));
    return 0;
}

template<typename T>
size_t (*Serialization<T>::length)(const T&) = defaultLengthSetter;

template<typename T>
size_t defaultSerialization(const T&, std::vector<uint8_t>&, size_t)
{
    printSerializationError("ERROR: " + std::string(typeid(T).name()) + " does not have a defined serialization function.\n");
    return 0;
}

template<typename T>
size_t(*Serialization<T>::serialize)(const T&, std::vector<uint8_t>&, size_t) = defaultSerialization;

template<typename T>
T defaultDeserialization(std::vector<uint8_t>&, size_t)
{
    printSerializationError("ERROR: " + std::string(typeid(T).name()) + " does not have a defined deserialization function.\n");
    return T();
}

template<typename T>
T (*Serialization<T>::deserialize)(std::vector<uint8_t>&, size_t) = defaultDeserialization;


namespace object
{
    struct ecs;

    /**
     * @brief Assigns a unique ID for data types to be used as components.
     * 
     * @tparam T The type to be given a unique ID.
     */
    template<typename T>
    struct ComponentType
    {
        static const uint32_t id; /** @brief The unique ID assigned to a type.*/
    };

    /**
     * @brief Assigns a unique ID for data types to be used as systems.
     * 
     * @tparam T The type to be given a unique ID.
     */
    template<typename T>
    struct SystemType
    {
        static uint32_t id; /** @brief The unique ID assigned to a type.*/
    };


    template <typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
    size_t length(const T& value)
    {
        return sizeof(T);
    }

    template <typename T, std::enable_if_t<!std::is_trivially_copyable<T>::value, int> = 0>
    size_t length(const T& value)
    {
        return Serialization<T>::length(value) + sizeof(size_t);
    }

    template <typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
    size_t serialize(const T& value, std::vector<uint8_t>& stream, size_t index, size_t length = sizeof(T))
    {
        std::memcpy(&stream[index], &value, length);
        return length;
    }

    template <typename T, std::enable_if_t<!std::is_trivially_copyable<T>::value, int> = 0>
    size_t serialize(const T& value, std::vector<uint8_t>& stream, size_t index, size_t length = sizeof(T))
    {
        object::serialize(length, stream, index);
        return Serialization<T>::serialize(value, stream, index + sizeof(size_t)) + sizeof(size_t);
    }

    template <typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
    T& deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        return *reinterpret_cast<T*>(&stream[index]);
    }

    template <typename T, std::enable_if_t<!std::is_trivially_copyable<T>::value, int> = 0>
    T deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        return Serialization<T>::deserialize(stream, index + sizeof(size_t));
    }

    template <typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
    size_t resize(size_t, std::vector<uint8_t>&, size_t)
    {
        return 0;
    }

    template <typename T, std::enable_if_t<!std::is_trivially_copyable<T>::value, int> = 0>
    size_t resize(size_t length, std::vector<uint8_t>& stream, size_t index)
    {
        size_t original = object::deserialize<size_t>(stream, index);
        size_t offset = length - original;

        std::vector<uint8_t> copy = stream;
        
        if(original < length)
        {
            stream.resize(stream.size() + offset);
            std::memcpy(&stream[index + length], &copy[index + original], copy.size() - index - original);
        }
        else if(original > length)
        {
            std::memcpy(&stream[index + length], &copy[index + original], copy.size() - index - original);
            stream.resize(stream.size() + offset);
        }
        
        return offset;
    }


    struct ecs
    {
        /**
         * @brief Holds a list of `Entity`'s based on their components.
         * 
         * @details Allows the user to iterate over `Entity`'s with common components in user-defined 
         *          functions, holds a static instance of the `System`'s parent data type.
         */
        struct system
        {
            int32_t priority = 0;
            uint32_t *index = NULL;
            /**
             * @brief Constructor for struct 'System'.
             * 
             * @param space The size of `System`'s parent data type needed for memory allocation.
             */
            system() {}

            system(uint8_t functionSize)
            {
                m_functions = std::vector<void (*)(object::ecs&, system&)>(functionSize, [](object::ecs&, system&){});
                m_initialized = false;
            }

            /**
             * @brief Finishes initializing this `System`.
             * 
             * @details The `System`'s static instance, its constructor, and its destructor are fully 
             *          initialized and set.
             * 
             * @tparam T The type of `System`'s static instance.
             * @param instance The initial data of `System's` static instance.
             */
            template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
            void initialize(const T& instance)
            {
                m_initialized = true;
                m_instance.resize(sizeof(T));
                object::serialize<T>(instance, m_instance, 0);
            }

            template<typename T, typename = std::enable_if_t<!std::is_trivially_copyable<T>::value>>
            void initialize(const T instance, bool temp = false)
            {
                m_initialized = true;
                m_instance.resize(object::length(instance) + sizeof(size_t));
                object::serialize<T>(instance, m_instance, 0);
            }

            /**
             * @brief Determines whether the current `System` has been fully initialized.
             * 
             * @return The initialization state of this `System`.
             */
            bool& initialized()
            {
                return m_initialized;
            }

            /**
             * @brief Creates a new usable space for a user-defined function.
             */
            void createFunction()
            {
                m_functions.push_back([](ecs&, system&){});
            }


            /**
             * @brief Sets a user-defined function pointer at a certain index.
             * 
             * @param index The index where the function pointer will be placed.
             * @param function The new function pointer that will be stored.
             */
            void setFunction(uint8_t index, void (*function)(ecs&, system&))
            {
                m_functions[index] = function;
            }


            /**
             * @brief Runs the user-defined or default function at a certain index.
             * 
             * @param index The index where the function pointer will be placed.
             */
            void runFunction(ecs& container, uint8_t index)
            {
                m_functions[index](container, *this);
            }

            /**
             * @brief Converts this `System`'s static instance from unsigned 8-bit data into its original type.
             * 
             * @tparam T The original data type of this `System`'s static instance.
             * @return The 8-bit data transformed into a usable type.
             */
            template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
            T& getInstance()
            {
                return object::deserialize<T>(m_instance, 0);
            }

            template<typename T, typename = std::enable_if_t<!std::is_trivially_copyable<T>::value>>
            T getInstance()
            {
                return object::deserialize<T>(m_instance, 0);
            }

            template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
            void pushInstance(const T& instance)
            {
                m_instance.resize(sizeof(T));
                object::serialize<T>(instance, m_instance, 0);
            }

            template<typename T, typename = std::enable_if_t<!std::is_trivially_copyable<T>::value>>
            void pushInstance(const T instance, bool temp = false)
            {
                m_instance.resize(object::length(instance) + sizeof(size_t));
                object::serialize<T>(instance, m_instance, 0);
            }
            
            private:
                bool m_initialized; /** @brief Tracks whether the `System` has been fully initialized.*/
                std::vector<uint8_t> m_instance;  /** @brief The raw static instance data.*/
                std::vector<void (*)(ecs&, system&)> m_functions; /** @brief Various user-defined or default functions that can be run on the `System`.*/
        };

        static uint16_t error;

        ecs()
        {
            entity totalEntities = entityManager.totalEntityCount();
            componentManager = ComponentManager(totalEntities);
            systemManager = SystemManager(totalEntities);
        }

        entity createEntity()
        {
            systemManager.update(entityManager.totalEntityCount());
            systemManager.addEntity();
            
            componentManager.update(entityManager.totalEntityCount());
            componentManager.addEntity();
            entity e = entityManager.createEntity();

            addComponent<bool>(e, true);

            return e;
        }

        void removeEntity(entity e)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    error = 6;
                    return;
                }
            #endif

            componentManager.removeEntity(e);
            systemManager.extractEntity(e, entityManager.getBitmap(e));
            entityManager.removeEntity(e);
        }

        entity numberOfEntities()
        {
            return entityManager.totalEntityCount();
        }

        bool active(entity e)
        {
            return getComponent<bool>(e);
        }

        void setActive(entity e, bool newState)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    error = 6;
                    return;
                }
            #endif
            bool& state = getComponent<bool>(e);
            if(state == newState)
            {
                return;
            }

            state = newState;
            if(newState)
            {
                std::vector<bool>& bitmap = entityManager.getBitmap(e);

                for(uint32_t i=0; i<systemManager.idCount; i++)
                {
                    if(systemManager.bitmapMatches(i, bitmap))
                    {
                        systemManager.insertEntity(e, i);
                    }
                }
            }
            else
            {
                systemManager.extractEntity(e, entityManager.getBitmap(e));
            }
        }

        size_t numberOfComponents()
        {
            return ComponentManager::idCount;
        }

        template <typename T>
        bool active(entity e)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    error = 6;
                    return false;
                }
            #endif

            return entityManager.getBitmap(e)[ComponentType<T>::id];
        }

        template <typename T>
        void setActive(entity e, bool newState)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    error = 6;
                    return;
                }
            #endif

            uint32_t id = ComponentType<T>::id;
            std::vector<bool>& bitmap = entityManager.getBitmap(e);
            if(bitmap[id] == newState || !componentManager.containsComponent<T>(e))
            {
                return;
            }

            if(!bitmap[id])
                bitmap[id] = true;

            if(newState)
            {
                for(int i=0; i<systemManager.idCount; i++)
                {
                    if(systemManager.bitmapMatches(i, bitmap, id))
                    {
                        systemManager.insertEntity(e, i);
                    }
                }
            }
            else
            {
                systemManager.componentRemoved(e, id, bitmap);
            }

            if(bitmap[id])
                bitmap[id] = false;
        }

        template <typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
        T& addComponent(entity e, const T& component = T())
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    error = 6;
                    return componentManager.getComponent<T>(-1);
                }
            #endif

            uint32_t id = ComponentType<T>::id;
            T& result = componentManager.addComponent<T>(e, id, component);
            addComponentConfiguration(e, id);    
            return result;
        }

        template <typename T, std::enable_if_t<!std::is_trivially_copyable<T>::value, int> = 0>
        T addComponent(entity e, const T& component = T())
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    error = 6;
                    return componentManager.getComponent<T>(-1);
                }
            #endif

            uint32_t id = ComponentType<T>::id;
            T result = componentManager.addComponent<T>(e, id, component);
            addComponentConfiguration(e, id);
            
            return result;
        }

        template <typename T>
        void shareComponent(entity e, entity share)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    error = 6;
                    return;
                }
            #endif

            uint32_t id = ComponentType<T>::id;
            componentManager.share<T>(e, share, id);
            addComponentConfiguration(e, id);
        }

        template<typename T>
        void setComponent(entity e, const T& update)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    e = -1;
                    error = 6;
                }
            #endif

            componentManager.setComponent<T>(e, update);
        }

        template <typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, int> = 0>
        T& getComponent(entity e)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    e = -1;
                    error = 6;
                }
            #endif

            return componentManager.getComponent<T>(e);
        }

        template <typename T, std::enable_if_t<!std::is_trivially_copyable<T>::value, int> = 0>
        T getComponent(entity e)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    e = -1;
                    error = 6;
                }
            #endif

            return componentManager.getComponent<T>(e);
        }

        template <typename T>
        T& getDefaultComponent()
        {
            return componentManager.getDefaultComponent<T>();
        }

        template<typename T>
        bool containsComponent(entity e)
        {
            return componentManager.containsComponent<T>(e);
        }

        template <typename T>
        T removeComponent(entity e)
        {
            #ifndef ECS_DEBUG_OFF
                if(!entityManager.contains(e))
                {
                    error = 6;
                    return T();
                }
            #endif

            uint32_t id = ComponentType<T>::id;
            systemManager.componentRemoved(e, id, entityManager.getBitmap(e));
            entityManager.setComponentBit(e, id, false);
                
            return componentManager.removeComponent<T>(e);
        }

        uint8_t createSystemFunction()
        {
            return systemManager.createSystemFunction();
        }

        template <typename T>
        std::vector<entity>& entities()
        {
            return systemManager.entities<T>();
        }

        template<typename T, typename... Args>
        system& createSystem(const T& instance = T(), int32_t priority = 0)
        {
            uint32_t& id = SystemType<T>::id;

            systemManager.update(entityManager.totalEntityCount());
            system& result = systemManager.createSystem<T>(instance, priority, id);

            systemManager.addRequirements<T, Args...>();
            
            entity totalEntities = entityManager.totalEntityCount();
            for(int i=0; i<totalEntities; i++)
            {
                if(entityManager.entityActive(i))
                {
                    std::vector<bool> bitmap = entityManager.getBitmap(i);
                    if(systemManager.bitmapMatches(id, bitmap))
                    {
                        systemManager.insertEntity(i, id);
                    }
                }
            }

            return result;
        }

        template<typename T>
        void setInsertion(void (*insert)(entity, std::vector<entity>&, std::vector<size_t>&))
        {
            systemManager.setInsertion(SystemType<T>::id, insert);
        }

        template<typename T>
        std::vector<size_t>& getMapping()
        {
            return systemManager.getIndexMap<T>();
        }

        void run(uint8_t index)
        {
            #ifndef ECS_DEBUG_OFF
                if(!systemManager.containsFunction(index))
                {
                    error = 5;
                    return;
                }
            #endif
            systemManager.runFunction(*this, index);
        }

        static uint16_t getError()
        {
            uint16_t err = error;
            error = 0;
            return err;
        }

        static void parseError();

        template<typename T>
        static const uint32_t newComponentId()
        {
            return ComponentManager::newId<T>();
        }

        template<typename T>
        static const uint32_t newSystemId()
        {
            return SystemManager::newId<T>();
        }

        private:
            void addComponentConfiguration(entity entity, uint32_t id)
            {
                entityManager.setComponentBit(entity, id, true);
                std::vector<bool>& bitmap = entityManager.getBitmap(entity);

                for(uint32_t i=0; i<systemManager.idCount; i++)
                {
                    if(systemManager.bitmapMatches(i, bitmap, id))
                    {
                        systemManager.insertEntity(entity, i);
                    }
                }
            }


            /**
             * @brief A component pool that holds various data of a certain type.
             * 
             * @details Although templates are not explicitly used for the struct, a `ComponentArray` can only hold a single type.
             *          A vector of 8-bit unsigned values is used to hold this information.
             *          Data can easily be transformed into and out of this format.
             */
            struct ComponentArray
            {
                size_t componentSize;            /** @brief The size of the component the `components` vector holds.*/

                ComponentArray(size_t size, bool complexity)
                {
                    m_components.resize(sizeof(bool) + size);
                    object::serialize<bool>(!complexity, m_components, 0);
                    componentSize = size;
                }

                size_t overwrite(size_t index)
                {
                    size_t offset;
                    if(complex())
                    {
                        offset = object::deserialize<size_t>(m_components, index) + sizeof(size_t);
                    }
                    else
                    {
                        offset = componentSize;
                    }

                    std::memcpy(&m_components[index], &m_components[index + offset], m_components.size() - (index + offset));
                    m_components.resize(m_components.size() - offset);

                    return offset;
                }

                bool complex()
                {
                    return object::deserialize<bool>(m_components, 0);
                }

                /**
                 * @brief Attaches data based off a provided `Entity`.
                 * 
                 * @details Defined after the definition of struct `ecs`.
                 *          Allows the user to access specific data using an `Entity` as a unique index.
                 *          Only single piece of data can be allocated per type per Entity.
                 * 
                 * @tparam T The type of component to be added to `entity`.
                 * @param entity An `Entity` made by the `createEntity` function. Used to index component data.
                 * @param component The component data to be linked to `entity`.
                 * @return Reference to the component in its component pool.
                 */
                template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
                T& addComponent(size_t& index, const T& component);


                template<typename T, typename = std::enable_if_t<!std::is_trivially_copyable<T>::value>>
                T addComponent(size_t& index, const T& component);

                /**
                 * @brief Returns data based off a provided `Entity`.
                 * 
                 * @details Defined after the definition of struct `ecs`.
                 *          Retrieves a piece of data of type T attached to the `entity` if such data exists.
                 * 
                 * @tparam T The type of component to be retrieved.
                 * @param entity An `Entity` made by the `createEntity` function. Used to index component data.
                 * @return Reference to the component in its component pool.
                 */
                template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
                T& getComponent(size_t index);

                template<typename T, typename = std::enable_if_t<!std::is_trivially_copyable<T>::value>>
                T getComponent(size_t index);

                template<typename T>
                size_t setComponent(size_t index, const T& update);

                /**
                 * @brief Retrieves a reference to an unattached component.
                 * 
                 * @details Used in error cases when attempting to access unavailable components.
                 * 
                 * @tparam T The type of base component to be retrieved.
                 * @return Reference to the unattached component.
                 */
                template<typename T>
                T& getDefaultComponent()
                {
                    return *reinterpret_cast<T*>(&m_components[sizeof(bool)]);
                }

                private:
                    std::vector<uint8_t> m_components; /** @brief An vector of 8-bit unsigned values that holds all the component data.*/
            };


            /**
             * @brief A manager for `ComponentArray`s of every type.
             * 
             * @details Handles organizing and locating component pools based on their type.
             * Works with `ComponentType` in order to give each pool data type a unique ID.
             */
            struct ComponentManager
            {
                static uint32_t idCount; /** @brief Holds the number of registered component types.*/

                ComponentManager() {}

                /**
                 * @brief Constructor for struct `ComponentManager`.
                 * 
                 * @details Further initializes each component pool with information 
                 *          related to the type of data they hold.
                 */
                ComponentManager(entity numberOfEntities) : m_indexMaps(idCount, std::vector<size_t>(numberOfEntities))
                {
                    for(int i=0; i<idCount; i++)
                    {
                        // space is allocated for an empty object of type T; this object can be used for error-handling
                        m_componentArrays.push_back(ComponentArray(spaceBuffer[i], complexBuffer[i]));
                    }
                }

                void addEntity()
                {
                    for(uint32_t i=0; i<idCount; i++)
                    {
                        m_indexMaps[i].push_back(-1);
                    }
                }

                /**
                 * @brief Signals each component pool that an `Entity` has been removed.
                 */
                void removeEntity(entity entity)
                {
                    for(uint32_t id=0; id<idCount; id++)
                    {
                        size_t index = m_indexMaps[id][entity];
                        if(index == (size_t)-1)
                            continue;

                        remove(id, index, entity);
                    }
                }

                void update(entity numberOfEntities)
                {
                    while(m_indexMaps.size() < idCount)
                    {
                        m_indexMaps.push_back(std::vector<size_t>(numberOfEntities));
                        m_componentArrays.push_back(ComponentArray(spaceBuffer[idCount-1], complexBuffer[idCount-1]));
                    }
                }

                bool complex(uint32_t id)
                {
                    return m_componentArrays[id].complex();
                }

                /**
                 * @brief Attaches data based off a provided data type and `Entity`.
                 * 
                 * @details Accesses the proper component pool and allocates space for a new component that
                 *          can be accessed with the value of `entity`. Also verifies that the provided entity
                 *          is `active` (a.k.a. is has not been removed).
                 * 
                 * @tparam T The type of component pool to be accessed and the type of component to be added to entity.
                 * @param e An `Entity` made by the `createEntity` function. Used to index component data in respective pool.
                 * @param component The component data to be linked to `entity`.
                 * @param active Boolean to determine whether the `entity` is usable.
                 * @return Reference to the component in its component pool.
                 */
                template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
                T& addComponent(entity e, uint32_t id, const T& component)
                {
                    ComponentArray& array = m_componentArrays[id];
                    return array.addComponent<T>(m_indexMaps[id][e], component);
                }


                template<typename T, typename = std::enable_if_t<!std::is_trivially_copyable<T>::value>>
                T addComponent(entity e, uint32_t id, const T& component)
                {
                    ComponentArray& array = m_componentArrays[id];
                    return array.addComponent<T>(m_indexMaps[id][e], component);
                }

                template<typename T>
                void share(entity e, entity share, uint32_t id)
                {
                    if(m_indexMaps[id][e] != -1)
                    {
                        removeComponent<T>(e);
                    }
                    m_indexMaps[id][e] = m_indexMaps[id][share];
                }

                /**
                 * @brief Retrieves data based off a provided data type and  `Entity`.
                 * 
                 * @details Accesses the proper component pool and retrieves a piece of data of type T
                 *          attached to the `entity` if such data exists.
                 * 
                 * @tparam T The type of component pool to be accessed and the type of component to be retrieved.
                 * @param entity An `Entity` made by the `createEntity` function. Used to index component data in respective pool.
                 * @return Reference to the component in its component pool.
                 */
                template<typename T, typename = std::enable_if_t<std::is_trivially_copyable<T>::value>>
                T& getComponent(entity entity)
                {
                    uint32_t id = ComponentType<T>::id;

                    #ifndef ECS_DEBUG_OFF
                        if(entity == (entity)-1)
                            return m_componentArrays[id].getDefaultComponent<T>();
                    #endif

                    return m_componentArrays[id].getComponent<T>(m_indexMaps[id][entity]);
                }

                template<typename T, typename = std::enable_if_t<!std::is_trivially_copyable<T>::value>>
                T getComponent(entity entity)
                {
                    uint32_t id = ComponentType<T>::id;

                    #ifndef ECS_DEBUG_OFF
                        if(entity == (entity)-1)
                            return m_componentArrays[id].getDefaultComponent<T>();
                    #endif

                    return m_componentArrays[id].getComponent<T>(m_indexMaps[id][entity]);
                }


                template<typename T>
                T& getDefaultComponent()
                {
                    uint32_t id = ComponentType<T>::id;
                    return m_componentArrays[id].getDefaultComponent<T>();
                }

                template<typename T>
                bool containsComponent(entity entity)
                {
                    uint32_t id = ComponentType<T>::id;
                    return (m_indexMaps[id][entity] != -1);
                }

                /**
                 * @brief Removes data based off a provided data type and `Entity`.
                 * 
                 * @details Accesses the proper component pool, deletes, and deallocates a piece of data 
                 *          of type T attached to the `entity` if such data exists.
                 * 
                 * @tparam T The type of component pool to be accessed and the type of component to be deleted.
                 * @param entity An `Entity` made by the `createEntity` function. Used to index component data in respective pool.
                 * @return Copy of the deleted component.
                 */
                template<typename T>
                T removeComponent(entity entity)
                {
                    T result = getComponent<T>(entity);

                    uint32_t id = ComponentType<T>::id;
                    size_t index = m_indexMaps[id][entity];

                    remove(id, index, entity);

                    return result;
                }
                
                template<typename T>
                void setComponent(entity entity, const T& update)
                {
                    uint32_t id = ComponentType<T>::id;
                    size_t index = m_indexMaps[id][entity];
                    size_t offset = m_componentArrays[id].setComponent<T>(index, update);

                    size_t size = m_indexMaps[id].size();
                    for(int i=0; i<size; i++)
                    {
                        if(m_indexMaps[id][i] > index)
                            m_indexMaps[id][i] += offset;
                    }
                }
                
                /**
                 * @brief Creates and returns a new 32-bit ID.
                 * 
                 * @details When a new `ComponentType<T>` is referenced, it is statically initialized with
                 *          this function.
                 * 
                 * @tparam T The data type whose size is stored.
                 * @return A unique 32-bit ID.
                 */
                template<typename T>
                static const uint32_t newId()
                {
                    // whenever the compiler finds a new ComponentType, this function is called
                    uint32_t index = idCount;
                    spaceBuffer.push_back(sizeof(T));
                    complexBuffer.push_back(std::is_trivially_copyable<T>());
                    idCount++;
                    return index;
                }

                private:
                    void remove(uint32_t id, size_t index, entity entity)
                    {
                        size_t offset = m_componentArrays[id].overwrite(index);
                        
                        m_indexMaps[id][entity] = -1;

                        size_t size = m_indexMaps[id].size();
                        for(int i=0; i<size; i++)
                        {
                            if(m_indexMaps[id][i] > index && m_indexMaps[id][i] != (size_t)-1)
                            {
                                m_indexMaps[id][i] -= offset;
                            }
                        } 
                    }

                    static std::vector<size_t> spaceBuffer;        /** @brief A temporary vector that holds the size of each component per pool.*/
                    static std::vector<bool> complexBuffer;        /** @brief A temporary vector that stores whether a component type is copyable.*/

                    std::vector<ComponentArray> m_componentArrays; /** @brief A vector of component pools.*/
                    
                    std::vector<std::vector<size_t>> m_indexMaps;
            };


            struct SystemSupplement
            {
                std::vector<uint32_t> requirement;
                std::vector<size_t> indexMap;
                std::vector<entity> reverseIndexMap;
                void (*insertion)(entity, std::vector<entity>&, std::vector<size_t>&);

                SystemSupplement(entity numberOfEntities)
                {
                    requirement = std::vector<uint32_t>();
                    indexMap = std::vector<size_t>(numberOfEntities);
                    reverseIndexMap = std::vector<entity>();
                    insertion = [] (entity e, std::vector<entity>& entities, std::vector<size_t>& map)
                    {
                        map[e] = entities.size();
                        entities.push_back(e);
                    };
                }

                void insert(entity e)
                {
                    insertion(e, reverseIndexMap, indexMap);
                }
            
                void extract(entity e)
                {
                    size_t revSize = reverseIndexMap.size();

                    entity last = reverseIndexMap[revSize-1];
                    reverseIndexMap[indexMap[e]] = last;
                    indexMap[last] = indexMap[e];

                    reverseIndexMap.pop_back();
                    indexMap[e] = -1;
                }
            };


            struct SystemManager
            {
                static uint32_t idCount;
                static uint8_t functionIndex;

                SystemManager() {}

                SystemManager(entity numberOfEntities) : m_stores(idCount, system(functionIndex)), m_supplements(idCount, SystemSupplement(numberOfEntities)) {}

                void runFunction(ecs& container, uint8_t index)
                {
                    for(system& store : m_stores)
                    {
                        if(store.initialized())
                        {
                            store.runFunction(container, index);
                        }
                    }
                }

                bool bitmapMatches(entity index, const std::vector<bool>& bitmap) const
                {
                    const std::vector<uint32_t>& requirement = m_supplements[index].requirement;
                    if(requirement.empty())
                    {
                        return false;
                    }
                    
                    for(uint32_t req : requirement)
                    {
                        if(!bitmap[req])
                            return false;
                    }
                    return true;
                }

                bool bitmapMatches(entity index, const std::vector<bool>& bitmap, uint32_t bit) const
                {
                    const std::vector<uint32_t>& requirement = m_supplements[index].requirement;
                    if(requirement.empty())
                    {
                        return false;
                    }
                    
                    bool valid = false;
                    for(uint32_t req : requirement)
                    {
                        if(!bitmap[req])
                        {
                            return false;
                        }
                        if(req == bit)
                            valid = true;
                    }
                    return valid;
                }

                void setInsertion(uint32_t index, void (*insert)(entity, std::vector<entity>&, std::vector<size_t>&))
                {
                    m_supplements[index].insertion = insert;
                }

                /**
                 * @brief Signals each system that a new `Entity` has been created.
                 */
                void addEntity()
                {
                    for(uint32_t i=0; i<idCount; i++)
                    {
                        m_supplements[i].indexMap.push_back(-1);
                    }
                }

                void insertEntity(entity e, uint32_t index)
                {
                    m_supplements[index].insert(e);
                }

                void extractEntity(entity e, const std::vector<bool>& bitmap)
                {
                    for(uint32_t i=0; i<idCount; i++)
                    {
                        if(bitmapMatches(i, bitmap))
                        {
                            m_supplements[i].extract(e);
                        }
                    }
                }

                void componentRemoved(entity e, uint32_t bit, const std::vector<bool>& bitmap)
                {
                    for(uint32_t i=0; i<idCount; i++)
                    {
                        if(bitmapMatches(i, bitmap, bit))
                        {
                            size_t revSize = m_supplements[i].reverseIndexMap.size();
                            entity last = m_supplements[i].reverseIndexMap[revSize-1];
                            m_supplements[i].reverseIndexMap[m_supplements[i].indexMap[e]] = last;
                            m_supplements[i].indexMap[last] = m_supplements[i].indexMap[e];

                            m_supplements[i].reverseIndexMap.pop_back();
                            m_supplements[i].indexMap[e] = -1;
                        }
                    }
                }


                bool contains(uint32_t index)
                {
                    return index < idCount;
                }

                bool containsFunction(uint32_t index)
                {
                    return index < functionIndex;
                }

                bool initialized(uint32_t index)
                {
                    return m_stores[index].initialized();
                }

                uint8_t createSystemFunction()
                {
                    for(system& store : m_stores)
                    {
                        store.createFunction();
                    }
                    return functionIndex++;
                }

                void update(entity numberOfEntities)
                {
                    while(m_supplements.size() < idCount)
                    {
                        m_stores.push_back(system(functionIndex));
                        m_supplements.push_back(SystemSupplement(numberOfEntities));
                    }
                }

                template<typename T>
                system& createSystem(const T& instance, int32_t priority, uint32_t& id)
                {
                    auto store = m_stores[id];
                    auto supplement = m_supplements[id];

                    uint32_t index = id/2;
                    uint32_t total = index, buffer;
                    if(id > 0)
                    {
                        do
                        {
                            buffer = index;
                            index = index/2;

                            if(m_stores[total].priority > priority)
                            {
                                total -= (index+(buffer % 2));
                            }
                            else
                            {
                                total += (index+(buffer % 2));
                                if(!index)
                                {
                                    total += id%2;
                                }
                            }
                        }
                        while (index != 0);
                    }
                    
                    for(int i=id; i>total; i--)
                    {
                        moveTo(i-1, i);
                        *m_stores[i].index = i;
                    }

                    system& system = m_stores[total];
                    m_supplements[total] = supplement;

                    system = store;
                    system.priority = priority;
                    system.index = &id;

                    id = total;

                    system.initialize<T>(instance);
                    return system;
                }

                system& getSystem(uint32_t index)
                {
                    return m_stores[index];
                }

                template<typename T>
                std::vector<size_t>& getIndexMap()
                {
                    uint32_t id = SystemType<T>::id;
                    return m_supplements[id].indexMap;
                }

                template<typename T>
                std::vector<entity>& entities()
                {
                    uint32_t id = SystemType<T>::id;
                    return m_supplements[id].reverseIndexMap;
                }

                template<typename T, typename... Args>
                void addRequirements();

                const std::vector<uint32_t>& getRequirement(uint32_t index) const
                {
                    return m_supplements[index].requirement;
                }

                template<typename T>
                static const uint32_t newId()
                {
                    uint32_t index = idCount;
                    // whenever the compiler finds a new ComponentType, this function is called
                    m_spaceBuffer.push_back(sizeof(T));
                    idCount++;
                    return index;
                }

                private:
                    static std::vector<size_t> m_spaceBuffer;

                    std::vector<system> m_stores;
                    std::vector<SystemSupplement> m_supplements;

                    void moveTo(uint32_t from, uint32_t to)
                    {
                        m_stores[to] = m_stores[from];
                        m_supplements[to] = m_supplements[from];
                    }

                    template<typename Sys, typename S>
                    void addRequirement(uint32_t id)
                    {
                        uint32_t componentId = ComponentType<S>::id;
                        std::vector<uint32_t>& requirement = m_supplements[id].requirement;

                        for(uint32_t index = 0; index < requirement.size(); index++)
                        {
                            if(requirement[index] == componentId)
                                return;
                        }
                        requirement.push_back(componentId);
                    }

                    // as the function runs recursively, `First` is set as each type from `Args` one by one until `Args` is empty
                    template<typename Sys, typename First, typename... Args>
                    void addRequirementsRecursive(uint32_t id)
                    {
                        addRequirement<Sys, First>(id);
                        addRequirementsRecursive<Sys, Args...>(id);
                    }

                    // this is the end point for `addRequirementsRecursive` (it only runs when there are no more arguments to proceess)
                    template<typename Sys, typename... Args>
                    std::enable_if_t<sizeof...(Args) == 0> addRequirementsRecursive(uint32_t id) {}
            };


            /**
             * @brief A manager for created and destroyed `Entity` variables.
             * 
             * @details Handles the creation and destruction of unique `Entity` values. Also tends to each 
             *          `Entity`'s component bitmap.
             */
            struct EntityManager
            {
                /**
                 * @brief Creates an 'Entity' with a unique value.
                 * 
                 * @details Either returns and Entity with an incremented value, or recycles an Entity that 
                 *          has been deleted. Also initializes the `Entity`'s bitmap.
                 * 
                 * @return Newly created `Entity` 
                 */
                entity createEntity()
                {
                    entity entity = m_entityCount++;

                    // recycles any entities that have been destroyed
                    if(!m_removedEntities.empty())
                    {
                        entity = m_removedEntities.back();
                        m_removedEntities.pop_back();
                    }
                    // creates a new bitmap if no entities can be recycled
                    else
                    {
                        m_componentBitmaps.push_back(std::vector<bool>(ComponentManager::idCount + 1));
                    }
                    m_componentBitmaps[entity].back() = true;
                    return entity;
                }

                /**
                 * @brief Removes and recycles an 'Entity'
                 * 
                 * @details Stores the deleted `Entity` in an internal vector. Resets that `Entity`'s bitmap.
                 * 
                 * @param entity An `Entity` created by the `createEntity` function.
                 */
                void removeEntity(entity entity)
                {
                    // resets the bitmap to be recycled
                    m_componentBitmaps[entity] = std::vector<bool>(ComponentManager::idCount + 1);
                    m_removedEntities.push_back(entity);
                    m_entityCount--;
                }

                /**
                 * @brief Sets the bit in the bitmap of a certain `Entity` at a defined index.
                 * 
                 * @param entity An `Entity` created by the `createEntity` function.
                 * @param index  The index of the bit to set.
                 * @param bit    The value the bit will be set to.
                 */
                void setComponentBit(entity entity, uint32_t index, bool bit)
                {
                    m_componentBitmaps[entity][index] = bit;
                }

                /**
                 * @brief Returns the bitmap of a certain `Entity`.
                 * 
                 * @param entity An `Entity` created by the `createEntity` function.
                 * @return The component bitmap attached to the given `Entity`.
                 */
                std::vector<bool>& getBitmap(entity entity)
                {
                    return m_componentBitmaps[entity];
                }

                /**
                 * @brief Determines whether the given `Entity` has been removed or is still active.
                 * 
                 * @param entity An `Entity` created by the `createEntity` function.
                 * @return The `active` state of an `Entity`.
                 */
                bool entityActive(entity entity) const
                {
                    return m_componentBitmaps[entity].back();
                }


                bool contains(entity entity) const
                {
                    return totalEntityCount() > entity;
                }

                /**
                 * @return The number of active entities.
                 */
                entity activeEntityCount() const
                {
                    return m_entityCount;
                }

                /**
                 * @return The total number of entities (active and removed).
                 */
                entity totalEntityCount() const
                {
                    return m_entityCount + m_removedEntities.size();
                }

                private:
                    entity m_entityCount = 0;                          /** @brief The total number of active entities.*/
                    std::vector<entity> m_removedEntities;             /** @brief A list of every entity that has been removed.*/
                    std::vector<std::vector<bool>> m_componentBitmaps; /** @brief The component bitmaps corresponding to each entity.*/
            };


            EntityManager entityManager;
            ComponentManager componentManager;
            SystemManager systemManager;
    };

    template <typename T>
    inline const uint32_t ComponentType<T>::id = ecs::newComponentId<T>();

    template <typename T>
    inline uint32_t SystemType<T>::id = ecs::newSystemId<T>();


    template<typename T, typename>
    T& ecs::ComponentArray::addComponent(size_t& index, const T& component)
    {
        #ifndef ECS_DEBUG_OFF
            if(index != (size_t)-1)
            {
                ecs::error = 1;
                return *reinterpret_cast<T*>(&m_components[index]);
            }
        #endif

        size_t arraySize = m_components.size();

        // copy data from `&component` to the newly allocated space

        m_components.resize(arraySize + sizeof(T));
        object::serialize(component, m_components, arraySize);

        // save the index for the entity
        index = arraySize;

        // could use `getComponent<T>()`, this avoids unnecessary index check
        return *reinterpret_cast<T*>(&m_components[arraySize]);
    }

    template<typename T, typename>
    T ecs::ComponentArray::addComponent(size_t& index, const T& component)
    {
        #ifndef ECS_DEBUG_OFF
            if(index != (size_t)-1)
            {
                ecs::error = 1;
                return T();
            }
        #endif

        size_t arraySize = m_components.size();

        // copy data from `&component` to the newly allocated space
        size_t length = Serialization<T>::length(component);

        m_components.resize(arraySize + sizeof(size_t) + length);
        object::serialize(component, m_components, arraySize, length);

        // save the index for the entity
        index = arraySize;

        // could use `getComponent<T>()`, this avoids unnecessary index check
        return object::deserialize<T>(m_components, index);
    }


    template<typename T, typename>
    T& ecs::ComponentArray::getComponent(size_t index)
    {
        // indices set to `-1` represent uninitialized components; this triggers an error
        #ifndef ECS_DEBUG_OFF
            if(index == (size_t)-1)
            {
                ecs::error = 2;
                
                // an empty component is stored at the start of each array; this is returned when an error occurs
                return getDefaultComponent<T>();
            }
        #endif
        return object::deserialize<T>(m_components, index);
    }

    template<typename T, typename>
    T ecs::ComponentArray::getComponent(size_t index)
    {
        #ifndef ECS_DEBUG_OFF
            if(index == (size_t)-1)
            {
                ecs::error = 2;
                T();
            }
        #endif
        return object::deserialize<T>(m_components, index);
    }


    template<typename T>
    size_t ecs::ComponentArray::setComponent(size_t index, const T& update)
    {
        // indices set to `-1` represent uninitialized components; this triggers an error
        #ifndef ECS_DEBUG_OFF
            if(index == (size_t)-1)
            {
                ecs::error = 2;
                return 0;
            }
        #endif
        
        size_t length = object::length(update);
        size_t offset = object::resize<T>(length, m_components, index);
        object::serialize<T>(update, m_components, index, length);
        return offset;
    }

    template<typename T, typename... Args>
    void ecs::SystemManager::addRequirements()
    {
        uint32_t id = SystemType<T>::id;
        addRequirementsRecursive<T, Args...>(id);
    }
}

template <>
struct Serialization<std::string>
{
    static size_t length(const std::string& data)
    {        
        return data.size()*sizeof(char) + sizeof(size_t);
    }

    static size_t serialize(const std::string& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t stringLength = value.size();
        size_t sizing = sizeof(size_t);
        size_t count = 0;

        std::memcpy(&stream[index], &stringLength, sizing);
        for(size_t i = 0; i<stringLength; i++)
        {
            auto v = value[i];
            count += object::serialize<char>(v, stream, index + sizing + count);
        }
        return count + 2*sizing;
    }

    static std::string deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        size_t size = object::deserialize<size_t>(stream, index);
        size_t offset = sizeof(size_t);
        size_t count = 0;

        std::string result = "";

        for(int i=0; i<size; i++)
        {
            auto v = object::deserialize<char>(stream, index + offset + count);
            result += v;
            count += object::length<char>(v);
        }

        return result;
    }
};

template<typename T>
struct Serialization<std::vector<T>>
{
    static size_t length(const std::vector<T>& data)
    {
        size_t result = 0;
        size_t length = data.size();
        for(int i=0; i<length; i++)
        {
            result += object::length(data[i]);
        }
        
        return result + sizeof(size_t);
    }

    static size_t serialize(const std::vector<T>& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t vectorLength = value.size();
        size_t sizing = sizeof(size_t);
        size_t count = 0;

        std::memcpy(&stream[index], &vectorLength, sizing);
        for(size_t i = 0; i<vectorLength; i++)
        {
            auto variable = value[i];

            size_t length = object::length<T>(variable);
            object::serialize<T>(variable, stream, index + sizing + count, length);
            count += length;
        }
        return count + sizing;
    }

    static std::vector<T> deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        size_t size = object::deserialize<size_t>(stream, index);
        size_t offset = sizeof(size_t);
        size_t count = 0;

        std::vector<T> result = std::vector<T>(size);

        for(int i=0; i<size; i++)
        {
            result[i] = object::deserialize<T>(stream, index + offset + count);
            count += object::length<T>(result[i]);
        }

        return result;
    }
};

#endif