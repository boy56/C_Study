/*
	�ʷ�����������
*/
#ifndef _WORDDEAL_H_
#define _WORDDEAL_H_
#include "varinfo.h"
#include "errordeal.h"
#include <iomanip>
namespace compiler{

    extern int word_count;   //���ڴʷ�������һ����Ҫ

    //std::string return_symbol_info(symbol sy); //�ʷ�������һ����Ҫ

    //void nextch();  //read next character; process line end
    void insymbol(); //read next symbol
}
#endif
