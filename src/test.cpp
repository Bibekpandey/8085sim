#include <iostream>
#include <vector>
#include <newParser.h>
#include <Processor.h>
#include <Share.h>
#include <exampleWindow.h>

void executeFile(ExampleWindow* window, Share_Resource &share_resource)
{
    try
    {
	    //processor.Run(share_resource);
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
