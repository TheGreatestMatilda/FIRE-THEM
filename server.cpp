#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <WS2tcpip.h> 
#include<Windows.h>
#include<graphics.h>
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<string>
#include<mmsystem.h>
#include <cstdlib>
#include <ctime>
#include<stdlib.h>
#include<list>
#pragma comment(lib,"winmm.lib")
#include<easyx.h>
#include<set>

using namespace std;
#define PORT 8888;
set<SOCKET>clients;
bool init_socket() {
    WSADATA wsadata;
    if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
        cout << "wsadata error" << endl;
        return false;
    }
    return true;
}
bool close_socket() {
    if (0 != WSACleanup) {
        cout << "wsacleanup error" << endl;
        return false;
    }
    return true;
}
SOCKET creat_serversocket() {
    SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == fd) {
        cout << "socket error" << endl;
        return INVALID_SOCKET;
    }
    string ip = "127.0.0.1";
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    if (SOCKET_ERROR == bind(fd, (struct sockaddr*)&addr, sizeof(addr))) {
        cout << "bind error" << endl;
        return INVALID_SOCKET;
    }
    listen(fd, 2);
    return fd;
}
SOCKET creat_clientsocket(const char* ip) {
    SOCKET fd = (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == fd) {
        cout << "socket error" << endl;
        return INVALID_SOCKET;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, ip, &addr.sin_addr);
    if (SOCKET_ERROR == connect(fd, (struct sockaddr*)&addr, sizeof(addr))) {
        cout << "connect error" << endl;
        return INVALID_SOCKET;
    }
    return fd;
}
int main() {
    init_socket();
    SOCKET fd = creat_serversocket();
    cout << "服务器已开启" << endl;

    SOCKET clientFD1 = accept(fd, NULL, NULL);
    cout << "有新的客户端连接" << (long long)clientFD1 << endl;
    SOCKET clientFD2 = accept(fd, NULL, NULL);
    cout << "有新的客户端连接" << (long long)clientFD1 << endl;
    char buf[BUFSIZ] = { 0 };
    int kill1;
    kill1=recv(clientFD1, buf, BUFSIZ, 0);
    int kill2;
    kill2 = recv(clientFD2, buf, BUFSIZ, 0);
    int kill = kill1 + kill2;
    send(clientFD1, (const char*)kill, BUFSIZ, 0);
    send(clientFD2, (const char*)kill, BUFSIZ, 0);
        //clients.insert(clientFD);
        //char buf[BUFSIZ] = { 0 };
        //int kill=recv(clientFD, buf, BUFSIZ, 0);
        //cout << kill << endl;

    
}