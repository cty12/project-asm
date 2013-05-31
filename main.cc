#include <iostream>
#include <string>//string
#include <cstring>//strcmp() / memset()
#include "class.hh"
using namespace std;

int hex_to_int(string hex_num)//注意只能是2位16进制数
{
	//HEX a  b  c  d  e  f  注意都是小写
	//DEC 10 11 12 13 14 15
	int digit[2];
	for(int i = 0; i < 2; i ++)
	{
		if(hex_num[i] >= '0' && hex_num[i] <= '9')
			digit[i] = hex_num[i] - 48;
		else if(hex_num[i] >= 'a' && hex_num[i] <= 'f')
			digit[i] = hex_num[i] - 87;
		else 
			return -1;
	}
	return 16 * digit[0] + digit[1];
}

Register::Register() : num_of_reg(7)
{
	memset(value, 0, num_of_reg);
}

Register* Register::instance = NULL;

Register* Register::getInstance()
{
	if(instance == NULL)
		instance = new Register();

	return instance;
}

void Register::save(string reg_name, int input)
{
	for(int i = 0; i < num_of_reg; i ++)
		if(reg_name == name[i])
			value[i] = input;
}

int Register::load(string reg_name)
{
	for(int i = 0; i < num_of_reg; i ++)
		if(reg_name == name[i])
			return value[i];
}

cmd::cmd()
{
	reg = Register::getInstance();
}

bool mov::is_reg(string str)
{
	if( ((str[0] >= '0' && str[0] <= '9') || (str[0] >= 'a' && str[0] <= 'f')) && 
	    ((str[1] >= '0' && str[1] <= '9') || (str[1] >= 'a' && str[1] <= 'f')) )
	    return 0;
	else return 1;
}

void mov::func()
{
	if(is_reg(operand_2))
		mov::func_2();
	else
		mov::func_1();
}

void mov::func_1()
{
	reg -> save(operand_1, hex_to_int(operand_2));
}

void mov::func_2()
{
	reg -> save(operand_1, reg -> load(operand_2));
}

/// TO DO

void inc::func()
{
	reg -> save(operand_1, reg -> load(operand_1) + 1);
}

void dec::func()
{
	reg -> save(operand_1, reg -> load(operand_1) - 1);
}

void add::func()
{
	reg -> save(operand_1, reg -> load(operand_1) + hex_to_int(operand_2));
}

void sub::func()
{
	reg -> save(operand_1, reg -> load(operand_1) - hex_to_int(operand_2));
}

void list::read_asm(char* file_name)
{
	fin.open(file_name);
	while(!fin.eof())
	{
		string cmd_line;
		getline(fin, cmd_line);
	}
}

int main(int argc, char** argv)
{
	list cmd_list;
	if(argc != 2 || argv[1][sizeof argv[1] - 5] != '.'
		         || argv[1][sizeof argv[1] - 4] != 'a'
		         || argv[1][sizeof argv[1] - 3] != 's'
		         || argv[1][sizeof argv[1] - 2] != 'm')
		cout << "invalid input format. " << endl
			 << "asm.exe file_name.asm"  << endl;

	else
	{
		//cmd_list.read_asm(argv[1]);
		//cmd_list.read_asm();
	}
	return 0;
}
