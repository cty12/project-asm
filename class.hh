#include <fstream>//ifstream
#include <string>//string
using namespace std;

#ifndef CLASS_H
#define CLASS_H

//到时候直接往类里加友元就可以
//还要有一个地址的缓存

//想要增减寄存器，只需改变num_of_reg和name[]即可
class Register//寄存器，使用了singleton设计模式，保证寄存器单元在
              //整个程序里只有一组
{
private:
	static Register* instance;

	const int num_of_reg;//常量，寄存器数目
	static const string name[];//寄存器单元名 
	int value[];//寄存器单元存储数据 
protected:
	Register();//构造函数
public:
	static Register* getInstance();
	void save(string, int);//存储数据(寄存器单元名，数据)
	int load(string);//读取数据（寄存器单元名） 
};

const string Register::name[] = {"ah", "al", "bh", "bl", "dh", "dl", "cx"};

class cmd//命令抽象类
{
protected:
	Register* reg;//寄存器

	string operand_1;//操作数1
	string operand_2;//操作数2
public:
	cmd();//构造函数
	virtual void func() = 0;//功能 纯虚函数
};

class mov : public cmd//mov [reg] [hex_value / reg]
{
	bool is_reg(string);//判断 操作数2指的是寄存器还是十六进制值
	void func_1();//1功能 将16进制值拷贝到寄存器
	void func_2();//2功能 将后一寄存器的值拷贝到前一寄存器
	friend int hex_to_int(string);//友元
public:
	void func();//1功能 或 2功能
};

class loop : public cmd//loop [label]
{
public:
	void func();//循环，修改list类里的指针
};

class jmp : public cmd//jmp [label]
{
public:
	void func();//跟jmp差不多，往后蹦
};

class jg : public jmp
{
	bool jmp_or_not();//判断BH寄存器里的值是否比DH的值大
public:
	void func_jg();//如果bool返回1，则执行继承来的func()
};

class jl : public jmp
{
	bool jmp_or_not();//同上
public:
	void func_jl();
};

class je : public jmp
{
	bool jmp_or_not();//同上
public:
	void func_je();
};

class jne : public jmp
{
	bool jmp_or_not();//同上
public:
	void func_jne();
};

class jz : public jmp
{
	bool jmp_or_not();//同上
public:
	void func_jz();
};

class jnz : public jmp
{
	bool jmp_or_not();//同上
public:
	void func_jnz();
};

class inc : public cmd//inc [reg]
{
public:
	void func();//寄存器值加1
};

class dec : public cmd//dec [reg]
{
public:
	void func();//寄存器值减1
};

class add : public cmd//add [reg] [hex_value]
{
	friend int hex_to_int(string);
public:
	void func();//寄存器的值 增加操作数2
};

class sub : public cmd//sub [reg] [hex_value]
{
public: 
	void func();//类似上面那个
};

class int_20 : public cmd//terminate
{
public: 
	void func();//终止
};

class int_21 : public cmd//i / o
{
	bool in_or_out();//ah里的值是几？1则输入，2则输出
	void func_1();//输入
	void func_2();//输出
public: 
	void func();//输入或输出
};

class label : public cmd//label
{
public: 
	void func();//什么也不做
};

class list 
{//ASM语句串
	cmd* this_cmd;
	list* ptr_next;//顺序执行
	list* ptr_loop;//jmp或loop跳转
	void trace();//在read_asm里调用它来决定访问哪一个节点并访问那个节点
public:
	static ifstream fin;//连续读取文件
	list(list* n_cmd = NULL, list* n_step = NULL) : ptr_next(n_cmd), ptr_loop(n_step) {}
	//构造函数
	void read_asm(char*);//读取文件，构造命令链表
	void run_asm();//按照命令链表，执行语句
};


#endif
