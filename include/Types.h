#pragma once

enum Type { NONE, EMPTY, MEMORY, REGISTER, REG_PAIR, BYTE, DOUBLE, LABEL, STACK_POINTER};

struct Argument
{
	Argument(Type t=NONE, std::string val=""):type(t), value(val)
	{
		if(t==MEMORY or t==REGISTER or t==NONE or t==EMPTY or t==REG_PAIR) size=0; 
		else if(t==BYTE) size=1;
		else if(t==DOUBLE or t==LABEL) size=2;
		else throw 1; // throw exception, exception class remains to be written
	}

	void SetValue(std::string str) { value = str; }

	Type type;
	std::string value;
	unsigned size;
};


struct Instruction
{
		Argument arg1, arg2;
		std::string command;

		Instruction(std::string commnd, Argument a1, Argument a2):
			command(commnd),arg1(a1), arg2(a2)
		{}
};

/*
std::ostream& operator <<(std::ostream& os, const Instruction& i)
{
	os << i.command << " > " << i.arg1.value << " , " << i.arg2.value << std::endl;
	return os;
}
*/

inline Type GetType(std::string a, std::string command)
{
	if(a == "BYT")
		return BYTE;
	else if (a == "DBL")
		return DOUBLE;
	else if (a == "A" or a=="B" or a=="C" or a=="D" or a=="E" or a=="H" or a=="L")
		return REGISTER;
	else if(a=="M")
		return MEMORY;
	else if ((a=="B" or a=="D" or a=="H" or a=="PSW") and 
			(command=="PUSH" or command=="POP" or command=="DCX" or command=="INX"))
		return REG_PAIR;
	else if (a=="SP")
		return STACK_POINTER;
	else if (a=="NOP")
		return EMPTY;
	else
		return NONE;
}
