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

class Register//�Ĵ���
{
		static const string name[];//�Ĵ�����Ԫ��
		static int value[];//�Ĵ�����Ԫ�洢����
		static const int num_of_reg;
	public:
		void save(string, int);//�洢����(�Ĵ�����Ԫ��������)
		int load(string);//��ȡ���ݣ��Ĵ�����Ԫ����
};

class cmd//���������
{
	protected:
		Register reg;//�Ĵ���
		static cmd_buffer* label_buf;

		string operand_1;//������1
		string operand_2;//������2
	public:
		static void set_buffer(cmd_buffer* b) { if (label_buf == NULL) label_buf = b; }
		virtual void func() = 0;//���� ���麯��
		virtual ~cmd(){};
};

class node//ASM��䴮
{
		cmd* substance;//�������
		static cmd_buffer* first_label;
        node* ptr_next;//ָ����һ������ָ��
		node* ptr_jump;//ָ����ת����ָ��
		vector<string> del_note(string);//ɾȥע�Ͳ���
		void save(vector<string>);
	public:
		static ifstream fin;//������ȡ�ļ�
		node() :  substance(NULL), ptr_next(NULL), ptr_jump(NULL)
		{
			if (first_label == NULL)
				first_label = new cmd_buffer;
			cmd::set_buffer(first_label);
		}
		void convert(node* p){ptr_jump = p;}//�ı�ptr_jump
		void read_asm();//��ȡһ����䲢�����������
		void run_asm();//ִ�е�ǰ�����
};

bool is_reg(string);//�жϲ������Ƿ����Ĵ���

int hex_to_int(string);//��16��������Ϊ10����

class mov : public cmd//mov [reg] [hex_value / reg]
{
		friend bool is_reg(string);//�ж� ������2ָ���ǼĴ�������ʮ������ֵ
		void func_1();//1���� ��16����ֵ�������Ĵ���
		void func_2();//2���� ����һ�Ĵ�����ֵ������ǰһ�Ĵ���
		friend int hex_to_int(string);//��Ԫ
	public:
		mov(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//�ж�ִ��1���� ���� 2����
};

class loop : public cmd//loopѭ�����
{
		node* ptr;
	public:
		loop(string o1 = "", node* p = NULL) { operand_1 = o1; ptr = p;}
		void func();//ѭ����������Ӧlabel��Ӧ�����ȥ
};

class jmp : public cmd//jmp [label]
{
	protected:
        node* ptr;
        void jump(string);
	public:
		jmp(string o1 = "", node* p = NULL) { operand_1 = o1; ptr = p;}
		void func();//��jmp��࣬�����
};

class jg : public jmp
{
	public:
		jg(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//���bh����dh��ֵ������ת
};

class jl : public jmp
{
	public:
		jl(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//��bhС��dh����ת
};

class je : public jmp
{
	public:
		je(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//��bh����dh����ת
};

class jne : public jmp
{
	public:
		jne(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//��bh������dh����ת
};

class jz : public jmp
{
	public:
		jz(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//��bh����0����ת
};

class jnz : public jmp
{
	public:
		jnz(string o1 = "", node* p = NULL): jmp(o1, p){};
		void func();//��bh������0����ת
};

class inc : public cmd//inc [reg]
{
	public:
		inc(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//�Ĵ���ֵ��1
};

class Dec : public cmd//Dec [reg]
{
	public:
		Dec(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//�Ĵ���ֵ��1
};

class add : public cmd//add [reg] [hex_value]
{
		friend bool is_reg(string);
		friend int hex_to_int(string);
	public:
		add(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//�Ĵ�����ֵ ���Ӳ�����2
};

class sub : public cmd//sub [reg] [hex_value]
{
		friend bool is_reg(string);
		friend int hex_to_int(string);
	public:
		sub(string o1 = "", string o2 = "") { operand_1 = o1; operand_2 = o2; }
		void func();//���������Ǹ�
};

class int_20 : public cmd//terminate
{
	public:
		void func(){};//��ֹ
};

class int_21 : public cmd//i / o
{
		void func_1();//����DL
		void func_2();//���DL���ַ�
		void func_3();//���DL����ֵ
	public:
		void func();//�ж������������������һ��
};

class label : public cmd//label
{
	public:
		void func(){};//ʲôҲ����
};

class translator//���б�����
{
		node* begin;
	public:
		void translate(char const*);
};

#endif
