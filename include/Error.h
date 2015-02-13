#pragma once
#include <iostream>

class Error
{
	Error(std::string error, int n=0):m_message(error), m_number(n) {}
	void GetMessage()
	{
		std::cout << "Error: " << m_message << " on line " << m_number;
	}
protected:
	std::string m_message;
	int number;
}
