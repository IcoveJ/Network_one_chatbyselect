#pragma once
#ifndef SOCKET_INITIAL_H_INCLUDED
#include"head.h"
#define SOCKET_INITIAL_H_INCLUDED
using std::vector;
using std::unordered_set;
using std::cout;
using std::endl;
using std::list;
using std::set;
using std::map;
using std::thread;
using std::deque;
using std::string;
using std::tuple;
class server_initial
{
private://服务器所需要的
	fd_set allsocket = { 0 };
	SOCKADDR_IN cAddr = { 0 };
	set<SOCKET> clientSet;//因为要频繁的插入和删除，并且还想效率高
	deque<std::pair<SOCKET, const char*>> msgdeque;
	SOCKET serverSocket;

//功能性数据结构
private:
	//这两个的string存放的都是loginNumber
	map<string,SOCKET>repeat_login;//目的是实现下线功能
	map<SOCKET, string>repeat_login_brthor;//他俩相辅相成

private://数据库所需要的成员变量
	MYSQL conn;
	MYSQL_RES* res_set;
	MYSQL_ROW row;
	bool mysql_flag = false;
	deque<tuple<SOCKET, string, string>>mq_deque;//存放套接字、账号和密码

private:
	std::mutex repeat_socket;//防止重复登录的时候，和广播时候socket造成了损失


public:
	server_initial();
	int socket_initial();//套接字初始化
	void broadcast();//广播
	bool mysql_initial(const string& usename, const string& password);//数据库的初始化
	void login(SOCKET client);//每个客户端另要开一个线程
	string select_usename(string& loginNumber);
	virtual ~server_initial();
};

//单独开根线程，用来广播信息
void server_initial::broadcast() {
	while (1) {
		while (!msgdeque.empty()) {
			auto msg_it = msgdeque.front();
			msgdeque.pop_front();
			SOCKET resource = msg_it.first;//这个消息源自那个客户端
			const char* msg = msg_it.second;
			std::lock_guard<std::mutex> protectSocket(repeat_socket);//队列的消息不需要阻塞
			//尽管resource有可能被去掉，但是clientSet是不可能给resource发消息，所以这里的锁没出错
			auto it_socket = clientSet.begin();//装的是socket
			string usename = select_usename(repeat_login_brthor[resource]);//找到这个客户端的loginNumber,再调用mysql的查询语句
			string newmsg = usename +":" + msg;
			
			while (it_socket != clientSet.end()) {
				if (*it_socket != resource) {
					int num = send(*it_socket, newmsg.c_str(), static_cast<int>(newmsg.size()), 0);
					if (num < 0) {
						cout << "广播数据发送失败,err:" << WSAGetLastError() << endl;
					}
				}
				++it_socket;
			}
		}
		Sleep(500);
	}
}


string server_initial::select_usename(string& loginNumber) {
	string query = "SELECT usename FROM stu where loginNumber = '" +
		loginNumber + "'";//查询语句的连接
	int status = mysql_query(&conn, query.c_str());
	res_set = mysql_store_result(&conn);
	row = mysql_fetch_row(res_set);// 
	if (row == nullptr)return "";
	string usename = "";

	if (row[0] != nullptr) {
		usename = row[0];
	}
	return usename;
}


bool server_initial::mysql_initial(const string& loginNumber,const string& password) {
	
	string query = "SELECT * FROM stu where loginNumber = '" + 
	loginNumber + "' and loginpassword = '" + password + "'";//查询语句的连接
	int status = mysql_query(&conn, query.c_str());
	res_set = mysql_store_result(&conn);
	uint64_t count = mysql_num_rows(res_set);//unsigned long long
	if (count > 0) {//有一条记录就返回登录成功
		return true;
	}
	return false;
}


void server_initial::login(SOCKET clientsocket) {//接收该客户端的登录账号和密码
	char buff[1501] = { 0 };
	string loginNumber, password;
	while (loginNumber.empty()|| password.empty()) {
		int r = recv(clientsocket, buff, 1499, 0);
		if (r > 0) {
			buff[r] = 0;
			cout << buff << endl;
			if (loginNumber.empty()) {
				loginNumber = buff;
			}
			else {
				password = buff;
			}
		}
	}
	cout << 2 << endl;
	//这里面是登录成功的情况
	if (mysql_initial(loginNumber, password)) {

		//处理重复登录的情况
		if (repeat_login.find(loginNumber) != repeat_login.end()) {
			//防止给被删除的socket发数据
			std::lock_guard<std::mutex> protectSocket(repeat_socket);
			
			send(repeat_login[loginNumber], "quit",static_cast<int>(strlen("quit")), 0);
			
			
			//清除一下他的信息
			FD_CLR(repeat_login[loginNumber],&allsocket);//把当前的套接字也去掉
			clientSet.erase(repeat_login[loginNumber]);
			closesocket(repeat_login[loginNumber]);
			repeat_login[loginNumber] = clientsocket;//把他的套接字变更一下
			repeat_login_brthor.erase(clientsocket);

		}


		repeat_login_brthor.emplace(clientsocket, loginNumber);
		send(clientsocket, "T", 1, 0);
		FD_SET(clientsocket, &allsocket);
		repeat_login.emplace(loginNumber, clientsocket);
		clientSet.emplace(clientsocket);//有一个连接就加入到套接字中来
		printf("有客户端连接到服务器了：%s!\n", inet_ntoa(cAddr.sin_addr));
		cout << "在线人数为" << allsocket.fd_count - 1 << endl;//有一个是服务器的套接字
		
	}
	//处理登录失败的情况
	else {
		//cout << "无效登录" << endl;
		send(clientsocket, "F", static_cast<int>(strlen("F")), 0);
	}
}


int server_initial::socket_initial() {
	if (!mysql_flag) {
		cout << "数据库启动失败，服务器配备失败" << endl;
		return 0 ;
	}
	int len = sizeof(SOCKADDR_IN);
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
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket) {
		printf("创建socket失败!\n");
		WSACleanup();
		return -2;
	}
	printf("创建socket成功!\n");

	//3 创建协议地址族
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;//协议版本
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.3.66");//用自己的ip

	addr.sin_port = htons(9996);//0 - 65535     10000左右
	//os内核 和其他程序  会占用掉一些端口   80  23  


	//4 绑定
	if (bind(serverSocket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
		printf("bind失败!\n");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("bind成功!\n");

	//5 监听
	int r = listen(serverSocket, 10);
	if (r == -1) {
		printf("listen失败!\n");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("listen成功!\n");

	FD_ZERO(&allsocket);
	FD_SET(serverSocket, &allsocket);
	timeval st;
	st.tv_sec = 0;
	st.tv_usec = 3000;

	//这个线程专门用来广播消息
	thread thread_broadcast(&server_initial::broadcast, this);
	thread_broadcast.detach();

	const string usename_flag = "smallqquse";
	const string password_flag = "smallqqpass";
	//6 等待客户端连接    
	//客户端协议地址族
    cout << "等待客户端连接：" << 1 << endl;
	while (1) {
		fd_set tmpSocket = allsocket;
		int num = select(0, &tmpSocket, NULL, &tmpSocket, NULL);//解决recv和accept傻等的问题
		cout << num << endl;
		cout << 2 << endl;
		SOCKET tmp_client_socket = 0;//用于超时的
		if (num > 0) {
			for (u_int i = 0; i < tmpSocket.fd_count; ++i) {
				if (tmpSocket.fd_array[i] == serverSocket) {//这个的时候接收客户端的连接
					SOCKET clientsocket = accept(serverSocket, (sockaddr*)&cAddr, &len);
					if (INVALID_SOCKET == clientsocket) {
						cout << "绑定失败" << endl;
						continue;
					}
					thread t(&server_initial::login,this,clientsocket);
					t.detach();
				}
				else {//等于别的套接字的时候
					char buff[1500] = { 0 };//最大的MTU
					cout << buff << endl;
					int r = recv(tmpSocket.fd_array[i], buff, 1499, 0);
					if (r == 0) {//客户端下线了
						auto close_client_socket = tmpSocket.fd_array[i];
						clientSet.erase(close_client_socket);
						
						FD_CLR(tmpSocket.fd_array[i], &allsocket);
						closesocket(close_client_socket);
						string close_loginNumber = repeat_login_brthor[close_client_socket];
						repeat_login.erase(close_loginNumber);
						repeat_login_brthor.erase(close_client_socket);
						cout << "关闭连接" << endl;
						continue;
					}

					else if (r > 0) {//接受到了消息可以进行广播
						buff[r] = 0;
						msgdeque.emplace_back(std::make_pair(tmpSocket.fd_array[i], buff));
						cout << repeat_login_brthor[tmpSocket.fd_array[i]] << ":" << buff << endl;//在服务器端发送者名字和内容
					}
					else {//有故障了
						int err = WSAGetLastError();
						if (err == 10054) {//这个是强制关闭，也要清理一下
							auto close_client_socket = tmpSocket.fd_array[i];
							clientSet.erase(close_client_socket);
							FD_CLR(tmpSocket.fd_array[i], &allsocket);//因为已经把这个下标删了，如果在释放那么就会释放错了
							closesocket(close_client_socket);
							cout << "错误退出" << ' ' << err << endl;
						}
						//cout << "错误代号:" << err << endl;
						continue;
					}
				}

			}
		}

		else if(num < 0){ //等于0说明超时了
			cout << "select函数出错了" <<WSAGetLastError()<< endl;
			//recv(tmp_client_socket,const_cast<char*>( "T"), 2, 0);
		}

	}
	
}

//构造函数
server_initial::server_initial() {
	
	mysql_init(&conn);//数据库初始化
	if (!mysql_real_connect(&conn, "localhost", "root", "icovej990823", "stu_chat", 3306, NULL, 0)) {
		fprintf(stderr, "Failed to connect to database: Error: %s\n",mysql_error(&conn));
		mysql_flag = false;
	}
	else {
		fprintf(stderr, "数据库创建成功!\n");
		mysql_set_character_set(&conn, "gbk");
		mysql_flag = true;
	}
	socket_initial();
}

//析构函数
server_initial::~server_initial() {
	for (u_int i = 0; i < allsocket.fd_count; ++i) {
		closesocket(allsocket.fd_array[i]);//将每个select函数模型里的套接字也删掉
	}
	FD_ZERO(&allsocket);
	//8 关闭socket
	closesocket(serverSocket);
	//9 清除协议信息
	WSACleanup();
	mysql_free_result(res_set);  //释放一个结果集合使用的内存。
	mysql_close(&conn);//关闭数据库
}
#endif 



