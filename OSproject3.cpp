#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <stdlib.h> 
using namespace std;

struct ArrInfo
{
	int len;
	int start;
	int end;
};


//调用<stdlib.h>里的qsort函数而准备的cmp函数。从小到大排序 
int cmp(const void *a,const void *b){
	return *(int*)a-*(int*)b;
}

DWORD WINAPI Qsort(LPVOID lpPara)
{
	ArrInfo *arrinfo = (ArrInfo *)lpPara;
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,"data");
	VOID *hMap = MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,0,0,0);
	//获得各个线程共享的数组首地址
	int *arr1 = (int *)hMap;

	int left = arrinfo->start;
	int right = arrinfo->end;
	
	qsort(arr1,right-left,sizeof(arr1[1]),cmp);//进行排序 

	UnmapViewOfFile(hMap);//取消映射 
	CloseHandle(hFileMap);//结束句柄 
	return 0;
}

int main()
{
	//从input.txt读取数据
	ifstream file("input.txt");
	int num = 1000000;
    vector<int> a;
	int x;
    for (int i=0; i<num; i++)
	{
		file >> x;
		a.push_back(x);
	}

	//创建文件映射对象
	HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,0x1000000,"data");
	//将文件视图映射到本进程的地址空间
	VOID *hMap = MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,0,0,0);

	//将数组首地址关联映射地址空间的首地址，使它们指向同一块内存
    memcpy(hMap, &a[0], num * sizeof(int));

	//快排线程参数
    ArrInfo arrinfo;
    arrinfo.start = 0;
    arrinfo.end = num - 1;
    arrinfo.len = num;
	//创建快排主线程
	HANDLE hQsort = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Qsort, &arrinfo, 0, NULL);
	WaitForSingleObject(hQsort, INFINITE);
	CloseHandle(hQsort);
	//将结果写入output.txt 
	int *result = (int *)hMap;
	ofstream output("output.txt");
	for (int i = 0; i < 1000000; i++)
		output << result[i] << ' ';

	UnmapViewOfFile(hMap);
	CloseHandle(hFileMap);	
}
