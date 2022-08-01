#pragma once
#include <iostream>
#include <string>
#include <map>

using namespace std;

typedef void(*Function)(const string&);

static class Command
{
public:
	static void callFunciton(const string& Funtion);

private:
	static map<string, Function>FuntionList;

	static void funtionA(const string& Args) {
		cout << "this is functionA :" << Args << endl;
	}

	static void funtionB(const string& Args) {
		cout << "this is functionB :" << Args << endl;
	}

	static void funtionC(const string& Args) {
		cout << "this is functionC :" << Args <<endl;
	}

	static void funtionD(const string& Args) {
		cout << "this is functionD :" << Args << endl;
	}
};

