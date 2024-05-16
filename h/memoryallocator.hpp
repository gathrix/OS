#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

struct memDesc{
    void* addr;
    size_t  size;
    memDesc* next;
};
class MemoryAllocator{
private:
    MemoryAllocator() {}
    ~MemoryAllocator(){}
    static memDesc* allocatedHead;
    static memDesc* freeMemHead;
    int tryToJoin(memDesc* node);

    static MemoryAllocator* instance;

public:




    static MemoryAllocator* get();
    void* kmem_alloc(size_t size);
    int kmem_free(void* addr);


};

#endif
