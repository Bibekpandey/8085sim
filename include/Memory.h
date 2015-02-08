#pragma once
#include <iostream>
#include <vector>

class Memory
{
	public:
		Memory()
		{
			for(int i=0;i<1<<14;i++)
				m_values.push_back(0);
		}

		int operator[](int a)
		{
			// assuming 4 bytes of integer
			int temp = m_values[a/4];
			// temp has all the four bytes, of which we need only one byte
			int i=a%4;
			// nth byte is given by a%4
			switch(i)
			{
				case 0:
					return temp & 0x000000ff;
				case 1:
					return (temp & 0x0000ff00) >> 8;
				case 2:
					return (temp & 0x00ff0000) >> 16;
				case 3:
					return (temp & 0xff000000) >> 24;
				default:
					throw 1; // throw some exception
			}
		}

		void SetValue(int index, int value)
		{
			int& temp = m_values[index/4];
			std::cout<< " actual index : " << index/4 << std::endl;
			std::cout << "temp : " << temp << std::endl;

			unsigned i = index%4;

			switch(i)
			{
				case 0:
					temp = temp & 0xffffff00;
					temp  = temp | value;
					break;
				case 1:
					temp = temp & 0xffff00ff;
					std::cout << "temp after anding "<< temp << std::endl;
					temp = temp | (value << 8);
					std::cout << "temp after oring" << temp << std::endl;
					break;
				case 2:
					temp = temp & 0xff00ffff;
					temp = temp | (value << 16);
					break;
				case 3:
					temp = temp & 0x00ffffff;
					temp = temp | (value << 24);
					break;
				default:
					throw 1; // throw some exception;
			}
		}
			
	private:
		std::vector<int> m_values;
};
