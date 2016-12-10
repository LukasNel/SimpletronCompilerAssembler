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
	return None;

}
int checkPrecedence(char op) {
	switch (op) {
	case '+':return 1; break;
	case '-':return 1; break;
	case '*':return 2; break;
	case '/':return 2; break;
	default: return 0; break;
	}
}
char EvaluateString(string expression,vector<string>& mem,char varName,compiler_tableEntry* fsymtable,int& symbol_counter,int& instruction_counter) {
	vector<string> output;
	stack<char> operator_stack;
	string curVariable = "";
	for (int i = 0; i < expression.length(); i++) {
		if (checkPrecedence(expression[i])) {
			output.push_back(curVariable);
			while (!operator_stack.empty() && checkPrecedence(expression[i]) < checkPrecedence(operator_stack.top())) {
				curVariable = "";
				curVariable += operator_stack.top();
				output.push_back(curVariable);
				operator_stack.pop();
			}
			operator_stack.push(expression[i]);
			curVariable = "";
		}
		else {
			curVariable += expression[i];
		}
	}
	output.push_back(curVariable);
	while (!operator_stack.empty()) {
		curVariable = "";
		curVariable += operator_stack.top();
		output.push_back(curVariable);
		operator_stack.pop();
	}
	if (StringIsNum(output[0])) {
		InsertSymbol(fsymtable, atoi(output[0].c_str()), 'C', -1, symbol_counter);
		mem.push_back(string("L ") + output[0]);
	}
	else {
		InsertSymbol(fsymtable, output[0][0], 'V', -1, symbol_counter);
		mem.push_back(string("L ") + output[0][0]);
	}
	instruction_counter++;
	for (int i = 1; i < output.size(); i += 2) {
		curVariable = "";
		switch(output[i+1][0]){
		case '+':curVariable += "A "; break;
		case '-':curVariable += "S "; break;
		case '*':curVariable += "M "; break;
		case '/':curVariable += "D "; break;
		default:cerr << "Operator Used Incorrectly" << endl;
		}
		curVariable += output[i][0];
		if (StringIsNum(output[i])) {
			InsertSymbol(fsymtable, atoi(output[0].c_str()), 'C', -1, symbol_counter);
		}
		else {
			InsertSymbol(fsymtable, output[0][0], 'V', -1, symbol_counter);
		}
		mem.push_back(curVariable);
		instruction_counter++;
	}
	InsertSymbol(fsymtable, varName, 'V', -1, symbol_counter);
	mem.push_back(string("ST ") + varName);
	instruction_counter++;
	return varName;
}
string ProcessIfStatement(vector<string> tokens,int* flags, int& instruction_counter,compiler_tableEntry* fsymtable,vector<string>& mem,int& symbol_counter) {
	string output = "";
	string checkStatement = "";
	vector<string> expressionList;	
	expressionList.push_back("");
	short ExpressionNumber = 0;
	short GotoPosition = 0;
	for (int i = 2; i < tokens.size(); i++) {
		if (CASELESS_EQUAL(tokens[i], "goto")) {
			GotoPosition = i;
			break;
		}
		checkStatement += tokens[i];
	}
	for (int i = 0; i < checkStatement.length(); i++) {
		switch (ExpressionNumber) {
		case 0:
			if (checkStatement[i] == '=' || checkStatement[i] == '<' || checkStatement[i] == '>') {
				ExpressionNumber = 1;
				expressionList.push_back("");
			}
			break;
		case 1:
			if (!(checkStatement[i] == '=' || checkStatement[i] == '<' || checkStatement[i] == '>') && ExpressionNumber > 0) {
				ExpressionNumber = 2;
				expressionList.push_back("");
			}
			break;
		}
		expressionList[ExpressionNumber] += checkStatement[i];
	}
	if (expressionList.size() < 3) {
		cerr << "missing a expression" << endl;
		return "";
	}
	switch (expressionList[1][0]) {
		case '>':
			EvaluateString(expressionList[0], mem, '?', fsymtable, symbol_counter, instruction_counter);
			EvaluateString(expressionList[2], mem, '!', fsymtable, symbol_counter, instruction_counter);
			mem.push_back(string("L ") + '?');
			mem.push_back(string("S ") + '!');
			instruction_counter += 2;
			output = "BN ";
			if (!StringIsNum(tokens[GotoPosition + 1])) {
				cerr << "Line number is incorrect;\n";
				break;
			}
			output += tokens[GotoPosition + 1] + "goto";
			flags[instruction_counter++] = atoi(tokens[GotoPosition + 1].c_str());
		break;
		case '<':
			EvaluateString(expressionList[0], mem, '?', fsymtable, symbol_counter, instruction_counter);
			EvaluateString(expressionList[2], mem, '!', fsymtable, symbol_counter, instruction_counter);
			mem.push_back(string("L ") + '!');
			mem.push_back(string("S ") + '?');
			instruction_counter += 2;
			output = "BN ";
			if (!StringIsNum(tokens[GotoPosition + 1])) {
				cerr << "Line number is incorrect;\n";
				break;
			}
			output += tokens[GotoPosition + 1] + "goto";
			flags[instruction_counter++] = atoi(tokens[GotoPosition + 1].c_str());

		break;	
		case '=':
			EvaluateString(expressionList[0], mem, '?', fsymtable, symbol_counter, instruction_counter);
			EvaluateString(expressionList[2], mem, '!', fsymtable, symbol_counter, instruction_counter);
			mem.push_back(string("L ") + '?');
			mem.push_back(string("S ") + '!');
			instruction_counter += 2;
		break;
		default:cerr << "needs a boolean expression" << endl; break;
	}
	if (expressionList[1].length() > 1 && expressionList[1][1] == '=') {
		output = "BZ ";
		if (!StringIsNum(tokens[GotoPosition + 1])) {
			cerr << "Line number is incorrect;\n";
		}
		output += tokens[GotoPosition + 1] + "goto";
		flags[instruction_counter++] = atoi(tokens[GotoPosition + 1].c_str());
	}
	return output;
}
string ProcessLet(vector<string> tokens,compiler_tableEntry* fsymtable, int& symbol_counter,vector<string>& mem,int& instruction_counter) {
	InsertSymbol(fsymtable, tokens[2][0], 'V', -1, symbol_counter);
	string expression = "";
	for (int i = 2; i < tokens.size(); i++) {
		expression += tokens[i];
	}
	expression = split(expression, '=')[1];

	mem.push_back(string("L ") + EvaluateString(expression,mem,'`',fsymtable,symbol_counter,instruction_counter));
	instruction_counter += 1;
	return string("ST ") + tokens[2][0];
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
		symboltable[i].hasGoto = false;
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
			case Let:
				output = ProcessLet(tokens,symboltable,symboltable_counter,mem,instruction_counter);
				instruction_counter++;
				break;
			case Print:
				output += "W ";
				output += tokens[2][0];
				InsertSymbol(symboltable, (int)tokens[2][0], 'V', -1, symboltable_counter);
				instruction_counter++;
				break;
			case If:
				output = ProcessIfStatement(tokens,flags,instruction_counter,symboltable,mem,symboltable_counter);
				break;
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
				else if(!symboltable[tIndex].hasGoto){
					tStr = to_string(flags[i]) + "goto ";
					mem[symboltable[tIndex].location] = tStr + mem[symboltable[tIndex].location];
					symboltable[tIndex].hasGoto = true;
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
			case 'C':
				tStr =  to_string(symboltable[i].symbol);
				tStr += " ";
				tStr += "DC ";
				tStr += to_string(symboltable[i].symbol);
				mem.push_back(tStr);
				break;
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
