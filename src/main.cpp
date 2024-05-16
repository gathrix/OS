#include "../h/riscv.hpp"
#include "../lib/hw.h"
#include "../lib/console.h"
#include "../h/syscall_c.hpp"
#include "../h/userMain.hpp"
#include "../lib/mem.h"
#include "../h/printing.hpp"


void wrapperForUserMain(void* arg){
   userMain();
}







void main(){

    Riscv::w_stvec((uint64) (&Riscv::supervisorInt)+1);






    TCB* userA;
    thread_create(&userA, wrapperForUserMain, nullptr);


    thread_join(userA);


    /*TCB* userB;
    thread_create(&userB, wrapperForUserB, nullptr);
    printString("B created\n");
    TCB* userC;
    thread_create(&userC, wrapperForUserC, nullptr);
    printString("C created\n");*/


  /*  while(!(userA->isFinished())){
        thread_dispatch();
    }*/
   // thread_join(userA);
    //thread_join(userB);
    //thread_join(userC);

    /*thread_t user2;
    thread_create(&user2,wrapperForUser, nullptr);*/
   //
   //thread_dispatch();
    //thread_join(user);




}