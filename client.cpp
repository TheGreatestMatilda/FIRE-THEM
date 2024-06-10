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
#include<vector>
using namespace std;
#define PORT 8888;
IMAGE img_gamer;
IMAGE img_bk;
IMAGE img_bullet;
IMAGE img_enemy[2];
int kill = 0;

vector<SOCKET>clients;
bool init_socket();
bool close_socket();
SOCKET creat_serversocket();
SOCKET creat_clientsocket(const char* ip);
SOCKET fd = creat_clientsocket("127.0.0.1");

int Timer(int duration, int id) {
    static int start[5];//记录开始时间
    int end = clock();//记录结束时间
    if (end - start[id] >= duration) {
        start[id] = end;
        return 1;
    }
    return 0;
}
void loadresoures() {
    loadimage(&img_bk, "resoures/bk.jpg");
    loadimage(&img_gamer, "resoures/W.jpg");
    loadimage(&img_bullet, "resoures/ancestors.jpg");
    loadimage(img_enemy + 0, "resoures/enemy2.png");
    loadimage(img_enemy + 1, "resoures/enemy1.jpg");
}

class button
{
public:
    button(int x, int y, int width, int height, COLORREF color, string text) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->color = color;
        this->pstr = text;
    }
    void drawbutton() {
        setfillcolor(this->color);//填充颜色
        settextstyle(15, 0, "宋体");//设置字体大小和形状
        setlinecolor(BLACK);//设置按钮边框为黑色
        settextcolor(BLACK);//设置字体颜色
        setbkmode(TRANSPARENT);//设置字体背景透明
        fillrectangle(this->x, this->y, this->x + this->width, this->y + this->height);//矩形大小
        outtextxy(this->x + 5, this->y + 8, this->pstr.c_str());//放置字体
    }
    bool mouseinButton(MOUSEMSG m) {
        if (this->x <= m.x && this->x + this->width >= m.x && this->y <= m.y && this->y + this->height >= m.y) {
            this->color = BLUE;
            return true;
        }
        this->color = WHITE;
        return false;
    }
    bool clickButton(MOUSEMSG m) {
        if (mouseinButton(m) && m.uMsg == WM_LBUTTONDOWN) {
            return true;
        }
        return false;
    }
    COLORREF& getcolor() {
        return color;
    }
protected:
    int x;
    int y;//矩形初始坐标
    int width;
    int height;
    COLORREF color;//填充颜色
    string pstr;//文本
};
struct plane {
    bool isDied;
    int x;
    int y;
    int frame;
}gamer;
struct enemyplane {
    bool isDied;
    int x;
    int y;
    int type;
}enemy[10];
struct bullet {
    int x;
    int y;
    bool isDied;
};
bullet b[30] = { 0 };
bullet a;
void gameover(int kill) {
    TCHAR* str = new TCHAR[128];
    _stprintf_s(str, 128, _T("击杀数：%d"), kill);
    LPCSTR end = _T("按enter退出游戏");
    outtextxy(150, 200, str);
    settextstyle(35, 0, "宋体");//设置字体大小和形状
    outtextxy(150, 300, end);
    while (1) {
        ExMessage m;
        getmessage(&m, EM_KEY);
        if (m.vkcode == 0x0D) {
            exit(0);
        }
    }
}
void creatbullet() {
    for (int i = 0; i < 30; i++) {
        if (b[i].isDied) {
            b[i].x = gamer.x + img_gamer.getwidth() / 2;
            b[i].y = gamer.y;
            b[i].isDied = false;
            break;
        }
    }
}
void bulletmove() {
    for (int i = 0; i < 30; i++) {
        if (!b[i].isDied) {
            b[i].y -= 4;
            if (b[i].y < 0) {
                b[i].isDied = true;
            }
        }
    }
}
void initplane(plane* pthis, int x, int y) {
    pthis->x = x;
    pthis->y = y;
    pthis->isDied = false;
    pthis->frame = 0;
}
void createnemy() {

    for (int i = 0; i < 10; i++) {
        if (enemy[i].isDied) {
            enemy[i].type = rand() % 2;
            enemy[i].x = rand() % getwidth();
            enemy[i].y = -img_enemy[enemy[i].type].getheight();
            enemy[i].isDied = false;
            break;
        }
    }
}
void enemymove() {
    for (int i = 0; i < 10; i++) {
        if (!enemy[i].isDied) {
            enemy[i].y += 3;
            if (enemy[i].y > getheight()) {
                enemy[i].isDied = true;
            }
        }
    }
}
void planemove(plane* pthis) {
    if (GetAsyncKeyState(VK_UP) && pthis->y > 0) {
        pthis->y -= 3;
    }
    if (GetAsyncKeyState(VK_DOWN) && pthis->y + img_gamer.getheight() / 2 < getheight()) {
        pthis->y += 3;
    }
    if (GetAsyncKeyState(VK_LEFT) && pthis->x + img_gamer.getwidth() / 2 > 0) {
        pthis->x -= 3;
    }
    if (GetAsyncKeyState(VK_RIGHT) && pthis->x + img_gamer.getwidth() / 2 < getwidth()) {
        pthis->x += 3;
    }
    if (GetAsyncKeyState(VK_SPACE) && Timer(200, 0)) {
        creatbullet();
    }
}
void init() {
    loadresoures();
    initplane(&gamer, (getwidth() - 100) / 2, getheight() - 120);
    for (int i = 0; i < 30; i++) {
        b[i].isDied = false;
    }
    for (int i = 0; i < 10; i++) {
        enemy[i].isDied = true;
    }
}
int frame = 0;
void drawplane(plane* pthis) {
    putimage(pthis->x, pthis->y, &img_gamer);
    pthis->frame = (pthis->frame + 1) % 2;
}
void draw() {
    putimage(0, 0, &img_bk);
    drawplane(&gamer);
    for (int i = 0; i < 30; i++) {
        if (!b[i].isDied)putimage(b[i].x, b[i].y, &img_bullet);
    }
    for (int i = 0; i < 10; i++) {
        if (!enemy[i].isDied) {
            //if (enemy[i].type == 1) {
            putimage(enemy[i].x, enemy[i].y, img_enemy + 0);
            //}
            //else {
            //    putimage(enemy[i].x, enemy[i].y, img_enemy + enemy[i].type);
            //}
        }

    }
}
bool clickbutton(int x, int y, RECT r) {
    return(r.left <= x && x <= r.right && r.top <= y && y <= r.bottom);
}
void welcome(MOUSEMSG m) {
    LPCSTR title = _T("飞机大战");
    button* begin = new button(150, 200, 100, 50, WHITE, "开始游戏");
    button* close = new button(150, 260, 100, 50, WHITE, "退出游戏");
    m = GetMouseMsg();
    while (1) {
        BeginBatchDraw();//添加双缓冲
        putimage(0, 0, &img_bk);
        setfillcolor(WHITE);//填充颜色
        settextstyle(35, 0, "宋体");//设置字体大小和形状
        outtextxy(150, 100, title);
        begin->drawbutton();
        close->drawbutton();
        if (begin->clickButton(m)) {
            break;
        }
        else if (close->clickButton(m)) {
            exit(0);
        }
        EndBatchDraw();
    }
    //
    //putimage(0, 0, &img_bk);
    //LPCSTR title = _T("飞机大战");
    //LPCSTR begin = _T("开始游戏");
    //LPCSTR end = _T("退出游戏");
    //RECT Begin, End;
    //BeginBatchDraw();//添加双缓冲
    //setfillcolor(WHITE);//填充颜色
    //settextstyle(25, 0, "宋体");//设置字体大小和形状
    //outtextxy(img_bk.getwidth() / 2 - textwidth(title) / 2, img_bk.getheight()/5, title);
    //settextstyle(15, 0, "宋体");//设置字体大小和形状
    //Begin.left = img_bk.getwidth() - textwidth(begin) / 2;
    //Begin.right = Begin.left - textwidth(begin);
    //Begin.top = img_bk.getheight() / 5*2.5;
    //Begin.bottom = Begin.top + textheight(begin);
    //End.left = img_bk.getwidth() - textwidth(end) / 2;
    //End.right = End.left - textwidth(end);
    //End.top = img_bk.getheight() / 5*3;
    //End.bottom = End.top + textheight(end);

    //outtextxy(205, 208, begin);
    //outtextxy(205, 268, end);
    //EndBatchDraw();
    //while (1) {
    //    ExMessage m;
    //    getmessage(&m,EM_MOUSE);
    //    if (m.lbutton) {
    //        if (clickbutton(m.x, m.y, Begin)){
    //            return;
    //        }
    //        else if (clickbutton(m.x, m.y, End)) {
    //            exit(0);
    //        }
    //    }
    //}
}
void hit() {
    for (int i = 0; i < 10; i++) {
        if (enemy[i].isDied) {

            continue;
        }
        for (int j = 0; j < 30; j++) {
            if (b[i].isDied) {
                continue;
            }
            /*enemy[i].type = rand() % 2;*/
            if (b[j].x > enemy[i].x && b[j].x < enemy[i].x + img_enemy[enemy[i].type].getwidth() && b[j].y > enemy[i].y && b[j].y < enemy[i].y + img_enemy[enemy[i].type].getheight())
            {
                b[j].isDied = true;
                enemy[i].isDied = true;
                kill++;
                 char buf[BUFSIZ] = { 0 };
                sprintf(buf, "%d", kill);
                send(fd, buf, strlen(buf),0);
                //if (enemy[i].isDied) {
                //    mciSendString("open C://Users/yezi2/Desktop/软件协会/Project4/Project4/boom.mp3", NULL, 0, NULL);
                //    mciSendString("play C://Users/yezi2/Desktop/软件协会/Project4/Project4/boom.mp3", NULL, 0, NULL);
                //}
            }
        }
    }
    for (int i = 0; i < 10; i++)
    {
        if (gamer.x + img_gamer.getwidth() >= enemy[i].x && gamer.x <= enemy[i].x + img_enemy[enemy[i].type].getwidth() && gamer.y <= enemy[i].y + img_enemy[enemy[i].type].getheight() && gamer.y + img_gamer.getheight() >= enemy[i].y)
        {
            gamer.isDied = true;
        }
    }
    if (gamer.isDied == true)
    {
        gameover(kill);
    }
}
int main() {
    initgraph(500, 720);
    init();
    init_socket();

    MOUSEMSG m;
    m = GetMouseMsg();
    while (true) {
        int starttime = clock();//获取程序执行到调用函数所用的毫秒数
        //welcome(m);
        draw();
        planemove(&gamer);
        bulletmove();
        if (Timer(200, 0)) {
            createnemy();
        }
        enemymove();
        hit();
        int frametime = clock() - starttime;//获取一次循环执行所需的毫秒数
        if (1000 / 10 - frametime > 0) {
            Sleep(1000 / 10 - frametime > 0);
        }
    }

    closegraph();
    return 0;
}
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
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    if (SOCKET_ERROR == bind(fd, (struct sockaddr*)&addr, sizeof(addr))) {
        cout << "bind error" << endl;
        return INVALID_SOCKET;
    }
    listen(fd, 10);
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