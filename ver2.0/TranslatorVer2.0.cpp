#include "TranslatorVer2.0.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

void Register::save(string reg_name, int input)
{
	for(int i = 0; i < num_of_reg; i ++)
		if(reg_name == name[i])
		{
			value[i] = input;
		}
}

int Register::load(string reg_name)
{

	for(int i = 0; i < num_of_reg; i ++)
	{
		if(reg_name == name[i])
		{
			return value[i];
		}
	}
	cout << reg_name << "(Register) is not exist!";
	return -1;
}

const string Register::name[] = {"ah", "al", "bh", "bl", "dh", "dl", "cx"};
int Register::value[7] = {0};
const int Register::num_of_reg = 7;

cmd_buffer* cmd::label_buf = NULL;

bool is_reg(string str)
{
	if( ((str[0] >= '0' && str[0] <= '9') || (str[0] >= 'a' && str[0] <= 'f')) &&
	    ((str[1] >= '0' && str[1] <= '9') || (str[1] >= 'a' && str[1] <= 'f')) )
	    return 0;
	else if (str[1] == 0)
		return 0;
	else return 1;
}

int hex_to_int(string hex_num)
{
	if (hex_num[1] == 0)
	{
        if(hex_num[0] >= '0' && hex_num[0] <= '9')
			return hex_num[0] - 48;
		else if(hex_num[0] >= 'a' && hex_num[0] <= 'f')
			return hex_num[0] - 87;
        else
			return -1;
	}
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

void xch::func()
{
	int temp = reg.load(operand_1);
	reg.save(operand_1, reg.load(operand_2));
	reg.save(operand_2, temp);
}

void clr::func()
{
    reg.save(operand_1, 0);
}

void ope::func()
{
	if(!is_reg(operand_2))
		reg.save(operand_1, operate(reg.load(operand_1) , hex_to_int(operand_2)));
	else
		reg.save(operand_1, operate(reg.load(operand_1) , reg.load(operand_2)));
}

int anl::operate(int left ,int right)
{
	return left && right;
}

int orl::operate(int left ,int right)
{
	return left || right;
}

int add::operate(int left ,int right)
{
	return left + right;
}

int sub::operate(int left ,int right)
{
	return left - right;
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
	reg.save(operand_1, hex_to_int(operand_2));
}

void mov::func_2()
{
	reg.save(operand_1, reg.load(operand_2));
}

void loop::func()
{
	reg.save("cx", reg.load("cx") - 1);
	if (reg.load("cx") > 0)
	{
        cmd_buffer* p;
		p = label_buf;
		while (p != NULL)
		{
			if (operand_1 == p->label_name)
			{
                ptr->convert(p->label_site);
                return;
			}
			p = p->next_label;
		}
	}
	else
	{
        ptr->convert(NULL);
	}
}

void jmp::jump(string site)
{
    cmd_buffer* p;
	p = label_buf;
	while (p != NULL)
	{
		if (site == p->label_name)
		{
    	    ptr->convert(p->label_site);
    	    return;
		}
		p = p->next_label;
	}
}

void jmp::func()
{
	jump(operand_1);
}

void jg::func()
{
	if (reg.load("bh") > reg.load("dh"))
	{
	    jump(operand_1);
	}
	else
	{
        ptr->convert(NULL);
	}
}

void jl::func()
{
    if (reg.load("bh") < reg.load("dh"))
    {
	    jump(operand_1);
	}
	else
	{
        ptr->convert(NULL);
	}
}

void je::func()
{
    if (reg.load("bh") == reg.load("dh"))
    {
	    jump(operand_1);
	}
	else
	{
        ptr->convert(NULL);
	}
}

void jne::func()
{
    if (reg.load("bh") != reg.load("dh"))
    {
	    jump(operand_1);
	}
	else
	{
        ptr->convert(NULL);
	}
}

void jz::func()
{
    if (reg.load("bh") == 0)
    {
	    jump(operand_1);
	}
	else
	{
        ptr->convert(NULL);
	}
}

void jnz::func()
{
    if (reg.load("bh") != 0)
    {
	    jump(operand_1);
	}
	else
	{
        ptr->convert(NULL);
	}
}

void inc::func()
{
	reg.save(operand_1, reg.load(operand_1) + 1);
}

void Dec::func()
{
	reg.save(operand_1, reg.load(operand_1) - 1);
}

void int_21::func_1()
{
	char ch;
	cin >> ch;
	reg.save("al", int(ch));
}

void int_21::func_2()
{
	cout << char(reg.load("dl"));
}

void int_21::func_3()
{
    cout << reg.load("dl") << ' ';
}

void int_21::func()
{
	if (reg.load("ah") == 1)
		func_1();
	else if (reg.load("ah") == 2)
		func_2();
    else if (reg.load("ah") == 3)
		func_3();
}

ifstream node::fin;
cmd_buffer* node::first_label = NULL;

vector<string> node::del_note(string sen)
{
	string temp = "";
	vector<string> word;
	for (int i = 0; i != int(sen.size()); i ++)
	{
		if (sen[i] == ';')
			break;
		if (sen[i] == ' ' || sen[i] == '	')
		{
			if (temp != "")
			{
				word.push_back(temp);
				temp = "";
			}
			continue;
		}
		string t(1, sen[i]);
		temp += t;
	}
	if (temp != "")
		word.push_back(temp);
	return word;
}

void node::save(vector<string> word)
{
	string command = (word.size() != 0 ? word[0] : "");
	if (command == "")
	{
		return;
	}
	///////////////////////////////////////////////////ÐÞ¸Ä´¦
	else if (command == "xch")
	{
        substance = new xch(word[1], word[2]);
	}
	else if (command == "clr")
	{
        substance = new clr(word[1]);
	}
	else if (command == "anl")
	{
        substance = new anl(word[1], word[2]);
	}
    else if (command == "orl")
	{
        substance = new orl(word[1], word[2]);
	}
	////////////////////////////////////////////////////////////
	else if (command == "mov")
	{
		substance = new mov(word[1], word[2]);
	}
	else if (command == "loop")
	{
		substance = new loop(word[1], this);
	}
	else if (command == "jmp")
	{
		substance = new jmp(word[1], this);
	}
	else if (command == "jg")
	{
		substance = new jg(word[1], this);
	}
	else if (command == "jl")
	{
		substance = new jl(word[1], this);
	}
	else if (command == "je")
	{
		substance = new je(word[1], this);
	}
	else if (command == "jne")
	{
		substance = new jne(word[1], this);
	}
	else if (command == "jz")
	{
		substance = new jz(word[1], this);
	}
	else if (command == "jnz")
	{
		substance = new jnz(word[1], this);
	}
	else if (command == "inc")
	{
		substance = new inc(word[1]);
	}
	else if (command == "dec")
	{
		substance = new Dec(word[1]);
	}
	else if (command == "add")
	{
		substance = new add(word[1], word[2]);
	}
	else if (command == "sub")
	{
		substance = new sub(word[1], word[2]);
	}
	else if	(command == "int" && word[1] == "20")
	{
		substance = new int_20;
	}
	else if (command == "int" && word[1] == "21")
	{
		substance = new int_21;
	}
	else if (word[0][word[0].size() - 1] == ':' ||
				(word.size() > 1 && word[1] == ":"))
	{
		if (word[0][word[0].size() - 1] == ':')
		{
			command = "";
			for (int i = 0; i != int(word[0].size()) - 1; i ++)
			{
				string t(1, word[0][i]);
				command += t;
			}
		}
		cmd_buffer* p = first_label;
		while (p -> next_label != NULL)
			p = p -> next_label;
		p -> label_site = this;
		p -> label_name = command;
		p -> next_label = new cmd_buffer;
		substance = new label;
	} else;
}

void node::read_asm()
{
	string sentence;
	vector<string> word;
	do {
		if (!fin)
			return;
		else
			getline(fin, sentence);
		word = del_note(sentence);
	} while (word.size() == 0);
	cout << "--> ";
	for (int i = 0; i != int(word.size()); i ++)
		cout << word[i] << " ";
	cout << endl;
	save(word);
	ptr_next = new node;
	ptr_next -> read_asm();
}

void node::run_asm()
{
	if (substance != NULL)
		substance -> func();

	if (ptr_jump != NULL)
	{
        ptr_jump-> run_asm();
	}
	else if (ptr_next != NULL)
	{
		ptr_next-> run_asm();
	}
	delete ptr_next;
}

void translator::translate(char const* ch)
{
    node::fin.open(ch);
	node* begin = new node;
	begin -> read_asm();
	node::fin.close();
	begin -> run_asm();

	cout << '\n' << "terminated. " << '\n';
	delete begin;
}
