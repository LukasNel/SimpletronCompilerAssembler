// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SimpletronAssembler.h"
#include "CompilerForSimpletron.h"
int main()
{
	SimpletronAssembler("V21.txt");
	
	SimpletronAssembler(SimpletronCompiler("SimpleCode.txt").c_str());

    return 0;
}

