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
		void run();//导入函数 
		Minshell(); 
	private:
		string command;//定义指令变量
		void instruction();//显示指令功能 
		void exe_file(const string& filepath);
		void create_process(const string& command);//创建进程 
		void get_command(string &command);//针对获取的command进行输出 
		string path();//显示当前目录路径 
};

Minshell::Minshell(){
	//无需进行初始化 
}

void Minshell::instruction(){
	cout<<"<<quit:退出Minshell>>"<<endl;
	cout<<"<<Minshell abc.txt:打开当前目录下的abc.txt并运行里面的指令>>"<<endl;
	cout<<"<<showRoute 显示当前目录路径>>"<<endl;
	cout<<"<<Minshell def.exe:执行当前目录下的def文件>>"<<endl;
	cout<<endl<<endl;//空两行为了显示更清晰 
}

void Minshell::run(){
	instruction();
	while(1){
		cout<<"当前目录路径是"<<getcwd(NULL,0)<<endl;
		cout<<"请输入指令"<<endl;
		get_command(command);
	}
}


void Minshell::get_command(string &command){
	getline(cin,command);
	if(command == "showRoute")
	{
		cout<<"当前目录是"<<getcwd(NULL,0)<<endl;
	}
	else if(command == "quit"){
		cout <<"正在退出Minshell"<<endl; 
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
		cout<<"FAILED,请检查目录or文件名是否正确"<<endl;
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

	//创建子进程 
	if (!CreateProcess(NULL, // 不在此指定可执行文件的文件名
		command_buf, // use command line
		NULL, // 默认进程安全性
		NULL, // 默认进程安全性
		FALSE, // 指定当前进程内的句柄不可以被子进程继承
		0, // 不需要创建新的控制台窗口 
		NULL, // 使用本进程的环境变量
		NULL, // 使用本进程的驱动器和目录
		&si,
		&pi)) {
		fprintf(stderr, "没有这样的命令\n");
	}

	WaitForSingleObject(pi.hProcess, INFINITE);//父进程等待子进程运行完毕 

	CloseHandle(pi.hProcess);//关闭句柄 
	CloseHandle(pi.hThread);//关闭句柄 
}

int main() {
	Minshell cmd;
	cmd.run();
	return 0;
}


