#include "exampleWindow.h"
#include <gtkmm.h>
#include <thread>
#include <iostream>
#include <vector>
#include <newParser.h>
#include <Processor.h>

Processor processor;
NewParser parser(8000); // 12345 is starting address

int main(int argc, char *argv[])
{
	parser.Initialize("opcodes.txt");
	//parser.LoadMnemonics("program.txt", processor.m_memory);


    Gtk::Main kit(argc, argv);
	ExampleWindow window;

    //processor.Initialize(&parser, &window);
//Shows the window and returns when it is closed.
	Gtk::Main::run(window);

	return 0;

}
 

