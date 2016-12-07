#include "CompilerForSimpletron.h"
int SearchSymbolTable(compiler_tableEntry* fsymtable, int symbol_counter, compiler_tableEntry ftEntry) {
	int tIndex = -1;
	for (int i = 0; i < symbol_counter; i++) {
		if (fsymtable[i] == ftEntry) tIndex = i;
	}
	return tIndex;
}
inline bool StringIsNum(string fstr) {
	for (int i = 0; i < fstr.length(); i++)if (fstr[i] < '0' || fstr[i] > '9')return false;
	return true;
}
int InsertLineNumberSymbol(compiler_tableEntry* fsymtable, int linenum, int& symbol_counter, int instruction_counter) {
	compiler_tableEntry linenumEntry(linenum,'L',instruction_counter);
	int tIndex = SearchSymbolTable(fsymtable, symbol_counter, linenumEntry);
	if (tIndex == -1) tIndex = symbol_counter++;
	fsymtable[tIndex] = linenumEntry;
	return tIndex;
}
int InsertVariableSymbol(compiler_tableEntry* fsymtable, char variable, int& symbol_counter, int instruction_counter) {
	compiler_tableEntry varEntry(variable, 'V', -1);
	int tIndex = SearchSymbolTable(fsymtable, symbol_counter, varEntry);
	if (tIndex == -1) tIndex = symbol_counter++;
	fsymtable[tIndex] = varEntry;
	return tIndex;
}
int InsertSymbol(compiler_tableEntry* fsymtable, int value,char type, int location, int& symbol_counter) {
	compiler_tableEntry varEntry(value, type, location);
	int tIndex = SearchSymbolTable(fsymtable, symbol_counter, varEntry);
	if (tIndex == -1) tIndex = symbol_counter++;
	fsymtable[tIndex] = varEntry;
	return tIndex;
}
string to_uppercase(string fstr) {
	string outStr = "";
	for (int i = 0; i < fstr.length(); i++)outStr += toupper(fstr[i]);
	return outStr;
}	
Command checkCompilerTokens(string fstr) {
	if (CASELESS_EQUAL(fstr,"rem")) { return Remark; }
	else if (CASELESS_EQUAL(fstr, "input")) { return Input; }
	else if (CASELESS_EQUAL(fstr, "let")) { return Let; }
	else if (CASELESS_EQUAL(fstr, "print")) { return Print; }
	else if (CASELESS_EQUAL(fstr, "goto")) { return Goto; }
	else if (CASELESS_EQUAL(fstr, "if")) { return If; }
	else if (CASELESS_EQUAL(fstr, "end")) { return End; }
	/*
	     if (!to_uppercase(fstr).compare(to_uppercase(string("rem")))) { return Remark; }
	else if (!to_uppercase(fstr).compare(to_uppercase(string("input")))) { return Input; }
	else if (!to_uppercase(fstr).compare(to_uppercase(string("let")))) { return Let; }
	else if (!to_uppercase(fstr).compare(to_uppercase(string("print")))) { return Print; }
	else if (!to_uppercase(fstr).compare(to_uppercase(string("goto")))) { return Goto; }
	else if (!to_uppercase(fstr).compare(to_uppercase(string("if")))) { return If; }
	else if (!to_uppercase(fstr).compare(to_uppercase(string("end")))) { return End; }
	*/
	return None;

}
string ProcessIfStatement() {
	string output = "";
	string checkStatement = "";
	for (int i = 2; i < tokens.size && !to_uppercase(fstr).compare(to_uppercase(string("goto"))) ;)
}
string SimpletronCompiler(const char* filename) {
	ifstream filereader(filename);
	string line = "";
	string output = "";
	string tStr = "";
	int linenum = 0;

	int instruction_counter = 0;
	int symboltable_counter = 0;
	int ndata = 0;

	int memory_location_counter = 9999;
	compiler_tableEntry symboltable[SYMBOLTABLESIZE];
	vector<string> mem;
	int data[MEMSIZE];
	int flags[MEMSIZE];
	for (int i = 0; i < MEMSIZE; i++) {
		data[i] = 0;
		flags[i] = -1;
	}
	for (int i = 0; i < SYMBOLTABLESIZE; i++) {
		symboltable[i].symbol = NULL;
		symboltable[i].type = NULL;
		symboltable[i].location = -1;
	}

	if (filereader.is_open()) {
		while (getline(filereader, line)) {
			output = "";
			vector<string> tokens = split(line, ' ');
			tokens.erase(std::remove(tokens.begin(), tokens.end(), ""), tokens.end());
			if (tokens.size() <= 1)continue;
			if (StringIsNum(tokens[0])) {
				InsertSymbol(symboltable,atoi(tokens[0].c_str()),'L', instruction_counter,symboltable_counter);
			}else {
				cerr << "Line number is incorrect or missing on line " << linenum << ";\n";
				continue;
			}
			if (tokens.size() < 2) {
				cerr << "Not enough commands on line " << linenum << ";\n";
				continue;
			}
			Command cur_command = checkCompilerTokens(tokens[1]);
			if (cur_command == None) {
				cerr << "A command is required on line " << linenum << ";\n";
				continue;
			}
			else if (cur_command == Remark) {
				continue;
			}
			//End has only 2 tokens, and the rest of the commands need three, so in order to limit the amount of if statements, End is moved here
			else if (cur_command == End) {
				output += "H";
				mem.push_back(output);
				continue;
			}
			if (tokens.size() < 3) {
				cerr << "Not enough commands on line " << linenum << ";\n";
				continue;
			}
			switch (cur_command) {
			case Goto:
				output += "B ";
				if (!StringIsNum(tokens[2])) {	
					cerr << "Line number is incorrect on line " << linenum << ";\n";
					goto EndOfLine;
				}
				output += tokens[2] + "goto";
				flags[instruction_counter++] = atoi(tokens[2].c_str());
				break;
			case Input:
				output += "R ";
				output += tokens[2][0];
				InsertSymbol(symboltable,(int)tokens[2][0],'V',-1,symboltable_counter);
				instruction_counter++;
				break;
			case Let:break;
			case Print:
				output += "W ";
				output += tokens[2][0];
				InsertSymbol(symboltable, (int)tokens[2][0], 'V', -1, symboltable_counter);
				instruction_counter++;
				break;
			case If:break;
			default:break;
			}
			mem.push_back(output);
			EndOfLine:
			linenum++;
		}
		for (int i = 0; i < instruction_counter; i++) {
			if (flags[i] != -1) {
				int tIndex = SearchSymbolTable(symboltable, symboltable_counter,compiler_tableEntry(flags[i],'L',-1));
				if (tIndex == -1) {
					cerr << "Goto is fecked on line " << i << ";\n";
				}
				else {
					tStr = to_string(flags[i]) + "goto ";
					mem[symboltable[tIndex].location] = tStr + mem[symboltable[tIndex].location];
				}
			}
		}
		for (int i = 0; i < symboltable_counter; i++) {
			switch (symboltable[i].type) {
			case 'V':
				tStr = (char)symboltable[i].symbol;
				tStr += " ";
				tStr += "DS";
				mem.push_back(tStr);
				break;
			case 'C':break;
			default:break;
			}
		}

		filereader.close();
	}
	line = "sasm_";
	ofstream fileout(line + filename);
	string outStr = "";
	for (int i = 0; i < mem.size(); i++) outStr += mem[i] + "\n";

	fileout << outStr << "buckchecker";
	fileout.close();
	return line + filename;
}
