#include <drivers/driver.h>

using namespace myos::drivers;

  
    Driver::Driver()
    {
      
      
    }
    Driver::~Driver()
    {
      
    }
    
    void Driver::Activate()//for the activation of hardware
    {
      
    }
    int Driver::Reset()//because when we start our computer, we don't know what state the device is in so we reset the hardware to its default state 
    //the int return type will have a value to tell us about how long we need to wait for safe boot sequence to occur
    {
      return 0;
      
    }
    void Driver::Deactivate()
    {
      
      
    }
  
  
    DriverManager::DriverManager()
    {
      
      
    }
    
    
    DriverManager::~DriverManager()
    {
      numDrivers = 0;
      
      
    }
    
    void DriverManager::AddDriver(Driver* drv)
    {
      drivers[numDrivers] = drv;
      numDrivers++;
      
    }
    
    void DriverManager::ActivateAll()
    {
      for(int i=0; i<numDrivers; i++)
	drivers[i]->Activate();
    }
