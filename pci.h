#ifndef __MYOS_HARDWARECOMMUNICATION__PCI_H
#define __MYOS_HARDWARECOMMUNICATION__PCI_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardwarecommunication/port.h>
#include <gdt.h> 
#include <memorymanagement.h>
#include <hardwarecommunication/interrupts.h>



  namespace myos
  {
    namespace hardwarecommunication
    {
      
      
      
      enum BaseAddressRegisterType
      {
	MemoryMapping = 0,
	InputOutput = 1
      };
      
      
      class BaseAddressRegister
      {
      public:
	bool prefetchable;
	myos::common::uint8_t* address;
	myos::common::uint32_t size;
	BaseAddressRegisterType type;
      };
  class PeripheralComponentInterconnectDeviceDescriptor //this class will be used to store the data that we get back that is the device id, subclass id etc
  
  {
  public:
    myos::common::uint32_t portBase;
    myos::common::uint32_t interrupt;
    
    myos::common::uint16_t bus;
    myos::common::uint16_t device;
    myos::common::uint16_t function;
    
    myos::common::uint16_t vendor_id;
    myos::common::uint16_t device_id;
    
    myos::common::uint8_t class_id ;
    myos::common::uint8_t subclass_id;
    myos::common::uint8_t interface_id;
    
    myos::common:: uint8_t revision;
    PeripheralComponentInterconnectDeviceDescriptor();
    ~PeripheralComponentInterconnectDeviceDescriptor();
  };





  class PeripheralComponentInterconnectController
  {
    myos::hardwarecommunication::Port32Bit dataPort;
    myos::hardwarecommunication::Port32Bit commandPort;
  public:
    PeripheralComponentInterconnectController();
    ~PeripheralComponentInterconnectController();
    
    myos::common::uint32_t Read(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function, myos::common::uint32_t registeroffset);  
    //the register offset provides the offset required for acquiring the vendorID, device ID etc as they are at particular offsets in the memory location of the device 
    void Write(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function, myos::common::uint32_t registeroffset, myos::common::uint32_t value);
    bool DeviceHasFunctions(myos::common::uint16_t bus, myos::common::uint16_t device);
    
    void SelectDrivers(myos::drivers::DriverManager* driverManager, myos::hardwarecommunication::InterruptManager* interrupt);//this will select the driver based upon the vendor id , device id , class id that we will get from read 
    myos::drivers::Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor dev, myos::hardwarecommunication::InterruptManager* interrupt);
    PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function);
    BaseAddressRegister GetBaseAddressRegister(myos::common::uint16_t bus, myos::common::uint16_t device, myos::common::uint16_t function,myos::common::uint16_t bar);
    
  };
    }
  }
  







#endif