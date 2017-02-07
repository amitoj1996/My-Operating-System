
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
    pushl $0
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x80
int_bottom:
    #save registers
/*     pusha */
    
    #pushl %ds
/*     pushl %es */
/*     pushl %fs */
/*     pushl %gs */

    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax
    
    #call c++ handler
    pushl %esp
    push (interruptnumber)
    call _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj
    #addl $5, %esp
    movl %eax, %esp
    #restore registers
/*     popl %gs */
/*     popl %fs */
/*     popl %es */
/*     popl %ds */
/*     popa */
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx
    
    popl %esi
    popl %edi
    popl %ebp
    
    add $4, %esp
_ZN4myos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv:


    iret 
    
.data
    interruptnumber: .byte 0
