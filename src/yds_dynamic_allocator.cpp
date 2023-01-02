#include "../include/yds_dynamic_allocator.h"

ysDynamicAllocator::ysDynamicAllocator() : ysMemoryAllocator("DYN_ALLOCATOR") {
    m_blockPool = NULL;
    m_blocks = NULL;

    m_allocatorBlock = NULL;

    m_nBlocks = 0;

    m_maxBlocks = 0;
    m_maxSize = 0;

    m_totalAllocation = 0;

    m_data = NULL;

    m_parent = NULL;
}

ysDynamicAllocator::~ysDynamicAllocator() {
#define REMINDER "Call Destroy() before this point."

    //RaiseError(m_blockPool == NULL, "Block pool still exists after destructor called.\n" REMINDER);
    //RaiseError(m_blocks == NULL, "Blocks still exist after destructor is called.\n" REMINDER);
    //RaiseError(m_data == NULL, "Data buffer still exists after destructor is called.\n" REMINDER);

#undef REMINDER
}

void ysDynamicAllocator::SetParent(ysMemoryAllocator *parent) {
    //RaiseError(!(m_data || m_blocks || m_blockPool), "Allocator has active memory. Call Destroy() before changing the parent.");
    //RaiseError(!(m_data || m_blocks || m_blockPool), "Allocator has active memory. Call Destroy() before changing the parent.");

    m_parent = parent;
}

void *ysDynamicAllocator::AllocateBlock(int size, int numObjects) {
    // Size of total allocation with metadata pointer
    unsigned int totalSize = size + sizeof(BlockLink *);

    m_totalAllocation += totalSize;

    // Get the current allocator block, or find a new one if this one is insufficient
    BlockLink *freeBlock = m_allocatorBlock;
    if (!freeBlock->Free || !freeBlock->Valid || freeBlock->Size < totalSize) freeBlock = UpdateAllocatorBlock();
    if (!freeBlock || freeBlock->Size < totalSize) return 0;

    void *allocatedData = (void *)((char *)freeBlock->Address + sizeof(BlockLink *));
    BlockLink **metadataPointer = (BlockLink **)((char *)freeBlock->Address);

    if (freeBlock->Size == totalSize) {
        freeBlock->Size = totalSize;
        freeBlock->Free = 0;
        freeBlock->NumObjects = numObjects;

        *metadataPointer = freeBlock;

    }
    else {
        BlockLink *usedBlock = AddBlock();
        if (!usedBlock) return 0;

        usedBlock->Address = freeBlock->Address;
        usedBlock->Free = 0;
        usedBlock->Size = totalSize;
        usedBlock->NumObjects = numObjects;

        usedBlock->Next = freeBlock;
        usedBlock->Previous = freeBlock->Previous;

        if (freeBlock->Previous) freeBlock->Previous->Next = usedBlock;
        freeBlock->Previous = usedBlock;
        freeBlock->Address = (void *)((char *)freeBlock->Address + totalSize);
        freeBlock->Size -= totalSize;

        *metadataPointer = usedBlock;
    }

    return allocatedData;
}

int ysDynamicAllocator::FreeBlock(void *block) {
    BlockLink **metadataPointer = (BlockLink **)( (char *)block - sizeof(BlockLink *) );
    BlockLink *metadata = *metadataPointer;

    m_totalAllocation -= metadata->Size;

    int nObjects = metadata->NumObjects;

    metadata->Free = 1;

    BlockLink *left, *right;
    left = metadata->Previous;
    right = metadata->Next;

    bool leftMerge, rightMerge;
    leftMerge = left && left->Free;
    rightMerge = right && right->Free;

    if (!leftMerge && rightMerge) {
        if (left) left->Next = right;
        right->Previous = left;

        right->Address = ((char *)right->Address - metadata->Size);
        right->Size += metadata->Size;

        if (right->Size > m_allocatorBlock->Size) m_allocatorBlock = right;

        RemoveBlock(metadata->Index);
    }

    if (leftMerge && !rightMerge) {
        if (right) right->Previous = left;
        left->Next = right;

        left->Size += metadata->Size;

        if (left->Size > m_allocatorBlock->Size) m_allocatorBlock = left;

        RemoveBlock(metadata->Index);
    }
    else if (leftMerge && rightMerge) {
        if (right->Next) right->Next->Previous = left;
        left->Next = right->Next;

        left->Size += metadata->Size + right->Size;
        if (left->Size > m_allocatorBlock->Size) m_allocatorBlock = left;

        RemoveBlock(metadata->Index);
        RemoveBlock(right->Index);
    }

    return nObjects;
}

void ysDynamicAllocator::CreateBuffer(int size) {
    //RaiseError(m_data == NULL, "Allocator already has a buffer.");

    m_maxSize = size;

    if (!m_parent) {
        m_data = new char[size];
    }
    else {
        m_data = m_parent->AllocateBlock(size);
    }

#ifdef _DEBUG
    // To make the buffer easier to see while debugging
    memset(m_data, 0, size);
#endif
}

void ysDynamicAllocator::CreateBlocks(int maxBlocks) {
    if (!m_parent) {
        m_blockPool = new BlockLink[maxBlocks];
        m_blocks = new BlockLink *[maxBlocks];
    }
    else {
        m_blockPool = m_parent->Allocate<BlockLink>(maxBlocks);
        m_blocks = m_parent->Allocate<BlockLink *>(maxBlocks);
    }

    m_maxBlocks = maxBlocks;
    m_nBlocks = 0;

    for(int i = 0; i < maxBlocks; i++) {
        m_blocks[i] = &m_blockPool[i];

        m_blocks[i]->Address = 0;
        m_blocks[i]->Free = 1;
        m_blocks[i]->Index = i;
        m_blocks[i]->Next = 0;
        m_blocks[i]->Previous = 0;
        m_blocks[i]->Size = 0;
        m_blocks[i]->Valid = 0;
        m_blocks[i]->NumObjects = 0;
    }

    BlockLink *initialBlock = AddBlock();
    initialBlock->Free = 1;
    initialBlock->Address = m_data;
    initialBlock->Size = m_maxSize;

    m_allocatorBlock = initialBlock;
}

void ysDynamicAllocator::Destroy() {
    if (!m_parent) {
        delete [] (char *)m_data;
        delete [] m_blockPool;
        delete[] m_blocks;
    }
    else {
        m_parent->FreeBlock(m_data);
        m_parent->Free(m_blockPool);
        m_parent->Free(m_blocks);
    }

    m_data = NULL;
    m_blockPool = NULL;
    m_blocks = NULL;
}

// Block Creation/Destruction

void ysDynamicAllocator::RemoveBlock(BlockIndex index) {
    BlockLink *temp = m_blocks[index];
    m_blocks[index] = m_blocks[m_nBlocks - 1];
    m_blocks[index]->Index = index;

    m_blocks[m_nBlocks - 1] = temp;
    m_blocks[m_nBlocks - 1]->Index = m_nBlocks - 1;
    m_blocks[m_nBlocks - 1]->Valid = 0;

    m_nBlocks--;
}

BlockLink *ysDynamicAllocator::UpdateAllocatorBlock() {
    int index = -1;
    unsigned int maxSize = m_allocatorBlock->Size;

    int n = 0;
    for(int i=0; i < m_maxBlocks; i++) {
        if (m_blockPool[i].Valid) {
            if (n >= m_nBlocks) return 0;

            if (m_blockPool[i].Free && m_blockPool[i].Size > maxSize) {
                index = i;
                maxSize = m_blockPool[i].Size;
            }

            n++;
        }
    }

    if (index < 0 ) return 0;
    else return &m_blockPool[index];
}

// Error Checking

bool ysDynamicAllocator::CheckValid() {
    int totalSize=0;
        
    for(int i=0; i < m_nBlocks; i++) {
        totalSize += m_blocks[i]->Size;
    }

    if (totalSize != m_maxSize) {
        return false;
    }

    for(int i=0; i < m_nBlocks; i++) {
        if (((char *)m_blocks[i]->Address + m_blocks[i]->Size) - (char *)m_data > m_maxSize) {
            return false;
        }
    }

    return true;
}

int ysDynamicAllocator::CheckMemoryAddress(void *address) {
    if (address >= ((char *)m_data + m_maxSize)) return 1;
    if (address < m_data) return -1;
    return 0;
}
