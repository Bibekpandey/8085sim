#include "exampleWindow.h"
#include <gtkmm.h>
#include <thread>
#include <iostream>
#include <vector>
#include <newParser.h>
#include <Processor.h>
#include <mutex>
#include <chrono>



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
 

