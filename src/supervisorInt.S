.extern _ZN5Riscv9externIntEv
.extern _ZN5Riscv12sysCallExIntEv
.extern _ZN5Riscv8timerIntEv
.align 4
.global _ZN5Riscv13supervisorIntEv
.type _ZN5Riscv13supervisorIntEv, @function

_ZN5Riscv13supervisorIntEv:

    j _ZN5Riscv12sysCallExIntEv //0 - ovde dolazi za ecall i izuzetke, a na ovo + br prekida skace na neku narednu liniju
    j _ZN5Riscv8timerIntEv //1 - ovde skace za tajmer
    li x0, 0 //2
    li x0, 0 //3
    li x0, 0//4
    li x0, 0 //5
    li x0, 0 //6
    li x0, 0 //7
    li x0, 0 //8
    j _ZN5Riscv9externIntEv //9 ovde skace za konzolu
    sret




