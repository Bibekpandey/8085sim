#include <Processor.h>
#include <cstdlib>

Processor::Processor()
{
    // initialize all registers value to zero;
    bc[0]=bc[1]=de[0]=de[1]=hl[0]=hl[1]=psw[0]=psw[1] = 0;
    pc = 0;
    // our memory has locations upto 1<<16 = 65535
    // so initialize sp to value 65535
    sp[0] = 65535%256;
    sp[1] = 65535/256;
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

    // data transfer instructions
    Command["MVI"] = &Processor::mvi;
    Command["MOV"] = &Processor::mov;
    Command["LXI"] = &Processor::lxi;
    Command["LDA"] = &Processor::lda;
    Command["LDAX"] = &Processor::ldax;
    Command["LHLD"] = &Processor::lhld;
    Command["STA"] = &Processor::sta;
    Command["STAX"] = &Processor::stax;
    Command["SHLD"] = &Processor::shld;
    Command["XCHG"] = &Processor::xchg;
    Command["SPHL"] = &Processor::sphl;
    Command["PCHL"] = &Processor::pchl;
    Command["XTHL"] = &Processor::xthl;
    Command["PUSH"] = &Processor::push;
    Command["POP"] = &Processor::pop;
    Command["OUT"] = &Processor::out; // function not written
    Command["IN"] = &Processor::in; // function not written
    // arithmetic instructions
    Command["ADD"] = &Processor::add;
    Command["ADC"] = &Processor::adc;
    Command["ADI"] = &Processor::adi;
    Command["ACI"] = &Processor::aci;
    Command["DAD"] = &Processor::dad;
    Command["SUB"] = &Processor::sub;
    Command["SBB"] = &Processor::sbb;
    Command["SUI"] = &Processor::sui;
    Command["INR"] = &Processor::inr;
    Command["INX"] = &Processor::inx;
    Command["DCR"] = &Processor::dcr;
    Command["DCX"] = &Processor::dcx;
    // logical instructions
    Command["CMP"] = &Processor::cmp;
    Command["CPI"] = &Processor::cpi;
    Command["ANA"] = &Processor::ana;
    Command["ANI"] = &Processor::ani;
    Command["XRA"] = &Processor::xra;
    Command["ORA"] = &Processor::ora;
    Command["XRI"] = &Processor::xri;
    Command["ORI"] = &Processor::ori;
    Command["RLC"] = &Processor::rlc;
    Command["RRC"] = &Processor::rrc;
    Command["RAL"] = &Processor::ral;
    Command["RAR"] = &Processor::rar;
    Command["CMA"] = &Processor::cma;
    Command["CMC"] = &Processor::cmc;
    Command["STC"] = &Processor::stc;
    // branching instructions
    Command["JMP"] = &Processor::jmp;
    Command["JZ"] = &Processor::jz;
    Command["JNZ"] = &Processor::jnz;
    Command["JC"] = &Processor::jc;
    Command["JNC"] = &Processor::jnc; // jp, jm, jpe, jpo remain
    Command["CALL"] = &Processor::call;
    Command["RET"] = &Processor::ret;
    Command["CZ"] = &Processor::cz;
    Command["CNZ"] = &Processor::cnz;
    Command["CC"] = &Processor::cc;
    Command["CNC"] = &Processor::cnc;

    //interrput instructions
    Command["EI"] = &Processor::ei;
    Command["DI"] = &Processor::di;
    Command["SIM"] = &Processor::sim;
    Command["RIM"] = &Processor::rim;

}

void Processor::PrintMemory(int a, int b)
{
    for(int i=0;i<b-a+1;i++)
        std::cout << a+i << ": " << m_memory[a+i] << "   ";
    std::cout << std::endl;
}

void Processor::Run()
{
    /* char input;
     char single;
     std::cout << "single step? (y or n)";
     std::cin >> single;
     if(single=='y')
     {
         while(Execute())
         {
             //std::cout << "want to view memory? ( 'm' if yes, else any char ):  ";
             std::cin.get();
             //if(input=='m')
             {
                 int a[2];
                 //std::cout << "enter memory location range(separated by space";
                 //std::cin >> a[0] >> a[1];
                 //PrintMemory(a[0], a[1]);
                 PrintMemory(8000, 8005);
             }
             PrintRegisters();
             PrintFlags();
         }
     }
     else
         while(Execute());
             int a[2];
             std::cout << "enter memory location range(separated by space";
             std::cin >> a[0] >> a[1];
             PrintMemory(a[0], a[1]);
             PrintRegisters();
             PrintFlags();
    */     
    while(Execute());
}


bool Processor::Execute()

{
    int opcode = m_memory[pc];
    //std::cout << pc << " " << opcode << " ";
    Instruction i = opcodeToInstr[opcode];

    if (i.command=="RST5")return false;

    pc_incr = 1; // the value by which pc is to be incremented
    
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
        i.arg1.value = Helper::ToHexStr(msbyte*256+lsbyte);
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
        i.arg2.value = Helper::ToHexStr(msbyte*256+lsbyte);
        pc_incr+=2;
    }
    // now call the required function
    std::map<std::string, f>::const_iterator it;
    it = Command.find(i.command);

   // std::cout << "executing instruction: " << i.command << " "<<i.arg1.value<<" "<<i.arg2.value<< std::endl;
    (this->*it->second)(i.arg1, i.arg2);

//check for the interrupt and status value
   if(pin.interruptEnableFlipFlop)
     {
        if(pin.TRAP.status && pin.TRAP.value)
          { 
          std::cout<<"Trap"<<std::endl;
          std::cout<<pin.TRAP.value<<std::endl; 
          }

        else if(pin.RST_7_5.status && pin.RST_7_5.value)
            {
             std::cout<<"RST 7.5"<<std::endl;
             std::cout<<pin.RST_7_5.value<<std::endl; 
            }

        else if(pin.RST_6_5.status && pin.RST_6_5.value)
           { 
            std::cout<<"RST 6.5" <<std::endl;
            std::cout<<pin.RST_6_5.value<<std::endl; 
           }
        else if(pin.RST_5_5.status && pin.RST_5_5.value)
           {
            std::cout<<"RST 5.5"<<std::endl;
            std::cout<<pin.RST_5_5.value<<std::endl; 
           }
     }

//    Share::interrupt = true;
   
    return true;
}

void Processor::Stackpush(int pushvalue)
{
    int spointer = sp[0] + sp[1]*256;
    m_memory.SetValue(spointer-1, pushvalue%256);
    m_memory.SetValue(spointer, pushvalue/256);
    spointer-=2;
    sp[0] = spointer%256;
    sp[1] = spointer/256;
}

int Processor::Stackpop()
{
    int spointer = sp[0] + sp[1]*256; // stack pointer's memory
    int valueAtmem = m_memory[spointer+2] * 256 + m_memory[spointer+1];
    spointer+=2;
    sp[0] = spointer%256;
    sp[1] = spointer/256;
    return valueAtmem;
}

void Processor::PrintFlags()
{
    std::cout << "S: " << ((psw[1]&1<<SIGN)>>SIGN) <<
               "    Z: " << ((psw[1]&1<<ZERO)>>ZERO) <<
                "    AC: " << ((psw[1]&1<<AUX_CARRY)>>AUX_CARRY) <<
                "    P: " << ((psw[1]&1<<PARITY)>>PARITY) <<
                "    C: " << ((psw[1]&1<<CARRY)>>CARRY) << std::endl;
}

void Processor::SetFlags(int& reg)
{
        // auxillary flag is already checked
        // check for carry 
        if(reg&0x100) // means if 9th bit is 1, set carry flag on
            psw[1] |= 1<<CARRY;
        else
            psw[1] &= (~(1<<CARRY)&0xff);

        // now remove extra bits in the front
        reg = reg&0xff;
        
        // check for zero 
        if(reg == 0)
            psw[1] |= 1<<ZERO;
        else
            psw[1] &= (~(1<<ZERO)&0xff);

        // check for sign bit
        if(reg&0x80)
            psw[1] |= 1<<SIGN;
        else
            psw[1] &= (~(1<<SIGN)&0xff);

        //check for parity
        if((reg&1<<0 + reg&1<<1 + reg&1<<2 + reg&1<<3 + reg&1<<4 + 
            reg&1<<5 + reg&1<<6 + reg&1<<7) % 2 == 0)
            psw[1] |= 1<<PARITY;
        else
            psw[1] &= (~(1<<PARITY)&0xff);
}

void Processor::SetZero(int reg)
{
        if(reg == 0)
            psw[1] |= 1<<ZERO;
        else
            psw[1] &= (~(1<<ZERO)&0xff);
}

void Processor::SetParity(int reg)
{
    if((reg&1<<0 + reg&1<<1 + reg&1<<2 + reg&1<<3 + reg&1<<4 + 
            reg&1<<5 + reg&1<<6 + reg&1<<7) % 2 == 0)
            psw[1] |= 1<<PARITY;
        else
            psw[1] &= (~(1<<PARITY)&0xff);
}

void Processor::SetSign(int reg)
{
    if(reg&0x80)
            psw[1] |= 1<<SIGN;
        else
            psw[1] &= (~(1<<SIGN)&0xff);
}

void Processor::SetCarry(int reg)
{
    if(reg&0x100) // means if 9th bit is 1, set carry flag on
            psw[1] |= 1<<CARRY;
        else
            psw[1] &= (~(1<<CARRY)&0xff);
}


