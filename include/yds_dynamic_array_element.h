#ifndef YDS_DYNAMIC_ARRAY_ELEMENT_H
#define YDS_DYNAMIC_ARRAY_ELEMENT_H

class ysDynamicArrayElement
{
public:
    ysDynamicArrayElement()
        : m_index(-1)
        , m_alignment(0)
    { }

    ~ysDynamicArrayElement() = default;

    inline int GetIndex() const {
        return m_index;
    }

    inline void SetIndex(int index) {
        m_index = index;
    }

    inline int GetAlignment() const {
        return m_alignment;
    }

    inline void SetAlignment(int alignment) {
        m_alignment = alignment;
    }

protected:
    int m_index;
    int m_alignment;
};

#endif // YDS_DYNAMIC_ARRAY_H