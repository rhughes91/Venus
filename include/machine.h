#ifndef MACHINE_H
#define MACHINE_H

#include "serialize.h"
#include <unordered_set>

namespace object
{
    using state = uint32_t;
    using transition = uint8_t;

    struct parameter
    {
        size_t type;
        uint32_t id;

        bool operator==(const parameter& comparison) const
        {
            return type == comparison.type && id == comparison.id; 
        }
    };

    enum conditions
    {
        LESS, GREATER, LESS_OR_EQUAL, GREATER_OR_EQUAL, NOT_EQUAL, EQUAL, LAST = EQUAL
    };

    struct Transition
    {
        state target;
        transition trans;

        parameter one, two;

        bool valid = false;

        Transition() {}
        Transition(state target_, transition trans_, parameter one_, parameter two_)
        {
            target = target_;
            trans = trans_;
            one = one_;
            two = two_;
        }

        bool ready(std::vector<uint8_t>& parameters)
        {
            switch(trans)
            {
                case conditions::LESS:

                break;
                case conditions::GREATER:

                break;
                case conditions::LESS_OR_EQUAL:

                break;
                case conditions::GREATER_OR_EQUAL:

                break;
                case conditions::EQUAL:

                break;
            }
            return true;
        }

        template<typename T>
        static const uint32_t newId()
        {
            uint32_t index = idCount;
            idCount++;
            return index;
        }

        private:
            inline static uint32_t idCount = 0;
    };

    struct ParameterLoc
    {
        state location;
        uint32_t index;

        ParameterLoc(state location__ = -1, uint32_t index__ = -1)
        {
            location = location__;
            index = index__;
        }
    };

    struct ParameterArray
    {
        std::vector<uint8_t> data;
        std::vector<std::vector<ParameterLoc>> states;

        ParameterArray()
        {
            data = std::vector<uint8_t>();
            states = std::vector<std::vector<ParameterLoc>>();
        }
    };

    template<typename T>
    struct ParameterType
    {
        static const uint32_t id; /** @brief The unique ID assigned to a type.*/
    };

    template<typename T>
    struct state_machine
    {
        std::vector<T> states;
        state currentState = 0;

        std::vector<std::vector<Transition>> transitions;
        std::vector<ParameterArray> parameters;

        state_machine()
        {
            states = std::vector<T>();
            transitions = std::vector<std::vector<Transition>>();
        }


        state createState(const T& newState)
        {
            size_t size = states.size();
            states.push_back(newState);
            transitions.push_back(std::vector<Transition>());
            return size;
        }

        void setState(state s, const T& newState)
        {
            states[s] = newState;
        }

        void setState(const T& newState)
        {
            states[currentState] = newState;
        }

        T getCurrent()
        {
            if(states.size() == 0)
                return T();
            return states[currentState];
        }

        template<typename S>
        parameter createParameter(const S& param = S())
        {
            uint32_t id = ParameterType<S>::id;
            size_t size = parameters.size();
            while(id >= size)
            {
                parameters.push_back(ParameterArray());
                size++;
            }
            std::vector<uint8_t>& stream = parameters[id].data;
            parameters[id].states.push_back(std::vector<ParameterLoc>());
            size_t len = stream.size();
            stream.resize(len + sizeof(S));
            object::serialize(param, stream, len);
            return {id, (uint32_t)(len / sizeof(S))};
        }

        template<typename S>
        S getParameter(parameter identifier)
        {
            uint32_t id = ParameterType<S>::id;
            std::vector<uint8_t>& stream = parameters[id].data;
            return object::deserialize<S>(stream, identifier.id * sizeof(S));
        }

        template<typename S>
        void setParameter(parameter identifier, const S& param)
        {
            uint32_t id = ParameterType<S>::id;
            std::vector<uint8_t>& stream = parameters[id].data;
            object::serialize(param, stream, identifier.id * sizeof(S));

            std::vector<ParameterLoc>& locs = parameters[id].states[identifier.id];
            bool updated = false;
            for(const ParameterLoc& loc : locs)
            {
                Transition& t = transitions[loc.location][loc.index];
                updated = t.valid;
                t.valid = parametersEqual<S>(t.trans, t.one, t.two);
                updated = (updated != t.valid);
            }

            if(updated)
                update();
        }

        template<typename S>
        void createTransition(state initial, state target, parameter one, parameter two, transition trans)
        {
            Transition t;
            if(initial < states.size())
            {
                ParameterLoc loc = ParameterLoc(initial, transitions[initial].size());
                parameters[one.type].states[one.id].push_back(loc);
                parameters[two.type].states[two.id].push_back(loc);
                transitions[initial].push_back(t = Transition(target, trans, one, two));
            
                if(parametersEqual<S>(t.trans, t.one, t.two))
                {
                    currentState = t.target;
                }
            }
        }

        template<typename S>
        parameter createTransition(state initial, state target, parameter one, const S& two, transition trans)
        {
            parameter t = createParameter<S>(two);
            createTransition<S>(initial, target, one, t, trans);
            return t;
        }

        private:
            void update()
            {
                std::unordered_set<state> lastStates = std::unordered_set<state>();
                lastStates.insert(currentState);
                bool switched = false;
                do
                {
                    for(const auto& trans : transitions[currentState])
                    {
                        if(switched = trans.valid && lastStates.find(trans.target) == lastStates.end())
                        {
                            currentState = trans.target;
                            lastStates.insert(currentState);
                        }
                    }
                }
                while(switched);
            }

            template<typename S>
            bool parametersEqual(transition t, parameter one, parameter two)
            {
                S first = getParameter<S>(one);
                S second = getParameter<S>(two);
                switch(t)
                {
                    case LESS:
                        return first < second;
                    break;
                    case GREATER:
                        return first > second;
                    break;
                    case LESS_OR_EQUAL:
                        return first <= second;
                    break;
                    case GREATER_OR_EQUAL:
                        return first >= second;
                    break;
                    case NOT_EQUAL:
                        return first != second;
                    break;
                    case EQUAL:
                        return first == second;
                    break;
                }
                return false;
            }
    };

    template <typename T>
    inline const uint32_t ParameterType<T>::id = Transition::newId<T>();
}

template<>
struct Serialization<object::ParameterArray>
{
    static size_t length(const object::ParameterArray& data)
    {
        return
            object::length(data.data) +
            object::length(data.states);
    }

    static size_t serialize(const object::ParameterArray& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.data, stream, index + count);
        count += object::serialize(value.states, stream, index + count);

        return count;
    }

    static object::ParameterArray deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        object::ParameterArray result = object::ParameterArray();
        size_t count = 0;

        result.data = object::deserialize<std::vector<uint8_t>>(stream, index + count);
        count += object::length(result.data);

        result.states = object::deserialize<std::vector<std::vector<object::ParameterLoc>>>(stream, index + count);
        count += object::length(result.states);

        return result;
    }
};

template<typename T>
struct Serialization<object::state_machine<T>>
{
    static size_t length(const object::state_machine<T>& data)
    {
        return
            object::length(data.states) +
            object::length(data.currentState) +
            object::length(data.transitions) +
            object::length(data.parameters);
    }

    static size_t serialize(const object::state_machine<T>& value, std::vector<uint8_t>& stream, size_t index)
    {
        size_t count = 0;

        count += object::serialize(value.states, stream, index + count);
        count += object::serialize(value.currentState, stream, index + count);
        count += object::serialize(value.transitions, stream, index + count);
        count += object::serialize(value.parameters, stream, index + count);

        return count;
    }

    static object::state_machine<T> deserialize(std::vector<uint8_t>& stream, size_t index)
    {
        object::state_machine<T> result = object::state_machine<T>();
        size_t count = 0;

        result.states = object::deserialize<std::vector<T>>(stream, index + count);
        count += object::length(result.states);

        result.currentState = object::deserialize<object::state>(stream, index + count);
        count += object::length(result.currentState);

        result.transitions = object::deserialize<std::vector<std::vector<object::Transition>>>(stream, index + count);
        count += object::length(result.transitions);

        result.parameters = object::deserialize<std::vector<object::ParameterArray>>(stream, index + count);
        count += object::length(result.parameters);

        return result;
    }
};

#endif