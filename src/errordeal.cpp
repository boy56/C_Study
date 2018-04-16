/*
	������Ϣ�Ѽ�����������
*/
#include "errordeal.h"

namespace compiler{
	std::string errormsg[55] = {
		"�ļ�������,�������",	//0
		"����̫����",	//1
		"�ַ�����Ϊ��",	//2
		"�ַ���βӦ��\'",	//3
		"�ַ����г��ַǷ��ַ�,���ַ�������",	//4
		"�ַ���δ����˫���žͽ����˻��в���,���ַ���������¼",	//5
		"!���������=",	//6
		"�����г��������ַ�",	//7
		"�ַ�������ַǷ��ַ�(Υ���ַ��ķ�)",	//8
		"����������0��ͷ,�ѽ����ݴ�,����0123ȡ123", //9

		"main������������",	//10
		"Ӧ���б�ʶ��",	//11
		"��������ֵ����(�������ʹ�������޷���ֵ)",	//12
		"ȱ��\'(\'",	//13
		"ȱ��\')\'",	//14
		"ȱ��\'[\'",	//15
		"ȱ��\']\'",	//16
		"ȱ��\'{\'",	//17
		"ȱ��\'}\'",	//18
		"ȱ��\';\'",	//19
		"ȱ��\'=\'",	//20
		"if��while���������������",	//21
		"case�������Ϊ����",	//22

		"��ʶ��(������ or ������ or ������)δ����",	//23
		"��ʶ��(������ or ������ or ������)�ظ�����",	//24
		"����Խ��",	//25
		"Ӧ��Ϊ�ؼ���int �� char",	//26
		"�����ĸ�ֵ�����������Ͳ���",	//27
		"����������ʽ����",		//28
		"const����Ӧ������ǰ",	//29
		"����������ʽ����",	//30
		"����������ʽ����", //31
		"������������������������� �׵��ʳ���",//32
		"����ʽ��ʽ����", //33
		"Ӧ��Ϊ�з���ֵ��������",	//34
		"ȱ��while",	//35
		"�βθ�����ʵ�θ�������", //36
		"���ܶԳ�����ֵ",	//37
		"��ֵ����ʽ����",	//38
		"scanf����ʽ����",	//39
		"printf����ʽ����",	//40
		"return����ʽ����",	//41
		"switch������",	//42
		"case����ʽ����",	//43
		"��������Ϊ0",	//44
		"�β���ʵ�����Ͳ���",				//45
		"���ܶ�char��ֵint����",			//46
		"case ������switch�б���ʽ���Ͳ���",	//47
		"���������ڲ����ڴ���",	//48
		"�����±����(Խ���Ƿ�����)",	//49
		"������ʽ����" //50
	};

	std::string fatalmsg[10] = {
		//�ʷ�����
		"�������ַ���̫����,�����ַ��������",	//0

		//�﷨����
		"���ű�tab���",		//1
		"�ֳ����btab���", 	//2
		"�����atab���",		//3
		"�м��������",		//4
		"mips��������",		//5
		"�м�����Ż������"	//6
	};

	//ע��error�ڴʷ��������﷨��������������
	//posΪ��������
	void error(int n , int pos){
		IsError = true;
		if (n == 17 || n == 45) {
			std::cout << "Warning: " << errormsg[n] << "   line: " << pos << std::endl;
			erroroutFile << "Warning: " << errormsg[n] << "   line: " << pos << std::endl;
		}
		else {
			std::cout << "Error: " << errormsg[n] << "   line: " << pos << std::endl;
			erroroutFile << "Error: " << errormsg[n] << "   line: " << pos << std::endl;
		}
	}

	void fatal(int n, int pos){
		std::cout << "Fatal: " << fatalmsg[n] << "   line: " << pos << std::endl;
		erroroutFile<<"Fatal: "<<fatalmsg[n]<<"   line: "<<pos<<std::endl;
		exit(1);
	}

}