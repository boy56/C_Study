#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <Mmsystem.h>

#define PAGENUM 50000		//��Ҫ���ʵ����ݵ�����(���ظ�)
#define PAGEDIFFN 1000		//�����в�ͬ��ҳ������(���ظ�)
#define BUFFERNUM 400		//buffer������
#define SLICESIZE 3			//�滻��Ƭ�Ĵ�С

/*
int SRC[] = { 0, 9, 8, 4, 4, 3, 6, 5, 1, 5, 0,
2, 1, 1, 1, 1, 8, 8, 5, 3, 9, 8,
9, 9, 6, 1, 8, 4, 6, 4, 3, 7, 1,
3, 2, 9, 8, 6, 2, 9, 2, 7, 2, 7,
8, 4, 2, 3, 0, 1, 9, 4, 7, 1, 5,
9, 1, 7, 3, 4, 3, 7, 1, 0, 3, 5,
9, 9, 4, 9, 6, 1, 7, 5, 9, 4, 9,
7, 3, 6, 7, 7, 4, 5, 3, 5, 3, 1,
5, 6, 1, 1, 9, 6, 6, 4, 0, 9, 4, 3};//��һ�ٸ�

*/

int SRC[PAGENUM];//��ҵ5��Ҫ
int HitHash[PAGEDIFFN];


typedef struct page {
	int num;//ҳ��
	int time;//δʹ��ʱ��
	struct page *next;	//�ڸĽ����㷨���õ�
}Page, *PPage;


//��ʼ��SRC����,��������ҵ5
//ÿ20������һ��
void init_SRC() {
	int i, j;//iѭ���±�
	int conNum = 1;
	srand((unsigned)time(NULL));
	for (i = 0; i < PAGENUM; i+=conNum) {
		SRC[i] = rand() % PAGEDIFFN;		
		conNum = rand() % 20 + 1;
		for (j = 0; j < conNum; j++) {
			if (i + j >= PAGENUM) {
				printf("error in init_SRC: i + j > PAGENUM\n");
				break;
			}
			SRC[i + j] = (SRC[i] + j) % PAGEDIFFN;
		}
		
	}
	
}

void init_HitHash() {
	//init HitHash
	int i;
	for (i = 0; i < PAGEDIFFN; i++) {
		HitHash[i] = -1;
	}
}

//����ʱ�䲻�õ�ҳ��,indexΪ��ǰSRC�±�
int GetLongTime(int index, int n, PPage array) {
	int i, j;
	int max = -1;
	int pos = 0;
	for (i = 0; i < n; i++) {
		for (j = index; j < PAGENUM; j++) {
			if (array[i].num == SRC[j]) break;
			array[i].time++;
		}
	}
	return GetMax(n, array);
}

//�õ��ڴ��д���ʱ�����ҳ��
int GetMax(int n, PPage array) {
	int i;
	int max = -1;
	int pos = 0;
	for (i = 0; i < n; i++) {
		if (array[i].time>max) {
			max = array[i].time;
			pos = i;
		}
	}
	return pos;
}

//��������ڴ��Ƿ�����,������-1, �������е��±�ֵ
int Check(int n, PPage array) {
	int i;
	for (i = 0; i < n; i++) {
		if (array[i].num == -1) return i;
	}
	return -1;
}

//���ĳҳ�Ƿ����ڴ�,�����ڣ����������±�,pageNΪSRC[i],nΪ�ڴ���ҳ����
int Equation(int pageN, int n, PPage array) {
	/*int i;
	for (i = 0; i < n; i++) {
		if (array[i].num == pageN) return i;
	}
	*/
	return HitHash[pageN];
}


//OPT�㷨��ȱҳ����,�������Ϊҳ����
int OPT(int n) {
	int i;
	int j;
	int breakNum = 0;
	int pos = 0;//��Ҫ�滻���±�ֵ
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//�����ǰҳ�Ų����ڴ���
			if (Check(n, pageArray) == -1) {//��ǰ�����ڴ������������滻����
				pos = GetLongTime(i, n, pageArray);
				pageArray[pos].num = SRC[i];
			}
			else {
				pageArray[Check(n, pageArray)].num = SRC[i];//�ڴ�δ��,�������

			}
			breakNum++;//��ҳ�����ڴ���ȱҳ����+1
		}
		for (j = 0; j < n; j++) pageArray[j].time = 0;//�����ʱ�����,��Ϊ��ʱ����GetLongTime��ÿ�ζ�Ҫ���¼���
	}
	return breakNum;
}

//FIFO�㷨��ȱҳ����,�������Ϊҳ����
int FIFO(int n) {
	int i;
	int j;
	int breakNum = 0;
	int pos = 0;//��Ҫ�滻���±�ֵ
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		for (j = 0; j < n; j++) {
			if (pageArray[j].num != -1) pageArray[j].time++;
		}//����ʱ��+1
		if (Equation(SRC[i], n, pageArray) == -1) {//�����ǰҳ�Ų����ڴ���
			if (Check(n, pageArray) == -1) {//��ǰ�����ڴ������������滻����
				pos = GetMax(n, pageArray);
				pageArray[pos].num = SRC[i];
				pageArray[pos].time = 0;
			}
			else {
				pageArray[Check(n, pageArray)].num = SRC[i];//�ڴ�δ��,�������
				pageArray[Check(n, pageArray)].time = 0;
			}
			breakNum++;//��ҳ�����ڴ���ȱҳ����+1
		}

	}
	return breakNum;
}

//LRU�㷨��ȱҳ�������������Ϊҳ����
int LRU(int n) {
	int i;//ѭ������
	int j;//ѭ������
	int breakNum = 0;
	int pos = 0;//��Ҫ�滻���±�ֵ
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//�����ǰҳ�Ų����ڴ���
			
			for (j = 0; j < n; j++) {
				if (pageArray[j].num != -1) pageArray[j].time++;
			}
			
			int checkPos = Check(n, pageArray);
			if (checkPos == -1) {//��ǰ�����ڴ������������滻����
				pos = GetMax(n, pageArray);
				//pos = GetMin(n, pageArray);
				HitHash[pageArray[pos].num] = -1;
				pageArray[pos].num = SRC[i];
				pageArray[pos].time = 0;
				//pageArray[pos].time = GetTickCount();
				HitHash[SRC[i]] = pos;
			}
			else {
				HitHash[pageArray[checkPos].num] = -1;
				pageArray[checkPos].num = SRC[i];//�ڴ�δ��,�������
				pageArray[checkPos].time = 0;
				//pageArray[checkPos].time = GetTickCount();
				HitHash[SRC[i]] = checkPos;

			}
			breakNum++;//��ҳ�����ڴ���ȱҳ����+1
		}
		else {
			for (j = 0; j < n; j++) {
				if (j == Equation(SRC[i], n, pageArray))pageArray[j].time = 0;//�����ڴ���,��ʱ����Ϊ0
				else if (pageArray[j].num != -1) pageArray[j].time++;
			}

		}
	}
	return breakNum;
}

//�Ľ���GetMax�㷨
int GetMax_extention(int n, PPage array, int sliceSize) {
	int i, j, sum;
	int max = -1;
	int pos = 0;
	for (i = 0; i < n; i+=sliceSize) {
		sum = 0;
		for (j = 0; j < sliceSize && i+j < n; j++) {
			if(i + j > n-1) printf("error in GetMax_extention: i+j > n-1\n");
			sum += array[i + j].time;
		}
		if (sum>max) {
			max = sum;
			pos = i;
		}
	}
	return pos;
}
//�Ľ���LRU�㷨,��Ƭ��װ�룬����Ϊbufferҳ������Ƭ�Ĵ�С
int LRU_extention(int n, int sliceSize) {
	int i;//ѭ������
	int j;//ѭ������
	int breakNum = 0;
	int pos = 0;//��Ҫ�滻���±�ֵ
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//�����ǰҳ�Ų����ڴ���
			for (j = 0; j < n; j++) {
				if (pageArray[j].num != -1) pageArray[j].time++;
			}
			int checkPos = Check(n, pageArray);
			if (checkPos == -1) {//��ǰ�����ڴ������������滻����
				//pos = GetMax_extention(n, pageArray, sliceSize);
				pos = GetMax(n, pageArray);
				int temp = 0;
				for (j = 0; temp < sliceSize && pos + j < n; temp++) {
					if (j == 0 || Equation((SRC[i] + temp) % PAGEDIFFN, n, pageArray) == -1) {
						HitHash[pageArray[pos + j].num] = -1;
						pageArray[pos + j].num = (SRC[i] + temp) % PAGEDIFFN;
						pageArray[pos + j].time = 0;
						HitHash[(SRC[i] + temp) % PAGEDIFFN] = pos + j;
						j++;
					}
				}
				/*
				for (j = 0; j < sliceSize && pos+j < n; j++) {
					if (j == 0) {
						pageArray[pos].num = SRC[i];	//��SRC[i]װ���Ϊ0, ��SRC[i+1]װ��1,SRC[i+2]װ��2......
						pageArray[pos].time = 0;
					}
					else {
						//��������װ���Ƿ����ڴ���,ͨ��temp�����ҵ���һ�������ڴ��е�ҳ��
						for (; Equation((SRC[i] + temp) % PAGEDIFFN, n, pageArray) != -1; temp++);
						pageArray[pos + j].num = (SRC[i] + temp) % PAGEDIFFN;
						pageArray[pos + j].time = 0;
						temp++;
					}
				}
				*/
			}
			//�ڴ�δ�����ڳ�ʼ����ʱ��������һ��һ��װ���
			else {
				HitHash[pageArray[checkPos].num] = -1;
				pageArray[checkPos].num = SRC[i];//�ڴ�δ��,�������
				pageArray[checkPos].time = 0;
				HitHash[SRC[i]] = checkPos;
			}
			breakNum++;//��ҳ�����ڴ���ȱҳ����+1
		}
		else {
			for (j = 0; j < n; j++) {
				if (j == Equation(SRC[i], n, pageArray))pageArray[j].time = 0;//�����ڴ���,��ʱ����Ϊ0
				else if (pageArray[j].num != -1) pageArray[j].time++;
			}
		}
	}
	return breakNum; 

}

//�õ����ڴ��з��ʴ������ٵ�ҳ��
int GetMin(int n, PPage array) {
	int i;
	int min = 100000;
	int pos = 0;
	for (i = 0; i < n; i++) {
		if (array[i].time < min) {
			min = array[i].time;
			pos = i;
		}
	}
	return pos;
}

//LFU�㷨��ȱҳ�������������Ϊҳ����
int LFU(int n) {
	int i;//ѭ������
	int j;//ѭ������
	int breakNum = 0;
	int pos = 0;//��Ҫ�滻���±�ֵ
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//�����ǰҳ�Ų����ڴ���
			int checkPos = Check(n, pageArray);
			if (checkPos == -1) {//��ǰ�����ڴ������������滻����
				pos = GetMin(n, pageArray);
				HitHash[pageArray[pos].num] = -1;
				pageArray[pos].num = SRC[i];
				pageArray[pos].time = 1;
				HitHash[SRC[i]] = pos;
			}
			else {
				HitHash[pageArray[checkPos].num] = -1;
				pageArray[checkPos].num = SRC[i];//�ڴ�δ��,�������
				pageArray[checkPos].time = 1;
				HitHash[SRC[i]] = checkPos;

			}
			breakNum++;//��ҳ�����ڴ���ȱҳ����+1
		}
		else {
			pageArray[Equation(SRC[i], n, pageArray)].time++;
		}
	}
	return breakNum;
}


//LFU�㷨��ȱҳ�������������Ϊҳ����
int LFU_extention(int n, int sliceSize) {
	int i;//ѭ������
	int j;//ѭ������
	int breakNum = 0;
	int pos = 0;//��Ҫ�滻���±�ֵ
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//�����ǰҳ�Ų����ڴ���
			int checkPos = Check(n, pageArray);
			if (checkPos == -1) {//��ǰ�����ڴ������������滻����
								 //pos = GetMax_extention(n, pageArray, sliceSize);
				pos = GetMin(n, pageArray);
				int temp = 0;
				for (j = 0; temp < sliceSize && pos + j < n; temp++) {
					if (j == 0 || Equation((SRC[i] + temp) % PAGEDIFFN, n, pageArray) == -1) {
						HitHash[pageArray[pos + j].num] = -1;
						pageArray[pos + j].num = (SRC[i] + temp) % PAGEDIFFN;
						pageArray[pos + j].time = 1;
						HitHash[(SRC[i] + temp) % PAGEDIFFN] = pos + j;
						j++;
					}
				}
			}
			//�ڴ�δ�����ڳ�ʼ����ʱ��������һ��һ��װ���
			else {
				HitHash[pageArray[checkPos].num] = -1;
				pageArray[checkPos].num = SRC[i];//�ڴ�δ��,�������
				pageArray[checkPos].time = 1;
				HitHash[SRC[i]] = checkPos;
			}
			breakNum++;//��ҳ�����ڴ���ȱҳ����+1
		}
		else {
			pageArray[Equation(SRC[i], n, pageArray)].time++;
		}
	}
	return breakNum;
}



//Clock�㷨,��ʱtimeΪ��־λ,ֻ��0��1����ֵ
int Clock(int n) {
	int i;//ѭ������
	int j = 0;//j%nΪ����ָ��,�˴�������������ʹ��
	int breakNum = 0;
	int pos = 0;//��Ҫ�滻���±�ֵ
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//�����ǰҳ�Ų����ڴ���
			breakNum++;
			while (1) {
				if (pageArray[j % n].time == 0) {
					HitHash[pageArray[j % n].num] = -1;
					pageArray[j % n].num = SRC[i];
					pageArray[j % n].time = 1;
					HitHash[SRC[i]] = j%n;
					j++;
					break;
				}
				else {
					pageArray[j % n].time = 0;
					j++;
					continue;
				}
			}
		}
		else {//��ǰҳ�����ڴ���
			pageArray[Equation(SRC[i], n, pageArray)].time = 1;//��ǰ��־λ��Ϊ1
			j = Equation(SRC[i], n, pageArray) + 1;
		}

	}
	return breakNum;

}

//Clock�㷨,��ʱtimeΪ��־λ,ֻ��0��1����ֵ
int Clock_extention(int n, int sliceSize) {
	int i;//ѭ������
	int j = 0;//j%nΪ����ָ��,�˴�������������ʹ��
	int breakNum = 0;
	int pos = 0;//��Ҫ�滻���±�ֵ
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//�����ǰҳ�Ų����ڴ���
			breakNum++;
			int temp = 0;
			int k;
			for (k = 0; temp < sliceSize && pos + k < n; temp++) {
				if (k == 0 || Equation((SRC[i] + temp) % PAGEDIFFN, n, pageArray) == -1) {
					if (k == 0) {
						while (1) {
							if (pageArray[j % n].time == 0) {
								HitHash[pageArray[j % n].num] = -1;
								pageArray[j % n].num = SRC[i];
								pageArray[j % n].time = 1;
								HitHash[SRC[i]] = j%n;
								j++;
								break;
							}
							else {
								pageArray[j % n].time = 0;
								j++;
								continue;
							}
						}
					}
					else {
						HitHash[pageArray[j%n].num] = -1;
						pageArray[j % n].num = (SRC[i] + temp) % PAGEDIFFN;
						pageArray[j % n].time = 1;
						HitHash[(SRC[i] + temp) % PAGEDIFFN] = j % n;
						j++;
					}
					k++;
				}
			}
		}
		else {//��ǰҳ�����ڴ���
			pageArray[Equation(SRC[i], n, pageArray)].time = 1;//��ǰ��־λ��Ϊ1
			j = Equation(SRC[i], n, pageArray) + 1;
		}

	}
	return breakNum;

}

int main() {
	float notHitLRU, notHitLRUE, notHitClock, notHit;
	DWORD  dwStart, dwEnd;
	FILE *fp;
	fp = fopen("result.txt","w");
	init_SRC();
	
	/*
	for (int i = 20; i < 521; i = i + 20) {
		//printf("OTP�㷨:��ҳ����Ϊ %d ʱȱҳ����Ϊ:%d:\n", i, OPT(i));
		//printf("FIFO�㷨:��ҳ����Ϊ %d ʱȱҳ����Ϊ: %d\n", i, FIFO(i));
		//printf("LRU�㷨:��ҳ����Ϊ %d ʱȱҳ����Ϊ: %d\n", i, LRU(i));
		//printf("Clock�㷨:��ҳ����Ϊ %d ʱȱҳ����Ϊ : %d\n", i, Clock(i));
	}
	for (int i = 501; i < 521; i++) {
		//printf("OTP�㷨:��ҳ����Ϊ %d ʱȱҳ����Ϊ:%d:\n", i, OPT(i));
		//printf("FIFO�㷨:��ҳ����Ϊ %d ʱȱҳ����Ϊ: %d\n", i, FIFO(i));
		//printf("LRU�㷨:��ҳ����Ϊ %d ʱȱҳ����Ϊ: %d\n", i, LRU(i));
		//printf("Clock�㷨:��ҳ����Ϊ %d ʱȱҳ����Ϊ : %d\n", i, Clock(i));
	}*/
	
	for (int i = 2; i < 20; i += 1) {
		init_HitHash();
		dwStart = GetTickCount();
		notHitLRU = ((float)LRU(BUFFERNUM) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("LRU�㷨:��ҳ����Ϊ %d , Ƭ��СΪ %d ʱȱҳ��Ϊ: %f%%, ����ʱ��Ϊ%u\n", BUFFERNUM, i, notHitLRU, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n", notHitLRU, (dwEnd - dwStart));
		
		init_HitHash();
		dwStart = GetTickCount();
		notHitLRUE = ((float)LRU_extention(BUFFERNUM, i) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("LRU_extention�㷨:��ҳ����Ϊ %d , Ƭ��СΪ %d ʱȱҳ��Ϊ: %f%%, ����ʱ��Ϊ%u\n\n", BUFFERNUM, i, notHitLRUE, (dwEnd-dwStart));
		fprintf(fp, "%f  %u\n", notHitLRUE, (dwEnd - dwStart));
		
		init_HitHash();
		dwStart = GetTickCount();
		notHit = ((float)LFU(BUFFERNUM) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("LFU�㷨:��ҳ����Ϊ %d ,Ƭ��СΪ %d ʱȱҳ��Ϊ: %f%%, ����ʱ��Ϊ%u\n", BUFFERNUM, i, notHit, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n", notHit, (dwEnd - dwStart));

		init_HitHash();
		dwStart = GetTickCount();
		notHit = ((float)LFU_extention(BUFFERNUM, i) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("LFU_extention�㷨:��ҳ����Ϊ %d , Ƭ��СΪ %d ʱȱҳ��Ϊ: %f%%, ����ʱ��Ϊ%u\n\n", BUFFERNUM, i, notHit, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n", notHit, (dwEnd - dwStart));

		init_HitHash();
		dwStart = GetTickCount();
		notHitClock = ((float)Clock(BUFFERNUM) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("Clock�㷨:��ҳ����Ϊ %d , Ƭ��СΪ %d ʱȱҳ��Ϊ: %f%%, ����ʱ��Ϊ%u\n", BUFFERNUM, i, notHitClock, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n", notHitClock, (dwEnd - dwStart));

		init_HitHash();
		dwStart = GetTickCount();
		notHit = ((float)Clock_extention(BUFFERNUM, i) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("Clock_extention�㷨:��ҳ����Ϊ %d , Ƭ��СΪ %d ʱȱҳ��Ϊ: %f%%, ����ʱ��Ϊ%u\n\n", BUFFERNUM, i, notHit, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n\n", notHit, (dwEnd - dwStart));

	}
	return 0;
}
