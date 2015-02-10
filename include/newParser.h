#pragma once
#include <vector>
#include <Memory.h>
#include <fstream>
#include <Types.h>

class ParseError
{
	ParseError(std::string s, int line)
	{
		std::cout << "parse Error: " << s << " on line "<<line<< std::endl;
	}
};
class NewParser
{
	public:
		NewParser(int start) : m_startAddress(start) {}

		// to initialize the list of opcodes and the arguments
		void Initialize(std::string file);

		// load the opcodes [OPCODES ONLY], pass them to memory
		void LoadOpcodes(std::string file, Memory& mem);

		// load the mnemomics, pass them to memory
		void LoadMnemonics(std::string file, Memory& mem);

		// load the labels, for mnemonic parser only
		void LoadLabels(std::ifstream& input);

		// check if label exists, for mnemonic parser only
		bool LabelExists(std::string label);

		// get opcode and values of arguments
		std::vector<int> GetOpcodeAndValues(std::string command, std::vector<std::string>& args);

		// command exists or not
		bool CommandExists(std::string command);

		// get first index of the instruction with given command
		int GetInstructionIndex(std::string command);

		// assign values of unassigned labels to the respective address
		void AssignValues(Memory& mem);

		// get label index
		int GetLabelIndex(std::string label);


	protected:
		std::vector<int> m_opcodes;
		std::vector<Instruction> m_instructions;
		std::vector<std::string> m_commands; // commands like MOV, MVI, NOP, DCR, etc.
		std::vector<std::string> m_labels;
		std::vector<int>m_labelIndices;
		int m_startAddress;
		std::vector<int> m_labelAddresses; // storing addresses of labels
		std::vector<int> m_unfilledAddresses; // storing unfilled address, i.e later replacing the labels with the addresses
		std::vector<std::string> m_unfilledAddLabel; // storing the label name whose address is to be filled in unfilledAddress
};
