#include "AssemblerForSimpletron.h"
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
struct tableEntry {
	string symbol;
	int location;
};

string AssembleIntoMachineCode(const char* filename) {

	string line = "";
	string output = "";
	string zeroes_padding_string = "";
	int linenum = 0;
	ifstream filereader(filename);
	if (filereader.is_open()) {
		while (getline(filereader, line)) {
			vector<string> tokens = split(line, ' ');
			cout << line << endl;
			zeroes_padding_string = "";
			if (tokens.size() > 2) {
				cerr << "Syntax error at Line " << linenum << ", Too many arguments" << endl;
			}else if (tokens.size() > 1) {
				if (atoi(tokens[1].c_str()) < -10000 || atoi(tokens[1].c_str()) > 10000) {
					cerr << "Syntax error at Line " << linenum << ", absolute value of Memory location exceeds 9999" << endl;
				}

				for (int i = 0; i < 4 - tokens[1].length(); i++) {
					zeroes_padding_string += "0";
				}
			} else if (!tokens[0].compare("HALT")) {
				output += "43"; output += "0000"; output += "\n"; 
				break;
			} else {
				cerr << "Syntax error at Line " << linenum << ", no memory location provided." << endl;
			}
			
			if (!tokens[0].compare("READ"))	    { output += "10"; output += zeroes_padding_string + tokens[1] + "\n"; }
		    else if (!tokens[0].compare("WRITE"))		{ output += "11"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("LOAD"))		{ output += "20"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("STORE"))		{ output += "21"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("ADD"))			{ output += "30"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("SUBTRACT"))	{ output += "31"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("DIVIDE"))		{ output += "32"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("MULTIPLY"))	{ output += "33"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("BRANCH"))		{ output += "40"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("BRANCHNEG"))	{ output += "41"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else if (!tokens[0].compare("BRANCHZERO")) { output += "42"; output += zeroes_padding_string + tokens[1] + "\n"; }
			else {
				cerr << "Syntax error at Line " << linenum << ", Unknown operation" <<endl;
			}
			
			linenum++;
		}
		filereader.close();
	}
	
	line = "siml_";
	ofstream fileout(line + filename);
	fileout << output;
	fileout.close();
	return line + filename;
}


int checkToken(string fstr) {
	if (!fstr.compare("R"))			{ return 10; }
	else if (!fstr.compare("W"))	{ return 11; }
	else if (!fstr.compare("L"))	{ return 20; }
	else if (!fstr.compare("ST"))	{ return 21; }
	else if (!fstr.compare("A"))		{ return 30; }
	else if (!fstr.compare("S")) { return 31; }
	else if (!fstr.compare("M"))	{ return 32; }
	else if (!fstr.compare("D")) { return 33; }
	else if (!fstr.compare("B"))	{ return 40; }
	else if (!fstr.compare("BN")) { return 41; }
	else if (!fstr.compare("BZ")) { return 42; }
	else if (!fstr.compare("H")) { return 43; }
	else { return 0; }

}
int SearchSymbolTable(tableEntry* fsymtable, int fsize, const char* fsearchstring) {
	int tIndex = -1;
	for (int i = 0; i < fsize; i++) {
		if(!fsymtable[i].symbol.compare(fsearchstring)) tIndex = i;
	}
	return tIndex;
}
int EnterIntoSymbolTable(tableEntry* symboltable, int* symboltable_counter,int instruction_counter, string token_str) {
	int tIndex = SearchSymbolTable(symboltable, *symboltable_counter, token_str.c_str());
	if (tIndex == -1) {
		//symboltable[symboltable_counter].location = instruction_counter;
		symboltable[*symboltable_counter].symbol = token_str;
		(*symboltable_counter)++;
	}
	return tIndex;
}
string AssembleIntoMachineCodeV2(const char* filename) {
	string line = "";
	string output = "";

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

	int linenum = 0;
	ifstream filereader(filename);
	if (filereader.is_open()) {
		while (getline(filereader, line)) {
			vector<string> tokens = split(line, ' ');
			tokens.erase(std::remove(tokens.begin(), tokens.end(), ""), tokens.end());
			//std::size_t found = line.find("\\");
			//if (found != string::npos)line.replace(line.begin() + found, line.end(), "");
			int tempNum = checkToken(tokens[0]);
			if (!tokens[0].compare("I")) {
				if (tokens.size() > 1) {
					data[ndata++] = atoi(tokens[1].c_str());
				}
			}else if (!tempNum) {
				int tIndex = SearchSymbolTable(symboltable, symboltable_counter, tokens[0].c_str());
				if (tIndex == -1) {
					//symboltable[symboltable_counter].location = instruction_counter;
					symboltable[symboltable_counter++].symbol = tokens[0];
				}else {
					symboltable[tIndex].location = instruction_counter;
				}
				tempNum = checkToken(tokens[1]);
				if (!(tempNum)) {
					if (tokens.size() > 1) {
						if (!tokens[1].compare("DS")) {
							mem[instruction_counter++] = 0;

						}
						else if (!tokens[1].compare("DC") && tokens.size() > 2) {
							int tVar = atoi(tokens[2].c_str());
							mem[instruction_counter++] = (tVar > -99999 && tVar < 99999) ? tVar : 0;
						}
					}
				}else {
					mem[instruction_counter] = tempNum * 10000;
					if (tokens.size() > 2) {
						int tIndex = SearchSymbolTable(symboltable, symboltable_counter, tokens[2].c_str());
						if (tIndex == -1) {
							//symboltable[symboltable_counter].location = instruction_counter;
							symboltable[symboltable_counter].symbol = tokens[2];
							flags[instruction_counter] = symboltable_counter;
							symboltable_counter++;
						}
						else {
							flags[instruction_counter] = tIndex;
						}
					}
					instruction_counter++;
				}
			}else {
				mem[instruction_counter] = tempNum*10000;
				if (tokens.size() > 1) {
					int tIndex = SearchSymbolTable(symboltable, symboltable_counter, tokens[1].c_str());
					if (tIndex == -1) {
						//symboltable[symboltable_counter].location = instruction_counter;
						symboltable[symboltable_counter].symbol = tokens[1];
						flags[instruction_counter] = symboltable_counter;
						symboltable_counter++;
					}
					else {
						flags[instruction_counter] = tIndex;
					}
				}
				instruction_counter++;
			}
		}
		for (int i = 0; i < instruction_counter; i++) {
			if (flags[i] != -1) {
				mem[i] += symboltable[flags[i]].location;
			}
		}
		mem[instruction_counter++] = -999999;
		for (int i = 0; i < ndata; i++) {
			mem[instruction_counter++] = data[i];
		}
		filereader.close();
	}
	line = "siml_";
	ofstream fileout(line + filename);
	for (int i = 0; i < instruction_counter; i++) {
		
		output += to_string(mem[i]);
		output += "\n";
	}
	
	fileout << output << "buckchecker";
	fileout.close();
	return line + filename;
}

