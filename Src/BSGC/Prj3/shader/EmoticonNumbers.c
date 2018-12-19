#include <stdio.h>

void emoticon(long long num)
{
	if (num >= 10000000) 
	{
		emoticon(num / 10000000);
		printf(" OAO");
		num %= 10000000;
	}
	if (num >= 100000) 
	{
		emoticon(num / 100000);
		printf(" OuO");
		num %= 100000;
	}
	if (num >= 1000) 
	{
		emoticon(num / 1000);
		printf(" QAQ");
		num %= 1000;
	}
	if (num >= 100) 
	{
		emoticon(num / 100);
		printf(" QQ");
		num %= 100;
	}
	if (num)
		printf(" %d", num);
}

int main() 
{
	long long  num;	
	FILE *f = fopen("number.txt", "r");
	//Read numbers
	while (fscanf(f,"%I64d", &num) != EOF)
	{		
		if (num > 0)
			emoticon(num);
		else
			printf(" 0");
		printf("\n");
	}
	fclose(f);	
	return 0;
}