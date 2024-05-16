#include "../h/memoryallocator.hpp"

MemoryAllocator* MemoryAllocator:: instance = nullptr;
memDesc* MemoryAllocator::freeMemHead = nullptr;
memDesc* MemoryAllocator::allocatedHead = nullptr;
MemoryAllocator* MemoryAllocator::get() {
    if(instance == nullptr){
        instance = (MemoryAllocator*)HEAP_START_ADDR;
        size_t blocks = sizeof(MemoryAllocator) / MEM_BLOCK_SIZE + (sizeof(MemoryAllocator) % MEM_BLOCK_SIZE == 0 ? 0: 1);
        freeMemHead = (memDesc*)((char*)HEAP_START_ADDR + blocks*MEM_BLOCK_SIZE);
        freeMemHead->size = (size_t)((((char*)HEAP_END_ADDR - (char*)freeMemHead)) / MEM_BLOCK_SIZE + (((char*)HEAP_END_ADDR - (char*)freeMemHead) % MEM_BLOCK_SIZE == 0 ? 0:1));
        freeMemHead->next = nullptr;

    }
    return instance;
}
void* MemoryAllocator::kmem_alloc(size_t size){

    if(size <= 0 ) return nullptr;
    memDesc* cur = freeMemHead;
    memDesc* prev = nullptr;
    size_t oldS = size;
    void* ret = nullptr;
    while(cur){
        // velicina u blokovima za smestanje memDesc cvora
        size_t  blocksForNode = sizeof(memDesc) / MEM_BLOCK_SIZE + (sizeof(memDesc) % MEM_BLOCK_SIZE == 0 ? 0 : 1);

        if(cur->size  >= size + blocksForNode){
            size += blocksForNode;
            ret = cur;
            //Prelancavanje free mem liste
            if(prev != nullptr){
                if(cur->size == size){
                    prev->next = cur->next;
                }
                else{
                    size_t nSize = cur->size - size;
                    prev->next = (memDesc*)((char*)cur + size*MEM_BLOCK_SIZE);
                    memDesc* temp = cur->next;
                    cur = prev->next;
                    cur->next = temp;
                    cur->size = nSize;

                }
            }else{
              if(cur->size == size){
                  freeMemHead = freeMemHead->next;

              }else{
                  size_t nSize = cur->size - size;
                  cur = (memDesc*)((char*)cur + size*MEM_BLOCK_SIZE);
                  cur->next = freeMemHead->next;
                  cur->size = nSize;
                  freeMemHead = cur;
              }
            }
            //Kreiranje novog zauzetog cvora, smesta se na kraju segmenta
            memDesc* newTaken = (memDesc*)((char*)ret + oldS*MEM_BLOCK_SIZE);
            newTaken->next = nullptr;
            newTaken->size = oldS; // Velicina predvidjena za podatke bez header!!!!
            newTaken->addr =ret; // adresa od koje pocinje!

            if(!allocatedHead){
                allocatedHead = newTaken;
            }else{
                memDesc* prev = nullptr;
                memDesc* cur = allocatedHead;
                while(cur){
                    if(newTaken->addr < cur->addr){
                        break;
                    }
                    prev = cur;
                    cur = cur->next;

                }
                if(!prev){
                    newTaken->next = cur;
                    allocatedHead = newTaken;
                }else{
                    newTaken->next = cur;
                    prev->next = newTaken;

                }
            }



            break;
        }



        prev = cur;
        cur = cur->next;
    }

    if(ret == nullptr ){
        freeMemHead = nullptr;
    }
    return ret;









}

int MemoryAllocator::kmem_free(void *addr){
    if(addr > HEAP_END_ADDR || addr<HEAP_START_ADDR) return -1;

    memDesc* cur = nullptr;
    if(!freeMemHead || addr < (char*)freeMemHead){
        cur = nullptr;
    }else{
        for(cur = freeMemHead; cur && (char*)cur->next < addr; cur= cur->next );
    }


    memDesc* out = allocatedHead;
    memDesc* prev = nullptr;
    while(out){
        if(out->addr == addr){
            break;
        }

        prev = out;
        out = out->next;
    }
    if(out == nullptr) {return -2;}

    size_t size = out->size + ((sizeof(memDesc) / MEM_BLOCK_SIZE) + (sizeof(memDesc)%MEM_BLOCK_SIZE==0?0 : 1));
    //Izabcivanje out iz liste zauzetih

    if(!prev){
        allocatedHead = allocatedHead->next;
    }else {
        prev->next = out->next;
    }
    //Kreiranje novog slobodnog segmenta
    memDesc* newFree = (memDesc*)addr;
    newFree->next = nullptr;
    newFree->size = size;

    if(cur == nullptr){
            newFree->next = freeMemHead;
            freeMemHead = newFree;
    }else{
        newFree->next = cur->next;
        cur->next = newFree;

    }
    tryToJoin(newFree);
    tryToJoin(cur);


    return 0;

}

int MemoryAllocator::tryToJoin(memDesc *node) {
    if(!node) return 0;
    if(node->next && ((char*)node + node->size) == (char*)node->next){
        node->size += node->next->size;
        node->next = node->next->next;
        return 1;
    }else{
        return 0;
    }
}