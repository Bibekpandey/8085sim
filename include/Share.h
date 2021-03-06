#ifndef SHARE_H
#define SHARE_H

#include <Memory.h>

class Share_Resource 
{
   public:
  
  int ioMemory[256];
  
  Memory systemMemory;

  int flag;
  int regA;
  int regB;
  int regC;
  int regD;
  int regE;
  int regH;
  int regL;
  bool interrupt;

  Share_Resource()
  {

    ioMemory[64] = 0;
    ioMemory[65] = 0;
    ioMemory[66] = 0;
    ioMemory[67] = 0;

    flag = 0;
    regA = 0;
    regB = 0;
    regC = 0;
    regD = 0;
    regE = 0;
    regH = 0;
    regL = 0;
  }

};

#endif
