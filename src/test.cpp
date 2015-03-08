#include <iostream>
#include <vector>
#include <newParser.h>
#include <Processor.h>

int main()
{
    Processor processor;
	NewParser p(8000); // 12345 is starting address
	p.Initialize("opcodes.txt");
	p.LoadMnemonics("program.txt", processor.m_memory);
    processor.Initialize(&p);
    processor.Run();
}
