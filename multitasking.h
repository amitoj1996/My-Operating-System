 
#ifndef __MULTITASKING_H 
#define __MULTITASKING_H

#include <gdt.h>
#include <common/types.h>

namespace myos
{
  struct CPUState
  {
    //user pushed
    myos::common::uint32_t eax;
    myos::common::uint32_t ebx;
    myos::common::uint32_t ecx;
    myos::common::uint32_t edx;
    
    myos::common::uint32_t esi;
    myos::common::uint32_t edi;
    myos::common::uint32_t ebp;
    /*
    myos::common::uint32_t gs;
    myos::common::uint32_t fs;
    myos::common::uint32_t es;
    myos::common::uint32_t ds;
    */
    myos::common::uint32_t error;
    
    //pushed by the processor
    
    myos::common::uint32_t eip;
    myos::common::uint32_t cs;
    myos::common::uint32_t eflags;
    myos::common::uint32_t esp;
    myos::common::uint32_t ss;
  } __attribute__((packed));
  class Task
  {
    friend class TaskManager;
  private:
    myos::common::uint8_t stack[4096]; // 4KB
    CPUState* cpustate;//this is the pointer to the top of the stack. As the top of the stack will contain CPU registers therefor name is CPU State
  public:
    Task(GlobalDescriptorTable *gdt, void entrypoint() );//entrypoint is the entry point for the interrupt funtion
    ~Task();
  };
  
  class TaskManager
  {
  private:
    Task* tasks[256];
    int numTasks;
    int currentTask;//will be needed for storing the CPU state of the currently executing task
  
    
  public:
    TaskManager();
    ~TaskManager();
    bool AddTask(Task* task);
    CPUState* Schedule(CPUState* cpustate);
  };
  
}
#endif