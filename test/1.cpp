#include <iostream>
using namespace std;

int hex_to_int(string hex_num)//注意只能是2位16进制数
							  //还得注意是小写的！！！
{
	int digit[2];
	for(int i = 0; i < 2; i ++)
	{
		if(hex_num[i] >= '0' && hex_num[i] <= '9')
			digit[i] = hex_num[i] - 48;
		else
			digit[i] = hex_num[i] - 87;
	}
	return 16 * digit[0] + digit[1];
}

int main()
{
	cout << hex_to_int("ae") << endl;
	cout << hex_to_int("1f") << endl;
	return 0;
}
