#ifndef YDS_ALLOCATOR_H
#define YDS_ALLOCATOR_H

#include <cstdlib>

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
        return ::aligned_alloc(Alignment, size);
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

#endif /* YDS_ALLOCATOR_H */
