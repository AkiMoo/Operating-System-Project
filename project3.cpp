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


//����<stdlib.h>���qsort������׼����cmp��������С�������� 
int cmp(const void *a,const void *b){
	return *(int*)a-*(int*)b;
}

DWORD WINAPI Qsort(LPVOID lpPara)
{
	ArrInfo *arrinfo = (ArrInfo *)lpPara;
	HANDLE hFileMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,"data");
	VOID *hMap = MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,0,0,0);
	//��ø����̹߳���������׵�ַ
	int *arr1 = (int *)hMap;

	int left = arrinfo->start;
	int right = arrinfo->end;
	
	qsort(arr1,right-left,sizeof(arr1[1]),cmp);//�������� 

	UnmapViewOfFile(hMap);//ȡ��ӳ�� 
	CloseHandle(hFileMap);//������� 
	return 0;
}

int main()
{
	//��input.txt��ȡ����
	ifstream file("input.txt");
	int num = 1000000;
    vector<int> a;
	int x;
    for (int i=0; i<num; i++)
	{
		file >> x;
		a.push_back(x);
	}

	//�����ļ�ӳ�����
	HANDLE hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,0x1000000,"data");
	//���ļ���ͼӳ�䵽�����̵ĵ�ַ�ռ�
	VOID *hMap = MapViewOfFile(hFileMap,FILE_MAP_ALL_ACCESS,0,0,0);

	//�������׵�ַ����ӳ���ַ�ռ���׵�ַ��ʹ����ָ��ͬһ���ڴ�
    memcpy(hMap, &a[0], num * sizeof(int));

	//�����̲߳���
    ArrInfo arrinfo;
    arrinfo.start = 0;
    arrinfo.end = num - 1;
    arrinfo.len = num;
	//�����������߳�
	HANDLE hQsort = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Qsort, &arrinfo, 0, NULL);
	WaitForSingleObject(hQsort, INFINITE);
	CloseHandle(hQsort);
	//�����д��output.txt 
	int *result = (int *)hMap;
	ofstream output("output.txt");
	for (int i = 0; i < 1000000; i++)
		output << result[i] << ' ';

	UnmapViewOfFile(hMap);
	CloseHandle(hFileMap);	
}
