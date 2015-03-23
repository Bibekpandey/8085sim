#include <Processor.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

Processor::Processor()
{
    // initialize all registers value to zero;
    bc[0]=bc[1]=de[0]=de[1]=hl[0]=hl[1]=psw[0]=psw[1] = 0;
    pc = 0;
    // our memory has locations upto 1<<16 = 65535
    // so initialize sp to value 65535

    sp[0] = 65535%256;
    sp[1] = 65535/256;
    
    std::ifstream vector_table("vectortable.txt");
  
    std::string temp;

    int a, b;
    
    while(!vector_table.eof())
    {
  
    vector_table>> a >> b;
    m_memory.SetValue(a, b); 
    
    }
    

}

void Processor::Initialize(NewParser*p, ExampleWindow * window)
{
    // map the opcodes in parser to instructions,and store it in opcodeToInstr map
    for(int i=0;i<p->m_opcodes.size();i++)
    {
        opcodeToInstr[p->m_opcodes[i]] = p->m_instructions[i];
    }

    // initialize program counter
    pc = p->m_startAddress; 

    // window
    m_exampleWindow = window;

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
    for(int i=0;i<b-a+1;i++){
        std::cout << a+i << ": " << m_memory[a+i] << "   ";
        if(i%9==8)std::cout <<'\n';
    }
    std::cout << std::endl;
}

void Processor::updateGUI()
{
    // set registers and flags
    static int a,b,c,d,e,h,l;
    static int s,z,ac,p,cy;
    static int portA, portB, portC;

    m_exampleWindow->a_value.set_markup(a==psw[0]?std::to_string(psw[0]):("<b>"+std::to_string(psw[0])+"</b>"));
    m_exampleWindow->b_value.set_markup(b==bc[0]?std::to_string(bc[0]):("<b>"+std::to_string(bc[0])+"</b>"));
    m_exampleWindow->c_value.set_markup(c==bc[1]?std::to_string(bc[1]):("<b>"+std::to_string(bc[1])+"</b>"));
    m_exampleWindow->d_value.set_markup(d==de[0]?std::to_string(d):("<b>"+std::to_string(de[0])+"</b>"));
    m_exampleWindow->e_value.set_markup(e==de[1]?std::to_string(e):("<b>"+std::to_string(de[1])+"</b>"));
    m_exampleWindow->h_value.set_markup(h==hl[0]?std::to_string(h):("<b>"+std::to_string(hl[0])+"</b>"));
    m_exampleWindow->l_value.set_markup(l==hl[1]?std::to_string(l):("<b>"+std::to_string(hl[1])+"</b>"));
    m_exampleWindow->s_value.set_markup(s==(psw[1]&1<<SIGN)?std::to_string(s?1:0) : ("<b>"+std::to_string(psw[1]&1<<SIGN?1:0)+"</b>"));
    m_exampleWindow->z_value.set_markup(z==(psw[1]&1<<ZERO)?std::to_string(z?1:0) : ("<b>"+std::to_string(psw[1]&1<<ZERO?1:0)+"</b>"));
    m_exampleWindow->ac_value.set_markup(ac==(psw[1]&1<<AUX_CARRY)?std::to_string(ac?1:0) : ("<b>"+std::to_string(psw[1]&1<<AUX_CARRY?1:0)+"</b>"));
    m_exampleWindow->p_value.set_markup(p==(psw[1]&1<<PARITY)?std::to_string(p?1:0) : ("<b>"+std::to_string(psw[1]&1<<PARITY?1:0)+"</b>"));
    m_exampleWindow->cy_value.set_markup(cy==(psw[1]&1<<SIGN)?std::to_string(cy?1:0) : ("<b>"+std::to_string(psw[1]&1<<CARRY?1:0)+"</b>"));

    m_exampleWindow->m_valA.set_markup(std::to_string(ioMemory[64]));
    m_exampleWindow->m_valB.set_markup(std::to_string(ioMemory[65]));
    m_exampleWindow->m_valC.set_markup(std::to_string(ioMemory[66]));
 
    a = psw[0],b=bc[0],c=bc[1],d=de[0],e=de[1],h=hl[0],l=hl[1];
    
    z=psw[1]&1<<ZERO;
    ac=psw[1]&1<<AUX_CARRY;
    p=psw[1]&1<<PARITY;
    cy=psw[1]&1<<CARRY;
    // fill the address
    Gtk::TreeModel::iterator selrow = m_exampleWindow->m_refTreeModel->children().begin();
    int memstart = Helper::ToDec("9000");
    while(selrow!= m_exampleWindow->m_refTreeModel->children().end())
    {
        Gtk::ListStore::Row row = *selrow;
        row[m_exampleWindow->m_Columns.m_col_address_hex] = 0;
        row[m_exampleWindow->m_Columns.m_col_address] = memstart;
        row[m_exampleWindow->m_Columns.m_col_data] = m_memory[memstart];
        memstart++;
        selrow ++;
    }
}


void Processor::Run()
{
    

  if(m_exampleWindow->run==1)
     {
        while(Execute());
        updateGUI();
     }
     else // meaning singlestep on
     {
         Execute();
         updateGUI();
     }
     return;

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
          //   int a[2];
         //    std::cout << "enter memory location range(separated by space";
         //    std::cin >> a[0] >> a[1];
         //    PrintMemory(a[0], a[1]);
          //   PrintRegisters();
         //    PrintFlags();
}

void Processor::copyArray(int *mat1, int *mat2)
{

    for(int i= 0; i<256; i++)
        mat1[i] = mat2[i];
}

bool Processor::Execute()
{

    copyArray(ioMemory, m_exampleWindow->share_resource.ioMemory);

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

    std::cout << "executing instruction: " << i.command << " "<<i.arg1.value<<" "<<i.arg2.value<< std::endl;
  
    (this->*it->second)(i.arg1, i.arg2);
    
//check for the interrupt and status value
   if(pin.interruptEnableFlipFlop)
     {
        if(pin.TRAP.status )
          { 

   if(m_exampleWindow->share_resource.strobeA)
             
           pin.TRAP.value =  m_exampleWindow->share_resource.strobeA;

   if(m_exampleWindow->share_resource.strobeB)
             
           pin.TRAP.value =   m_exampleWindow->share_resource.strobeB;
         
          }

        else if(pin.RST_7_5.status )
            {
   

    if(m_exampleWindow->share_resource.strobeA)
             pin.RST_7_5.value =   m_exampleWindow->share_resource.strobeA;

   if(m_exampleWindow->share_resource.strobeB)
             pin.RST_7_5.value =   m_exampleWindow->share_resource.strobeB;
        
            }

        else if(pin.RST_6_5.status) 
           { 
    if(m_exampleWindow->share_resource.strobeA)
             pin.RST_6_5.value =   m_exampleWindow->share_resource.strobeA;

   if(m_exampleWindow->share_resource.strobeB)
             pin.RST_6_5.value =   m_exampleWindow->share_resource.strobeB;
                   
           }
        else if(pin.RST_5_5.status) 
           {
         
    if(m_exampleWindow->share_resource.strobeA)
             pin.RST_5_5.value =   m_exampleWindow->share_resource.strobeA;

   if(m_exampleWindow->share_resource.strobeB)
             pin.RST_5_5.value =  m_exampleWindow-> share_resource.strobeB;
                   
           }
     }

    if(pin.TRAP.value)
        {
            //it has no vector table
        }

    else if(pin.RST_7_5.value)
      {
           Stackpush(pc+pc_incr);
           pc=36793;
      }

    else if(pin.RST_6_5.value)
      {
           Stackpush(pc+pc_incr);
           pc=36790;
      }
     
    else if(pin.RST_5_5.value)
     {
           Stackpush(pc);
           pc=36787;
     }

m_exampleWindow->share_resource.strobeA = false;
m_exampleWindow->share_resource.strobeB = false;

pin.TRAP.value = false;
pin.RST_7_5.value = false;
pin.RST_6_5.value = false;
pin.RST_5_5.value = false;

copyArray(m_exampleWindow->share_resource.ioMemory, ioMemory);

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
