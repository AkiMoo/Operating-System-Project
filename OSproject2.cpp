#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <process.h>
#include <iostream>
using namespace std;

//����ȫ�ֱ��� 
int producer;// ����������
int consumer;// ����������
int product_num;// ������Ʒ����
int counter;// ��ǰ��Ʒ����
int have_produce;// �Ѿ���������
int bufferSize;// ��������С

HANDLE Mutex;//�����ź��� 
CRITICAL_SECTION critical_section;   //�ٽ��� 

// �ٽ������������̺߳���
DWORD WINAPI producer1(LPVOID Param) {
    int index = *(int*)Param;  // ��ȡ�̶߳�Ӧ�Ĳ����±�
    while (have_produce < product_num) {  // ֻ��Ŀǰ������ĿС�������������ܽ���ѭ��
        EnterCriticalSection(&critical_section);  // �����ٽ���  
        if (counter < bufferSize && have_produce < product_num) { // counter���ܴ��ڻ�������С
            have_produce++;
            counter++;
            cout << "�߳�" << index
                << "��������, ��ǰ��Ʒ����Ϊ: " << counter
                << " , �ܹ�������: " << have_produce << endl;
        }
        LeaveCriticalSection(&critical_section);  // �뿪�ٽ���
    }
    return 0;
}

// �ٽ������������̺߳���
DWORD WINAPI consumer1(LPVOID Param) {
    int index = *(int*)Param;
    while (have_produce < product_num) {
        EnterCriticalSection(&critical_section);
        if (have_produce < product_num && counter > 0) {
            counter--;
            cout << "�߳�" << index
                << "��������, ��ǰ��Ʒ����Ϊ: " << counter << endl;
        }
        LeaveCriticalSection(&critical_section);
    }
    return 0;
}

// ����: �������̺߳���
DWORD WINAPI producer2(LPVOID Param) {
    int index = *(int*)Param;
    while (have_produce < product_num) {
        WaitForSingleObject(Mutex, INFINITE);  // ���޵ȴ������߳��ͷ�mutex
        if (have_produce < product_num && counter < bufferSize) {
            have_produce++;
            counter++;
            cout << "�߳�" << index
                << "��������, ��ǰ��Ʒ����Ϊ: " << counter
                << " , �ܹ�������: " << have_produce << endl;
        }
        ReleaseMutex(Mutex);   // �ͷ�mutex
    }
    return 0;
}

// ����: �������̺߳���
DWORD WINAPI consumer2(LPVOID Param) {
    int index = *(int*)Param;
    while (have_produce < product_num) {
        WaitForSingleObject(Mutex, INFINITE);
        if (have_produce < product_num && counter > 0) {
            counter--;
            cout << "�߳�" << index
                << "��������, ��ǰ��Ʒ����Ϊ: " << counter << endl;
        }
        ReleaseMutex(Mutex);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    while (true) {
        cout << "\n\n            ----------------------------------------------------"
             << "\n                      ���ѽ���������&�����߲��Գ���"
             << "\n                    �������֡�1 ��ѡ���ٽ���ģʽ"
             << "\n                    �������֡�2 ��ѡ�񻥳��ź���ģʽ"
             << "\n            ----------------------------------------------------"
             << "\n            ����������:";
        string num;
        while (cin >> num) {
            if (num == "1" || num == "2") break;
            else cout << "�Ƿ����룬ֻ�����롰1 ����2 ��:";
        }
        system("cls");
        cout << "�����������ߵ�����:";
        while (cin >> producer) {
            if (producer > 0) break;
            else cout << "��������������������\n���ٴ�����: ";
        } 
        cout << "�����������ߵ�����:";
        while (cin >> consumer) {
            if (consumer > 0) break;
            else cout << "��������������������.\n���ٴ�����: ";     
        }
        cout << "������������������:";
        while (cin >> product_num) {
            if (product_num > 0) break;
            else cout << "����������������������.\n���ٴ�����: ";
        }
        cout << "�����û�������С:";
        while (cin >> bufferSize) {
            if (bufferSize > 0) break;
            else cout << "��������Сһ��Ҫ����0.\n���ٴ�����: ";
        }

        system("cls");

        if (num == "1") InitializeCriticalSection(&critical_section);   // ��ʼ���ٽ���
        else Mutex = CreateMutex(NULL, FALSE, NULL); // ����һ��������    

        // ��ʼ��������
        counter = 0;
        have_produce = 0;

        HANDLE* handles = new HANDLE[producer + consumer];
        DWORD* ThreadId = new DWORD[producer + consumer];

        // ���鴢������߳��±����
        int* index_pro = new int[producer];
        for (int i = 0; i < producer; i++)  index_pro[i] = i;
        int* index_con = new int[consumer];
        for (int i = 0; i < consumer; i++)  index_con[i] = i;


        int threadCount = 0; // �߳����� 
        if (num == "1") {  // �ٽ���
            for (int i = 0; i < producer; ++i, ++threadCount)  // �����������߳�
                handles[threadCount] = CreateThread(NULL, 0, producer1, index_pro + i, 0, &ThreadId[threadCount]);      
            for (int i = 0; i < consumer; ++i, ++threadCount)  // �����������߳�
                handles[threadCount] = CreateThread(NULL, 0, consumer1, index_con + i, 0, &ThreadId[threadCount]);
        } 
		else {           //����
            for (int i = 0; i < producer; ++i, ++threadCount)  // �����������߳�
                handles[threadCount] = CreateThread(NULL, 0, producer2, index_pro + i, 0, &ThreadId[threadCount]);
            for (int i = 0; i < consumer; ++i, ++threadCount)  // �����������߳�
                handles[threadCount] = CreateThread(NULL, 0, consumer2, index_con + i, 0, &ThreadId[threadCount]);
        }

        WaitForMultipleObjects(threadCount, handles, TRUE, INFINITE); // �ȴ��߳̽���

        for (int i = 0; i < threadCount; i++) { // �رվ��
            CloseHandle(handles[i]);
        }

        cout << "-----------------------------------------------"
             << "\n|     ���Խ���          |"
             << "\n-----------------------------------------------"
             << "\n �汾: " << ((num == "1") ? "�ٽ���ģʽ" : "�����ź���ģʽ")
             << "\n ����������:  " << producer
             << "\n ����������:  " << consumer
             << "\n ��������С:  " << bufferSize
             << "\n ��������Ʒ��:  " << product_num
             << "\n �Ƿ�������г��� ( Y / N): ";

        if (num == "1")  DeleteCriticalSection(&critical_section);  // �����ٽ��� 
        else ReleaseMutex(Mutex);      //�ͷŻ�����

        string selection;
        while (cin >> selection) {
            if (selection == "Y" || selection == "N") break;
            else cout << "�Ƿ�����.�������д�� Y �� N:";
        }
        if (selection == "N") break;  // �˳�����
        system("cls");
    }
    system("pause");
    return 0;
}

