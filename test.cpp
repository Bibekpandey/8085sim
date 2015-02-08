#include <iostream>
#include <fstream>
#include "include/Helper.h"

int main()
{
	std::cout << sizeof(int);
	std::ifstream i("ops.txt");
	std::ofstream o("opcodes.txt");
	
	std::string s;
	while(getline(i,s))
	{
		s = Helper::ToUpper(s);
		o << s;
		o << '\n';
	}
	i.close();
	o.close();
}
