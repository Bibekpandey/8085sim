# pragma once
#include <map>
#include <functional>
#include <Memory.h>
#include <Helper.h>
#include <Types.h>
#include <newParser.h>


//typedef std::function<void(Argument, Argument)> func; // argument from Types.h

class Processor
{
public:
    typedef void(Processor::*f)(Argument, Argument);
    enum { SIGN=7, ZERO=5, AUX_CARRY=3, PARITY=1, CARRY=0}; // the bits of flag register

    Processor();
    ~Processor(){}

    void Run();
    void PrintMemory(int a, int b);
    void Initialize(NewParser*);
    bool Execute(); // executes the instruction pointed by the PC, returns true if further execution, else false if rst5 encountered

    void Stackpush(int a); // helper to push to stack
    int Stackpop(); // helper to pop from stack

    void SetFlags(int &reg); // set flags a/c register input

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

        // calling address is two bytes after the current memory location
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
            psw[1] &= (~(1<<AUX_CARRY)&0xff)

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
            psw[1] &= (~(1<<AUX_CARRY)&0xff)

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
            psw[1] &= (~(1<<AUX_CARRY)&0xff)

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
            psw[1] &= (~(1<<AUX_CARRY)&0xff)

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
        if(sum&0x100) // checking 9th bit
            psw[1] |= 1<<CARRY;
        else
            psw[1] &= (~(1<<CARRY)&0xff)
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
           int addr 
};
