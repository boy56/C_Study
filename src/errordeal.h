/*
	������Ϣ�Ѽ����������
*/
#ifndef _ERRORDEAL_H_
#define _ERRORDEAL_H_
#include "varinfo.h"
#include "funcinfo.h"
namespace compiler{
	void error(int n, int pos); // �Ա��Ϊn�Ĵ�����д�����,���������Ϣ
	void fatal(int n, int pos); // �Ա��Ϊn��ջ���������д���
	void skip(symbol st[], int n);	//����Դ����ֱ��ȡ�����ַ����ڸ����ķ��ż�st, nΪ�����С
}
#endif
