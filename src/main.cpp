/*
	ProjectTitle: compiler(C0)
	Author: 56
	CreatTime:	2017.11.19
*/


#include "main.h"
using namespace compiler;

int main() {
	//int->string,string->int

	init();
	char name[100] = "void";
	block();
	printMidResult();
	
	midtomips(0);
	printMipsResult();
	if(!IsError){
	//Dag图优化
	expOpt();
	printMidOptResult();
	//midtomips(0);
	//printMipsResult();

	//寄存器优化
	midtomips(1);
	printMipsOptResult();
	}

	//根据key值在map中查找,以下两种方式均可
	//std::map<std::string,symbol>::iterator iter = KeyMap.find("a");
	//char iden[10] = "int\0";
	//auto iter = KeyMap.find(iden);
	//std::cout<<(iter == KeyMap.end())<<std::endl;
	//std::cout<<(iter->first)<<std::endl;

	//test - nextch

	//std::cout<<KeyMap["int"]<<std::endl;

	std::cout<<"end"<<std::endl;

	std::cin>>name;
	return 0;
}
