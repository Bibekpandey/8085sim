#include <newParser.h>
#include <iostream>
#include <Helper.h>

void NewParser::Initialize(std::string file)
{
	// load the file
	std::ifstream opcodes_file(file);

	// temporary variables
	std::string temp, command, arg1, arg2;

	while(getline(opcodes_file, temp))
	{
		if(temp=="<<<") break; // end of file

		if(temp[0]=='>') // means, line contains command
		{
			command = temp.substr(1,temp.length()-1);
			Helper::RTrim(command);
		}
		else // line contains arguments and opcode
		{
			// split line by spaces
			std::vector<std::string> argsAndOpcodes;
			argsAndOpcodes = Helper::SplitIntoTwo(temp, ' ');

			// store the opcode in m_opcodes
			m_opcodes.push_back(Helper::ToDec(argsAndOpcodes[1]));
			
			// first element of argsAndOpcodes contains args, split by ,
			std::vector<std::string> args;
			args = Helper::SplitIntoTwo(argsAndOpcodes[0], ',');

			// if args count is 1, ...
			if(args.size()==1)
			{
				m_instructions.push_back(Instruction(
							command, Argument(GetType(args[0], command)), Argument()));
			}
			else if(args.size()==2)
			m_instructions.push_back(Instruction(
							command, Argument(GetType(args[0], command)), Argument(GetType(args[1], command))));
			else // something error
			{
				std::cout << "internal error\n";
				throw 1; // some other exception
			}
		}
	}
}
