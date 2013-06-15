// v1.0
// 添加新命令
// 添加delete
// log4cpp
// 添加源代码显示

#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

class node;

class cmd_buffer
{
		node* label_site;
		string label_name;
		cmd_buffer* next_label;
		friend class node;
		friend class loop;
		friend class jmp;
	public :
		cmd_buffer() : next_label(NULL) {}
};

class Register//寄存器
{
		static const string name[];//寄存器单元名
		static int value[];//寄存器单元存储数据
		static const int num_of_reg;
	public:
		void save(string, int);//存储数据(寄存器单元名，数据)
		int load(string);//读取数据（寄存器单元名）
};

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
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class cmd//命令抽象类
{
	protected:
		Register reg;//寄存器
		static cmd_buffer* label_buf;
	
		string operand_1;//操作数1
		string operand_2;//操作数2
	public:
		static void set_buffer(cmd_buffer* b) { if (label_buf == NULL) label_buf = b; }
		virtual void func() = 0;//功能 纯虚函数
		virtual ~cmd(){};
};

cmd_buffer* cmd::label_buf = NULL;

class node//ASM语句串
{
		cmd* substance;//语句内容
		static cmd_buffer* first_label;
        node* ptr_next;//指向下一行语句的指针
		node* ptr_jump;//指向跳转语句的指针
		vector<string> del_note(string);//删去注释部分
		void save(vector<string>);
	public:
		static ifstream fin;//连续读取文件
		node() : ptr_next(NULL), ptr_jump(NULL), substance(NULL)
		{
			if (first_label == NULL)
				first_label = new cmd_buffer;
			cmd::set_buffer(first_label);
		}
		void convert(node* p){ptr_jump = p;};//改变ptr_jump
		void read_asm();//读取一行语句并储存相关命令
		void run_asm();//执行当前行语句
};

bool is_reg(string str)
{
	if( ((str[0] >= '0' && str[0] <= '9') || (str[0] >= 'a' && str[0] <= 'f')) &&
	    ((str[1] >= '0' && str[1] <= '9') || (str[1] >= 'a' && str[1] <= 'f')) )
	    return 0;
	else if (str[1] == 0)
		return 0;
	else return 1;
}

int hex_to_int(string hex_num)//注意只能是2位16进制数
{
	//HEX a  b  c  d  e  f  注意都是小写
	//DEC 10 11 12 13 14 15
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

class mov : public cmd//mov [reg] [hex_value / reg]
{
		friend bool is_reg(string);//判断 操作数2指的是寄存器还是十六进制值
		void func_1();//1功能 将16进制值拷贝到寄存器
		void func_2();//2功能 将后一寄存器的值拷贝到前一寄存器
		friend int hex_to_int(string);//友元
	public:
		mov(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//1功能 或 2功能
};

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


class loop : public cmd//loop循环语句
{
		node* ptr;
	public:
		loop(string o1 = "", node* p = NULL) { operand_1 = o1; ptr = p;}
		void func();//循环，跳到相应label对应的语句去
};

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


class jmp : public cmd//jmp [label]
{
	protected:
        node* ptr;
        void jump(string);
	public:
		jmp(string o1 = "", node* p = NULL) { operand_1 = o1; ptr = p;}
		void func();//跟jmp差不多，往后蹦
};

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


class jg : public jmp
{
	public:
		jg(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//如果bh大于dh的值，则跳转
};

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


class jl : public jmp
{
	public:
		jl(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//如bh小于dh则跳转
};

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

class je : public jmp
{
	public:
		je(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//若bh等于dh则跳转
};

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

class jne : public jmp
{
	public:
		jne(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//若bh不等于dh则跳转
};

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

class jz : public jmp
{
	public:
		jz(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();
};

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

class jnz : public jmp
{
	public:
		jnz(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();
};

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

class inc : public cmd//inc [reg]
{
	public:
		inc(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//寄存器值加1
};

void inc::func()
{
	reg.save(operand_1, reg.load(operand_1) + 1);
}

class Dec : public cmd//Dec [reg]
{
	public:
		Dec(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//寄存器值减1
};

void Dec::func()
{
	reg.save(operand_1, reg.load(operand_1) - 1);
}

class add : public cmd//add [reg] [hex_value]
{
		friend bool is_reg(string);
		friend int hex_to_int(string);
	public:
		add(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//寄存器的值 增加操作数2
};

void add::func()
{
	if(!is_reg(operand_2))
		reg.save(operand_1, reg.load(operand_1) + hex_to_int(operand_2));
	else
		reg.save(operand_1, reg.load(operand_1) + reg.load(operand_2));
}

class sub : public cmd//sub [reg] [hex_value]
{
		friend bool is_reg(string);
		friend int hex_to_int(string);
	public:
		sub(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//类似上面那个
};

void sub::func()
{
	if(!is_reg(operand_2))
		reg.save(operand_1, reg.load(operand_1) - hex_to_int(operand_2));
	else
		reg.save(operand_1, reg.load(operand_1) - reg.load(operand_2));
}

class int_20 : public cmd//terminate
{
	public:
		void func(){};//终止
};

class int_21 : public cmd//i / o
{
		void func_1();//输入DL
		void func_2();//输出DL的字符
		void func_3();//输出DL的数值
	public:
		void func();//输入或输出
};

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

class label : public cmd//label
{
	public:
		void func(){};//什么也不做
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//node类的具体实现


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
	if (!fin)//文件结束
		return;
	else
		getline(fin, sentence);
	vector<string> word = del_note(sentence);
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
}

int main()
{
	//node::fin.open(argv[1]);
	string name;
	cin >> name;
	node::fin.open(name.c_str());
	node* begin = new node;
	begin -> read_asm();
	node::fin.close();
	
	begin -> run_asm();
	
	cout << '\n' << "process end normally. " << '\n';
	system ("pause");
	return 0;
}
