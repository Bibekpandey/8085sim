#pragma once
#include <iostream>
#include <vector>

class Memory
{
	public:
		Memory()
		{
			for(int i=0;i<1<<16;i++)
				m_values.push_back(0);
		}

		int operator[](int a)
		{
			if(a<0 or a >= 1<<16)
				throw "invalid size";
			return m_values[a];
		}

		void SetValue(int index, int value)
		{
			if(index<0 or index>= 1<<16)
				throw "invalid size";
			m_values[index] = value;
		}
			
	private:
		std::vector<int> m_values;
};
