#ifndef YDS_ALLOCATOR_H
#define YDS_ALLOCATOR_H

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX & iOS (Darwin)
    #include <stdlib.h>
#elif defined(_WIN64)
    #include <malloc.h>
#endif

class ysAllocator {
public:
    template <int Alignment>
    static void *BlockAllocate(int size) {
        #if defined(__APPLE__) && defined(__MACH__)
            return ::aligned_alloc(size, Alignment);
        #elif
            return ::_aligned_malloc(size, Alignment);
        #endif
    }

    template <>
    static void *BlockAllocate<1>(int size) {
        return ::malloc(size);
    }

    static void BlockFree(void *block, int alignment) {
        if (alignment != 1) {
            #if defined(__APPLE__) && defined(__MACH__)
                free(block);
            #elif
                ::_aligned_free(block);
            #endif
        }
        else {
            ::free(block);
        }
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
