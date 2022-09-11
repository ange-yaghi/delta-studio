#pragma once

#include <array>

template<typename Subclass, typename... Args> struct ysRegisterSubclass;

template<typename Enum, typename AbstractClass, typename... Args>
class ysSubclassRegistry {
    using ConstructorFn = AbstractClass*(Args...);
    static constexpr size_t num_subclasses = static_cast<size_t>(Enum::MAX_ENUM) - 1;

    std::array<ConstructorFn*, num_subclasses> m_constructors;

    template<typename Subclass, typename... _Args>
    friend struct ysRegisterSubclass;

    constexpr void RegisterSubclass(const Enum key, ConstructorFn* fn) {
        m_constructors[static_cast<size_t>(key)] = fn;
    }

public:
    constexpr ysSubclassRegistry() {
        m_constructors.fill(nullptr);
    }

    AbstractClass *New(const Enum key, Args&&... args) const {
        return m_constructors[static_cast<size_t>(key)](std::forward<Args>(args)...);
    }

    bool Has(const Enum key) const {
        size_t index = static_cast<size_t>(key);
        return (index < num_subclasses && m_constructors[index]);
    }
};

// use ctor magic to easily register a subclass
template<typename Subclass, typename... Args>
struct ysRegisterSubclass {
    template <typename AbstractClass>
    static AbstractClass* New(Args&&... args)  {
        return new Subclass(std::forward<Args>(args)...);
    }

    constexpr ysRegisterSubclass() noexcept {
        Subclass::s_registry.RegisterSubclass(Subclass::SubclassID, &New);
    }
};
