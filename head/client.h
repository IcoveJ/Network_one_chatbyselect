// #pragma once
// #ifndef SOCKET_INITIAL_H_INCLUDED
// #define SOCKET_INITIAL_H_INCLUDED
// #define _WIN32_WINNT _WIN32_WINNT_WIN10
// #include <stdio.h>
// #include <string>
// #include <iostream>
// #include <thread>
// #include <windows.h>
// #include <atomic>
// #include <thread>

// using std::string;
// using std::cout;
// using std::endl;


// class chat_client{
// private:
//     SOCKET chatsocket = 0;
    
//     std::atomic<bool> flag{false};
    
//     void chat_client_recv(){
//         int r = 0;
//         fd_set readSocket;
//         FD_ZERO(&readSocket);
//         FD_SET(chatsocket, &readSocket);
//         timeval st;
//         st.tv_sec = 2;
//         st.tv_usec = 0;
//         while(1){
//             char recvBuff[1501];
//             if(flag){
//                 break;
//             }
//             auto tmpSocket = readSocket;
//             int num = select(0, &tmpSocket, NULL, &tmpSocket, &st);
//             if(num > 0){
//                 for(u_int i = 0; i < tmpSocket.fd_count; ++i){
//                     r = recv(tmpSocket.fd_array[i], recvBuff, 1500, 0);
//                     if(r > 0){
//                         recvBuff[r] = 0;
//                         if(strcmp(recvBuff, "quit") == 0){
//                             flag = true;
//                             cout << "?????????????????????\n";
//                             break;
//                         }
//                         cout << endl;
//                         cout << recvBuff << endl;
//                     }
//                     else if(r < 0){
//                     cout << "err:" << WSAGetLastError();
//                     }
//                 }
//             }
//             else if(num < 0){
//                 cout << "select????????" << WSAGetLastError() << endl;
//             }
//         }
//         for(u_int i = 0; i < readSocket.fd_count; ++i){
//             closesocket(readSocket.fd_array[i]);
//             readSocket.fd_array[i] = INVALID_SOCKET;
//         }
//         FD_ZERO(&readSocket);
//         cout << "????????\n";
//     }
    
//     bool login(){
//         cout << "????????????????" << endl;
//         string usename, password;
//         getchar();
//         std::getline(std::cin, usename);
//         std::getline(std::cin, password);
//         int num = send(chatsocket, usename.c_str(), static_cast<int>(usename.size()), 0);
// 	    num = send(chatsocket, password.c_str(), static_cast<int>(password.size()), 0);
// 	    if(num < 0){
//             return 0;
//         }
//         string flag;
//         while(flag.empty()){
//             char buff[1024] = {0};
//             int r = recv(chatsocket, buff, 1023, NULL);
//             if(r > 0){
//                 buff[r] = 0;
//                 flag = buff;
//             }else{
//                 cout << WSAGetLastError() << endl;
//                 return false;
//             }
//         }
//         if(flag == "T"){
//                 cout << "??????\n";
//                 return true;
//             }else{
//                 cout << flag;
//                 cout << "??????\n";
//             }
//             return false;
//     }
// public:
//     chat_client() = default;
//     string ip;

//     int init(){
//         //1 ????Э??汾
// 	    WSADATA wsaData;
//         WSAStartup(MAKEWORD(2, 2), &wsaData);
// 	    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
// 		    cout << "????Э??汾???!\n";
// 		    return -1;
// 	    }else{
// 	        cout << "????Э??汾???!\n";
//         }


//         //2 ????socket
//         chatsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//         if(chatsocket == SOCKET_ERROR){
//             cout << "????socket???!\n";
//             WSACleanup();
//             return -2;
//         }else{
//             cout << "????socket???!\n";
//         }

//         //3 ?????????Э??????
//         sockaddr_in  addr;
//         addr.sin_family = AF_INET;
//         addr.sin_port = htons(8000);
// 	    addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
//         printf("IP??%s\n", ip.c_str());


//         //4 ?????????
//         int c = connect(chatsocket, (sockaddr*)&addr, sizeof(addr));
//         if (c == SOCKET_ERROR) {
// 	        cout << "????????????!\n";
//             closesocket(chatsocket);
// 		    return -1;
// 	    }else{
// 	        cout << "????????????!\n";
//         }

// 	    if (!login()) {
// 		    return 0;
// 	    }

//         std::thread t(&chat_client::chat_client_recv, this);
//         while(1){
//             cout << flag << endl;
//             if(flag){
//                 break;
//             }
//             string buff;
//             cout << "????????\n";
//             std::getline(std::cin, buff);
//             if(buff == "quit"){
//                 flag = true;
//                 break;
//             }
//             send(chatsocket, buff.c_str(), static_cast<int>(buff.size()), 0);
//         }

//         if(t.joinable()){
//             t.join();
//         }   

//         cout << "????????\n";
//         return 0;
//     }

//     ~chat_client(){
//         if(chatsocket != INVALID_SOCKET){
//             closesocket(chatsocket);
// 		    chatsocket = INVALID_SOCKET; 
//         }
//         WSACleanup();
//     }
// };
// #endif

#include <stdio.h>
#include<string>
#include<iostream>
#include<thread>
#include <windows.h>
#include<atomic>
#include<future>
//#pragma comment(lib, "ws2_32.lib")
using std::string;
using std::cout;
using std::endl;
class smallqq_client {
	SOCKET smallqq_clientSocket = 0;
	//HWND hWnd;
	std::atomic_bool flag{false};
void smallqq_clientrecv() {
	int r = 0;
	fd_set readSocket;
	FD_ZERO(&readSocket);
	FD_SET(smallqq_clientSocket, &readSocket);
	timeval st;
	st.tv_sec = 2;
	st.tv_usec = 0;
	while (1) {
		char recvBuff[1501];
		if (flag) {
			break;
		}
		auto tmpSocket = readSocket;
		//FD_SET(smallqq_clientSocket, &tmpSocket);
		int num = select(0, &tmpSocket, NULL, &tmpSocket, &st);//解决recv傻等的问题
		if (num > 0) {
			for (u_int i = 0; i < tmpSocket.fd_count; ++i) {
				r = recv(tmpSocket.fd_array[i], recvBuff, 1500, 0);
				if (r > 0) {
					recvBuff[r] = 0;
					if (strcmp(recvBuff, "quit") == 0) {//等于0说明这两个字符串相等
						flag = true;
							cout << "该账号重复登录，被迫下线" << endl;
							break;
						}
					cout << endl;
					// << "接受到一条消息:";
					cout << recvBuff << endl;
				}
				else if (r < 0) {
					cout << "err:" << WSAGetLastError();
				}
			}
				//cout << readSocket.fd_count << endl;
		}
		else if (num < 0) {
				cout << "select函数错误" << WSAGetLastError() << endl;
		}
	}
	for (u_int i = 0; i < readSocket.fd_count; ++i) {

		closesocket(readSocket.fd_array[i]);
		readSocket.fd_array[i] = INVALID_SOCKET;
	}
	FD_ZERO(&readSocket);//清空信息
	cout << "该线程退出" << endl;
}

bool login() {
	cout << "请输入登录账号和密码" << endl;
	string usename, password;
	std::getline(std::cin, usename);
	std::getline(std::cin, password);
	int num = send(smallqq_clientSocket, usename.c_str(), static_cast<int>(usename.size()), 0);//参数是int类型，做一个强制转换
	num = send(smallqq_clientSocket, password.c_str(), static_cast<int>(password.size()), 0);
	if (num < 0)return 0;
	cout << num << endl;
	string flag;
	while (flag.empty()) {
		char judge_buff[1024] = { 0 };
		int r = recv(smallqq_clientSocket, judge_buff, 1023, NULL);
		if (r > 0) {
			judge_buff[r] = 0;
			cout << judge_buff << endl;
			flag = judge_buff;
		}
		else {
			cout << WSAGetLastError() << endl;
			return false;
		}
	}
	if (flag == "T") {
		cout << "登录成功" << endl;
		return true;
	}
	else {
		cout << "登录失败" << endl;
	}
	return false;
}
bool outTime() {
	fd_set fd = { 0 };
	FD_ZERO(&fd);
	FD_SET(smallqq_clientSocket, &fd);
	timeval st;
	st.tv_sec = 0;
	st.tv_usec = 6000;
	if (select(0, &fd,&fd,&fd,&st) == 0) {
		cout << "超时" << endl;
		return false;
	}
	return true;
}

public:
	smallqq_client() = default;
	int initial() {
		//1 请求协议版本
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (LOBYTE(wsaData.wVersion) != 2 ||
			HIBYTE(wsaData.wVersion) != 2) {
			printf("请求协议版本失败!\n");
			return -1;
		}
		printf("请求协议成功!\n");

		//2 创建socket
		smallqq_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (SOCKET_ERROR == smallqq_clientSocket) {
			printf("创建socket失败!\n");
			WSACleanup();
			return -2;
		}
		printf("创建socket成功!\n");

		//3 获取服务器协议地址族
		SOCKADDR_IN addr = { 0 };
		addr.sin_family = AF_INET;//协议版本
		addr.sin_addr.S_un.S_addr = inet_addr("192.168.117.1");//绑定的是服务器的ip
		addr.sin_port = htons(8000);//0 - 65535     10000左右
		//os内核 和其他程序  会占用掉一些端口   80  23  

		//4 连接服务器
		int c = connect(smallqq_clientSocket, (sockaddr*)&addr, sizeof addr);
		if (c == -1) {
			printf("连接服务器失败!\n");
			return -1;
		}
		printf("连接服务器成功!\n");
		std::future<bool>f = std::async(&smallqq_client::outTime,this);
		if (!f.get()|| !login()) {
			return 0;
		}
		std::thread t(&smallqq_client::smallqq_clientrecv, this);
		while (1) {
			if (flag) {//收到服务器断开的消息
				break;
			}
			string buff;
			cout << "想说点什么:";
			std::getline(std::cin, buff);
			if (buff == "quit") {
				flag = true;//准备让接收的线程也退出来。
				
				break;
			}
			send(smallqq_clientSocket, buff.c_str(), static_cast<int>(buff.size()), 0);
		}
		if (t.joinable()) {
			t.join();//等待线程退出
		}
		
		
		cout << "已断开连接" << endl;
		return 0;
	}
	
	~smallqq_client() {
		if (smallqq_clientSocket != INVALID_SOCKET)
		{
			//cout << "套接字被析构了" << endl;
			closesocket(smallqq_clientSocket);
			smallqq_clientSocket = INVALID_SOCKET;
		}
		WSACleanup();//清除协议
	}
};


//自动获取IP地址
/*
bool GetLocalIP(char* ip)
{
	//1.初始化wsa
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return false;
	}
	//2.获取主机名
	char hostname[256];
	ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR)
	{
		return false;
	}
	//3.获取主机ip
	HOSTENT* host = gethostbyname(hostname);
	if (host == NULL)
	{
		return false;
	}
	//4.转化为char*并拷贝返回
	strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
	return true;
}
*/
