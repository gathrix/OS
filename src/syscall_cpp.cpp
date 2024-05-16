#include "../h/syscall_cpp.hpp"


void* operator new(size_t size) {
    return mem_alloc(size);
}

void operator delete(void* addr) {
    mem_free(addr);
}

Thread::Thread(void (*body)(void *), void *arg) {
    thread_create(&myHandle, body,arg);

}

Thread::~Thread() {
    delete myHandle;
}

int Thread::start() {
    thread_create(&myHandle, &TCB::runWrapper, this);
    return 0;
}

void Thread::join(thread_t handle) {
    thread_join(handle);

}

void Thread::dispatch() {
    thread_dispatch();

}

Thread::Thread() {

}

Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle,init);

}

Semaphore::~Semaphore() {
    delete myHandle;
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);

}
