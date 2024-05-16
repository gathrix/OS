#ifndef __syscall_c_h__
#define __syscall_c_h__

#include "../lib/hw.h"
#include "riscv.hpp"

//ALOKACIJA
void* mem_alloc(size_t size);
int mem_free(void*);

//NITI
int thread_create(thread_t* handle,void(*start_routine)(void*),void* arg);
void thread_dispatch();
void thread_join(thread_t handle);
int thread_exit ();

//SEMAFORI
int sem_open(sem_t* handle, unsigned init);
int sem_close(sem_t handle);
int sem_wait(sem_t id);
int sem_signal(sem_t id);

//KONZOLA
char getc();
void putc(char c);



#endif