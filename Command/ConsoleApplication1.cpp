
#include <iostream>
#include "Command.h"
using namespace std;

int main(int argc, char** argv)
{
	cout << *argv<<endl;

	Command::callFunciton("funcitonB", "-c -v THis is Args");
}

