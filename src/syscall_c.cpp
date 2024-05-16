#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"

void* mem_alloc(size_t size){

   size_t  sizeB = (size / MEM_BLOCK_SIZE) + (size % MEM_BLOCK_SIZE ==0? 0 : 1 );
   Riscv::commonFunc(0x01, &sizeB);
   void* addr;
   __asm__ volatile("mv %[rv], a0" : [rv] "=r" (addr));

    return addr;
}

int mem_free(void* addr){
    void* ret = Riscv::commonFunc(0x02, addr);
    if(ret != nullptr){
        return 0;
    }else{
        return -1;
    }

}


void thread_dispatch(){
    Riscv::commonFunc(0x13,nullptr);
}

int thread_create(
        thread_t* handle,
        void(*start_routine)(void*),
        void* arg
        ){
    __asm__ volatile("addi sp, sp, -24");
    __asm__ volatile("sd a0, 0x00(sp)");
    __asm__ volatile("sd a1, 0x08(sp)");
    __asm__ volatile("sd a2, 0x10(sp)");

    __asm__ volatile ("mv a4, %[arg]": :[arg] "r" ((uint64*) mem_alloc(DEFAULT_STACK_SIZE * sizeof(uint64)) + DEFAULT_STACK_SIZE));
    __asm__ volatile ("mv a0, %[code]" :: [code]"r" (0x11));
    __asm__ volatile("ld a1, 0x00(sp)");
    __asm__ volatile("ld a2, 0x08(sp)");
    __asm__ volatile("ld a3, 0x10(sp)");
    __asm__ volatile("addi sp, sp, 24");

    __asm__ volatile("ecall");



    if(*handle != nullptr){
        return 0;
    }else{
        return -1;
    }



}

int thread_exit (){
    Riscv::commonFunc(0x12,nullptr);

    // Ako se vratimo ovde, bice neuspesno jer radimo dispatch ukoliko smo uspesno ugasili nit!!!
    return -1;
}

void thread_join(thread_t handle){
    Riscv::commonFunc(0x14, handle);
}

int sem_open(sem_t* handle, unsigned val){
    //u a0 ide kod
    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a0, 0x00(sp)"); // cuvamo handle na stek
    __asm__ volatile("sd a1, 0x08(sp)"); // cuvamo val na stek


    __asm__ volatile("mv a0, %[code]" : : [code]"r" (0x21));
    __asm__ volatile("ld a1, 0x00(sp)"); // a1 = handle
    __asm__ volatile("ld a2, 0x08(sp)"); // a2 = val


    __asm__ volatile("addi sp, sp, 16");
    __asm__ volatile("ecall");

    void* retVal;
    __asm__ volatile ("mv %[par], a0": [par]"=r" (retVal));
    if (retVal != nullptr)
        return 0;
    else{
        return -1;
    }

}

int sem_close(sem_t handle){
    Riscv::commonFunc(0x22, handle);
    void* ret;
    __asm__ volatile("mv %[rv], a0" : [rv] "=r" (ret));
    if(ret != nullptr) {
        return 0;
    }else{
        return -1;
    }



}
int sem_wait(sem_t id){

    Riscv::commonFunc(0x23, id);
    int ret = TCB::running->getWaitReturn();
    return ret;

}

int sem_signal(sem_t id){
    Riscv::commonFunc(0x24,id);
    void* ret;
    __asm__ volatile("mv %[rv], a0": [rv]"=r"(ret));
    if(ret != nullptr){
        return 0;
    }else{
        return -1;
    }
}

void putc(char c){
    Riscv::commonFunc(0x42, &c);
}

char getc(){
    Riscv::commonFunc(0x41,nullptr);
    char* ret;

    __asm__ volatile("mv %[rv], a0" : [rv]"=r" (ret));

    return *ret;
}
