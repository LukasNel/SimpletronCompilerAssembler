#pragma once
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;
#define SYMBOLTABLESIZE 2000
#define MEMSIZE 10000
struct tableEntry {
	string symbol;
	int location;
};
vector<string> split(const string &s, char delim);
string SimpletronAssembler(const char* filename);
#include "SimpletronAssembler.cpp"