#ifndef PROJECT_BASE_SCHEDULER_H
#define PROJECT_BASE_SCHEDULER_H
#include "list.hpp"

class TCB;




class Scheduler{
private:
    static List<TCB> readyQueue;


public:
    friend class Riscv;
    //friend class SCB;

    static TCB *get();
    static void put(TCB *tcb);

};

#endif