#include <Parser.h>
#include <iostream>
#include <Helper.h>

void Parser::Initialize(std::string rulefile)
{
	// load the file		
	std::ifstream rules_file(rulefile);
	// temporary variables
	std::string temp, rule, opcode, prev_opcode="..."; 
	// prev_opcode="..." is set because it would have no value
	// as we have not read any opcode yet

	std::string complete_rule="aa"; // in case opcode has multiple rules
	unsigned numOperands;
	int cnt=0;

	while(1) 
	{
		// read two strings from ifstream, they will be opcode and rule
		rules_file >> opcode;
		rules_file >> rule;

		if(opcode=="<<<")break; // <<< is end mark for the rules file

		if(opcode == prev_opcode)
		{
			// we just need to concatenate the rules
			complete_rule+=("|"+rule);
		}
		else 
		{
			// we have a new opcode
			// so store previous opcode in opcode vector
			// and previous rules in rules vector
			m_opcodes.push_back(opcode);
			if(cnt>0)
			{
				m_rules.push_back(complete_rule);
			}
			cnt=cnt+1;
			// assign new rule to complete rule
			complete_rule = rule;
			numOperands = Helper::Split(rule, ',').size();

			m_numOperands.push_back(numOperands);// store number of operands
		}
		getline(rules_file, temp); // only to make sure ifstream points tonext line
		prev_opcode = opcode;
	}
	m_rules.push_back(complete_rule);
}

void Parser::ParseLine(std::string line)
{
	// first trim spaces of the line
	Helper::LTrim(line);
	if(line.length()==0) return;
	Helper::RTrim(line);
	if(line.length()==0) return;
	// now line has left trimmed string;

	// split the instruction into opcode and operands, which are separated by a space
	std::vector<std::string> instruction;
	instruction = Helper::SplitIntoTwo(line, ' ');
	// now instruction has opcode and operands
	// check if there is no operand, if not store ##
	if(instruction.size()==1)
		instruction.push_back(std::string("##"));

	// check if opcode is correct, first convert to upper case
	Helper::ToUpper(instruction[0]);
	Helper::ToUpper(instruction[1]);

	// if opcode is valid, check for valid operands
	int opIndex = ValidateOpcode(instruction[0]);
	if(opIndex >=0 and opIndex < m_opcodes.size() )
	{
		// validate operands
		ValidateOperands(opIndex, instruction[1]);
	}
	else
	{
		//throw ParseError("invalid opcode");
		std::cout << "invalid opcode";
		//exit(2);
	}
}

int Parser::ValidateOpcode(std::string opcode)
{
	for(unsigned i=0;i<m_opcodes.size();i++)
		if(m_opcodes[i]==opcode)
			return i;
	return -1;
}
				
void Parser::ValidateOperands(int opIndex, std::string operandString)
{
	if(opIndex<0 and opIndex>=m_opcodes.size())
	{
		//throw ParserError("internal error");
		std::cout << "internal error";
		exit(2);
	}
	std::vector<std::string> operands, ruleVec;
	operands = Helper::Split(operandString, ',');
	ruleVec = Helper::Split(m_rules[opIndex], '|');// because, in rules, multiple rules are separated by |

	// now, check for each rule if the operands match or not.
	// if matched in at least one rule, it is accepted

	// FIRST check number of operands match or not
	if(operands.size() != m_numOperands[opIndex])
	{
		std::cout << "invalid operands number";
		exit(2);
	}
	else // operands number match.
	{
		unsigned cnt = 0;
		// check if rules match with operands type
		for(unsigned i=0;i<ruleVec.size();i++)
			cnt+=CheckRule(operandString, ruleVec[i]);// returns true or false

		if (cnt==0)
		{
			std::cout << "invalid operands";
			exit(2);
		}
		else // cnt > 0, should be 1
		{
			std::cout << "Hurray!!!";
		}
	}
}

bool Parser::CheckRule(std::string inputOps, std::string ruleOps)
{
	std::vector<std::string> input = Helper::Split(inputOps, ',');
	std::vector<std::string> rule = Helper::Split(ruleOps, ',');
	if(input.size()!=rule.size())
	{
		std::cout << "internal error";
		exit(2);
	}
	for(int i=0;i<rule.size();i++)
	{
		// first trim input
		Helper::LTrim(input[i]);
		Helper::RTrim(input[i]);
		if(!IsType(input[i], rule[i]))
			return false;
	}
	return true;
}

bool Parser::IsType(std::string operand, std::string type)
{
		if(type=="r")	
			if(operand=="A" || operand=="B" || operand=="C" || operand=="D" || operand=="E" || operand=="H" || operand=="L")
				return true;
			else
				return false;

		else if(type=="m")
			if(operand=="M")
				return true;
			else
				return false;

		else if(type=="rp")
			if(operand=="B" || operand=="D" || operand=="H" || operand=="PSW")
				return true;
			else
				return false;

		else if(type=="sp")
			if(operand=="SP")
				return true;
			else
				return false;

		else if(type=="byt")
		{
			if(operand.length()==1 and !Helper::IsDigit(operand[0])) return false;
			bool dec=true;
			unsigned i;
			for(i=0;i<operand.length()-1;i++) // remaining last is checked for 'H'
			{
				if(!Helper::IsDigit(operand[i]))
					dec = false;
				if(!Helper::IsHex(operand[i]))
					return false;
			}
			// if last is 'H', then length of string should be <= 3 and >=2
			// else if last is digit
			if(operand[i]=='H' and (operand.length()>3 or operand.length()<2))
				return false; // coz one byte cant have 3 or more nibbles
			else if(operand[i]=='H' and operand.length()<=3 and operand.length()>=2)
				return true;
			else
				if(dec==false) // H is not specified but hex digit is entered
					return false;
				else // if no hex digit(A-F) is entered check value <=255
					return (atoi(operand.c_str()) < 256);
		}	

		else if(type=="dbl")
		{
			if(operand.length()==1 and !Helper::IsDigit(operand[0])) return false;
			bool dec=true;
			unsigned i;
			for(i=0;i<operand.length()-1;i++) // remaining last is checked for 'H'
			{
				if(!Helper::IsDigit(operand[i]))
					dec = false;
				if(!Helper::IsHex(operand[i]))
					return false;
			}
			// if last is 'H', then length of string should be <= 3 and >=2
			// else if last is digit
			if(operand[i]=='H' and (operand.length()>5 or operand.length()<2))
				return false; // coz one byte cant have 3 or more nibbles
			else if(operand[i]=='H' and operand.length()<=5 and operand.length()>=2)
				return true;
			else
				if(dec==false) // H is not specified but hex digit is entered
					return false;
				else // if no hex digit(A-F) is entered check value <=255
					return (atoi(operand.c_str()) < 65536);
		}
		else if(type=="nop")
			return operand=="##";

		else return false;
}

void Parser::ParseFile(std::string filename)
{
}
	
