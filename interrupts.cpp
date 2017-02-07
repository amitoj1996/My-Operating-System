#include <hardwarecommunication/interrupts.h>
using namespace myos;
using namespace myos::common;
using namespace myos::hardwarecommunication;



void printf(char* str);
void printfHex(uint8_t);

InterruptHandler::InterruptHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
{
  this->InterruptNumber = InterruptNumber;
  this->interruptManager = interruptManager;
  interruptManager->handlers[InterruptNumber] = this;
}
InterruptHandler::~InterruptHandler()
{
  if(interruptManager->handlers[InterruptNumber] == this)
    interruptManager->handlers[InterruptNumber] = 0;
}
 
uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
  return esp;
}



InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::ActiveInterruptManager = 0; 
            void InterruptManager::SetInterruptDescriptorTableEntry(
            uint8_t interruptNumber, 
            uint16_t gdt_codeSegmentSelectorOffset,
            void (*handler)(),
            uint8_t DescriptorPrivilegeLevel,
            uint8_t DescriptorType)
        {
            
            
            const uint8_t IDT_DESC_PRESENT = 0x80;
            interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
            interruptDescriptorTable[interruptNumber].handlerAddressHighBits = ((uint32_t)handler >> 16) & 0xFFFF;
            interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = gdt_codeSegmentSelectorOffset;
            interruptDescriptorTable[interruptNumber].reserved = 0;
            interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | (DescriptorPrivilegeLevel&3) << 5; 
        }
        
        
InterruptManager::InterruptManager(GlobalDescriptorTable* gdt, TaskManager *taskManager):picMasterCommand(0x20),picMasterData(0x21), picSlaveCommand(0xA0), picSlaveData(0xA1)
{
    this->taskManager = taskManager;
    uint32_t CodeSegment = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;
    
    for(uint16_t i = 255; i > 0; --i)
    {
	handlers[i]=0;
        SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0,IDT_INTERRUPT_GATE);
    }
    SetInterruptDescriptorTableEntry(0, CodeSegment, &IgnoreInterruptRequest, 0,IDT_INTERRUPT_GATE);
    handlers[0]=0;
    SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0,IDT_INTERRUPT_GATE);
     SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0,IDT_INTERRUPT_GATE);
     SetInterruptDescriptorTableEntry(0x2C, CodeSegment, &HandleInterruptRequest0x0C, 0,IDT_INTERRUPT_GATE);
     SetInterruptDescriptorTableEntry(0x80, CodeSegment, &HandleInterruptRequest0x80, 0, IDT_INTERRUPT_GATE);
     
     picMasterCommand.Write(0x11);
     picSlaveCommand.Write(0x11);
     
     picMasterData.Write(0x20);
     picSlaveData.Write(0x28);
     
     picMasterData.Write(0x04);
     picSlaveData.Write(0x02);
     
     picMasterData.Write(0x01);
     picSlaveData.Write(0x01);
     
     picMasterData.Write(0x00);
     picSlaveData.Write(0x00);
     
     
     InterruptDescriptorTablePointer idt;
     idt.size = 256 * sizeof(GateDescriptor) - 1;
     idt.base = (uint32_t)interruptDescriptorTable;
     asm volatile("lidt %0" : : "m" (idt));
}
      
InterruptManager::~InterruptManager()
{
    Deactivate();
}



void InterruptManager::Activate()
{
  if(ActiveInterruptManager != 0)
    ActiveInterruptManager->Deactivate();
  ActiveInterruptManager = this;
  asm("sti");
    
}


void InterruptManager::Deactivate()
{
  if(ActiveInterruptManager == this)
  {
    
    ActiveInterruptManager = 0;
    asm("cli");
  } 
}

uint32_t InterruptManager::HandleInterrupt(uint8_t interrupt, uint32_t esp)
{
    if(ActiveInterruptManager != 0)
    {
     return ActiveInterruptManager->DoHandleInterrupt(interrupt, esp);
    }
    
    return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interrupt, uint32_t esp)
{
    if(handlers[interrupt] != 0)
    {
      esp = handlers[interrupt]->HandleInterrupt(esp); 
    }
    
      else if(interrupt != 0x20)
    {
      printf("UNHANDLED INTERRUPT 0x");
      printfHex(interrupt);
      
    }
    if(interrupt == 0x20)
    {
      esp = (uint32_t)taskManager->Schedule((CPUState*)esp);
    }
    
    if(0x20 <=interrupt && interrupt < (0x20) + 16)
    {
      picMasterCommand.Write(0x20);//to tell the pic that wehave acknowledged the interrupt send us the other one
      if(0x28<=interrupt)
	picSlaveCommand.Write(0x20);
    }
    
    return esp;
}