#include "SimpletronAssembler.h"
vector<string> split(const string &s, char delim) {
	vector<string> elems;
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

int checkToken(string fstr) {
	if (!fstr.compare("R")) { return 10; }
	else if (!fstr.compare("W")) { return 11; }
	else if (!fstr.compare("L")) { return 20; }
	else if (!fstr.compare("ST")) { return 21; }
	else if (!fstr.compare("A")) { return 30; }
	else if (!fstr.compare("S")) { return 31; }
	else if (!fstr.compare("M")) { return 32; }
	else if (!fstr.compare("D")) { return 33; }
	else if (!fstr.compare("B")) { return 40; }
	else if (!fstr.compare("BN")) { return 41; }
	else if (!fstr.compare("BZ")) { return 42; }
	else if (!fstr.compare("H")) { return 43; }
//	else if (!fstr.compare("I")) { return 50; }
	else { return 0; }

}
int SearchSymbolTable(tableEntry* fsymtable, int fsize, const char* fsearchstring) {
	int tIndex = -1;
	for (int i = 0; i < fsize; i++) {
		if (!fsymtable[i].symbol.compare(fsearchstring)) tIndex = i;
	}
	return tIndex;
}
int EnterIntoSymbolTable(tableEntry* symboltable, int& symboltable_counter, string token_str) {
	int tIndex = SearchSymbolTable(symboltable, symboltable_counter, token_str.c_str());
	if (tIndex == -1) {
		symboltable[symboltable_counter].symbol = token_str;
		tIndex = symboltable_counter;
		symboltable_counter++;
	}
	return tIndex;
}
int StandardTokenInsertion(int* mem, int& instruction_counter, vector<string>& tokens, int command_id, tableEntry* symboltable, int& symboltable_counter, int* flags, int cur_position_in_tokens) {
	mem[instruction_counter] = command_id * 10000;
	if (tokens.size() > cur_position_in_tokens) {
		flags[instruction_counter] = EnterIntoSymbolTable(symboltable, symboltable_counter, tokens[cur_position_in_tokens]);
	}
	instruction_counter++;
	return instruction_counter;
}
int StorageCustomTokenInsertion(int* mem, int& instruction_counter, vector<string>& tokens, tableEntry* symboltable, int& symboltable_counter, int* flags, int cur_position_in_tokens) {
	int tIndex = SearchSymbolTable(symboltable, symboltable_counter, tokens[cur_position_in_tokens].c_str());
	if (tIndex != -1) symboltable[tIndex].location = instruction_counter;

	EnterIntoSymbolTable(symboltable, symboltable_counter, tokens[cur_position_in_tokens]);

	if (tokens.size() > cur_position_in_tokens + 1) {
		int command_id = checkToken(tokens[cur_position_in_tokens + 1]);
		if (!command_id) {
			if (!tokens[1].compare("DS")) {
				mem[instruction_counter++] = 0;
			}
			else if (!tokens[1].compare("DC") && tokens.size() > cur_position_in_tokens + 2) {
				int tVar = atoi(tokens[cur_position_in_tokens + 2].c_str());
				mem[instruction_counter++] = (tVar > -99999 && tVar < 99999) ? tVar : 0;
			}
		}
		else {
			StandardTokenInsertion(mem, instruction_counter, tokens, command_id, symboltable, symboltable_counter, flags, cur_position_in_tokens + 2);
		}
	}
	return instruction_counter;
}
string SimpletronAssembler(const char* filename) {
	ifstream filereader(filename);
	string line = "";
	string output = "";
	int linenum = 0;

	int instruction_counter = 0;
	int symboltable_counter = 0;
	int ndata = 0;

	int memory_location_counter = 9999;
	tableEntry symboltable[SYMBOLTABLESIZE];
	int mem[MEMSIZE];
	int data[MEMSIZE];
	int flags[MEMSIZE];
	for (int i = 0; i < MEMSIZE; i++) {
		mem[i] = 24242;
		data[i] = 0;
		flags[i] = -1;
	}
	for (int i = 0; i < SYMBOLTABLESIZE; i++) {
		symboltable[i].symbol = "";
		symboltable[i].location = -1;
	}

	if (filereader.is_open()) {
		while (getline(filereader, line)) {
			vector<string> tokens = split(line, ' ');
			tokens.erase(std::remove(tokens.begin(), tokens.end(), ""), tokens.end());

			int tempNum = checkToken(tokens[0]);
			if (tokens.size() > 1 && !tokens[0].compare("I"))
				data[ndata++] = atoi(tokens[1].c_str());
			else if (!tempNum) 
			        StorageCustomTokenInsertion(mem, instruction_counter, tokens, symboltable, symboltable_counter, flags, 0);
			else 
				StandardTokenInsertion(mem, instruction_counter, tokens, tempNum, symboltable, symboltable_counter, flags, 1);
		}
		
		for (int i = 0; i < instruction_counter; i++) {
			if (flags[i] != -1)
				mem[i] += symboltable[flags[i]].location;
		}
		
		mem[instruction_counter++] = -999999;
		
		for (int i = 0; i < ndata; i++) {
			mem[instruction_counter++] = data[i];
		}
		
		filereader.close();
	}
	
	line = "siml_";
	ofstream fileout(line + filename);

	for (int i = 0; i < instruction_counter; i++)  {
		output += to_string(mem[i]) + "\n";
	}


	fileout << output << "buckchecker";
	fileout.close();
	return line + filename;
}
