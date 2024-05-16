#ifndef PROJECT_BASE_SCB_HPP
#define PROJECT_BASE_SCB_HPP

#include "list.hpp"
#include "tcb.hpp"
#include "memoryallocator.hpp"
//Semaphore Control Block
class SCB{
private:
    int val;
    List<TCB> waiting;
    TCB::Context* saveContext(TCB::Context* runningContext);
    void changeContext();
    SCB(unsigned val):val(val) {}
    void putThread();
    TCB* getThread();
    int getJump();
public:
    friend class Semaphore;
    friend class Riscv;
   // friend class SystemConsole;
    void ksem_wait();
    void ksem_signal();
    static SCB* ksem_open(unsigned val);
    static int ksem_close(SCB* sem);
    int getValue(){
        return val;
    }


};

#endif