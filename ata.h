#ifndef __MYOS__DRIVERS__ATA_H
#define __MYOS__DRIVERS__ATA_H

#include <hardwarecommunication/port.h>
#include <common/types.h>
namespace myos 
  {
    namespace drivers 
    {
      class AdvancedTechnologyAttachment
      {
      protected:
	myos::hardwarecommunication::Port16Bit dataPort; //for sending and recieving data to/from hdd
	myos::hardwarecommunication::Port8Bit errorPort; //for reading error messages
	myos::hardwarecommunication::Port8Bit sectorCountPort; //tells the hdd about how many sectors we want to read 
	myos::hardwarecommunication::Port8Bit lbaLowPort; //lba(Logical Block Address) is transmitted through these 3 ports
	myos::hardwarecommunication::Port8Bit lbaMidPort;
	myos::hardwarecommunication::Port8Bit lbaHiPort;
	myos::hardwarecommunication::Port8Bit devicePort;
	myos::hardwarecommunication::Port8Bit commandPort;
	myos::hardwarecommunication::Port8Bit controlPort;
	
	bool master;
	myos::common::uint16_t bytesPerSector; //will be used to define the sector size 
      public:
	AdvancedTechnologyAttachment(myos::common::uint16_t portBase, bool master);
	~AdvancedTechnologyAttachment();
	
	void Identify();// to identify which kind of hard drive is attached to the system
	void Read28(myos::common::uint32_t sector, myos::common::uint8_t* data, int count);
	void Write28(myos::common::uint32_t sector, myos::common::uint8_t* data, int count);
	void Flush();//used to flush the cache of the hdd because harddrive first stores it in the cache so if you flush the buffer only then the hdd will actually write the data
	
      };
    }
  }






#endif
