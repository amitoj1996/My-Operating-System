 
#ifndef __MYOS__DRIVERS__DRIVER_H
#define __MYOS__DRIVERS__DRIVER_H


namespace myos 
{
  namespace drivers
  {
  class Driver
  {
  public:
    Driver();
    ~Driver();
    
    virtual void Activate();//for the activation of hardware
    virtual int Reset();//because when we start our computer, we don't know what state the device is in so we reset the hardware to its default state 
    //the int return type will have a value to tell us about how long we need to wait for safe boot sequence to occur
    virtual void Deactivate();
  };
  
  class DriverManager
  {
   public:
    Driver* drivers[255];
    int numDrivers;
    
 
    DriverManager();
    ~DriverManager();
    void AddDriver(Driver*);
    
    void ActivateAll();
    
    
  };
  }
}
#endif