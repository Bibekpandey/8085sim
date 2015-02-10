#include <iostream>
#include <vector>
#include <Parser.h>
#include <newParser.h>
#include <Helper.h>
#include <Memory.h>

int main()
{
	Memory m;
	NewParser p(8000); // 12345 is starting address
	p.Initialize("opcodes.txt");
	p.LoadMnemonics("program.txt", m);
	//Parser p;
	//p.Initialize("rules.txt");
	//p.ParseFile(std::string("program.txt"));
}
