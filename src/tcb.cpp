#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/scheduler.h"
#include  "../h/syscall_cpp.hpp"

TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCnt = 0;





void TCB::contextSwitch(TCB::Context *old, TCB::Context *newContext) {

    __asm__ volatile("sd ra, 0x00(a0)");
    __asm__ volatile("sd sp, 0x08(a0)");

    __asm__ volatile("ld ra, 0x00(a1)");
    __asm__ volatile("ld sp, 0x08(a1)");

}

void TCB::yield() {
    __asm__ volatile("li a0, 0x00");
    __asm__ volatile("ecall");
}


void TCB::dispatch() {
    uint64 oldStatus = Riscv::r_sstatus();
    TCB* oldRunning = TCB::running;

    if (!oldRunning) {
        oldRunning = new TCB(nullptr, nullptr, nullptr);
    }


    if(!(oldRunning->isFinished())){
        Scheduler::put(oldRunning);
    }



    running = Scheduler::get();
    Riscv::w_sstatus(oldStatus);

    contextSwitch(&oldRunning->context, &running->context);
}


void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    yield();
}

TCB* TCB::createThread(TCB::Body body, void *arg, uint64 *st) {
    return new TCB(body,st,arg);
}

void TCB::runWrapper(void *t) {
    if(t)((Thread*)t)->run();

}
