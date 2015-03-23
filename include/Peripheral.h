#ifndef PERIPHERAL_H
#define PERIPHERAL_H

class Handshaking
{

public:
    bool INTEa, INTEb;

    Handshaking()
    {
        INTEa = false;
        INTEb = false;
    }

};

class Peripheral
{

    private:
              //to get the type of address
       int returnType(int address);
           
       //to know i/o mode or bsr mode
       bool whichMode(int value);


     public:
   
       Handshaking handshaking;
      
      void updateValues(int address,int regA, int *ioMemory);
      void insertValues(int address, int &regA, int *ioMemory);
 
};

#endif
