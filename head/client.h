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
//         //1 ????��??��
// 	    WSADATA wsaData;
//         WSAStartup(MAKEWORD(2, 2), &wsaData);
// 	    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
// 		    cout << "????��??��???!\n";
// 		    return -1;
// 	    }else{
// 	        cout << "????��??��???!\n";
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

//         //3 ?????????��??????
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
		int num = select(0, &tmpSocket, NULL, &tmpSocket, &st);//���recvɵ�ȵ�����
		if (num > 0) {
			for (u_int i = 0; i < tmpSocket.fd_count; ++i) {
				r = recv(tmpSocket.fd_array[i], recvBuff, 1500, 0);
				if (r > 0) {
					recvBuff[r] = 0;
					if (strcmp(recvBuff, "quit") == 0) {//����0˵���������ַ������
						flag = true;
							cout << "���˺��ظ���¼����������" << endl;
							break;
						}
					cout << endl;
					// << "���ܵ�һ����Ϣ:";
					cout << recvBuff << endl;
				}
				else if (r < 0) {
					cout << "err:" << WSAGetLastError();
				}
			}
				//cout << readSocket.fd_count << endl;
		}
		else if (num < 0) {
				cout << "select��������" << WSAGetLastError() << endl;
		}
	}
	for (u_int i = 0; i < readSocket.fd_count; ++i) {

		closesocket(readSocket.fd_array[i]);
		readSocket.fd_array[i] = INVALID_SOCKET;
	}
	FD_ZERO(&readSocket);//�����Ϣ
	cout << "���߳��˳�" << endl;
}

bool login() {
	cout << "�������¼�˺ź�����" << endl;
	string usename, password;
	std::getline(std::cin, usename);
	std::getline(std::cin, password);
	int num = send(smallqq_clientSocket, usename.c_str(), static_cast<int>(usename.size()), 0);//������int���ͣ���һ��ǿ��ת��
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
		cout << "��¼�ɹ�" << endl;
		return true;
	}
	else {
		cout << "��¼ʧ��" << endl;
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
		cout << "��ʱ" << endl;
		return false;
	}
	return true;
}

public:
	smallqq_client() = default;
	int initial() {
		//1 ����Э��汾
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (LOBYTE(wsaData.wVersion) != 2 ||
			HIBYTE(wsaData.wVersion) != 2) {
			printf("����Э��汾ʧ��!\n");
			return -1;
		}
		printf("����Э��ɹ�!\n");

		//2 ����socket
		smallqq_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (SOCKET_ERROR == smallqq_clientSocket) {
			printf("����socketʧ��!\n");
			WSACleanup();
			return -2;
		}
		printf("����socket�ɹ�!\n");

		//3 ��ȡ������Э���ַ��
		SOCKADDR_IN addr = { 0 };
		addr.sin_family = AF_INET;//Э��汾
		addr.sin_addr.S_un.S_addr = inet_addr("192.168.117.1");//�󶨵��Ƿ�������ip
		addr.sin_port = htons(8000);//0 - 65535     10000����
		//os�ں� ����������  ��ռ�õ�һЩ�˿�   80  23  

		//4 ���ӷ�����
		int c = connect(smallqq_clientSocket, (sockaddr*)&addr, sizeof addr);
		if (c == -1) {
			printf("���ӷ�����ʧ��!\n");
			return -1;
		}
		printf("���ӷ������ɹ�!\n");
		std::future<bool>f = std::async(&smallqq_client::outTime,this);
		if (!f.get()|| !login()) {
			return 0;
		}
		std::thread t(&smallqq_client::smallqq_clientrecv, this);
		while (1) {
			if (flag) {//�յ��������Ͽ�����Ϣ
				break;
			}
			string buff;
			cout << "��˵��ʲô:";
			std::getline(std::cin, buff);
			if (buff == "quit") {
				flag = true;//׼���ý��յ��߳�Ҳ�˳�����
				
				break;
			}
			send(smallqq_clientSocket, buff.c_str(), static_cast<int>(buff.size()), 0);
		}
		if (t.joinable()) {
			t.join();//�ȴ��߳��˳�
		}
		
		
		cout << "�ѶϿ�����" << endl;
		return 0;
	}
	
	~smallqq_client() {
		if (smallqq_clientSocket != INVALID_SOCKET)
		{
			//cout << "�׽��ֱ�������" << endl;
			closesocket(smallqq_clientSocket);
			smallqq_clientSocket = INVALID_SOCKET;
		}
		WSACleanup();//���Э��
	}
};


//�Զ���ȡIP��ַ
/*
bool GetLocalIP(char* ip)
{
	//1.��ʼ��wsa
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return false;
	}
	//2.��ȡ������
	char hostname[256];
	ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR)
	{
		return false;
	}
	//3.��ȡ����ip
	HOSTENT* host = gethostbyname(hostname);
	if (host == NULL)
	{
		return false;
	}
	//4.ת��Ϊchar*����������
	strcpy(ip, inet_ntoa(*(in_addr*)*host->h_addr_list));
	return true;
}
*/
