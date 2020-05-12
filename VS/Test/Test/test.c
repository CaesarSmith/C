#include <stdio.h>

void func();

int main()
{
	int a = 88;
	func(a);
	printf("a = %d\n", a);
	printf("this is a string\n");
}

void func(a)
{
	a = 99;
}