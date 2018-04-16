/*
	代码优化
	此文件只进行DAG图的处理
*/

#ifndef _OPTIMIZEDEAL_H_
#define _OPTIMIZEDEAL_H_
#include "varinfo.h"
#include "enterinfo.h"
#include "funcinfo.h"
#include <algorithm>

namespace compiler{
	
	//公共子表达式消除函数
	void expOpt();

}
#endif