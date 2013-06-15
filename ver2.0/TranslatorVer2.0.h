//相比TranslatorVer1.0的版本
//增加了一下指令（没有扩展寄存器）
//①：交换指令：xch
//②：清除指令：clr
//③：逻辑与指令：anl
//④：逻辑或指令：orl
//PS：由于运算指令过于相似，故把所有二元运算指令从一个ope类（继承自cmd类）里继承
//（包括add，sub已经上面的anl，orl）
#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__
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
		node() :  substance(NULL), ptr_next(NULL), ptr_jump(NULL)
		{
			if (first_label == NULL)
				first_label = new cmd_buffer;
			cmd::set_buffer(first_label);
		}
		void convert(node* p){ptr_jump = p;}//改变ptr_jump
		void read_asm();//读取一行语句并储存相关命令
		void run_asm();//执行当前行语句
};

bool is_reg(string);//判断操作数是否代表寄存器

int hex_to_int(string);//将16进制数变为10进制

////////////////////////////////////////////////////////////////////////////////////
//增加的指令(除了此处外，仅需要修改node::save函数中的判断部分)

class xch : public cmd//交换两个寄存器内的值
{
    public:
		xch(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();
};

class clr : public cmd//寄存器清零
{
    public:
		clr(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();
};

class ope : public cmd//二元运算指令的基类
{
	protected:
    	friend bool is_reg(string);
		friend int hex_to_int(string);
		virtual int operate(int,int) = 0;
	public:
        ope(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//将两个操作数进行运算，然后存到第一个寄存器里
};

class anl : public ope//逻辑与运算
{
	public:
		anl(string o1 = "", string o2 = ""): ope(o1, o2) {}
		int operate(int,int);
};

class orl : public ope//逻辑或运算
{
	public:
		orl(string o1 = "", string o2 = ""): ope(o1, o2) {}
		int operate(int,int);
};

class add : public ope//加法运算
{
	public:
		add(string o1 = "", string o2 = ""): ope(o1, o2) {}
		int operate(int,int);
};

class sub : public ope//减法运算
{
	public:
		sub(string o1 = "", string o2 = ""): ope(o1, o2) {}
		int operate(int,int);
};
///////////////////////////////////////////////////////////////////////////////////

class mov : public cmd//mov [reg] [hex_value / reg]
{
		friend bool is_reg(string);//判断 操作数2指的是寄存器还是十六进制值
		void func_1();//1功能 将16进制值拷贝到寄存器
		void func_2();//2功能 将后一寄存器的值拷贝到前一寄存器
		friend int hex_to_int(string);//友元
	public:
		mov(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//判断执行1功能 还是 2功能
};

class loop : public cmd//loop循环语句
{
		node* ptr;
	public:
		loop(string o1 = "", node* p = NULL) { operand_1 = o1; ptr = p;}
		void func();//循环，跳到相应label对应的语句去
};

class jmp : public cmd//jmp [label]
{
	protected:
        node* ptr;
        void jump(string);
	public:
		jmp(string o1 = "", node* p = NULL) { operand_1 = o1; ptr = p;}
		void func();//跟jmp差不多，往后蹦
};

class jg : public jmp
{
	public:
		jg(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//如果bh大于dh的值，则跳转
};

class jl : public jmp
{
	public:
		jl(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//如bh小于dh则跳转
};

class je : public jmp
{
	public:
		je(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//若bh等于dh则跳转
};

class jne : public jmp
{
	public:
		jne(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//若bh不等于dh则跳转
};

class jz : public jmp
{
	public:
		jz(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//若bh等于0则跳转
};

class jnz : public jmp
{
	public:
		jnz(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//若bh不等于0则跳转
};

class inc : public cmd//inc [reg]
{
	public:
		inc(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//寄存器值加1
};

class Dec : public cmd//Dec [reg]
{
	public:
		Dec(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//寄存器值减1
};

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
		void func();//判断是以上三种情况的哪一种
};

class label : public cmd//label
{
	public:
		void func(){};//什么也不做
};

class translator//运行编译器
{
		node* begin;
	public:
		void translate(char const*);
};

#endif
