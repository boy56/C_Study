/*
	词法分析及处理
*/
#ifndef _WORDDEAL_H_
#define _WORDDEAL_H_
#include "varinfo.h"
#include "errordeal.h"
#include <iomanip>
namespace compiler{

    extern int word_count;   //用于词法分析第一次需要

    //std::string return_symbol_info(symbol sy); //词法分析第一次需要

    //void nextch();  //read next character; process line end
    void insymbol(); //read next symbol
}
#endif
