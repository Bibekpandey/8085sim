#include <iostream>
#include <vector>
#include <newParser.h>
#include <Processor.h>

void executeFile()
{
    try
    {
    Processor processor;
	NewParser p(8000); // 12345 is starting address
	p.Initialize("opcodes.txt");
	p.LoadMnemonics("program1.txt", processor.m_memory);
    processor.Initialize(&p);
    processor.Run();
    }
    catch(std::string a)
    {
        std::cout << a << std::endl;
    }
    catch(const char* s)
    {
        std::cout << s;
    }
}
