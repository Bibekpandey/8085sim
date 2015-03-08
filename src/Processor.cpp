#include <Processor.h>


Processor::Processor()
{
    // initialize all registers value to zero;
    sp[0]=sp[1]=bc[0]=bc[1]=de[0]=de[1]=hl[0]=hl[1]=psw[0]=psw[1] = 0;
    pc = 0;
}

void Processor::Initialize(NewParser*p)
{
    // map the opcodes in parser to instructions,and store it in opcodeToInstr map
    for(int i=0;i<p->m_opcodes.size();i++)
    {
        opcodeToInstr[p->m_opcodes[i]] = p->m_instructions[i];
    }

    // initialize program counter
    pc = p->m_startAddress; 

    // map registers  and register pairs
    RP["SP"] = sp;
    RP["PSW"] = psw;
    R["A"] = &(psw[0]);
    RP["B"] = R["B"] = &(bc[0]);
    R["C"] = &(bc[1]);
    RP["D"] = R["D"] = &(de[0]);
    R["E"] = &(de[1]);
    RP["H"] = R["H"] = &(hl[0]);
    R["L"] = &(hl[1]);

    // map commands and functions
    Command["MVI"] = &Processor::mvi;
    Command["MOV"] = &Processor::mov;
}

void Processor::Run()
{
    while(Execute());
}


bool Processor::Execute()
{
    int opcode = m_memory[pc];
    Instruction i = opcodeToInstr[opcode];

    if (i.command=="RST5")return false;

    int pc_incr = 1; // the value by which pc is to be incremented
    
    // check for the type of arguments of the instruction
    // if byte and double then, need to access memory
    if (i.arg1.type==BYTE) // get one from mem
    {
        int byte = m_memory[pc+1];
        // convert to hex and store in the instruction
        i.arg1.value = Helper::ToHexStr(byte);
        pc_incr+=1;
    }
    else if(i.arg1.type==DOUBLE) // get two values from mem
    {
        int msbyte = m_memory[pc+2];
        int lsbyte = m_memory[pc+1];
        i.arg1.value = Helper::ToHexStr(msbyte) + Helper::ToHexStr(lsbyte);
        pc_incr+=2;
    }
    if (i.arg2.type==BYTE) // get one from mem
    {
        int byte = m_memory[pc+1];
        // convert to hex and store in the instruction
        i.arg2.value = Helper::ToHexStr(byte);
        pc_incr+=1;
    }
    else if(i.arg2.type==DOUBLE) // get two values from mem
    {
        int msbyte = m_memory[pc+2];
        int lsbyte = m_memory[pc+1];
        i.arg2.value = Helper::ToHexStr(msbyte) + Helper::ToHexStr(lsbyte);
        pc_incr+=2;
    }
    // now call the required function
    std::map<std::string, f>::const_iterator it;
    it = Command.find(i.command);

    (this->*it->second)(i.arg1, i.arg2);
    pc+=pc_incr;
    PrintRegisters();
    return true;
}
