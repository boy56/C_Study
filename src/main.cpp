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
	//Dagͼ�Ż�
	expOpt();
	printMidOptResult();
	//midtomips(0);
	//printMipsResult();

	//�Ĵ����Ż�
	midtomips(1);
	printMipsOptResult();
	}

	//����keyֵ��map�в���,�������ַ�ʽ����
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
