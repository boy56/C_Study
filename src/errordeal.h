/*
	错误信息搜集及处理程序
*/
#ifndef _ERRORDEAL_H_
#define _ERRORDEAL_H_
#include "varinfo.h"
#include "funcinfo.h"
namespace compiler{
	void error(int n, int pos); // 对标号为n的错误进行错误处理,输出错误信息
	void fatal(int n, int pos); // 对标号为n的栈溢出错误进行处理
	void skip(symbol st[], int n);	//跳读源程序直到取到的字符属于给定的符号集st, n为数组大小
}
#endif
