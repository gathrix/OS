#include "../h/scb.hpp"




void SCB::putThread() {

    waiting.addLast(TCB::running);
}

TCB* SCB::getThread() {
    return waiting.removeFirst();
}

void SCB::ksem_wait() {
    saveContext(&(TCB::running->context));
    int toJump = getJump();
    if(toJump == 0){
        putThread();
        TCB::running = Scheduler::get();
        changeContext();
    }
}

void SCB::ksem_signal() {
    TCB* unblocked = getThread();
    unblocked->context.toJump = 1;
    Scheduler::put(unblocked);

}

SCB* SCB::ksem_open(unsigned int val) {
    return new SCB(val);
}

int SCB::ksem_close(SCB *sem) {
    TCB* thread = nullptr;
    while((thread = sem->getThread())){
        thread->context.waitReturn = -1;
        Scheduler::put(thread);

    }

    return MemoryAllocator::get()->kmem_free(sem);

}

void SCB::changeContext() {

    __asm__ volatile("mv ra, %[ra]"::[ra]"r"(TCB::running->context.ra));
    __asm__ volatile("mv sp, %[sp]"::[sp]"r"(TCB::running->context.sp));

}

TCB::Context* SCB::saveContext(TCB::Context *runningContext) {

    __asm__ volatile("sd ra, 0x00(a1)");
    __asm__ volatile("sd sp, 0x08(a1)");

    __asm__ volatile("mv t1, %[val]"::[val]"r" (0x00));
    __asm__ volatile("sd t1, 0x10(a1)");

    return runningContext;

}

int SCB::getJump() {
    return TCB::running->getToJump();
}



