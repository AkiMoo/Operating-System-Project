#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <process.h>
#include <iostream>
using namespace std;

//定义全局变量 
int producer;// 生产者数量
int consumer;// 消费者数量
int product_num;// 生产产品个数
int counter;// 当前产品数量
int have_produce;// 已经生产个数
int bufferSize;// 缓冲区大小

HANDLE Mutex;//互斥信号量 
CRITICAL_SECTION critical_section;   //临界区 

// 临界区：生产者线程函数
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

// 临界区：消费者线程函数
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

// 互斥: 生产者线程函数
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

// 互斥: 消费者线程函数
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

int main(int argc, char *argv[]) {
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
        system("cls");
        cout << "请输入生产者的数量:";
        while (cin >> producer) {
            if (producer > 0) break;
            else cout << "生产者数量必须是正数\n请再次输入: ";
        } 
        cout << "请输入消费者的数量:";
        while (cin >> consumer) {
            if (consumer > 0) break;
            else cout << "消费者数量必须是正数.\n请再次输入: ";     
        }
        cout << "请输入所需生产总数:";
        while (cin >> product_num) {
            if (product_num > 0) break;
            else cout << "所需生产总数必须是正数.\n请再次输入: ";
        }
        cout << "请设置缓冲区大小:";
        while (cin >> bufferSize) {
            if (bufferSize > 0) break;
            else cout << "缓冲区大小一定要大于0.\n请再次输入: ";
        }

        system("cls");

        if (num == "1") InitializeCriticalSection(&critical_section);   // 初始化临界区
        else Mutex = CreateMutex(NULL, FALSE, NULL); // 创建一个互斥体    

        // 初始化计数器
        counter = 0;
        have_produce = 0;

        HANDLE* handles = new HANDLE[producer + consumer];
        DWORD* ThreadId = new DWORD[producer + consumer];

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
		else {           //互斥
            for (int i = 0; i < producer; ++i, ++threadCount)  // 创建生产者线程
                handles[threadCount] = CreateThread(NULL, 0, producer2, index_pro + i, 0, &ThreadId[threadCount]);
            for (int i = 0; i < consumer; ++i, ++threadCount)  // 创建消费者线程
                handles[threadCount] = CreateThread(NULL, 0, consumer2, index_con + i, 0, &ThreadId[threadCount]);
        }

        WaitForMultipleObjects(threadCount, handles, TRUE, INFINITE); // 等待线程结束

        for (int i = 0; i < threadCount; i++) { // 关闭句柄
            CloseHandle(handles[i]);
        }

        cout << "-----------------------------------------------"
             << "\n|     测试结束          |"
             << "\n-----------------------------------------------"
             << "\n 版本: " << ((num == "1") ? "临界区模式" : "互斥信号量模式")
             << "\n 生产者数量:  " << producer
             << "\n 消费者数量:  " << consumer
             << "\n 缓冲区大小:  " << bufferSize
             << "\n 总生产产品数:  " << product_num
             << "\n 是否继续运行程序 ( Y / N): ";

        if (num == "1")  DeleteCriticalSection(&critical_section);  // 销毁临界区 
        else ReleaseMutex(Mutex);      //释放互斥体

        string selection;
        while (cin >> selection) {
            if (selection == "Y" || selection == "N") break;
            else cout << "非法输入.请输入大写的 Y 或 N:";
        }
        if (selection == "N") break;  // 退出程序
        system("cls");
    }
    system("pause");
    return 0;
}

