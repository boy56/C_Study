#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <Mmsystem.h>

#define PAGENUM 50000		//需要访问的数据的数量(有重复)
#define PAGEDIFFN 1000		//数据中不同的页号数量(无重复)
#define BUFFERNUM 400		//buffer的数量
#define SLICESIZE 3			//替换的片的大小

/*
int SRC[] = { 0, 9, 8, 4, 4, 3, 6, 5, 1, 5, 0,
2, 1, 1, 1, 1, 8, 8, 5, 3, 9, 8,
9, 9, 6, 1, 8, 4, 6, 4, 3, 7, 1,
3, 2, 9, 8, 6, 2, 9, 2, 7, 2, 7,
8, 4, 2, 3, 0, 1, 9, 4, 7, 1, 5,
9, 1, 7, 3, 4, 3, 7, 1, 0, 3, 5,
9, 9, 4, 9, 6, 1, 7, 5, 9, 4, 9,
7, 3, 6, 7, 7, 4, 5, 3, 5, 3, 1,
5, 6, 1, 1, 9, 6, 6, 4, 0, 9, 4, 3};//共一百个

*/

int SRC[PAGENUM];//作业5需要
int HitHash[PAGEDIFFN];


typedef struct page {
	int num;//页号
	int time;//未使用时间
	struct page *next;	//在改进的算法中用到
}Page, *PPage;


//初始化SRC数组,适用于作业5
//每20个连成一组
void init_SRC() {
	int i, j;//i循环下标
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

//检查最长时间不用的页面,index为当前SRC下标
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

//得到内存中存在时间最长的页面
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

//检查物理内存是否已满,满返回-1, 不满空闲的下标值
int Check(int n, PPage array) {
	int i;
	for (i = 0; i < n; i++) {
		if (array[i].num == -1) return i;
	}
	return -1;
}

//检查某页是否在内存,若存在，返回所在下标,pageN为SRC[i],n为内存中页框数
int Equation(int pageN, int n, PPage array) {
	/*int i;
	for (i = 0; i < n; i++) {
		if (array[i].num == pageN) return i;
	}
	*/
	return HitHash[pageN];
}


//OPT算法的缺页数量,传入参数为页框数
int OPT(int n) {
	int i;
	int j;
	int breakNum = 0;
	int pos = 0;//需要替换的下标值
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//如果当前页号不在内存中
			if (Check(n, pageArray) == -1) {//当前物理内存已满，进行替换操作
				pos = GetLongTime(i, n, pageArray);
				pageArray[pos].num = SRC[i];
			}
			else {
				pageArray[Check(n, pageArray)].num = SRC[i];//内存未满,向内添加

			}
			breakNum++;//该页不在内存中缺页数量+1
		}
		for (j = 0; j < n; j++) pageArray[j].time = 0;//将间隔时间清空,因为该时间在GetLongTime中每次都要重新计算
	}
	return breakNum;
}

//FIFO算法的缺页数量,传入参数为页框数
int FIFO(int n) {
	int i;
	int j;
	int breakNum = 0;
	int pos = 0;//需要替换的下标值
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		for (j = 0; j < n; j++) {
			if (pageArray[j].num != -1) pageArray[j].time++;
		}//所有时间+1
		if (Equation(SRC[i], n, pageArray) == -1) {//如果当前页号不在内存中
			if (Check(n, pageArray) == -1) {//当前物理内存已满，进行替换操作
				pos = GetMax(n, pageArray);
				pageArray[pos].num = SRC[i];
				pageArray[pos].time = 0;
			}
			else {
				pageArray[Check(n, pageArray)].num = SRC[i];//内存未满,向内添加
				pageArray[Check(n, pageArray)].time = 0;
			}
			breakNum++;//该页不在内存中缺页数量+1
		}

	}
	return breakNum;
}

//LRU算法的缺页数量，传入参数为页框数
int LRU(int n) {
	int i;//循环变量
	int j;//循环变量
	int breakNum = 0;
	int pos = 0;//需要替换的下标值
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//如果当前页号不在内存中
			
			for (j = 0; j < n; j++) {
				if (pageArray[j].num != -1) pageArray[j].time++;
			}
			
			int checkPos = Check(n, pageArray);
			if (checkPos == -1) {//当前物理内存已满，进行替换操作
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
				pageArray[checkPos].num = SRC[i];//内存未满,向内添加
				pageArray[checkPos].time = 0;
				//pageArray[checkPos].time = GetTickCount();
				HitHash[SRC[i]] = checkPos;

			}
			breakNum++;//该页不在内存中缺页数量+1
		}
		else {
			for (j = 0; j < n; j++) {
				if (j == Equation(SRC[i], n, pageArray))pageArray[j].time = 0;//存在内存中,将时间置为0
				else if (pageArray[j].num != -1) pageArray[j].time++;
			}

		}
	}
	return breakNum;
}

//改进的GetMax算法
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
//改进的LRU算法,成片和装入，参数为buffer页框数和片的大小
int LRU_extention(int n, int sliceSize) {
	int i;//循环变量
	int j;//循环变量
	int breakNum = 0;
	int pos = 0;//需要替换的下标值
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//如果当前页号不在内存中
			for (j = 0; j < n; j++) {
				if (pageArray[j].num != -1) pageArray[j].time++;
			}
			int checkPos = Check(n, pageArray);
			if (checkPos == -1) {//当前物理内存已满，进行替换操作
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
						pageArray[pos].num = SRC[i];	//若SRC[i]装入的为0, 则SRC[i+1]装入1,SRC[i+2]装入2......
						pageArray[pos].time = 0;
					}
					else {
						//检查后续的装入是否在内存中,通过temp递增找到第一个不在内存中的页号
						for (; Equation((SRC[i] + temp) % PAGEDIFFN, n, pageArray) != -1; temp++);
						pageArray[pos + j].num = (SRC[i] + temp) % PAGEDIFFN;
						pageArray[pos + j].time = 0;
						temp++;
					}
				}
				*/
			}
			//内存未满还在初始化的时候依旧是一个一个装入的
			else {
				HitHash[pageArray[checkPos].num] = -1;
				pageArray[checkPos].num = SRC[i];//内存未满,向内添加
				pageArray[checkPos].time = 0;
				HitHash[SRC[i]] = checkPos;
			}
			breakNum++;//该页不在内存中缺页数量+1
		}
		else {
			for (j = 0; j < n; j++) {
				if (j == Equation(SRC[i], n, pageArray))pageArray[j].time = 0;//存在内存中,将时间置为0
				else if (pageArray[j].num != -1) pageArray[j].time++;
			}
		}
	}
	return breakNum; 

}

//得到在内存中访问次数最少的页面
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

//LFU算法的缺页数量，传入参数为页框数
int LFU(int n) {
	int i;//循环变量
	int j;//循环变量
	int breakNum = 0;
	int pos = 0;//需要替换的下标值
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//如果当前页号不在内存中
			int checkPos = Check(n, pageArray);
			if (checkPos == -1) {//当前物理内存已满，进行替换操作
				pos = GetMin(n, pageArray);
				HitHash[pageArray[pos].num] = -1;
				pageArray[pos].num = SRC[i];
				pageArray[pos].time = 1;
				HitHash[SRC[i]] = pos;
			}
			else {
				HitHash[pageArray[checkPos].num] = -1;
				pageArray[checkPos].num = SRC[i];//内存未满,向内添加
				pageArray[checkPos].time = 1;
				HitHash[SRC[i]] = checkPos;

			}
			breakNum++;//该页不在内存中缺页数量+1
		}
		else {
			pageArray[Equation(SRC[i], n, pageArray)].time++;
		}
	}
	return breakNum;
}


//LFU算法的缺页数量，传入参数为页框数
int LFU_extention(int n, int sliceSize) {
	int i;//循环变量
	int j;//循环变量
	int breakNum = 0;
	int pos = 0;//需要替换的下标值
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//如果当前页号不在内存中
			int checkPos = Check(n, pageArray);
			if (checkPos == -1) {//当前物理内存已满，进行替换操作
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
			//内存未满还在初始化的时候依旧是一个一个装入的
			else {
				HitHash[pageArray[checkPos].num] = -1;
				pageArray[checkPos].num = SRC[i];//内存未满,向内添加
				pageArray[checkPos].time = 1;
				HitHash[SRC[i]] = checkPos;
			}
			breakNum++;//该页不在内存中缺页数量+1
		}
		else {
			pageArray[Equation(SRC[i], n, pageArray)].time++;
		}
	}
	return breakNum;
}



//Clock算法,此时time为标志位,只有0和1两个值
int Clock(int n) {
	int i;//循环变量
	int j = 0;//j%n为队列指针,此处将数组做队列使用
	int breakNum = 0;
	int pos = 0;//需要替换的下标值
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//如果当前页号不在内存中
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
		else {//当前页号在内存中
			pageArray[Equation(SRC[i], n, pageArray)].time = 1;//当前标志位置为1
			j = Equation(SRC[i], n, pageArray) + 1;
		}

	}
	return breakNum;

}

//Clock算法,此时time为标志位,只有0和1两个值
int Clock_extention(int n, int sliceSize) {
	int i;//循环变量
	int j = 0;//j%n为队列指针,此处将数组做队列使用
	int breakNum = 0;
	int pos = 0;//需要替换的下标值
	PPage pageArray;
	pageArray = (PPage)malloc(n*sizeof(Page));
	for (i = 0; i < n; i++) {
		pageArray[i].num = -1;
		pageArray[i].time = 0;
	}
	for (i = 0; i < PAGENUM; i++) {
		if (Equation(SRC[i], n, pageArray) == -1) {//如果当前页号不在内存中
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
		else {//当前页号在内存中
			pageArray[Equation(SRC[i], n, pageArray)].time = 1;//当前标志位置为1
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
		//printf("OTP算法:当页框数为 %d 时缺页数量为:%d:\n", i, OPT(i));
		//printf("FIFO算法:当页框数为 %d 时缺页数量为: %d\n", i, FIFO(i));
		//printf("LRU算法:当页框数为 %d 时缺页数量为: %d\n", i, LRU(i));
		//printf("Clock算法:当页框数为 %d 时缺页数量为 : %d\n", i, Clock(i));
	}
	for (int i = 501; i < 521; i++) {
		//printf("OTP算法:当页框数为 %d 时缺页数量为:%d:\n", i, OPT(i));
		//printf("FIFO算法:当页框数为 %d 时缺页数量为: %d\n", i, FIFO(i));
		//printf("LRU算法:当页框数为 %d 时缺页数量为: %d\n", i, LRU(i));
		//printf("Clock算法:当页框数为 %d 时缺页数量为 : %d\n", i, Clock(i));
	}*/
	
	for (int i = 2; i < 20; i += 1) {
		init_HitHash();
		dwStart = GetTickCount();
		notHitLRU = ((float)LRU(BUFFERNUM) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("LRU算法:当页框数为 %d , 片大小为 %d 时缺页率为: %f%%, 运行时间为%u\n", BUFFERNUM, i, notHitLRU, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n", notHitLRU, (dwEnd - dwStart));
		
		init_HitHash();
		dwStart = GetTickCount();
		notHitLRUE = ((float)LRU_extention(BUFFERNUM, i) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("LRU_extention算法:当页框数为 %d , 片大小为 %d 时缺页率为: %f%%, 运行时间为%u\n\n", BUFFERNUM, i, notHitLRUE, (dwEnd-dwStart));
		fprintf(fp, "%f  %u\n", notHitLRUE, (dwEnd - dwStart));
		
		init_HitHash();
		dwStart = GetTickCount();
		notHit = ((float)LFU(BUFFERNUM) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("LFU算法:当页框数为 %d ,片大小为 %d 时缺页率为: %f%%, 运行时间为%u\n", BUFFERNUM, i, notHit, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n", notHit, (dwEnd - dwStart));

		init_HitHash();
		dwStart = GetTickCount();
		notHit = ((float)LFU_extention(BUFFERNUM, i) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("LFU_extention算法:当页框数为 %d , 片大小为 %d 时缺页率为: %f%%, 运行时间为%u\n\n", BUFFERNUM, i, notHit, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n", notHit, (dwEnd - dwStart));

		init_HitHash();
		dwStart = GetTickCount();
		notHitClock = ((float)Clock(BUFFERNUM) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("Clock算法:当页框数为 %d , 片大小为 %d 时缺页率为: %f%%, 运行时间为%u\n", BUFFERNUM, i, notHitClock, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n", notHitClock, (dwEnd - dwStart));

		init_HitHash();
		dwStart = GetTickCount();
		notHit = ((float)Clock_extention(BUFFERNUM, i) * 100) / PAGENUM;
		dwEnd = GetTickCount();
		printf("Clock_extention算法:当页框数为 %d , 片大小为 %d 时缺页率为: %f%%, 运行时间为%u\n\n", BUFFERNUM, i, notHit, (dwEnd - dwStart));
		fprintf(fp, "%f  %u\n\n", notHit, (dwEnd - dwStart));

	}
	return 0;
}
