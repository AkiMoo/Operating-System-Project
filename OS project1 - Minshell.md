<center><font size =7>OS project1 - Minshell</font></center>

要求：

•Design and implement a simple command interpreter **min_shell.exe** (a command shell) using the Windows application programming interface (**Windows API**).

•The command interpreter should be able to read input line by line either from standard input or from a file (given on the command line). Each line of input should be interpreted as a program, which is executed within a separate process.

使用范例：

•Usage of **min_shell.exe**:

•$type script.txt

​     C:\Winddows\system32\notepad.exe script.txt

​     C:\Winddows\system32\calc.exe

​     C:\Winddows\system32\charmap.exe

•$min_shell.exe script.txt

•Use the Windows functions **CreateProcess****()** and **WaitForSingleObject****()** to sequentially execute commands in separate processes.

<font size = 6>**函数设计：**</font>

```c++
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
};
```

**建立Minshell类，定义各种函数**



```C++
void Minshell::instruction(){
	cout<<"<<quit:退出Minshell>>"<<endl;
	cout<<"<<Minshell abc.txt:打开当前目录下的abc.txt并运行里面的指令>>"<<endl;
	cout<<"<<showRoute 显示当前目录路径>>"<<endl;
	cout<<"<<Minshell def.exe:执行当前目录下的def文件>>"<<endl;
	cout<<endl<<endl;//空两行为了显示更清晰 
}
```

**这个部分主要是用来显示可以用的指令，个人设置了Minshell可以打开.exe和.txt文件**

**外加个quit&showRoute功能：**

**showRoute用来显示目录，判断是否是自己想要的位置；**

**quit函数，我这里实现的退出是返回到可以操作的cmd命令行。**



```c++
void Minshell::run(){
	instruction();
	while(1){
		cout<<"当前目录路径是"<<getcwd(NULL,0)<<endl;
		cout<<"请输入指令"<<endl;
		get_command(command);
	}
}
```

**这部分用于导入，不再过多阐述**



```c++
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
```

**get_command就如字面意思，获取命令。在处理的时候把command里面的xxx.txt/exe提取出来，然后调用create_process/exe_file函数**



```c++
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
```

**利用ifstream来读取文件内部数据，读取的时候使用getline逐行读取**



```
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
```

**这里就是调用API，使用CreateProcess来模仿cmd实现我们.exe文件的运行。**

**运行过程中两个以上exe同时开的时候会出现停等现象，要关掉一个才能继续往下进行新的.exe运行**



**<font size =6>运行结果：</font>**

因为想显示出同时打开程序的效果，我进行了录像演示，详情看附件。
