#include <iostream>
#include <vector>
#include <Parser.h>
#include <Helper.h>

int main()
{
	std::string s = "mov a, b";
	std::string t = "A, B";

	/*std::vector<std::string> splitted;
	splitted = Helper::Split(s, '|');
	for(int i=0;i<splitted.size();i++)
		std::cout << splitted[i] << "\n";
		*/
	Parser p;
	p.Initialize("rules.txt");
	p.ParseLine(s);
}
