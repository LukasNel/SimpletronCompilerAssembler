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

vector<string> split(const string &s, char delim);
string AssembleIntoMachineCode(const char* filename);
string AssembleIntoMachineCodeV2(const char* filename);
#include "AssemblerForSimpletron.cpp"