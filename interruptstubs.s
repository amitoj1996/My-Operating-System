
.set IRQ_BASE, 0x20


.section .text

.extern _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj#16 is the length of namespace/class(InterruptManager), 15 is the length of the method HandleInterrupt 
.global _ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv

.macro HandleException num
.global _ZN4myos21hardwarecommunication16InterruptManager26HandleException\num\()Ev #c++ syntax
_ZN4myos21hardwarecommunication16InterruptManager26HandleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev #c++ syntax
_ZN4myos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C
int_bottom:
    
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    
    pushl %esp
    push (interruptnumber)
    call _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj
    #addl $5, %esp
    movl %eax, %esp
    
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa
_ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv:


    iret 
    
.data
    interruptnumber: .byte 0
