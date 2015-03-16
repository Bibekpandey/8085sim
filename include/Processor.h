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

    Processor();
    ~Processor(){}

    void Run();
    void PrintMemory(int a, int b);
    void Initialize(NewParser*);
    bool Execute(); // executes the instruction pointed by the PC, returns true if further execution, else false if rst5 encountered

    void PrintRegisters()
    {
        std::cout << "A "<<psw[0] <<" \nB " << bc[0]<<
        " \nC " << bc[1]<<
        " \nD " << de[0]<<
        " \nE " << de[1]<<
        " \nH " << hl[0]<<
        " \nL " << hl[1]<<
        " \nSP lsb " << sp[0]<<
        " \nSP msb " << sp[1]<<std::endl<< std::endl;;
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

    void mov(Argument a, Argument b) // both a and b must be registers
    {
        pc+=pc_incr;
        *(R[a.value]) = *(R[b.value]);
    }

    void mvi(Argument a, Argument b) // a is register and b is value
    {
        pc+=pc_incr;
        *(R[a.value]) = Helper::ToDec(b.value);
    }

    void lxi(Argument a, Argument b)
    {
        pc+=pc_incr;
        if(a.type!=REG_PAIR)
            throw std::string("argument 1 for lxi is not reg pair");
        if(b.type!=DOUBLE)
            throw std::string("argument 2 for lxi is not double");

        int doublevalue = Helper::ToDec(b.value);
        RP[a.value][0] = doublevalue/256;
        RP[a.value][1] = doublevalue%256;
    }

    void jmp(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // jump address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        pc_incr = 0;
        pc = memlocation;
    }

    void call(Argument a, Argument b)
    {
        // b must be NOP type
        // a must be double

        // calling address is two bytes after the current memory location
        int memlocation = m_memory[pc+2]*256 + m_memory[pc+1];

        // return address is 3 bytes after the current memory location
        int retlocation = pc+3;

        // stack pointer points to address in memory
        int spointer = sp[0] + sp[1]*256; // memory pointed by sp

        pc_incr = 0;

        pc = memlocation;
        m_memory.SetValue(spointer-1, retlocation%256);
        m_memory.SetValue(spointer, retlocation/256);
        spointer-=2;
        sp[0] = spointer%256;
        sp[1] = spointer/256;
    }

    void ret(Argument a, Argument b)
    {
        int spointer = sp[0] + sp[1]*256; // stack pointer's memory

        int mem = m_memory[spointer+2] * 256 + m_memory[spointer+1];

        spointer+=2;
        sp[0] = spointer%256;
        sp[1] = spointer/256;

        pc = mem;
    }
};
