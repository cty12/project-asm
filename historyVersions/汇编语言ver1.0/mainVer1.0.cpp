#include "TranslatorVer1.0.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

int main(int argc, char const *argv[])
{
	if(argc != 2 || argv[1][sizeof(argv[1]) - 2] != 'm'
				 || argv[1][sizeof(argv[1]) - 3] != 's'
				 || argv[1][sizeof(argv[1]) - 4] != 'a'
				 || argv[1][sizeof(argv[1]) - 5] != '.')
		cout << "usage: a file_name.asm" << '\n';

	translator tran;
	tran.translate(argv[1]);
	
	system ("pause");
	return 0;
}
