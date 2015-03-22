# pragma once
#include <map>
#include <functional>
#include <Memory.h>
#include <Helper.h>
#include <Types.h>
#include <newParser.h>
#include <Peripheral.h>
#include <Share.h>
//typedef std::function<void(Argument, Argument)> func; // argument from Types.h

class Signal
{

public:

  bool value;
  bool status;
  
   Signal()
   {
    value = false;
    status = false;
   }
};

class Pin
{
  
   public:
  
   Signal RST_7_5, RST_6_5, RST_5_5, TRAP;
   bool interruptEnableFlipFlop;

   Pin()
   {
    interruptEnableFlipFlop = false;
   }

};


class Processor
{
public:
    typedef void(Processor::*f)(Argument, Argument);
    enum { SIGN=7, ZERO=6, AUX_CARRY=4, PARITY=2, CARRY=0}; // the bits of flag register

    Processor();
    ~Processor(){}

//Microprocessor pins configuration
     Pin pin;   

//I/o memory
     int ioMemory[256] = {0};

     Peripheral peripheral;

    void Run();
    void PrintMemory(int a, int b);
    void Initialize(NewParser*);
    bool Execute(); // executes the instruction pointed by the PC, returns true if further execution, else false if rst5 encountered

    void Stackpush(int a); // helper to push to stack
    int Stackpop(); // helper to pop from stack

    void SetFlags(int &reg); // set flags a/c register input
    void SetZero(int reg);
    void SetParity(int reg);
    void SetAuxCarry(int reg);// not written yet
    void SetSign(int reg);
    void SetCarry(int reg);

    void PrintFlags();

    void PrintRegisters()
    {
        std::cout << "A: "<<psw[0] <<"    B: " << bc[0]<<
        "    C: " << bc[1]<<
        "    D: " << de[0]<<
        "    E: " << de[1]<<
        "    H: " << hl[0]<<
        "    L: " << hl[1]<<
        "    SP lsb: " << sp[0]<<
        "    SP msb: " << sp[1]<< "    PC: "<<pc<<std::endl<<std::endl;
    }

    // memory
    Memory m_memory;

private:
    int sp[2], pc, bc[2], de[2], hl[2], psw[2];
    std::map<std::string, int*> R; // stores map of strings and registers
    std::map<std::string, int*> RP; // stores map of strings and reg pairs
    // map functions
    std::map<std::string, f> Command;
    // map opcodes and instructions
    std::map<int, Instruction> opcodeToInstr;

    // pc_increment
    int pc_incr;


    // our functions, for all the commands

    //EI ********************
    void ei(Argument a, Argument b)
    {
   
       pin.interruptEnableFlipFlop = true;
       pc+=pc_incr;
 
    }

    //DI ********************
    void di(Argument a, Argument b)
    {

       pin.interruptEnableFlipFlop = false;
       pc+=pc_incr;
    }

    //SIM *********************
    void sim(Argument a, Argument b)
    {

        if( psw[0]>>4 & 1)
            pin.RST_7_5.value = false;

        if(psw[0]>>3 & 1)
        {
           if(!(psw[0]>>2 & 1))
            pin.RST_7_5.status = true;
 
           if(!(psw[0]>>1 & 1))
            pin.RST_6_5.status = true;

           if(!(psw[0] & 1))
            pin.RST_5_5.status = true;
        
        }
         
        pc += pc_incr;
    }

    void rim(Argument a, Argument b)
    {

        //it seems very difficult to implement so fuckoff
        pc+= pc_incr;
    }
    // MOV ******************
    void mov(Argument a, Argument b) // both a and b must be registers
    {
        pc+=pc_incr;

        int memlocation = hl[0]*256 + hl[1]; // in case M is used

        if(a.type == MEMORY and b.type==REGISTER)
            m_memory.SetValue(memlocation, *(R[b.value]));
        else if(a.type==REGISTER and b.type==MEMORY)
            *(R[a.value]) = m_memory[memlocation];
        else if (a.type==REGISTER and b.type==REGISTER)
            *(R[a.value]) = *(R[b.value]);
    }

    // MVI *******************
    void mvi(Argument a, Argument b) // a is register and b is value
    {
        pc+=pc_incr;

        int memlocation = hl[0]*256 + hl[1]; // in case M is used

        if(a.type==REGISTER)
            *(R[a.value]) = Helper::ToDec(b.value);
        else if(a.type==MEMORY) 
            m_memory.SetValue(memlocation, Helper::ToDec(b.value)); 
    }

    // LXI *******************
    void lxi(Argument a, Argument b)
    {
        pc+=pc_incr;
        if(a.type!=REG_PAIR)
            throw std::string("argument 1 for lxi is not reg pair");
        if(b.type!=DOUBLE)
            throw std::string("argument 2 for lxi is not double");

        int doublevalue = Helper::ToDec(b.value);
        std::cout << "vlaue: "<< b.value<< std::endl;
        RP[a.value][0] = doublevalue/256;
        RP[a.value][1] = doublevalue%256;
    }

    // JMP ********************
    void jmp(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // jump address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        pc_incr = 0;
        pc = memlocation;
    }

    // CALL ********************
    void call(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // ing address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        // return address is 3 bytes after the current memory location
        int retlocation = pc+3;

        pc_incr = 0;

        pc = memlocation;

        Stackpush(retlocation); // push the return location to stack

    }

    // RET ********************
    void ret(Argument a, Argument b)
    {
        pc = Stackpop();
    }

    // PUSH *******************
    void push(Argument a, Argument b)
    {
        pc+= pc_incr;
        // a is reg_pair and b is NOP
        int push_value = *(RP[a.value]+1) + *(RP[a.value]+0)*256;
        Stackpush(push_value);
    }

    // POP **********************
    void pop(Argument a, Argument b)
    {
        pc+= pc_incr;
        // a is reg_pair and b is NOP
        int pop_value = Stackpop();
        *(RP[a.value] + 0) = pop_value/256;
        *(RP[a.value] + 1) = pop_value%256;
    }

    // ADD ********************
    void add(Argument a, Argument b)
    {
        pc+=pc_incr;
        int addvalue = 0; // value to be added to A, reg or mem
        // a is register/memory, b is NOP

        if(a.type==MEMORY)
            addvalue = m_memory[hl[0]*256 * hl[1]];
        else if (a.type==REGISTER)
            addvalue = *(R[a.value]);

        int oldvalue = psw[0]; // for aux carry check
        
        // now perform addition
        psw[0]+= addvalue;

        // check for auxillary carry, using the old value
        if((addvalue%16 + oldvalue%16) > 15)
            psw[1] |= 1<<AUX_CARRY;
        else
            psw[1] &= (~(1<<AUX_CARRY)&0xff);

        // set other flags
        SetFlags(psw[0]); // set flags may not come in use
        // because different instructions affect different flags
        // but setFlags affects all flags
    }

    // LDA *********************
    void lda(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is double, b is NOP
        int address = Helper::ToDec(a.value);
        psw[0] = m_memory[address];
    }

    // LDAX *******************
    void ldax(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is reg_pair, b is NOP
        if(a.type!=REG_PAIR)
            throw "not a register pair in ldax";

        int address = *(RP[a.value]+0) * 256 + *(RP[a.value]+1);
        psw[0] = m_memory[address];
    }

    // LHLD ***********************
    void lhld(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is double, b is NOP
        int address = Helper::ToDec(a.value);
        hl[0] = m_memory[address+1]; // H contains data of next address
        hl[1] = m_memory[address];
    }

    // STA **********************
    void sta(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is double, b is nop
        int address = Helper::ToDec(a.value);
        m_memory.SetValue(address, psw[0]);
    }

    // STAX *******************
    void stax(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is reg_pair, b is NOP
        int address = *(RP[a.value]+0) * 256 + *(RP[a.value]+1);
        m_memory.SetValue(address, psw[0]);
    }

    // SHLD ******************
    void shld(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is double, b is NOP
        int address = Helper::ToDec(a.value);
        m_memory.SetValue(address+1, hl[0]);
        m_memory.SetValue(address, hl[1]);
    }

    // XCHG *******************
    void xchg(Argument a, Argument b)
    {
        pc+=pc_incr;

        Helper::swap(hl[0], de[0]);
        Helper::swap(hl[1], de[1]);
    }

    // SPHL *******************
    void sphl(Argument a, Argument b)
    {
        pc+=pc_incr;
        sp[0] = hl[1]; // content of l
        sp[1] = hl[0]; // content of h
    }

    // PCHL ******************
    void pchl(Argument a, Argument b)
    {
        pc+=pc_incr;
        pc = hl[0]*256 + hl[1];
    }

    // XTHL ******************
    void xthl(Argument a, Argument b)
    {
        int address = sp[1]*256 + sp[0];

        int t = m_memory[address];
        m_memory.SetValue(address, hl[1]);
        hl[1] = t;

        t = m_memory[address+1];
        m_memory.SetValue(address+1, hl[0]);
        hl[0] = t;
    }

    // OUT *******************
    void out(Argument a, Argument b)
    {
    }

    // IN **********************
    void in(Argument a, Argument b)
    {
    }
    
    // ADC ********************
    void adc(Argument a, Argument b)
    {
        pc+=pc_incr;
        int addvalue = 0; // value to be added to A, reg or mem
        // a is register/memory, b is NOP

        if(a.type==MEMORY)
            addvalue = m_memory[hl[0]*256 * hl[1]];
        else if (a.type==REGISTER)
            addvalue = *(R[a.value]);

        int oldvalue = psw[0]; // for aux carry check
        int carry = psw[1]&1<<CARRY;// carry value
        
        // now perform addition
        psw[0]+= addvalue+carry;

        // check for auxillary carry, using the old value
        if((addvalue%16 + oldvalue%16+carry) > 15)
            psw[1] |= 1<<AUX_CARRY;
        else
            psw[1] &= (~(1<<AUX_CARRY)&0xff);

        // set other flags
        SetFlags(psw[0]); // set flags may not come in use
        // because different instructions affect different flags
        // but setFlags affects all flags
    }
    
    // ADI ********************
    void adi(Argument a, Argument b)
    {
        // a is Byte, b is NOP
        pc+=pc_incr;
        int addvalue = Helper::ToDec(a.value); // value to be added to A, reg or mem
        // a is register/memory, b is NOP

        if(a.type==MEMORY)
            addvalue = m_memory[hl[0]*256 * hl[1]];
        else if (a.type==REGISTER)
            addvalue = *(R[a.value]);

        int oldvalue = psw[0]; // for aux carry check
        
        // now perform addition
        psw[0]+= addvalue;

        // check for auxillary carry, using the old value
        if((addvalue%16 + oldvalue%16) > 15)
            psw[1] |= 1<<AUX_CARRY;
        else
            psw[1] &= (~(1<<AUX_CARRY)&0xff);

        // set other flags
        SetFlags(psw[0]); // set flags may not come in use
        // because different instructions affect different flags
        // but setFlags affects all flags
    }
    
    // ACI ********************
    void aci(Argument a, Argument b)
    {
        // a is Byte, b is NOP
        pc+=pc_incr;
        int addvalue = Helper::ToDec(a.value); // value to be added to A, reg or mem
        // a is register/memory, b is NOP

        if(a.type==MEMORY)
            addvalue = m_memory[hl[0]*256 * hl[1]];
        else if (a.type==REGISTER)
            addvalue = *(R[a.value]);

        int oldvalue = psw[0]; // for aux carry check
        int carry = psw[1]&1<<CARRY;
        
        // now perform addition
        psw[0]+= addvalue+carry;

        // check for auxillary carry, using the old value
        if((addvalue%16 + oldvalue%16+carry) > 15)
            psw[1] |= 1<<AUX_CARRY;
        else
            psw[1] &= (~(1<<AUX_CARRY)&0xff);

        // set other flags
        SetFlags(psw[0]); // set flags may not come in use
        // because different instructions affect different flags
        // but setFlags affects all flags
    }

    // DAD ***********************
    void dad(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is reg_pair, b is NOP
        int hlcontent = hl[0]*256 + hl[1];
        int reg_paircontent = RP[a.value][0] * 256 + RP[a.value][1];
        int sum = hlcontent + reg_paircontent;
        // set carry if needed
        SetCarry(sum);

        sum = sum&0xff;
        hl[0] = sum/256;
        hl[1] = sum%256;
    }
    
    // SUB ********************
    void sub(Argument a, Argument b)
    {
        pc+=pc_incr;
        int subtractvalue = 0; // value to be subtracted from A, reg or mem
        // a is register/memory, b is NOP

        if(a.type==MEMORY)
            subtractvalue = m_memory[hl[0]*256 * hl[1]];
        else if (a.type==REGISTER)
            subtractvalue = *(R[a.value]);

        int oldvalue = psw[0]; // for aux carry check
        
        // now perform subtraction 
        psw[0]-= subtractvalue;

        // check for auxillary carry, perhaps not needed for sub
        
        //if((addvalue%16 + oldvalue%16) > 15)
         //   psw[1] |= 1<<AUX_CARRY;
            psw[1] &= (~(1<<AUX_CARRY)&0xff);// i may be wrong, i think aux is always reset

        // set other flags
        SetFlags(psw[0]); // set flags may not come in use
        // because different instructions affect different flags
        // but setFlags affects all flags
    }
    
    // SBB ********************
    void sbb(Argument a, Argument b)
    {
        pc+=pc_incr;
        int subtractvalue = 0; // value to be subtracted from A, reg or mem
        // a is register/memory, b is NOP

        if(a.type==MEMORY)
            subtractvalue = m_memory[hl[0]*256 * hl[1]];
        else if (a.type==REGISTER)
            subtractvalue = *(R[a.value]);

        int oldvalue = psw[0]; // for aux carry check
        
        // now perform subtraction 
        psw[0]-= (subtractvalue+psw[1] & 1<<CARRY);

        // check for auxillary carry, perhaps not needed for sub
        
        //if((addvalue%16 + oldvalue%16) > 15)
         //   psw[1] |= 1<<AUX_CARRY;
            psw[1] &= (~(1<<AUX_CARRY)&0xff);// i may be wrong, i think aux is always reset

        // set other flags
        SetFlags(psw[0]); // set flags may not come in use
        // because different instructions affect different flags
        // but setFlags affects all flags
    }
    
    // SUI ********************
    void sui(Argument a, Argument b)
    {
        pc+=pc_incr;
        int subtractvalue = Helper::ToDec(a.value); // value to be subtracted from A, reg or mem
        // a is register/memory, b is NOP

        if(a.type==MEMORY)
            subtractvalue = m_memory[hl[0]*256 * hl[1]];
        else if (a.type==REGISTER)
            subtractvalue = *(R[a.value]);

        int oldvalue = psw[0]; // for aux carry check
        
        // now perform subtraction 
        psw[0]-= subtractvalue;

        // check for auxillary carry, perhaps not needed for sub
        
        //if((addvalue%16 + oldvalue%16) > 15)
         //   psw[1] |= 1<<AUX_CARRY;
            psw[1] &= (~(1<<AUX_CARRY)&0xff);// i may be wrong, i think aux is always reset

        // set other flags
        SetFlags(psw[0]); // set flags may not come in use
        // because different instructions affect different flags
        // but setFlags affects all flags
    }
    
    // SBI ********************
    void sbi(Argument a, Argument b)
    {
        pc+=pc_incr;
        int subtractvalue = Helper::ToDec(a.value); // value to be subtracted from A, reg or mem
        // a is register/memory, b is NOP

        if(a.type==MEMORY)
            subtractvalue = m_memory[hl[0]*256 * hl[1]];
        else if (a.type==REGISTER)
            subtractvalue = *(R[a.value]);

        int oldvalue = psw[0]; // for aux carry check
        
        // now perform subtraction 
        psw[0]-= (subtractvalue + psw[1]&1<<CARRY);

        // check for auxillary carry, perhaps not needed for sub
        
        //if((addvalue%16 + oldvalue%16) > 15)
         //   psw[1] |= 1<<AUX_CARRY;
            psw[1] &= (~(1<<AUX_CARRY)&0xff);// i may be wrong, i think aux is always reset

        // set other flags
        SetFlags(psw[0]); // set flags may not come in use
        // because different instructions affect different flags
        // but setFlags affects all flags
    }


    // INR *********************
    void inr(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is register/memory
        if(a.type==MEMORY)
        {
           int address = hl[0]*256 + hl[1];
           // check for auxillary carry
           if(m_memory[address] % 16 + 1 >15)
               psw[1] |= 1<<AUX_CARRY;
            else psw[1] &= (~(1<<AUX_CARRY)&0xff);
           // increment
           int incr = m_memory[address]+1;
           // check for sign
           SetSign(incr);
           m_memory.SetValue(address, incr&0xff);
           // check for zero
           SetZero(m_memory[address]);
        }
        else if(a.type == REGISTER)
        {
            // check for auxillary carry
            if(*(R[a.value]) % 16 + 1 > 15)
                psw[1] |= 1<<AUX_CARRY;
            // increment
            int incr = *(R[a.value]) + 1;
            // check for sign
            SetSign(incr);

            *(R[a.value])  = incr&0xff;
            // check for zero
            SetZero(*R[a.value]);
        }
    }

    // INX *******************
    void inx(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is register pair.
        int value = RP[a.value][0] * 256 + RP[a.value][1];
        value+=1;
        RP[a.value][0] = value/256;
        RP[a.value][1] = value%256;
    }
    
    // DCR *********************
    void dcr(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is register/memory
        if(a.type==MEMORY)
        {
           int address = hl[0]*256 + hl[1];
           // decrement 
           int decr = m_memory[address]-1;
           decr&=0xff;
           // check for sign
           SetSign(decr);

           m_memory.SetValue(address, decr);
           // check for zero
           SetZero(m_memory[address]);
        }
        else if(a.type == REGISTER)
        {
            // decrement 
            *(R[a.value])  = (*(R[a.value]) - 1) & 0xff;
            // check for sign
            SetSign(*(R[a.value]));
            // check for zero
            SetZero(*(R[a.value]));
        }
    }
    
    // DCX *******************
    void dcx(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is register pair.
        int value = RP[a.value][0] * 256 + RP[a.value][1];
        value-=1;
        RP[a.value][0] = value/256;
        RP[a.value][1] = value%256;
    }
    
    // CMP ******************
    void cmp(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is register/memory, b is NOP
        int compare;
        if(a.type==MEMORY)
        {
            int address = hl[0]*256 + hl[1];
            compare = m_memory[address];
        }
        else // a.type==REGISTER
            compare = *(R[a.value]);
        // now compare
        int val = psw[0] - compare;
        if(val==0)// set zero
            psw[1] |= 1<<ZERO;
        else if(val<0) // set carry
            psw[1] |= 1<<CARRY;
        else if(val>0) // reset carry and zero
        {
            psw[1] &= (~(1<<CARRY)&0xff);
            psw[1] &= (~(1<<ZERO)&0xff);
        }
    }
    
    // CPI ******************
    void cpi(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is immediate value(byte), b is NOP
        int compare=Helper::ToDec(a.value);
        // now compare
        int val = psw[0] - compare;
        if(val==0)// set zero
            psw[1] |= 1<<ZERO;
        else if(val<0) // set carry
            psw[1] |= 1<<CARRY;
        else if(val>0) // reset carry and zero
        {
            psw[1] &= (~(1<<CARRY)&0xff);
            psw[1] &= (~(1<<ZERO)&0xff);
        }
    }

    // ANA ******************
    void ana(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is Register/mem, b is NOP
        int andwith;
        if(a.type==MEMORY)
        {
            int address = hl[0]*256 + hl[1];
            andwith = m_memory[address];
        }
        else // a.type = REGISTER
            andwith = *(R[a.value]);
        // do and
        psw[0]&=andwith;
        // check sign
        SetSign(psw[0]);
        // check zero
        SetZero(psw[0]);
        // check parity
        SetParity(psw[0]);
        // reset carry
        psw[1] &= (~(1<<CARRY)&0xff);
        // set aux
        psw[1] |= 1<<AUX_CARRY;
    }
    
    // ANI ******************
    void ani(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is byte, b is NOP
        int andwith= Helper::ToDec(a.value);
        // do and
        psw[0]&=andwith;
        // check sign
        SetSign(psw[0]);
        // check zero
        SetZero(psw[0]);
        // check parity
        SetParity(psw[0]);
        // reset carry
        psw[1] &= (~(1<<CARRY)&0xff);
        // set aux
        psw[1] |= 1<<AUX_CARRY;
    }
    
    // XRA ******************
    void xra(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is Register/mem, b is NOP
        int xorwith;
        if(a.type==MEMORY)
        {
            int address = hl[0]*256 + hl[1];
            xorwith = m_memory[address];
        }
        else // a.type = REGISTER
            xorwith = *(R[a.value]);
        // do xor
        psw[0]^=xorwith;
        // check sign
        SetSign(psw[0]);
        // check zero
        SetZero(psw[0]);
        // check parity
        SetParity(psw[0]);
        // reset carry
        psw[1] &= (~(1<<CARRY)&0xff);
        // set aux
        psw[1] |= 1<<AUX_CARRY;
    }
    
    // ORA ******************
    void ora(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is Register/mem, b is NOP
        int orwith;
        if(a.type==MEMORY)
        {
            int address = hl[0]*256 + hl[1];
            orwith = m_memory[address];
        }
        else // a.type = REGISTER
            orwith = *(R[a.value]);
        // do or
        psw[0]|=orwith;
        // check sign
        SetSign(psw[0]);
        // check zero
        SetZero(psw[0]);
        // check parity
        SetParity(psw[0]);
        // reset carry
        psw[1] &= (~(1<<CARRY)&0xff);
        // set aux
        psw[1] |= 1<<AUX_CARRY;
    }
    
    // ORI ******************
    void ori(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is byte, b is NOP
        int orwith= Helper::ToDec(a.value);
        // do or 
        psw[0]&=orwith;
        // check sign
        SetSign(psw[0]);
        // check zero
        SetZero(psw[0]);
        // check parity
        SetParity(psw[0]);
        // reset carry
        psw[1] &= (~(1<<CARRY)&0xff);
        // set aux
        psw[1] |= 1<<AUX_CARRY;
    }
    
    // XRI ******************
    void xri(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is byte, b is NOP
        int xorwith= Helper::ToDec(a.value);
        // do xor 
        psw[0]&=xorwith;
        // check sign
        SetSign(psw[0]);
        // check zero
        SetZero(psw[0]);
        // check parity
        SetParity(psw[0]);
        // reset carry
        psw[1] &= (~(1<<CARRY)&0xff);
        // set aux
        psw[1] |= 1<<AUX_CARRY;
    }

    // RLC *******************
    void rlc(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is NOP, B is NOP
        // shift accumulator
        psw[0]<<=1;
        // modify carry bit
        SetCarry(psw[0]);
        // set D0, according to carry bit(which is D7)
        if(psw[0]&1<<CARRY) //set D0
            psw[0]&=0xffffff;
        else
            psw[0]&=0xfffffe;
        psw[0]&=0xff; // making it 8 bit
    }
    
    // RRC *******************
    void rrc(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is NOP, B is NOP
        // shift accumulator
        psw[0]>>=1;
        // if D0 is 0 reset carry, else set carry
        if(psw[0]&1) // means D0 is 1
            psw[1] |= 1<<CARRY;
        else
            psw[1] &= (~(1<<CARRY)&0xff);
        // set D7, according to  D0
        if(psw[0]&1) //set D7
            psw[0]|=0x80;
        else // reset D7
            psw[0]&=0x7f;
        psw[0]&=0xff; // making it 8 bit
    }
    
    // RAL *******************
    void ral(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is NOP, B is NOP
        // shift accumulator
        psw[0]<<=1;
        // store the carry bit
        int carry = psw[1]&1<<CARRY;
        // modify carry bit
        SetCarry(psw[0]);
        // set D0, according to carry bit(which is D7)
        if(psw[0]&1<<CARRY) //set D0
            psw[0]&=0xffffff;
        else
            psw[0]&=0xfffffe;

        psw[0]&=0xff; // making it 8 bit
        // modify D0 a/c carry
        if(carry)
            psw[0] |= 1;
        // else it is already 0;
    }
    
    // RAR *******************
    void rar(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is NOP, B is NOP
        // shift accumulator
        psw[0]>>=1;
        // store carry
        int carry = psw[1]&1<<CARRY;
        // if D0 is 0 reset carry, else set carry
        if(psw[0]&1) // means D0 is 1
            psw[1] |= 1<<CARRY;
        else
            psw[1] &= (~(1<<CARRY)&0xff);
        // set D7, according to carry 
        if(carry) //set D7
            psw[0]|=0x80;
        else // reset D7
            psw[0]&=0x7f;
        psw[0]&=0xff; // making it 8 bit
    }

    // CMA ***************
    void cma(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is NOP, b is NOP
        psw[0]^=0xff;
        psw[0]&=0xff;
    }

    // CMC ****************
    void cmc(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is NOP, b is NOP
        // complement carry flag
        psw[1] ^= 1<<CARRY;
    }

    // STC *****************
    void stc(Argument a, Argument b)
    {
        pc+=pc_incr;
        // a is NOP, b is NOP
        psw[1] |= 1<<CARRY;
    }
    
    // JZ ********************
    void jz(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // jump address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        pc_incr = 3;
        pc+=pc_incr;
        if(psw[1]&1<<ZERO)
            pc = memlocation;
    }
    
    // JNZ ********************
    void jnz(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // jump address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        pc_incr = 3;
        pc+=pc_incr;
        if(!(psw[1]&1<<ZERO))
            pc = memlocation;
    }
    
    // JC ********************
    void jc(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // jump address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        pc_incr = 3;
        pc+=pc_incr;
        if(psw[1]&1<<CARRY)
            pc = memlocation;
    }
    
    // JNC ********************
    void jnc(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // jump address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        pc_incr = 3;
        pc+=pc_incr;

        if(!(psw[1]&1<<CARRY))
            pc = memlocation;
    }
    
    // CZ ********************
    void cz(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // ing address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        // return address is 3 bytes after the current memory location
        int retlocation = pc+3;

        pc_incr = 3;

        if(psw[1]&1<<ZERO)
        {
            pc = memlocation;
            Stackpush(retlocation); // push the return location to stack
        }
    }

    // CNZ ********************
    void cnz(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // ing address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        // return address is 3 bytes after the current memory location
        int retlocation = pc+3;

        pc_incr = 3;

        if(!(psw[1]&1<<ZERO))
        {
            pc = memlocation;
            Stackpush(retlocation); // push the return location to stack
        }
    }
    
    // CC ********************
    void cc(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // ing address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        // return address is 3 bytes after the current memory location
        int retlocation = pc+3;

        pc_incr = 3;

        if(psw[1]&1<<CARRY)
        {
            pc = memlocation;
            Stackpush(retlocation); // push the return location to stack
        }
    }

    // CNC ********************
    void cnc(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // ing address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        // return address is 3 bytes after the current memory location
        int retlocation = pc+3;

        pc_incr = 3;

        if(!(psw[1]&1<<CARRY))
        {
            pc = memlocation;
            Stackpush(retlocation); // push the return location to stack
        }
    }
};
