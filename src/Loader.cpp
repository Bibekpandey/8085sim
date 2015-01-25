#include <iostream>
#include <vector>
#include <Parser.h>
#include <Helper.h>

int main()
{
	std::string s = "mov a;, ;b ;;this is comment";

	/*std::vector<std::string> splitted;
	splitted = Helper::Split(s, ',');
	std::cout << splitted.size();
	for(int i=0;i<splitted.size();i++)
		std::cout << splitted[i] << "\n";
	return 0;
	*/
	Parser p;
	p.Initialize("rules.txt");
	p.ParseLine(s);
}
