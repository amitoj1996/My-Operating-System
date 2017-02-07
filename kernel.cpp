#include <common/types.h>
#include <gdt.h>
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>
#include <hardwarecommunication/pci.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/driver.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>
#include <filesystem/msdospart.h>
#include <drivers/ata.h>
#include <syscalls.h>
#include <multitasking.h>
#include <gui/widget.h>
using namespace myos;
using namespace myos::common;
using namespace myos::gui;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;
using namespace myos::filesystem; 
void printf(char* str)
{
        static uint16_t* VideoMemory = (uint16_t*)0xb8000;
        
        static uint8_t x=0, y=0;
        for(int32_t i=0;str[i] != '\0'; ++i)
        {
            switch(str[i])
            {
                case '\n':
                   y++;
                   x=0;
                   break;
                default:    
                VideoMemory[80*y + x] = (VideoMemory[80*y + x] & 0xFF00) | str[i];
                x++;
            }
            if(x >= 80)
            {
                y++;
                x=0;
            }
            
            if(y>=25)
            {
                for(y=0; y<25; y++)
                    for(x=0;x<80;x++)
                        VideoMemory[80*y + x] = (VideoMemory[80*y + x] & 0xFF00) | ' ';
                x=0;
                y=0;
                
            }
        }
}

void printfHex(uint8_t key)
{
  char* foo = "00";
      char* hex = "0123456789ABCDEF";
      foo[0] = hex[(key>>4) & 0xF];
      foo[1] = hex[key & 0xF];
      printf(foo);
}


class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
  void OnKeyDown(char c)
  {
    char* foo= " ";
    foo[0] = c;
    
    printf(foo);
  }
};

class MouseToConsole : public MouseEventHandler
{
  int8_t x,y;
public:
  MouseToConsole()
  {
    x = 40;
    y = 12;
    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
    | (VideoMemory[80*y+x] & 0xF000) >> 4
    | (VideoMemory[80*y+x] & 0x00FF);// this is used for flipping the colours as higher nibble of the higher byte stores foreground color and lower nibble stores the  background color
  }
  void OnMouseMove(int xoffset, int yoffset)
  {
	  static uint16_t* VideoMemory = (uint16_t*)0xb8000;
	  VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
	  | (VideoMemory[80*y+x] & 0xF000) >> 4
	  | (VideoMemory[80*y+x] & 0x00FF);
	  x += xoffset;
	  if(x >= 80) x = 79;
	  if(x < 0) x = 0;
	  y -= yoffset;
	  if(y >= 25) y = 24;
	  if(y < 0) y = 0;
	  VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
	  | (VideoMemory[80*y+x] & 0xF000) >> 4
	  | (VideoMemory[80*y+x] & 0x00FF);
	   }
};


void sysprintf(char* str)
{
  asm("int $0x80" : : "a" (4), "b" (str));
}
void taskA()
{
  
    sysprintf("A");
}

void taskB()
{
  
    sysprintf("B");
}
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors ; i != &end_ctors; i++)
    (*i)();
}
    

extern "C" void kernelMain(void* multiboot_structure, uint32_t magicnumber)
{
    printf("Hello World! --- Amitoj Singh :-)\n");
    
    GlobalDescriptorTable gdt;
    
    uint32_t* memupper = (uint32_t*)(((size_t)multiboot_structure) + 8);
    size_t heap = 10*1024*1024;
    MemoryManager memoryManager(heap, (*memupper)*1024 - heap - 10*1024);//the size is the entire size of the ram minus the 10 mb that is given to the os and minus the 10kb
    
    
    printf("heap: 0x");
    printfHex((heap >>24) & 0xFF);
    printfHex((heap >>16) & 0xFF);
    printfHex((heap >>8) & 0xFF);
    printfHex((heap) & 0xFF);
    
    void* allocated = memoryManager.malloc(1024);
    
    printf("\nallocated: 0x");
    printfHex(((size_t)allocated >>24) & 0xFF);
    printfHex(((size_t)allocated >>16) & 0xFF);
    printfHex(((size_t)allocated >>8) & 0xFF);
    printfHex((size_t)allocated & 0xFF);
    printf("\n");
    
    
    
    TaskManager taskManager;
    
    Task task1(&gdt, taskA);
    Task task2(&gdt, taskB);
    taskManager.AddTask(&task1);
    taskManager.AddTask(&task2);
    
    InterruptManager interrupts(&gdt, &taskManager);
    SyscallHandler syscall(&interrupts, 0x80);
     //VideoGraphicsArray vga;
    
     //Desktop desktop(320,200,0x00,0x00,0xA8);
    printf("Inititalizing Hardware, Stage 1\n");
    DriverManager drvManager;
     PrintfKeyboardEventHandler kbhandler;
       KeyboardDriver keyboard(&interrupts, &kbhandler);
       //KeyboardDriver keyboard(&interrupts, &desktop);
      drvManager.AddDriver(&keyboard);
      
       MouseToConsole mousehandler;
       MouseDriver mouse(&interrupts, &mousehandler);
       //MouseDriver mouse(&interrupts, &desktop);
      drvManager.AddDriver(&mouse);
      
      PeripheralComponentInterconnectController PCIController;
      PCIController.SelectDrivers(&drvManager, &interrupts);
      
      
      
       printf("Inititalizing Hardware, Stage 2\n");
      drvManager.ActivateAll();
       printf("Inititalizing Hardware, Stage 3\n");
   
       //first//interrupt 14
       AdvancedTechnologyAttachment ata0m(0x1F0, true);//master
       printf("ATA Primary Master : ");
       ata0m.Identify();
       
       AdvancedTechnologyAttachment ata0s(0x1F0, true);//slave
       printf("ATA Primary Slave : ");
       ata0s.Identify();
       
       MSDOSPartitionTable::ReadPartitions(&ata0s);
       
       
       //char* atabuffer = "AmitojSingh";
       //ata0s.Write28(0,(uint8_t*)atabuffer,11);
       //ata0s.Flush();
       //ata0s.Read28(0,(uint8_t*)atabuffer,11);
       //second//interupt 15
       AdvancedTechnologyAttachment ata1m(0x170, true);
       AdvancedTechnologyAttachment ata1s(0x170, false);
       
       //third : 0x1E8
       //fourth : 0x168
       
       interrupts.Activate();
       /*
      vga.SetMode(320,200,8);
      Window win1(&desktop, 10,10,10,20,0xA8,0x00,0x00);
      desktop.AddChild(&win1);
      Window win2(&desktop, 40,15,30,30,0x00,0xA8,0x00);
      desktop.AddChild(&win2);   
   */
    
    while(1)
    {
        //desktop.Draw(&vga);
    }
}
 