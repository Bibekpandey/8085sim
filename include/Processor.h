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
    void Initialize(NewParser*);
    bool Execute(); // executes the instruction pointed by the PC, returns true if further execution, else false if rst5 encountered

    void PrintRegisters()
    {
        std::cout << "A "<<psw[0] <<" \nB " << bc[0]<<
        " \nc " << bc[1]<<
        " \nd " << de[0]<<
        " \ne " << de[1]<<std::endl<< std::endl;;
    }

    Memory m_memory;

private:
    int sp[2], pc, bc[2], de[2], hl[2], psw[2];
    std::map<std::string, int*> R; // stores map of strings and registers
    std::map<std::string, int*> RP; // stores map of strings and reg pairs
    // map functions
    std::map<std::string, f> Command;
    // map opcodes and instructions
    std::map<int, Instruction> opcodeToInstr;

    // our functions, for all the commands
    void mov(Argument a, Argument b) // both a and b must be registers
    {
        *(R[a.value]) = *(R[b.value]);
    }

    void mvi(Argument a, Argument b) // a is register and b is value
    {
        *(R[a.value]) = Helper::ToDec(b.value);
    }
};
