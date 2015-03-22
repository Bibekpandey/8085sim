#ifndef PERIPHERAL_H
#define PERIPHERAL_H

class Interrupt 

{

public:

    bool  INTEa, INTRa, IBFa, STBa, OBFa, ACKa, INTEb,  IBFb, STBb, INTRb, OBFb, ACKb;

Interrupt()

{
  //high active signal
  INTEa = INTRa = IBFa = INTEb = IBFb = INTRb = ACKa = ACKb =  false;

  //low active signal
  STBa = STBb = OBFa = OBFb = true;
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
   
      Interrupt interrupt;

      void updateValues(int address,int regA, int *ioMemory);
      void insertValues(int address, int &regA, int *ioMemory);
 
};

#endif
