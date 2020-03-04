#include "../include/yds_memory_base.h"

ysMemoryAllocator::ysMemoryAllocator() : ysObject("MEMORY_ALLOCATOR") {
    /* void */
}

ysMemoryAllocator::ysMemoryAllocator(const char *typeID) : ysObject(typeID) {
    /* void */
}

ysMemoryAllocator::~ysMemoryAllocator() {
    /* void */
}
