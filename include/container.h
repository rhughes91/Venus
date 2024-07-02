#ifndef CONTAINER_H
#define CONTAINER_H

#include <algorithm>
#include <vector>

// operators (namespace): used for data structures that may require sorting
// namespace operators
// {
//     template<typename T>
//     bool greater(const T& a, const T& b)
//     {
//         return a > b;
//     }

//     template<typename T>
//     bool less(const T& a, const T& b)
//     {
//         return a < b;
//     }

//     template<typename T>
//     bool equals(const T& a, const T& b)
//     {
//         return a == b;
//     }

//     template<typename T>
//     bool nequals(const T& a, const T& b)
//     {
//         return a != b;
//     }
// }

// namespace ctr
// {
//     template<typename T>
//     struct priority_node
//     {
//         uint32_t priority;
//         T value = value;

//         priority_node(uint32_t p_priority, const T& p_value) : priority(p_priority), value(p_value) {}

//         bool operator==(const priority_node& value)
//         {
//             return priority == value.priority;
//         }
//         bool operator<(const priority_node& value)
//         {
//             return priority < value.priority;
//         }
//         bool operator>(const priority_node& value)
//         {
//             return priority > value.priority;
//         }
//         bool operator!=(const priority_node& value)
//         {
//             return !(priority == value.priority);
//         }
//         bool operator<=(const priority_node& value)
//         {
//             return !(priority > value.priority);
//         }
//         bool operator>=(const priority_node& value)
//         {
//             return !(priority < value.priority);
//         }
        
//     };

//     template<typename T>
//     struct priority_queue
//     {
//         bool empty() const
//         {
//             return queue.empty();
//         }
//         size_t size() const
//         {
//             return queue.size();
//         }
//         T &top()
//         {
//             return queue.front().value;
//         }
//         T& push(const T& value, uint32_t priority = 0)
//         {
//             queue.push_back(priority_node<T>(priority, value));
//             T& result = queue[queue.size()-1].value;
//             std::sort(queue.begin(), queue.end());

//             return result;
//         }
//         void pop()
//         {
//             queue.pop_back();
//         }
//         typename std::vector<priority_node<T>>::reverse_iterator begin()
//         {
//             return queue.rbegin();
//         }
//         typename std::vector<priority_node<T>>::reverse_iterator end()
//         {
//             return queue.rend();
//         }

//         private:
//             std::vector<priority_node<T>> queue;
//     };
// }

#endif