#include <iostream>
#include <vector>
#include <newParser.h>
#include <Processor.h>
#include <Share.h>

void executeFile(Share_Resource &share_resource)
{
    try
    {
    Processor processor;
	NewParser p(8000); // 12345 is starting address
	p.Initialize("opcodes.txt");
	p.LoadMnemonics("program.txt", processor.m_memory);
    processor.Initialize(&p);
    processor.Run(share_resource);
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
