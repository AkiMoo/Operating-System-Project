#include<stdio.h> 
#include<iostream>
#include<string>
#include<fstream>
#include<cstdlib>
#include<Windows.h>
#include<direct.h>
using namespace std;

class Minshell{
	public:
		void run();//���뺯�� 
		Minshell(); 
	private:
		string command;//����ָ�����
		void instruction();//��ʾָ��� 
		void exe_file(const string& filepath);
		void create_process(const string& command);//�������� 
		void get_command(string &command);//��Ի�ȡ��command������� 
		string path();//��ʾ��ǰĿ¼·�� 
};

Minshell::Minshell(){
	//������г�ʼ�� 
}

void Minshell::instruction(){
	cout<<"<<quit:�˳�Minshell>>"<<endl;
	cout<<"<<Minshell abc.txt:�򿪵�ǰĿ¼�µ�abc.txt�����������ָ��>>"<<endl;
	cout<<"<<showRoute ��ʾ��ǰĿ¼·��>>"<<endl;
	cout<<"<<Minshell def.exe:ִ�е�ǰĿ¼�µ�def�ļ�>>"<<endl;
	cout<<endl<<endl;//������Ϊ����ʾ������ 
}

void Minshell::run(){
	instruction();
	while(1){
		cout<<"��ǰĿ¼·����"<<getcwd(NULL,0)<<endl;
		cout<<"������ָ��"<<endl;
		get_command(command);
	}
}


void Minshell::get_command(string &command){
	getline(cin,command);
	if(command == "showRoute")
	{
		cout<<"��ǰĿ¼��"<<getcwd(NULL,0)<<endl;
	}
	else if(command == "quit"){
		cout <<"�����˳�Minshell"<<endl; 
		string quiting="quit.txt";
		exe_file(quiting);
	}
	else if(command.length()>9){
		string prefix = command.substr(0,8);
		if(prefix == "Minshell"){
			command = command.substr(9);
			if (command.substr(command.length() - 3, command.length()) == "exe") create_process(command);
			if (command.substr(command.length() - 3, command.length()) == "txt") exe_file(command);
		}
		else{
			system(command.c_str());
		}
	}
	else{
		system(command.c_str());
	}
}

void Minshell::exe_file(const string& filepath){
	ifstream ifile(filepath.c_str(),ios::in);
	if(!ifile){
		cout<<"FAILED,����Ŀ¼or�ļ����Ƿ���ȷ"<<endl;
	}
	else
	{
		string content;
		while(getline(ifile,content)){
			cout<<content<<endl;
			create_process(content);
		}
	}
	ifile.close();
}

void Minshell::create_process(const string& command) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	char command_buf[999];
	sprintf(command_buf, "%s", command.c_str());

	//�����ӽ��� 
	if (!CreateProcess(NULL, // ���ڴ�ָ����ִ���ļ����ļ���
		command_buf, // use command line
		NULL, // Ĭ�Ͻ��̰�ȫ��
		NULL, // Ĭ�Ͻ��̰�ȫ��
		FALSE, // ָ����ǰ�����ڵľ�������Ա��ӽ��̼̳�
		0, // ����Ҫ�����µĿ���̨���� 
		NULL, // ʹ�ñ����̵Ļ�������
		NULL, // ʹ�ñ����̵���������Ŀ¼
		&si,
		&pi)) {
		fprintf(stderr, "û������������\n");
	}

	WaitForSingleObject(pi.hProcess, INFINITE);//�����̵ȴ��ӽ���������� 

	CloseHandle(pi.hProcess);//�رվ�� 
	CloseHandle(pi.hThread);//�رվ�� 
}

int main() {
	Minshell cmd;
	cmd.run();
	return 0;
}


