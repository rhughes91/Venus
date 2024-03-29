#ifndef MACHINE_H
#define MACHINE_H


#include <array>
#include <bitset>
#include <memory>
#include <vector>
#include <unordered_map>

/**
 * State Machine Architecture: switches between various pieces of data based on provided conditions
 * Credit: Austin Morlan for basic infrastructure for holding parameters
 */

const int MAX_STATES = 16;

// Operation (namespace): standard operations used to create transitions in StateMachines
namespace state_op
{
    template<typename T>
    bool equals(T& one, const T& two)
    {
        return one == two;
    }

    template<typename T>
    bool not_equals(T& one, const T& two)
    {
        return one != two;
    }

    template<typename T>
    bool less(T& one, const T& two)
    {
        return one < two;
    }

    template<typename T>
    bool greater(T& one, const T& two)
    {
        return one > two;
    }

    template<typename T>
    bool less_equals(T& one, const T& two)
    {
        return one <= two;
    }

    template<typename T>
    bool greater_equals(T& one, const T& two)
    {
        return one >= two;
    }
};

// Transition (struct): contains the string labels of two StateMachine parameters and a function to compare them
template<typename T>
struct Transition
{
    std::string parameterOne, parameterTwo;
    bool(*compare)(T& one, const T& two);
};

// IParameterArray (struct): acts as the foundation for "ParameterArray" :: holds generic methods and fields
struct IParameterArray
{
    virtual void setTransition(uint16_t parent) = 0;
    virtual void handleTransition(uint16_t parent, std::bitset<MAX_STATES>& state) = 0;

    virtual ~IParameterArray() = default;
};

// ParameterArray (struct: extends IParameterArray): acts as an array for both Transitions and parameters :: handles transitions between parameters
template<typename T>
struct ParameterArray : public IParameterArray
{
    std::unordered_map<uint16_t, std::unordered_map<uint16_t, Transition<T>>> m_transitions;
    std::unordered_map<std::string, T> m_parameters;

    T& add(const std::string& key, const T& value)
    {
        return m_parameters[key] = value;
    }
    T& get(const std::string& key)
    {
        return m_parameters.at(key);
    }
    void set(const std::string& key, const T& value)
    {
        m_parameters.at(key) = value;
    }

    // creates a transition from every other state to another state
    void addTransition(const char *keyOne, bool (*compare)(T&, const T&), const char *keyTwo, uint16_t destination, std::unordered_map<uint16_t, std::bitset<MAX_STATES>>& m_validTransitions)
    {
        if(!m_parameters.count(keyOne) || !m_parameters.count(keyTwo))
        {
            // std::cout << "ERROR :: Parameters \"" << (!m_parameters.count(keyOne) ? keyOne:keyTwo) << "\" not found." << std::endl;
            return;
        }

        for(auto const &comparison : m_transitions)
        {
            if(destination == comparison.first)
                continue;
            m_transitions[comparison.first][destination] = (Transition<T>{keyOne, keyTwo, compare});
            m_validTransitions[comparison.first][destination] = 1;
        }
    }
    void addTransition(const char *keyOne, bool (*compare)(T&, const T&), const T &keyTwo, uint16_t destination, std::unordered_map<uint16_t, std::bitset<MAX_STATES>>& m_validTransitions)
    {
        std::string key = "_const" + std::to_string(std::hash<T>{}(keyTwo));
        if(!m_parameters.count(key))
            add(key, keyTwo);
        if(!m_parameters.count(keyOne))
        {
            // std::cout << "ERROR :: Parameter \"" << keyOne << "\" not found." << std::endl;
            return;
        }
        
        for(auto const &comparison : m_transitions)
        {
            if(destination == comparison.first)
                continue;
            m_transitions[comparison.first][destination] = (Transition<T>{keyOne, key, compare});
            m_validTransitions[comparison.first][destination] = 1;
        }
    }
    
    // creates a transition from one other state to another state
    void addTransition(const char *keyOne, bool (*compare)(T&, const T&), const char *keyTwo, const std::pair<uint16_t, uint16_t>& path, std::unordered_map<uint16_t, std::bitset<MAX_STATES>>& m_validTransitions)
    {
        if(!m_parameters.count(keyOne) || !m_parameters.count(keyTwo))
        {
            // std::cout << "ERROR :: Parameters \"" << (!m_parameters.count(keyOne) ? keyOne:keyTwo) << "\" not found." << std::endl;
            return;
        }
        
        m_transitions[path.first][path.second] = (Transition<T>{keyOne, keyTwo, compare});
        m_validTransitions[path.first][path.second] = 1;
    }
    void addTransition(const char *keyOne, bool (*compare)(T&, const T&), const T &keyTwo, const std::pair<uint16_t, uint16_t>& path, std::unordered_map<uint16_t, std::bitset<MAX_STATES>>& m_validTransitions)
    {
        std::string key = "_const" + std::to_string(std::hash<T>{}(keyTwo));
        if(!m_parameters.count(key))
            add(key, keyTwo);
        if(!m_parameters.count(keyOne))
        {
            // std::cout << "ERROR :: Parameter \"" << keyOne << "\" not found." << std::endl;
            return;
        }

        m_transitions[path.first][path.second] = (Transition<T>{keyOne, key, compare});
        m_validTransitions[path.first][path.second] = 1;
    }

    // initializes the transition for a 'parent' state
    void setTransition(uint16_t parent) override
    {
        m_transitions[parent] = std::unordered_map<uint16_t, Transition<T>>();
    }
    
    // checks whether the 'parent' state can successfully transition :: multiple transition conditions acts as an AND gate
    void handleTransition(uint16_t parent, std::bitset<MAX_STATES>& state) override
    {
        std::bitset<MAX_STATES> validTransitions;
        validTransitions.set();
        for(auto const& comparison : m_transitions.at(parent))
        {
            T& test = m_parameters[comparison.second.parameterOne];
            if(comparison.second.compare(test, m_parameters.at(comparison.second.parameterTwo)))
            {
                continue;
            }
            validTransitions[comparison.first] = 0;
        }
        state &= validTransitions;
    }
};

// StateMachine (struct): allows for pieces of data to switch between each other based on provided parameters and Transitions
template<typename T>
struct StateMachine
{
    uint16_t currentVariable = 0;

    // void operator=(const StateMachine &rhs)
    // {
    //     currentVariable = rhs.currentVariable;
    //     m_variableID = rhs.m_variableID;
    //     m_variableIndices = rhs.m_variableIndices;
    //     m_variableStatuses = rhs.m_variableStatuses;
    //     m_variables = rhs.m_variables;
    //     m_validTransitions = rhs.m_validTransitions;

    //     added = rhs.added;
    //     changed = rhs.changed;
    //     updated = rhs.updated;
    //     removed = rhs.removed;

    //     m_parameterArrays = rhs.m_parameterArrays;
    // }
    
    bool equals(const std::string& key)
    {
        return m_variableIndices[key] == currentVariable;
    }

    T& add(const T& variable, const std::string str)
    {
        std::cout << this << " : " << m_variableIndices.size() << '\n';
        m_variableIndices[str] = m_variableID;
        std::cout << "end\n";
        m_variableStatuses[m_variableID] = 1;
        m_variables[m_variableID] = variable;

        for(auto const &parameterArray :  m_parameterArrays)
        {
            parameterArray.second -> setTransition(m_variableID);
        }
        
        m_variableID++;
        added(m_variables[m_variableID-1]);
        return m_variables[m_variableID-1];
    }
    T& get(const std::string& variableName)
    {
        return m_variables.at(m_variableIndices.at(variableName));
    }
    T& transition()
    {
        std::bitset<MAX_STATES> result = m_variableStatuses;
        result[currentVariable] = 0;
        result &= m_validTransitions[currentVariable];
        for(auto const &parameterArray :  m_parameterArrays)
        {
            parameterArray.second -> handleTransition(currentVariable, result);
        }

        for(int i=0; i<m_variableID; i++)
        {
            if(result[i])
            {
                if(changed(m_variables[i], (m_variables[currentVariable])))
                {
                    currentVariable = i;
                    break;
                }
            }
        }
        return m_variables[currentVariable];
    }
    void update()
    {
        for(int i=0; i<m_variableID; i++)
        {
            updated(m_variables[i]);
        }
    }
    uint16_t getID() const
    {
        return m_variableID;
    }

    void onAdd(void (*event)(T&))
    {
        added = event;
    }
    void onChange(bool (*event)(T&, T&))
    {
        changed = event;
    }
    void onUpdate(void (*event)(T&))
    {
        updated = event;
    }
    void onRemove(void (*event)(T&))
    {
        added = event;
    }

    template<typename V>
    V& addParameter(const std::string &key, const V &value = V())
    {
        if(!m_parameterArrays.count(typeid(V).name()))
        {
            m_parameterArrays[typeid(V).name()] = std::make_shared<ParameterArray<V>>();
            for(auto ID : m_variableIndices)
            {
                getParameterArray<V>() -> setTransition(ID.second);
            }
        }
        return(getParameterArray<V>() -> add(key, value));
    }

    template<typename V>
    void setParameter(const std::string &key, const V &value)
    {
        getParameterArray<V>() -> set(key, value);
    }

    template<typename V>
    V& getParameter(const std::string &key)
    {
        return getParameterArray<V>() -> get(key);
    }

    template<typename V>
    void addTransition(const char *keyOne, bool (*compare)(V&, const V&), const char *keyTwo, std::string destination)
    {
        if(!m_variableIndices.count(destination))
        {
            // std::cout << "ERROR :: Variable \"" << destination << "\" not found." << std::endl;
            return;
        }
        getParameterArray<V>() -> addTransition(keyOne, compare, keyTwo, m_variableIndices[destination], m_validTransitions);
    }

    template<typename V>
    void addTransition(const char *keyOne, bool (*compare)(V&, const V&), const char *keyTwo, std::pair<std::string, std::string> path)
    {
        if(!m_variableIndices.count(path.first) || !m_variableIndices.count(path.second))
        {
            // std::cout << "ERROR :: Variable \"" << (!m_variableIndices.count(path.first) ? path.first:path.second) << "\" not found." << std::endl;
            return;
        }
        getParameterArray<V>() -> addTransition(keyOne, compare, keyTwo, {m_variableIndices[path.first], m_variableIndices[path.second]}, m_validTransitions);
    }

    template<typename V>
    void addTransition(const char *keyOne, bool (*compare)(V&, const V&), const V &keyTwo, std::string destination)
    {
        if(!m_variableIndices.count(destination))
        {
            // std::cout << "ERROR :: Variable \"" << destination << "\" not found." << std::endl;
            return;
        }
        getParameterArray<V>() -> addTransition(keyOne, compare, keyTwo, m_variableIndices[destination], m_validTransitions);
    }
    
    template<typename V>
    void addTransition(const char *keyOne, bool (*compare)(V&, const V&), const V &keyTwo, std::pair<std::string, std::string> path)
    {
        if(!m_variableIndices.count(path.first) || !m_variableIndices.count(path.second))
        {
            // std::cout << "ERROR :: Variable \"" << (!m_variableIndices.count(path.first) ? path.first:path.second) << "\" not found." << std::endl;
            return;
        }
        getParameterArray<V>() -> addTransition(keyOne, compare, keyTwo, std::pair{m_variableIndices[path.first], m_variableIndices[path.second]}, m_validTransitions);
    }

    private:
        uint16_t m_variableID = 0;
        std::unordered_map<std::string, uint16_t> m_variableIndices;
        
        std::bitset<MAX_STATES> m_variableStatuses;
        std::array<T, MAX_STATES> m_variables;

        std::unordered_map<uint16_t, std::bitset<MAX_STATES>> m_validTransitions;
        std::unordered_map<std::string, std::shared_ptr<IParameterArray>> m_parameterArrays;

        template<typename V>
        std::shared_ptr<ParameterArray<V>> getParameterArray()
        {
            return std::static_pointer_cast<ParameterArray<V>>(m_parameterArrays.at(typeid(V).name()));
        }

        void (*added)(T&) = [](T& varOne){};
        bool (*changed)(T&, T&) = [](T& varOne, T& varTwo){return true;};
        void (*updated)(T&) = [](T& varOne){};
        void (*removed)(T&) = [](T& varOne){};
};

// machine (namespace): holds standard StateMachine transition methods
namespace machine
{
    template<typename T>
    bool recursiveChange(StateMachine<T>& machine, StateMachine<T>& na)
    {
        machine.update();
        return true;
    }
};

#endif