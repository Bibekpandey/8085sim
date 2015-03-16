#include <iostream>
#include <vector>
#include <newParser.h>
#include <Processor.h>

int main()
{
    try
    {
    Processor processor;
	NewParser p(8000); // 12345 is starting address
	p.Initialize("opcodes.txt");
	p.LoadMnemonics("program.txt", processor.m_memory);
    processor.Initialize(&p);
    processor.Run();
    }
    catch(std::string a)
    {
        std::cout << a << std::endl;
    }
}
