//相比TranslatorVer1.0的版本
//增加了一下指令（没有扩展寄存器）
//①：交换指令：xch
//②：清除指令：clr
//③：逻辑与指令：anl
//④：逻辑或指令：orl
//PS：由于运算指令过于相似，故把所有二元运算指令从一个ope类（继承自cmd类）里继承
//（包括add，sub已经上面的anl，orl）
#include "TranslatorVer2.0.h"
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
