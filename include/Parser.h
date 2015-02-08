#pragma once
#include <vector>
#include <fstream>
#include <Types.h>

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

			// check if the input operands match the type in rule, if match, creates Instruction object containing opcode and arguments
			bool CheckRule(std::string inpOperands, std::string ruleOperands, int opindex);
			
			// get the argument according to type of input operand 
			Argument GetArgument(std::string operand, std::string type);

			// store all the labels present in m_labels
			void GetLabels(std::ifstream& input);

			// check if the label exists or not
			bool LabelExists(std::string label);

	private:
			std::vector<std::string> m_opcodes; // contains all the opcodes
			std::vector<std::string> m_rules; // contains rules for the corresponding opcodes(related by the index number in the vector)
			std::vector<unsigned> m_numOperands; // stores number of operands for corresponding opcodes
			std::vector<std::string> m_labels; // to store the labels

			std::vector<Instruction> m_instructions; // to store instructions(command and arguments e.g MOV A, B > MOV is command, 
			// A and B are arguments
};
