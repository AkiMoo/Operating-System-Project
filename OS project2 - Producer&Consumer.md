<center><font size =7>OS project2 - Producer&Consumer</font></center>


要求：

•Design and implement two versions of a Producer-Consumer application.

•The Producer thread shall increment a counter value and display it on standard output. 

•The Consumer thread shall decrement the counter and print the current counter value on standard output.

•Both threads shall communicate using a shared variable.

•Version 1: Write a version of your program that uses **CritcalSection** for synchronization.

•Version 2: Write a version of your program that uses **Mutex** for synchronization.

•Do a performance comparison of both versions of your program. 

•Measure the maximum/minimum counter values reached by both versions of the program. Explain the results. 

•Have a GUI (option).

•Can set the number of threads (option).

<font size = 6>**函数设计：**</font>

因为实验要求做临界区模式和互斥信号量模式的生产者&消费者问题，数据部分分2part说明：

**<font size = 4>*Part one：临界区模式*</font>**

```C++
DWORD WINAPI producer1(LPVOID Param) {
    int index = *(int*)Param;  // 获取线程对应的参数下标
    while (have_produce < product_num) {  // 只有目前生产数目小于所需总数才能进入循环
        EnterCriticalSection(&critical_section);  // 进入临界区  
        if (counter < bufferSize && have_produce < product_num) { // counter不能大于缓冲区大小
            have_produce++;
            counter++;
            cout << "线程" << index
                << "的生产者, 当前产品数量为: " << counter
                << " , 总共已生产: " << have_produce << endl;
        }
        LeaveCriticalSection(&critical_section);  // 离开临界区
    }
    return 0;
}
```

这是临界区的生产者函数，基本上都在使用WINDOWS API接口了，一开始先设置线程函数参数LPVOID Param，然后使用EnterCirticalSection和LeaveCirticalSection做临界区出入接口。中间部分就是普通的计数问题，注意一下生产者的生产条件就可以了，当前产品不能大于缓冲区大小&目标生产量达成则不再生产。

因为一开始变量设计有累计产品和当前产品，所以生产者函数中要记录两个变量的变化。



```C++
DWORD WINAPI consumer1(LPVOID Param) {
    int index = *(int*)Param;
    while (have_produce < product_num) {
        EnterCriticalSection(&critical_section);
        if (have_produce < product_num && counter > 0) {
            counter--;
            cout << "线程" << index
                << "的消费者, 当前产品数量为: " << counter << endl;
        }
        LeaveCriticalSection(&critical_section);
    }
    return 0;
}
```

临界区的消费者函数与生产者几乎一致了，只是在中间的计数问题变成了修改counter变量的数值而已。



**<font size = 4>*Part two：互斥信号量模式*</font>**

```C++
DWORD WINAPI producer2(LPVOID Param) {
    int index = *(int*)Param;
    while (have_produce < product_num) {
        WaitForSingleObject(Mutex, INFINITE);  // 无限等待其他线程释放mutex
        if (have_produce < product_num && counter < bufferSize) {
            have_produce++;
            counter++;
            cout << "线程" << index
                << "的生产者, 当前产品数量为: " << counter
                << " , 总共已生产: " << have_produce << endl;
        }
        ReleaseMutex(Mutex);   // 释放mutex
    }
    return 0;
}
```

互斥信号量的生产者函数。其实与上面临界区的生产者函数大同小异，区别就是在于生产前先获取互斥信号量，完成任务之后再释放。这些都可以直接调用API接口函数来实现。数据修改部分和Part one的生产者完全一样。



```C++
DWORD WINAPI consumer2(LPVOID Param) {
    int index = *(int*)Param;
    while (have_produce < product_num) {
        WaitForSingleObject(Mutex, INFINITE);
        if (have_produce < product_num && counter > 0) {
            counter--;
            cout << "线程" << index
                << "的消费者, 当前产品数量为: " << counter << endl;
        }
        ReleaseMutex(Mutex);
    }
    return 0;
}
```

互斥信号量的消费者函数与生产者几乎一致了，只是在中间的计数问题变成了修改counter变量的数值而已。



主函数部分：

```C++
while (true) {
        cout << "\n\n            ----------------------------------------------------"
             << "\n                      你已进入生产者&消费者测试程序"
             << "\n                    输入数字“1 ”选择临界区模式"
             << "\n                    输入数字“2 ”选择互斥信号量模式"
             << "\n            ----------------------------------------------------"
             << "\n            请输入数字:";
             string num;
        while (cin >> num) {
            if (num == "1" || num == "2") break;
            else cout << "非法输入，只能输入“1 ”或“2 ”:";
        }
```

主循环体的开头加一个小UI界面，选择所需要的模式



```C++
if (num == "1") InitializeCriticalSection(&critical_section);   // 初始化临界区
        else Mutex = CreateMutex(NULL, FALSE, NULL); // 创建一个互斥体    

// 初始化计数器
counter = 0;
have_produce = 0;

HANDLE* handles = new HANDLE[producer + consumer];
DWORD* ThreadId = new DWORD[producer + consumer];
```

   变量初始化和创建部分，就是调用一下两种模式的API相关功能了



```C++
// 数组储存的是线程下标参数
    int* index_pro = new int[producer];
    for (int i = 0; i < producer; i++)  index_pro[i] = i;
    int* index_con = new int[consumer];
    for (int i = 0; i < consumer; i++)  index_con[i] = i;

    int threadCount = 0; // 线程数量 
    if (num == "1") {  // 临界区
        for (int i = 0; i < producer; ++i, ++threadCount)  // 创建生产者线程
            handles[threadCount] = CreateThread(NULL, 0, producer1, index_pro + i, 0, &ThreadId[threadCount]);      
        for (int i = 0; i < consumer; ++i, ++threadCount)  // 创建消费者线程
            handles[threadCount] = CreateThread(NULL, 0, consumer1, index_con + i, 0, &ThreadId[threadCount]);
    } 
	else {  //互斥
        for (int i = 0; i < producer; ++i, ++threadCount)  // 创建生产者线程
            handles[threadCount] = CreateThread(NULL, 0, producer2, index_pro + i, 0, &ThreadId[threadCount]);
        for (int i = 0; i < consumer; ++i, ++threadCount)  // 创建消费者线程
            handles[threadCount] = CreateThread(NULL, 0, consumer2, index_con + i, 0, &ThreadId[threadCount]);
    }

    WaitForMultipleObjects(threadCount, handles, TRUE, INFINITE); // 等待线程结束

    for (int i = 0; i < threadCount; i++) { // 关闭句柄
        CloseHandle(handles[i]);
    }
```

这段代码是用于显示输出结果的。每个生产者消费者都有对应的线程号。于是采用建立数组来保存这些号码，然后使用CreateThead函数来运行调用对应的线程。按照接口的规定传参就OK，没什么特别的，每个线程的处理方式都是一样的。记得在最后处理完加上WaitForMultipleObjects和CloseHandle函数，不然会引发错误。



```C++
cout << "-----------------------------------------------"
             << "\n|     测试结束          |"
             << "\n-----------------------------------------------"
             << "\n 版本: " << ((num == "1") ? "临界区模式" : "互斥信号量模式")
             << "\n 生产者数量:  " << producer
             << "\n 消费者数量:  " << consumer
             << "\n 缓冲区大小:  " << bufferSize
             << "\n 总生产产品数:  " << product_num
             << "\n 是否继续运行程序 ( Y / N): ";
```

最后加一个运行结束的显示UI。



<font size = 6>**运行截图：**</font>

备注：本程序中的设定是同一个生产者可以反复生产。因为所需生产量是由用户自设定的，程序是运行到达所需生产量后停止。

![主界面](C:\Users\HP\Desktop\主界面.png)

<center>主界面

​    

![模式1运行测试](C:\Users\HP\Desktop\模式1运行测试.png)

<center>模式1运行测试</center>



![模式2运行测试](C:\Users\HP\Desktop\模式2运行测试.png)

<center>模式2运行测试</center>

