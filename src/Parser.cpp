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

	std::string complete_rule=""; // in case opcode has multiple rules
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
			if(cnt>0) // because, for the first time there would be nothing in the rule
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

bool Parser::ParseLine(std::string line)
{
	// first trim spaces of the line
	Helper::LTrim(line);
	if(line.length()==0) return true;
	Helper::RTrim(line);
	if(line.length()==0) return true;
	// now line has left and right trimmed string;
	// check if it is comment
	if(line[0]==';')
		return true;

	// split into two with delimiter ';' and process the first element
	std::vector<std::string> temp = Helper::SplitIntoTwo(line, ';');

	// again check for labels
	std::vector<std::string> labelled = Helper::SplitIntoTwo(temp[0], ':');
	std::string instruction;
	std::string label="";
	// the first element has label, if exists
	if(labelled.size()==2) // means there is label
	{
		label = labelled[0];
		instruction = labelled[1];
		Helper::LTrim(instruction);
	}
	else // there is no label,ie. labelled has size 1
		instruction = labelled[0];

	// split the instruction into opcode and operands, which are separated by a space
	std::vector<std::string> opcodeOperands;
	opcodeOperands = Helper::SplitIntoTwo(instruction, ' ');

	// if opcodeOperands has only one element add ## 
	if (opcodeOperands.size()==1)
		opcodeOperands.push_back(std::string("##"));

	// now opcodeOperands has opcode and operands
	// check if there is no operand, if not store ##

	// check if opcode is correct, first convert to upper case
	opcodeOperands[0] = Helper::ToUpper(opcodeOperands[0]);
	opcodeOperands[1] = Helper::ToUpper(opcodeOperands[1]);

	// if opcode is valid, check for valid operands
	int opIndex = ValidateOpcode(opcodeOperands[0]);
	if(opIndex >=0 and opIndex < m_opcodes.size() )
	{
		// validate operands
		return ValidateOperands(opIndex, opcodeOperands[1]);
	}
	else
	{
		//throw ParseError("invalid opcode");
		std::cout << "invalid opcode ";
		return false;
	}
}

int Parser::ValidateOpcode(std::string opcode)
{
	for(unsigned i=0;i<m_opcodes.size();i++)
		if(m_opcodes[i]==opcode)
			return i;
	return -1;
}
				
bool Parser::ValidateOperands(int opIndex, std::string operandString)
{
	if(opIndex<0 and opIndex>=m_opcodes.size())
	{
		//throw ParserError("internal error");
		std::cout << "internal error ";
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
		std::cout << "invalid number of operands ";
		return false;
	}
	else // operands number match.
	{
		// check if rules match with operands type
		for(unsigned i=0;i<ruleVec.size();i++)
			if(CheckRule(operandString, ruleVec[i], opIndex)) // returns true or false
				return true;
		// no rules for operands match, return false
		std::cout << "invalid operands ";
		return false;
	}
}

bool Parser::CheckRule(std::string inputOps, std::string ruleOps, int opIndex)
{
	std::vector<std::string> input = Helper::Split(inputOps, ',');
	std::vector<std::string> rule = Helper::Split(ruleOps, ',');
	if(input.size()!=rule.size())
	{
		std::cout << "internal error";
		exit(2);
	}
	// get the arguments, if argument type is NONE return false
		Argument a1, a2;
		if(rule.size()==1)
		{
			Helper::LTrim(input[0]);
			Helper::RTrim(input[0]);
			a1 = GetArgument(input[0],rule[0]);
		}
		else if(rule.size()==2)
		{
			Helper::LTrim(input[0]);Helper::LTrim(input[1]);
			Helper::RTrim(input[0]);Helper::RTrim(input[1]);
			a1 = GetArgument(input[0], rule[0]);
			a2 = GetArgument(input[1], rule[1]);
		}
		else
			return false;

		// check type
		if(a1.type==NONE and a2.type==NONE)
			return false;
		else  // create instruction object
		{
			m_instructions.push_back(Instruction(m_opcodes[opIndex], a1, a2));
			return true;
		}
}

Argument Parser::GetArgument(std::string operand, std::string type)
{
		if(type=="r")	
		{
			if(operand=="A" || operand=="B" || operand=="C" || operand=="D" || operand=="E" || operand=="H" || operand=="L")
				return Argument(REGISTER, operand);
		}
		else if(type=="m")
		{
			if(operand=="M")
				return Argument(MEMORY, operand);
		}
		else if(type=="rp") 
		{
			if(operand=="B" || operand=="D" || operand=="H" || operand=="PSW")
				return Argument(REG_PAIR, operand);
		}
		else if(type=="sp")
		{
			if(operand=="SP")
				return Argument(STACK_POINTER, operand);
		}
		else if(type=="byt")
		{
			if(Helper::IsHexStr(operand) and operand.length()==2)
				return Argument(BYTE, operand);
		}
		else if(type=="dbl")
		{
			if(Helper::IsHexStr(operand) and operand.length()==4)
				return Argument(DOUBLE, operand);
		}
		else if(type=="nop")
		{
			if(operand=="##")
				return Argument(EMPTY, std::string(""));
		}
		else if(type=="lbl")
		{
			if (LabelExists(operand))
				return Argument(LABEL, operand);
		}

		return Argument(NONE, std::string(""));
}

void Parser::ParseFile(std::string filename)
{
	std::ifstream input(filename);

	// first make list of the labels
	GetLabels(input);

	// come back to the beginning of stream to parse further
	input.clear();
	input.seekg(0, std::ios::beg);

	std::string line;
	int cnt=1;
	bool error = false;
	while(getline(input, line))
	{
		if(!ParseLine(line))
		{
			std::cout << "on line " << cnt << std::endl;
			error= true;
		}
		cnt++;
	}
	if(!error)
		std::cout << "all is well!!\n";

	input.close();
	
	std::cout << "THE INSTRUCTIONS:\n";
	for(int i=0;i<m_instructions.size();i++)
		std::cout<<m_instructions[i].command << " > "<< m_instructions[i].arg1.value << ", "<<m_instructions[i].arg2.value<< std::endl;
}
	
void Parser::GetLabels(std::ifstream& input)
{
	std::string line;
	std::vector<std::string> splitted;
	
	while(getline(input, line))
	{
		Helper::LTrim(line);
		Helper::RTrim(line);
		if(line.length()==0) // means no thing in the line
			continue;
		// split the line and assign it into splitted vector
		splitted = Helper::SplitIntoTwo(line, ':');
		if(splitted.size() <=1) // means no label in the line
			continue;
		else // splitted.size()==2, there is a label
			m_labels.push_back(splitted[0]);
	}
}

bool Parser::LabelExists(std::string label)
{
	for(int i=0;i<m_labels.size();i++)
	{
		if(Helper::ToUpper(m_labels[i])==label)
			return true;
	}
	return false;
}
