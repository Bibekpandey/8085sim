#include "exampleWindow.h"
#include <gtkmm.h>
#include <thread>
#include <iostream>
#include <vector>
#include <newParser.h>
#include <Processor.h>
#include <mutex>
#include <chrono>


void executeFile();

std::mutex mu;

int test = 0;

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);
	ExampleWindow window;

//Shows the window and returns when it is closed.
	Gtk::Main::run(window);

	return 0;

}
 
void executeFile()
{
    try
    {
    Processor processor;
	NewParser p(8000); // 12345 is starting address
	p.Initialize("opcodes.txt");
	p.LoadMnemonics("program1.txt", processor.m_memory);
    processor.Initialize(&p);
    processor.Run();
    }
    catch(std::string a)
    {
        std::cout << a << std::endl;
    }
    catch(const char* s)
    {
        std::cout << s;
    }
}
