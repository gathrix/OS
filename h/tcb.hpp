#ifndef PROJECT_BASE_TCB_HPP
#define PROJECT_BASE_TCB_HPP
#include "../lib/hw.h"
#include "scheduler.h"
class TCB{
public:
    friend class Riscv;
    friend class Thread;
    friend class SCB;


    ~TCB(){

        delete stack;

    }
    bool isFinished() const {return finished;}
    void setFinished(bool value) {this->finished = value;}

    bool isAsleep() const {return sleeping;}
    void setSleep(bool value) {sleeping = value;}

    uint64 getTimeSlice() const { return timeSlice;}
    void setTimeSlice(time_t t) {timeSlice = t;}

    using Body = void(*)(void*);

    static void yield();
    static void dispatch();
    static TCB* createThread(Body body, void* arg, uint64* stack);

    static TCB* running;

    int getToJump()const{
        return context.toJump;
    }
    int getWaitReturn() const{
        return context.waitReturn;
    }

   static void runWrapper(void* t);



private:
    void* arg;
    Body body;
    uint64 *stack;
    TCB(Body b, uint64* stack, void* a ):body(b), stack(stack), context({(uint64) &threadWrapper,
                                                                                         (uint64)stack,
                                                                         (int)0,
                                                                         (int)0
                                                                                        }){
        this->arg = a;
        this->finished = false;
        if(body != nullptr){
            Scheduler::put(this);
        }

    }

    struct Context{
        uint64 ra; // 0x00
        uint64 sp; // 0x08
        int toJump; // 0x10
        int waitReturn; // 0x18


    };

    static void threadWrapper();

    static void contextSwitch(Context* old, Context* newContext);
    static uint64 timeSliceCnt;
    bool finished;
    uint64 status;
    Context context;
    bool sleeping =  false;
    uint64 timeSlice = DEFAULT_TIME_SLICE;
};




#endif
