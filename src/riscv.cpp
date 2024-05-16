#include "../h/riscv.hpp"
#include "../h/memoryallocator.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.hpp"
#include "../h/printing.hpp"
void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    uint64 mask = 0x100;
   /* if(TCB::running->status & 0x100){
        __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
    }else{
        __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
    }*/
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
    mask = 0x01 << 5;
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));

    __asm__ volatile ("sret");
}

void Riscv::handleSysCallExInt(){

    __asm__ volatile("addi sp, sp, -16");
    __asm__ volatile("sd a3, 0x00(sp)");
    __asm__ volatile("sd a4, 0x08(sp)");
    uint64 scause = r_scause();
    if(scause == 0x0000000000000008UL || scause == 0x0000000000000009UL){

        uint64 volatile code;
        __asm__ volatile("mv %[val], a0" : [val] "=r"(code));
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();
        bool modeSwitched = false;

        switch(code){
            //yield:
            case 0x00: {
                mc_sip(SIP_SSIP);
                TCB* oldRunning = TCB::running;

                if (!oldRunning) {
                    oldRunning = new TCB(nullptr, nullptr, nullptr);
                }


                if(!(oldRunning->isFinished())){
                    Scheduler::put(oldRunning);
                }



                TCB::running = Scheduler::get();


                TCB::contextSwitch(&oldRunning->context, &TCB::running->context);
                break;

            }
            //mem_alloc
            case 0x01: {
                void *arg;
                __asm__ volatile("mv %[size], a1" : [size]"=r"(arg));
                size_t *size = (size_t *) arg;
                void *addr = MemoryAllocator::get()->kmem_alloc(*size);
                __asm__ volatile("mv a0, %[addr]" : : [addr]"r"(addr));

                break;
            }
            //mem_free
            case 0x02: {
                void *addrV;
                __asm__ volatile("mv %[var], a1" : [var]"=r"(addrV));
                int ret = MemoryAllocator::get()->kmem_free(addrV);
                if (ret < 0) {
                    addrV = nullptr;

                }
                __asm__ volatile("mv a0, %[addr]":: [addr]"r"(addrV));
                break;
            }

            case 0x03:{
                modeSwitched = true;
                __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(SSTATUS_SPP));
                __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(SSTATUS_SPIE));
                break;
            }

            //thread_create:
            case 0x11:{
                thread_t* handle;
                TCB::Body body;
                void* arg;
                uint64 * stack_space;
                __asm__ volatile("mv %[arg],a1" : [arg]"=r" (handle));
                __asm__ volatile("mv %[arg],a2" : [arg]"=r" (body));
                __asm__ volatile("ld %[arg],0x00(sp)" : [arg]"=r" (arg));
                __asm__ volatile("ld %[arg],0x08(sp)" : [arg]"=r" (stack_space));

                *handle = TCB::createThread(body,arg,stack_space);
                (*handle)->status = sstatus;
                break;

            }
            //thread_exit:
            case 0x12:{
                TCB::running->setFinished(true);

                int ret = mem_free(TCB::running->stack);

                if(ret == 0){
                    TCB::dispatch();
                }else{
                    __asm__ volatile("mv a0, %[addr]":: [addr]"r"(nullptr));
                }

                break;
            }

            //thread_dispatch:
            case 0x13:{
                TCB::dispatch();
                break;
            }
            //thread_join:
            case 0x14:{
                thread_t handle;
                __asm__ volatile("mv %[handle], a1": [handle]"=r" (handle));

                while(!handle->isFinished()){
                    TCB::dispatch();
                }
            }

            //sem_open:
            case 0x21:{
                sem_t* handle;
                unsigned val;
                __asm__ volatile("mv %[arg],a1" : [arg]"=r" (handle));
                __asm__ volatile("mv %[arg],a2" : [arg]"=r" (val));


                *handle = SCB::ksem_open(val);
                if (*handle != nullptr){
                    __asm__ volatile ("mv a0, %[var]": : [var]"r" (handle));
                }
                else{
                    __asm__ volatile ("mv a0, %[var]": : [var]"r" (nullptr));
                }

                break;

            }
            //sem_close
            case 0x22:{
                sem_t handle;
                __asm__ volatile("mv %[handle], a1": [handle]"=r" (handle));
                int ret = SCB::ksem_close(handle);
                if(ret == 0) {
                    __asm__ volatile("mv a0, %[rv]":: [rv]"r"(&ret));
                }else{
                    __asm__ volatile("mv a0, %[rv]":: [rv]"r"(nullptr));
                }
                break;
            }
            //sem_wait:
            case 0x23:{
                sem_t sem;

                __asm__ volatile("mv %[sem], a1": [sem]"=r" (sem));
                if(sem == nullptr){
                    TCB::running->context.waitReturn = -1;
                }
                sem->val--;
                if(sem->val < 0){
                    sem->ksem_wait();
                }


                break;
            }
            //sem_signal:
            case 0x24:{
                sem_t sem;
                __asm__ volatile("mv %[sem], a1": [sem]"=r" (sem));
                if(sem == nullptr){
                    __asm__ volatile("mv a0, %[rv]":: [rv]"r"(nullptr));
                }
                sem->val++;
                if(sem->val <= 0){
                    sem->ksem_signal();
                }
                __asm__ volatile("mv a0, %[rv]":: [rv]"r"(sem));
                break;

            }
            //getc
            case 0x41:{
                char* c = (char*) mem_alloc(sizeof(char));
                *c  = __getc();
                __asm__ volatile("mv a0, %[rv]":: [rv]"r"(c));

                break;
            }
            //putc
            case 0x42:{
                char* c;
                __asm__ volatile("mv %[par], a1": [par]"=r" (c));
                __putc(*c);

                break;
            }

        }


        __asm__ volatile("addi sp, sp, 16");


        if(!modeSwitched) {
            w_sstatus(sstatus);
        }
        w_sepc(sepc);

    }
    else{
        if(scause == 2) {


            printString("IZUZETAKK!\n");
        }else if(scause == 5){
            printString("Izuzetak upis");
        }else{
            printString("Izuzetak citanje");
        }

    }
}


void Riscv::handleTimerInt() {
    mc_sip(SIP_SSIP);
}

void Riscv::handleExternInt() {
    console_handler();
}

void* Riscv::commonFunc(int code, void *arg) {

    __asm__ volatile("ecall");
    void* ret;
    __asm__ volatile("mv %[rv], a0" : [rv] "=r" (ret));


    return ret;

}