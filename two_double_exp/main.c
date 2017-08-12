/*
	用来进行二进制编码和double类型转换的,输入文件为in.csv，输出文件为out.csv
*/

#include <stdio.h>

//str should have at least 64 byte.
void doubletostr(double* a, char* str) {
	long long c;
	int i;
	c = ((long long*)a)[0];
	for (i = 0; i<64; i++) {
		str[63 - i] = (char)(c & 1) + '0';
		c >>= 1;
	}
	str[64] = '\0';
}

//str should have at least 33 byte.
void floattostr(float* a, char* str) {
	unsigned int c;
	int i;
	c = ((unsigned int*)a)[0];
	for (i = 0; i<32; i++) {
		printf("%d\n", c);
		str[31 - i] = (char)(c & 1) + '0';
		c >>= 1;
	}
	str[32] = '\0';
}

float strtofloat(char * str) {
	unsigned int flt = 0;
	int i;
	for (i = 0; i<31; i++) {
		flt += (str[i] - '0');
		flt <<= 1;
	}
	flt += (str[31] - '0');
	float * ret = (float*)&flt;
	return *ret;
}


double strtodbl(char * str) {
	long long dbl = 0;
	int i;
	for (i = 0; i<63; i++) {
		dbl += (str[i] - '0');
		dbl <<= 1;
	}
	dbl += (str[63] - '0');
	double* db = (double*)&dbl;
	return *db;
}




int main()
{
	FILE *fpIn, *fpOut;
	char binData[65];
	int breakFlag = 0, i;
	double doubleData = 0;

	fpIn = fopen("in.csv", "r"); // 以文本方式打开
	fpOut = fopen("out.csv", "w");
	if (fpIn == NULL) return -1; // 打开文件失败
	while (1)
	{
		//printf("==\n");
		i = 0;//字符串标记清零
		while (1)
		{
			binData[i] = fgetc(fpIn);//读入一个字符。
			//printf("%c\n", binData[i]);
			if (binData[i] == '\n')//换行
			{
				binData[i] = '\0'; //赋值字符串结束符。
				break;//退出读取字符串。
			}
			else if (binData[i] == EOF) {
				binData[i] = '\0'; //赋值字符串结束符
				breakFlag = 1;
				break;//退出读取字符串。
			}
			i++;
		}
		doubleData = strtodbl(binData);
		//printf("lf\n", doubleData);
		fprintf(fpOut, "%lf\n", doubleData);
		if (breakFlag == 1) break;
	}

	fclose(fpIn); //关闭文件。
	fclose(fpOut);
	return 0;
}




