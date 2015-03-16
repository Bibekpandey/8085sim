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
			m_commands.push_back(command);
		}
		else // line contains arguments and opcode
		{
			// split line by spaces
			std::vector<std::string> argsAndOpcodes;
			argsAndOpcodes = Helper::SplitIntoTwo(temp, ' ');

			// store the opcode in m_opcodes
			Helper::RTrim(argsAndOpcodes[1]);// first trimming to remove trailing spaces
			m_opcodes.push_back(Helper::ToDec(argsAndOpcodes[1]));
			//outfile << argsAndOpcodes[1] << " ";
			
			// first element of argsAndOpcodes contains args, split by ,
			std::vector<std::string> args;
			args = Helper::SplitIntoTwo(argsAndOpcodes[0], ',');

			// if args count is 1, ...
			if(args.size()==1)
			{
				m_instructions.push_back(Instruction(
							command, GetArgument(args[0], command), Argument(NONE, "NOP")));
				//outfile << command << " " << args[0] <<"\n";
			}
			else if(args.size()==2)
			{
				m_instructions.push_back(Instruction(
							command, GetArgument(args[0], command), GetArgument(args[1], command)));
				//outfile << command << " " << args[0] << " " << args[1]<<"\n";
			}
			else // something error
			{
				std::cout << "internal error\n";
				throw 1; // some other exception
			}
		}
	}
	opcodes_file.close();	
}

void NewParser::LoadOpcodes(std::string file, Memory& mem)
{
	std::ifstream program_file(file);

	std::string temp;

	int start = m_startAddress;

	while(getline(program_file, temp))
	{
		Helper::LTrim(temp);
		Helper::RTrim(temp);

		if(temp.length()==0) continue;

		if(Helper::IsHexStr(temp) and temp.length()==2)
			mem.SetValue(start, Helper::ToDec(temp));
		else
		{
			std::cout << "invalid opcode!!\n";
			throw 2;
		}
		start++;
	}
}

void NewParser::LoadMnemonics(std::string file, Memory& mem)
{
	std::ifstream program_file(file);

	std::string temp;

	int address = m_startAddress;
	int count = 1;
	int error =0;

	while(getline(program_file, temp))
	{
		// trim spaces
		Helper::LTrim(temp);
		Helper::RTrim(temp);
		if(temp.length()==0) continue; // no instruction on the line
		if(temp[0]==';') continue; // comment line

		temp = Helper::ToUpper(temp);

		// break the line into comment and instruction
		std::vector<std::string> insAndCommnt = Helper::SplitIntoTwo(temp, ';');
		// first element has the instruction
		// now break into label and instruction
		std::vector<std::string> labelAndIns = Helper::SplitIntoTwo(insAndCommnt[0], ':');

		std::string instruction;
		std::string label;

		// if length of labelAndIns == 1 , no label, else label, process second element
		if(labelAndIns.size()==2)
		{
			Helper::RTrim(labelAndIns[0]);
			m_labels.push_back(labelAndIns[0]);
			m_labelAddresses.push_back(address);

			instruction = labelAndIns[1];
			label = labelAndIns[0];
		}
		else if (labelAndIns.size()==1)
			instruction = labelAndIns[0];
		else
			throw "error in separating label and instruction";

		// process instruction

		// now split instruction by a space to command and arguments
		std::vector<std::string> commndAndArg = Helper::SplitIntoTwo(instruction);
		// commandAndArg[0] is command rest is arguments

		std::vector<std::string> args;
		if(commndAndArg.size()==1) // means, only command, no arguments
		{
			args.push_back("NOP");
			args.push_back("NOP");
		}
		else if (commndAndArg.size()==2) // means there are arguments 
		{
			// split by ,
			args = Helper::SplitIntoTwo(commndAndArg[1], ',');
			
			if(args.size()==1)
				args.push_back("NOP");
			else if (args.size()==2) // means, we have two arguments already, nothing to be done
			{} // do nothing
			else
			{
				error+=1;
				std::cout << "invalid number of arguments in line " << count<<std::endl;

				std::cout << "more than 2 arguments or less than 0 found";
				throw 1; // error, split into 2 should not give other results
			}
		}
		else 
		{
			throw "unexpected error";
		}

		// now that we have command and arguments, get opcode and values 
		try
		{
			std::vector<int> opcodeAndValues = GetOpcodeAndValues(commndAndArg[0], args);
	
			for(int i=0;i<opcodeAndValues.size();i++)
			{
				if(opcodeAndValues[i] == -1) // -1 means it is label, address is filled later, for now do not do anything
				{
					m_unfilledAddresses.push_back(address);
					m_unfilledAddLabel.push_back(args[0]);
					address+=2;
					break;
				}
                else 
                {
			    	mem.SetValue(address, opcodeAndValues[i]);
			    	address+=1;
                }
			}
		}
		catch(char* s)
		{
			std::cout << s << " on line "<< address-m_startAddress<<std::endl;
		}
		catch(Error err)
		{
			switch(err)
			{
				case INVALID_COMMAND:
					std::cout << "Invalid command: ";
					break;
				case INVALID_OPERANDS:
					std::cout << "Invalid operands/operands number: ";
					break;
				default:
					std::cout << "oops!! internal error\n";
					break;
			}
			std::cout << "on line " << count << std::endl;
			error+=1;
		}
		count++;
	}

	// assign addresses to the unassigned addresses(due to labels)
	try
	{
		AssignValues(mem);
	}
	catch(std::string s)
	{
		std::cout << "Label doesnot exist: "<< s << std::endl;
		error+=1;
	}
	catch(int)
	{
		std::cout << "internal error";
		error+=1;
	}

	if(error==0)
	{
		for(int i=m_startAddress;i<address;i++)
			std::cout << Helper::ToHexStr(i)<<" "<< Helper::ToHexStr(mem[i]) << std::endl;;
	}
}

void NewParser::AssignValues(Memory& mem)
{
	for(int i=0;i<m_unfilledAddresses.size();i++)
	{
		int index = GetLabelIndex(m_unfilledAddLabel[i]);
		if(index<0 or index >= m_labels.size())
		{
			std::string s(m_unfilledAddLabel[i]);
			throw s;
		}
		// first store LOWER byte of the address
		mem.SetValue(m_unfilledAddresses[i], m_labelAddresses[index]%256);
		// then sotre higher byte of the address
		mem.SetValue(m_unfilledAddresses[i]+1, m_labelAddresses[index]/256);
	}
}

int NewParser::GetLabelIndex(std::string label)
{
	for(int i=0;i<m_labels.size();i++)
		if(label==m_labels[i])
			return i;
	return -1;
}

std::vector<int> NewParser::GetOpcodeAndValues(std::string cmmd, std::vector<std::string>& args) // returns a vector containing opcode and byte increment
{
	std::vector<int> opcodeAndArgs;
	if(!CommandExists(cmmd)) // command doesnot exist,throw exception 
	{
		throw INVALID_COMMAND;
	}
	else // command exists	
	{
		int n = GetInstructionIndex(cmmd); // the index of the first instruction whose command matches cmmd
		if(n<0 or n>=m_instructions.size())
		{
			throw INVALID_COMMAND;
		}

		// check all the instructions whose command is cmmd
		while(1)
		{
			if(m_instructions[n].command!=cmmd or n>=m_instructions.size()) // command != cmmd, and not yet any match to instruction, throw exception
			{
				throw INVALID_OPERANDS;
			}
			else  // command == cmmd
			{
				opcodeAndArgs.clear();// for new sets of opcodes and data
				int errors = 0;
				// check first argument
				switch(m_instructions[n].arg1.type)
				{
					case BYTE:
						if((Helper::IsHexStr(args[0]) and args[0].length()==2))
						{
							opcodeAndArgs.push_back(m_opcodes[n]);
							opcodeAndArgs.push_back(Helper::ToDec(args[1]));
						}
						else
							errors+=1;
						break;
					case DOUBLE:
						if((Helper::IsHexStr(args[0]) and args[0].length()==4))
						{
							opcodeAndArgs.push_back(m_opcodes[n]);
							opcodeAndArgs.push_back(Helper::ToDec(args[0].substr(2,4)));
							opcodeAndArgs.push_back(Helper::ToDec(args[0].substr(0,2)));
						}
						else
							errors+=1;
						break;
					case LABEL:
						opcodeAndArgs.push_back(m_opcodes[n]);
						opcodeAndArgs.push_back(-1); // for now, dont send any other byte value, later to be filled
						opcodeAndArgs.push_back(-1);
						break;
					default:
						if(args[0]!=m_instructions[n].arg1.value)
							errors+=1;
						// need not push
						break;
				}
				// check second argument
				switch(m_instructions[n].arg2.type)
				{
					case BYTE:
						if((Helper::IsHexStr(args[1]) and args[1].length()==2))
						{
							opcodeAndArgs.push_back(m_opcodes[n]);
							opcodeAndArgs.push_back(Helper::ToDec(args[1]));
						}
						else
							errors+=1;
						break;
					case DOUBLE:
						if((Helper::IsHexStr(args[1]) and args[1].length()==4))
						{
							opcodeAndArgs.push_back(m_opcodes[n]);
							opcodeAndArgs.push_back(Helper::ToDec(args[1].substr(2,4)));
							opcodeAndArgs.push_back(Helper::ToDec(args[1].substr(0,2)));
						}
						else
							errors+=1;
						break;
					case LABEL:
						opcodeAndArgs.push_back(m_opcodes[n]);
						opcodeAndArgs.push_back(-1);
						opcodeAndArgs.push_back(-1);
						break;
					default:
						if(args[1]!=m_instructions[n].arg2.value)
							errors+=1;
						else
							opcodeAndArgs.push_back(m_opcodes[n]);
						break;
				}

				if(errors==0)
				{
					return opcodeAndArgs;
				}
				n++;
			}
		}
	}
}

bool NewParser::CommandExists(std::string command)
{
	for(int i=0;i<m_commands.size();i++)
		if(m_commands[i]==command)
			return true;
	return false;
}

int NewParser::GetInstructionIndex(std::string command)
{
	for(int i=0;i<m_instructions.size();i++)
		if(m_instructions[i].command == command)
			return i;
	return -1;
}
