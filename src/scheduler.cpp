#include "../h/scheduler.h"
#include "../h/tcb.hpp"

List<TCB> Scheduler::readyQueue;


TCB* Scheduler::get() {
    return Scheduler::readyQueue.removeFirst();
}

void Scheduler::put(TCB *tcb) {
    Scheduler::readyQueue.addLast(tcb);
}

