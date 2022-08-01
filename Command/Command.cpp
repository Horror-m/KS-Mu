#include "Command.h"

void Command::callFunciton(const string& Funtion)
{
	auto FindPos = Funtion.find(" ");
	string FuntionName = FindPos == string::npos ? Funtion : Funtion.substr(0, FindPos);
	string FuntionArgs = FindPos == string::npos ? "" : Funtion.substr(FindPos + 1, Funtion.length());
	auto finder = FuntionList.find(FuntionName);
	if (finder != FuntionList.end())
		finder->second(FuntionArgs);
}

map<string, Function> Command::FuntionList = {
	{"funcitonA",Command::funtionA},
	{"funcitonB",Command::funtionB},
	{"funcitonC",Command::funtionC},
	{"funcitonD",Command::funtionD},
};
