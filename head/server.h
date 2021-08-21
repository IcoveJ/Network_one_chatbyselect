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
private://����������Ҫ��
	fd_set allsocket = { 0 };
	SOCKADDR_IN cAddr = { 0 };
	set<SOCKET> clientSet;//��ΪҪƵ���Ĳ����ɾ�������һ���Ч�ʸ�
	deque<std::pair<SOCKET, const char*>> msgdeque;
	SOCKET serverSocket;

//���������ݽṹ
private:
	//��������string��ŵĶ���loginNumber
	map<string,SOCKET>repeat_login;//Ŀ����ʵ�����߹���
	map<SOCKET, string>repeat_login_brthor;//�����ศ���

private://���ݿ�����Ҫ�ĳ�Ա����
	MYSQL conn;
	MYSQL_RES* res_set;
	MYSQL_ROW row;
	bool mysql_flag = false;
	deque<tuple<SOCKET, string, string>>mq_deque;//����׽��֡��˺ź�����

private:
	std::mutex repeat_socket;//��ֹ�ظ���¼��ʱ�򣬺͹㲥ʱ��socket�������ʧ


public:
	server_initial();
	int socket_initial();//�׽��ֳ�ʼ��
	void broadcast();//�㲥
	bool mysql_initial(const string& usename, const string& password);//���ݿ�ĳ�ʼ��
	void login(SOCKET client);//ÿ���ͻ�����Ҫ��һ���߳�
	string select_usename(string& loginNumber);
	virtual ~server_initial();
};

//���������̣߳������㲥��Ϣ
void server_initial::broadcast() {
	while (1) {
		while (!msgdeque.empty()) {
			auto msg_it = msgdeque.front();
			msgdeque.pop_front();
			SOCKET resource = msg_it.first;//�����ϢԴ���Ǹ��ͻ���
			const char* msg = msg_it.second;
			std::lock_guard<std::mutex> protectSocket(repeat_socket);//���е���Ϣ����Ҫ����
			//����resource�п��ܱ�ȥ��������clientSet�ǲ����ܸ�resource����Ϣ�������������û����
			auto it_socket = clientSet.begin();//װ����socket
			string usename = select_usename(repeat_login_brthor[resource]);//�ҵ�����ͻ��˵�loginNumber,�ٵ���mysql�Ĳ�ѯ���
			string newmsg = usename +":" + msg;
			
			while (it_socket != clientSet.end()) {
				if (*it_socket != resource) {
					int num = send(*it_socket, newmsg.c_str(), static_cast<int>(newmsg.size()), 0);
					if (num < 0) {
						cout << "�㲥���ݷ���ʧ��,err:" << WSAGetLastError() << endl;
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
		loginNumber + "'";//��ѯ��������
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
	loginNumber + "' and loginpassword = '" + password + "'";//��ѯ��������
	int status = mysql_query(&conn, query.c_str());
	res_set = mysql_store_result(&conn);
	uint64_t count = mysql_num_rows(res_set);//unsigned long long
	if (count > 0) {//��һ����¼�ͷ��ص�¼�ɹ�
		return true;
	}
	return false;
}


void server_initial::login(SOCKET clientsocket) {//���ոÿͻ��˵ĵ�¼�˺ź�����
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
	//�������ǵ�¼�ɹ������
	if (mysql_initial(loginNumber, password)) {

		//�����ظ���¼�����
		if (repeat_login.find(loginNumber) != repeat_login.end()) {
			//��ֹ����ɾ����socket������
			std::lock_guard<std::mutex> protectSocket(repeat_socket);
			
			send(repeat_login[loginNumber], "quit",static_cast<int>(strlen("quit")), 0);
			
			
			//���һ��������Ϣ
			FD_CLR(repeat_login[loginNumber],&allsocket);//�ѵ�ǰ���׽���Ҳȥ��
			clientSet.erase(repeat_login[loginNumber]);
			closesocket(repeat_login[loginNumber]);
			repeat_login[loginNumber] = clientsocket;//�������׽��ֱ��һ��
			repeat_login_brthor.erase(clientsocket);

		}


		repeat_login_brthor.emplace(clientsocket, loginNumber);
		send(clientsocket, "T", 1, 0);
		FD_SET(clientsocket, &allsocket);
		repeat_login.emplace(loginNumber, clientsocket);
		clientSet.emplace(clientsocket);//��һ�����Ӿͼ��뵽�׽�������
		printf("�пͻ������ӵ��������ˣ�%s!\n", inet_ntoa(cAddr.sin_addr));
		cout << "��������Ϊ" << allsocket.fd_count - 1 << endl;//��һ���Ƿ��������׽���
		
	}
	//�����¼ʧ�ܵ����
	else {
		//cout << "��Ч��¼" << endl;
		send(clientsocket, "F", static_cast<int>(strlen("F")), 0);
	}
}


int server_initial::socket_initial() {
	if (!mysql_flag) {
		cout << "���ݿ�����ʧ�ܣ��������䱸ʧ��" << endl;
		return 0 ;
	}
	int len = sizeof(SOCKADDR_IN);
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
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket) {
		printf("����socketʧ��!\n");
		WSACleanup();
		return -2;
	}
	printf("����socket�ɹ�!\n");

	//3 ����Э���ַ��
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;//Э��汾
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.3.66");//���Լ���ip

	addr.sin_port = htons(9996);//0 - 65535     10000����
	//os�ں� ����������  ��ռ�õ�һЩ�˿�   80  23  


	//4 ��
	if (bind(serverSocket, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR) {
		printf("bindʧ��!\n");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("bind�ɹ�!\n");

	//5 ����
	int r = listen(serverSocket, 10);
	if (r == -1) {
		printf("listenʧ��!\n");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("listen�ɹ�!\n");

	FD_ZERO(&allsocket);
	FD_SET(serverSocket, &allsocket);
	timeval st;
	st.tv_sec = 0;
	st.tv_usec = 3000;

	//����߳�ר�������㲥��Ϣ
	thread thread_broadcast(&server_initial::broadcast, this);
	thread_broadcast.detach();

	const string usename_flag = "smallqquse";
	const string password_flag = "smallqqpass";
	//6 �ȴ��ͻ�������    
	//�ͻ���Э���ַ��
    cout << "�ȴ��ͻ������ӣ�" << 1 << endl;
	while (1) {
		fd_set tmpSocket = allsocket;
		int num = select(0, &tmpSocket, NULL, &tmpSocket, NULL);//���recv��acceptɵ�ȵ�����
		cout << num << endl;
		cout << 2 << endl;
		SOCKET tmp_client_socket = 0;//���ڳ�ʱ��
		if (num > 0) {
			for (u_int i = 0; i < tmpSocket.fd_count; ++i) {
				if (tmpSocket.fd_array[i] == serverSocket) {//�����ʱ����տͻ��˵�����
					SOCKET clientsocket = accept(serverSocket, (sockaddr*)&cAddr, &len);
					if (INVALID_SOCKET == clientsocket) {
						cout << "��ʧ��" << endl;
						continue;
					}
					thread t(&server_initial::login,this,clientsocket);
					t.detach();
				}
				else {//���ڱ���׽��ֵ�ʱ��
					char buff[1500] = { 0 };//����MTU
					cout << buff << endl;
					int r = recv(tmpSocket.fd_array[i], buff, 1499, 0);
					if (r == 0) {//�ͻ���������
						auto close_client_socket = tmpSocket.fd_array[i];
						clientSet.erase(close_client_socket);
						
						FD_CLR(tmpSocket.fd_array[i], &allsocket);
						closesocket(close_client_socket);
						string close_loginNumber = repeat_login_brthor[close_client_socket];
						repeat_login.erase(close_loginNumber);
						repeat_login_brthor.erase(close_client_socket);
						cout << "�ر�����" << endl;
						continue;
					}

					else if (r > 0) {//���ܵ�����Ϣ���Խ��й㲥
						buff[r] = 0;
						msgdeque.emplace_back(std::make_pair(tmpSocket.fd_array[i], buff));
						cout << repeat_login_brthor[tmpSocket.fd_array[i]] << ":" << buff << endl;//�ڷ������˷��������ֺ�����
					}
					else {//�й�����
						int err = WSAGetLastError();
						if (err == 10054) {//�����ǿ�ƹرգ�ҲҪ����һ��
							auto close_client_socket = tmpSocket.fd_array[i];
							clientSet.erase(close_client_socket);
							FD_CLR(tmpSocket.fd_array[i], &allsocket);//��Ϊ�Ѿ�������±�ɾ�ˣ�������ͷ���ô�ͻ��ͷŴ���
							closesocket(close_client_socket);
							cout << "�����˳�" << ' ' << err << endl;
						}
						//cout << "�������:" << err << endl;
						continue;
					}
				}

			}
		}

		else if(num < 0){ //����0˵����ʱ��
			cout << "select����������" <<WSAGetLastError()<< endl;
			//recv(tmp_client_socket,const_cast<char*>( "T"), 2, 0);
		}

	}
	
}

//���캯��
server_initial::server_initial() {
	
	mysql_init(&conn);//���ݿ��ʼ��
	if (!mysql_real_connect(&conn, "localhost", "root", "icovej990823", "stu_chat", 3306, NULL, 0)) {
		fprintf(stderr, "Failed to connect to database: Error: %s\n",mysql_error(&conn));
		mysql_flag = false;
	}
	else {
		fprintf(stderr, "���ݿⴴ���ɹ�!\n");
		mysql_set_character_set(&conn, "gbk");
		mysql_flag = true;
	}
	socket_initial();
}

//��������
server_initial::~server_initial() {
	for (u_int i = 0; i < allsocket.fd_count; ++i) {
		closesocket(allsocket.fd_array[i]);//��ÿ��select����ģ������׽���Ҳɾ��
	}
	FD_ZERO(&allsocket);
	//8 �ر�socket
	closesocket(serverSocket);
	//9 ���Э����Ϣ
	WSACleanup();
	mysql_free_result(res_set);  //�ͷ�һ���������ʹ�õ��ڴ档
	mysql_close(&conn);//�ر����ݿ�
}
#endif 



