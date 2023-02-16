#pragma once

#include "Vector.h"
#include "Utility.h"

namespace TK {

// Max Binary Heap

template<typename T>
class PriorityQueue {
public:
    PriorityQueue() = default;
    ~PriorityQueue() = default;

    unsigned size() const { return m_items.size(); }
    bool is_empty() const { return size() == 0; }

    void push(T&& element)
    {
        unsigned i = size();
        m_elements.emplace_back(forward<T>(element));
        sift_up(i);
    }

    T pop()
    {
        swap(m_elements.front(), m_elements.back());
        T result = m_elements.back();
        m_elements.pop_back();
        sift_down(0);
        return result;
    }

    const T& top() const { return m_elements[0]; }
    T& top() { return m_elements[0]; }

private:
    ALWAYS_INLINE unsigned parent_of(unsigned i) const { return i / 2; }
    ALWAYS_INLINE unsigned left_child_of(unsigned i) const  { return i * 2 + 1; }
    ALWAYS_INLINE unsigned right_child_of(unsigned i) const { return i * 2 + 2; }

    void sift_up(unsigned i)
    {
        while (i) {
            unsigned parent = parent_of(i);
            if (m_elements[parent] > m_elements[i])
                break;

            swap(m_elements[i], m_elements[parent]);
            i = parent;
        }
    }

    void sift_down(unsigned i)
    {
        while (left_child_of(i) < size()) {
            unsigned left_child = left_child_of(i);
            unsigned right_child = right_child_of(i);

            unsigned max_child = left_child;
            if (right_child < size() && m_elements[right_child] > m_elements[left_child])
                max_child = right_child;

            if (m_elements[max_child] < m_elements[i])
                break;

            swap(m_elements[i], m_elements[max_child]);
            i = max_child;
        }
    }

private:
    Vector<T> m_elements { };
};

}
