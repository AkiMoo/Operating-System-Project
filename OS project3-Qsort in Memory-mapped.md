<center><font size =7>OS project3 - </center>
<center><font size=7>Qsort in Memory-mapped</font></center>

<center><font size = 6>学号：17341126 姓名：莫烜奇</font></center>

要求：

•Design an implement a simple sort program that uses memory-mapped file I/O to open a binary file filled with integer values and uses quicksort to sort the file content.

• Use the function **qsort****()** from **stdlib.h** to perform the actual sorting.

•Use the Windows-functions **CreateFileMapping****()**, **MapViewOfFile****()** and **FlushViewOfFile****()** to perform the memory-mapped file I/O.



<font size=5>**个人理解：**</font>

要求描述得相对简单，一开始不知道实验比一般的排序实验要多做什么工作。通过查询了要求中给出来的内存映射函数，并且查阅了相关的“内存映射读取大文件”的博客和资料，个人认为本实验是要求我们用内存映射读取较大的数据，并使用线程的方式对其进行快速排序。

<font size=5>**个人补充：**</font>

题目中要求读取二进制文件，但是以前没有这种经验，尝试了一些网络上的方法，并没有成功。

由于时间限制以及个人能力有限，于是在临截止的最后一天，使用读入十进制数字的方式完成内存映射——线程排序的实验。

<font size = 6>**函数设计：**</font>

准备工作提要：

巨大的数字文件没有采用在程序中生成随机数的方式，是在别的程序上面生成了一份随机数文件，然后在这个实验中作为input.txt来使用。



下面是正式的代码部分：

```C++
struct ArrInfo
{
	int len;
	int start;
	int end;
};

```

定义ArrInfo结构体，用于排序线程读参。



```C++
//调用<stdlib.h>里的qsort函数而准备的cmp函数。从小到大排序 
int cmp(const void *a,const void *b){
	return *(int*)a-*(int*)b;
}
```

要求中提到要我们用<stdlib.h>里面的qsort函数来排序，而这里的qsort最主要是自己定义的cmp函数。因为排int类型，且无特殊要求，在这里我只做了强制转换int，然后按从小到大的序列的cmp



```C++
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
```
定义线程函数Qsort，设置线程函数参数LPVOID lpPara，之后就是应用OpenFileMapping和MapViewOfFile了。具体的函数接口也是按网上学的，OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE,文件名)读入数据，然后用MapViewOfFile将文件映射对象映射到当前应用程序的地址空间。

具体排序的处理，只需要提取qsort的参数就可以了，数组名称，数组长度，数据类型大小，以及上面的提到的cmp。



```C++
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
```
运行的主函数部分，其实就是调用准备好的排序线程了。

用ifstream file("input.txt")读取数据入vector<int>里，数字按顺序读入，使用vector STL里的pushback。

之后就是使用CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,0x1000000,"data");

来将读入的文件映射到内存空间了，后续的MapViewOfFile使用与前文提到的一样。

使用memcpy函数的功能是从源内存地址的起始位置开始拷贝若干个字节到目标vector元组中。

接下来调入数组的各样参数，传入Qsort线程进行排序。

因为数据量大，最后我的处理是把排序结果写到output.txt文件里。

结束之后就关闭映射和句柄，所有工作结束。

<font size = 6>**运行截图：**</font>

排序前：

![before](C:\Users\HP\Desktop\before.png)

排序后：

![after](C:\Users\HP\Desktop\after.png)

![after2](C:\Users\HP\Desktop\after2.png)

