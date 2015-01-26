#pragma once
#include <vector>
#include <fstream>

class Parser
{
	public:
			Parser() {}

			// to initialize the list of opcodes and rules from a file
			void Initialize(std::string rulefile);

			// parse a line
			bool ParseLine(std::string line);
			
			// parse whole file containing the assembly code
			void ParseFile(std::string filename);

			// validate opcode, returns the index of opcode, -1 if invalid
			int ValidateOpcode(std::string opcode);

			// validate the operands by comparing with the rules
			bool ValidateOperands(int opIndex, std::string operands);

			// check if the input operands match the type in rule
			bool CheckRule(std::string inpOperands, std::string ruleOperands);
			
			// check if types match
			bool IsType(std::string, std::string);

	private:
			std::vector<std::string> m_opcodes; // contains all the opcodes
			std::vector<std::string> m_rules; // contains rules for the corresponding opcodes(related by the index number in the vector)
			std::vector<unsigned> m_numOperands; // stores number of operands for corresponding opcodes
};
