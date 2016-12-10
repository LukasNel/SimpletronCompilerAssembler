#pragma once
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <stack>
#include <sstream>
using namespace std;
#define CASELESS_EQUAL(a, b) ((!to_uppercase(a).compare(to_uppercase(string(b)))) ? 1 : 0)
#define STR_EQUAL(a, b) (((!a.compare(b))) ? 1 : 0)
enum Command : int {
	None,
	Remark,
	Goto,
	Input,
	Let,
	Print,
	If,
	End
};
#define SYMBOLTABLESIZE 2000
struct compiler_tableEntry {
	int symbol;
	char type;
	int location;
	bool hasGoto;
	bool operator== (compiler_tableEntry ftEntry) {
		return (symbol == ftEntry.symbol && type == ftEntry.type) ? 1 : 0;
	}
	compiler_tableEntry() {
		symbol = NULL;
		type = NULL;
		location = -1;
	}
	compiler_tableEntry(int fsymbol,char ftype,int flocation) {
		symbol = fsymbol;
		type = ftype;
		location = flocation;
		hasGoto = false;
	}

};
#include "CompilerForSimpletron.cpp"