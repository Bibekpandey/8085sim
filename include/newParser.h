#pragma once
#include <vector>
#include <fstream>
#include <Types.h>

class NewParser
{
	public:
		NewParser() {}

		// to initialize the list of opcodes and the arguments
		void Initialize(std::string file);

	private:
		std::vector<int> m_opcodes;
		std::vector<Instruction> m_instructions;
};
