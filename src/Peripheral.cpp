#include <Peripheral.h>
#include <Helper.h>
#include <iostream>

//identifies the address belong to which component
int Peripheral::returnType(int address)
{
   return address%16;
}

//to know the bsr or i/o mode of control register
bool Peripheral::whichMode(int value)
{
  if(value>>7)
        return true;
  else
        return false;
}

//transfer value from accumulator to i/o memory
void Peripheral::updateValues(int address, int regA, int *ioMemory)
{


   int type = returnType(address);

   switch(type)
   {
      
      //port A
      case 0:
          
         if(whichMode(ioMemory[address+3]))
         {

            if((ioMemory[address+3]>>4 & 1) ==0)
              {
                 ioMemory[address] = regA;
              }
         }
             break;

      //port B
      case 1:
         
         if(whichMode(ioMemory[address+2]))
         {
 
            if((ioMemory[address+2]>>1 & 1) ==0)
              {
                 ioMemory[address] = regA;
              }                
         }


             break;

      //port C
      case 2:

         if(whichMode(ioMemory[address+1]))
         {
  
            if(((ioMemory[address+1]>>3 & 1) ==0) and (ioMemory[address+1] & 1) ==0)

              {
                 ioMemory[address] = regA;
              }                   
         }

             break;

      //control register
      case 3:
 
            //i/o mode

            if(regA>>7)
            {  
               
                ioMemory[address] =regA;
            
            } 
            else
             //bsr mode
               {
               
                //update the control register value
                 ioMemory[address ] = regA;
            
                //get which bit of port C is changed
                 int getalue = (regA>>1) & 7;

                //set the port c corresponding bit
                 if(regA & 1)
                     ioMemory[address-1] |= 1<<getalue;
                 else
                     ioMemory[address-1] &= ~(1<<getalue);

               }
           
            break;

      default:
         std::cout<<"Here comes the default"<<std::endl;
         break;
   }

}

//transfer values from i/o memory to accumulator
void Peripheral::insertValues(int address, int &regA, int *ioMemory)
{

    int type = returnType(address);

   switch(type)
   {
      
      //port A
      case 0:
          
           if(whichMode(ioMemory[address+3]))
        {


            if((ioMemory[address+3]>>4 & 1) == 1)
              {
             
                regA = ioMemory[address];
              
              }
         }
             break;

      //port B
      case 1:
         
         if(whichMode(ioMemory[address+2]))
         {
 
            if((ioMemory[address+2]>>1 & 1) == 1)
              {
                 regA = ioMemory[address];
              }                
         }


             break;

      //port C
      case 2:

         if(whichMode(ioMemory[address+1]))
         {
  
            if(((ioMemory[address+1]>>3 & 1) ==1) and (ioMemory[address+1] & 1) ==1)

              {
                 regA = ioMemory[address];
              }                   
         }

             break;

      //control register
      case 3:
        
            break;

      default:
         std::cout<<"Here comes the default"<<std::endl;
         break;
   }
}
