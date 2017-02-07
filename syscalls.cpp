#include <syscalls.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;

      SyscallHandler::SyscallHandler(InterruptManager* interruptManager,uint8_t InterruptNumber)
      : InterruptHandler(interruptManager, InterruptNumber + 0x20)
      {
	
      }
      SyscallHandler::~SyscallHandler()
      {
	
      }
      
      void printf(char*);
      
      uint32_t myos::SyscallHandler::HandleInterrupt(uint32_t esp)
      {
	CPUState* cpu = (CPUState*)esp;
	
	switch(cpu->eax)
	{
	  case 4:
	    printf((char*)cpu->ebx);
	    break;
	  default:
	    break;
	  
	  
	}
	
	
	return esp;
      }
