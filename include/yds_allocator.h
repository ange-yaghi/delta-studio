#ifndef YDS_ALLOCATOR_H
#define YDS_ALLOCATOR_H

#include <cstdlib>
#include <utility>

class ysAllocator {
public:
    template <int Alignment>
    static void *BlockAllocate(int size) {
        static_assert((Alignment & (Alignment - 1)) == 0, "Alignment must be a power of 2");
        // Round up size to a multiple of Alignment
        const size_t mask = Alignment - 1;
        size = (size + mask) & ~mask;
#ifdef _WIN32
        // MSVC doesn't - and won't - support std::aligned_alloc()
        // https://learn.microsoft.com/en-us/cpp/standard-library/cstdlib?view=msvc-170#remarks-6
        return _aligned_malloc(size, Alignment);
#else
        // posix_memalign() requires the alignment is at least that of a pointer. So much for abstraction...
        if (Alignment < sizeof(void*)) {
            return malloc(size);
        } else {
            return ::aligned_alloc(Alignment, size);
        }
#endif
    }

    static void BlockFree(void *block, int alignment) {
#ifdef _WIN32
        _aligned_free(block);
#else
        ::free(block);
#endif
    }

    template <typename T_Create, int Alignment>
    static T_Create *TypeAllocate(int n = 1, bool construct = true) {
        void *block = BlockAllocate<Alignment>(sizeof(T_Create) * n);
        T_Create *typedArray = reinterpret_cast<T_Create *>(block);

        if (construct) {
            for (int i = 0; i < n; i++) {
                new(typedArray + i) T_Create;
            }
        }

        return typedArray;
    }

    template <typename T_Free>
    static void TypeFree(T_Free *data, int n = 1, bool destroy = true, int alignment = 1) {
        void *block = reinterpret_cast<void *>(data);

        if (destroy) {
            for (int i = 0; i < n; i++) {
                data[i].~T_Free();
            }
        }

        BlockFree(block, alignment);
    }
};

// Like a unique_ptr, but aligned
template <typename AllocationConfig>
class ysAlignedAllocation
{
public:
    // Workaround GCC warning about ysVector dropping attributes
    using T = typename AllocationConfig::Type;
    static constexpr int Alignment = AllocationConfig::Alignment;

public:
    ysAlignedAllocation() = default;
    ysAlignedAllocation(int count) {
        m_count = count;
        if (m_count > 0) {
            m_allocation = ysAllocator::TypeAllocate<T, Alignment>(m_count, true);
        }
    }
    ~ysAlignedAllocation() {
        clear();
    }
    ysAlignedAllocation(ysAlignedAllocation &&o) noexcept : ysAlignedAllocation() {
        operator=(std::move(o));
    }
    ysAlignedAllocation& operator=(ysAlignedAllocation &&o) noexcept {
        std::swap(m_allocation, o.m_allocation);
        std::swap(m_count, o.m_count);
        return *this;
    }

    void clear() {
        if (m_allocation != nullptr) {
            ysAllocator::TypeFree<T>(m_allocation, m_count, true, Alignment);
            m_allocation = nullptr;
        }
    }

    T *data() { return m_allocation; }
    size_t size() const { return m_count; }

    T &operator[](int idx) { return m_allocation[idx]; }

    operator bool() const { return m_allocation != nullptr; }

private:
    ysAlignedAllocation(const ysAlignedAllocation&) = delete;
    ysAlignedAllocation& operator=(const ysAlignedAllocation&) = delete;

private:
    T *m_allocation = nullptr;
    int m_count = 0;
};

#endif /* YDS_ALLOCATOR_H */
