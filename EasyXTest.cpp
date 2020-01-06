#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <math.h>
#include <atlstr.h>

#pragma comment(lib,"winmm.lib")

// 窗口宽高	
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 580;

//	背景宽高
const int BACKGROUND_WIDTH = 400;
const int BACKGROUND_HEIGHT = 580;

// 背景_1坐标
int BACKGROUND_1_X = 0;
int BACKGROUND_1_Y = 0;

// 背景_2坐标
int BACKGROUND_2_X = 0;
int BACKGROUND_2_Y = -BACKGROUND_HEIGHT;

// 背景图像
IMAGE BACK_1_IMG, BACK_2_IMG;

// 背景音乐


//开始界面头像
IMAGE START_GAME_IMG;

//-------------------------------------------------------------

// 主机结构体
typedef struct MainPlane {
	int x;
	int y;
	int survive; // 0:死亡 1:存活
} Plane;

// 主机图片
IMAGE MAIN_PLANE_IMAGE;

// 主机宽高
const int MAIN_PLANE_WIDTH = 60;
const int MAIN_PLANE_HEIGHT = 60;

//-------------------------------------------------------------
// 计分
static int myScore = 0;

// 游戏结束标志(主机爆炸)
static int mainPlaneBomb = 0;

// ---------------------------------------------------


// 子弹结构体
typedef struct MainBullet {
	int x;
	int y;
	struct MainBullet* next;
} MBullet;

// 子弹图片
IMAGE MAIN_BULLET_IMAGE;

// 子弹宽高
const int MAIN_BULLET_WIDTH = 10;
const int MAIN_BULLET_HEIGHT = 10;

//-------------------------------------------------------------

// 敌机结构体
typedef struct EnemyPlane {
	int x;
	int y;
	int trajectory; // 1, 2, 4
	struct EnemyPlane* next;
}EPlane;

//敌机图片
IMAGE ENEMY_PLANE_IMG;

//敌机宽高
const int ENEMY_PLANE_WIDTH = 60;
const int ENEMY_PLANE_HEIGHT = 60;

//-----------------------------------------------------------------
//敌机子弹结构体
typedef struct EnemyBullet {
	int x;
	int y;
	struct EnemyBullet* next;
} EBullet;

// 敌机子弹图片
IMAGE ENEMY_BULLET_IMG;

// 敌机子弹宽高
const int ENEMY_BULLET_WIDTH = 6;
const int ENEMY_BULLET_HEIGHT = 6;

//-----------------------------------------------------------------
struct PlaneBomb {
	int x;
	int y;
	int life; // 1, 2, 3, 4, 5, 6, 7, 8
	struct PlaneBomb* next;
};

//飞机爆炸图片
IMAGE PLANE_BOMB_IMG_1;
IMAGE PLANE_BOMB_IMG_2;
IMAGE PLANE_BOMB_IMG_3;
IMAGE PLANE_BOMB_IMG_4;
IMAGE PLANE_BOMB_IMG_5;
IMAGE PLANE_BOMB_IMG_6;
IMAGE PLANE_BOMB_IMG_7;
IMAGE PLANE_BOMB_IMG_8;

// 爆炸宽高
const int PLANE_BOMB_WIDTH = 60;
const int PLANE_BOMB_HEIGHT = 60;

// ---------------------------------------------------------


//载入PNG图并去透明部分
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y);
//背景移动
void backMove();
//初始化(加载图片)
void init();
//绘制
void paint(Plane* plane, MBullet* mhead, EnemyPlane* ehead, EnemyBullet* ebhead, PlaneBomb* phead);
//飞机移动 & 发射子弹
void planeMoveAndFireBullet(int ch, Plane* plane, MBullet* mbullet);
//主机子弹移动
void mainBulletMove(MBullet* mbullet);
//开始界面
void gameStartInterface();
//敌机移动
void enemyPlaneMove(Plane* plane, EnemyPlane* ehead);
//产生敌机
void produceEnemyPlane(EnemyPlane* ehead);
//敌机发射子弹
void enemyPlaneOpenFire(EnemyPlane* ehead, EnemyBullet* ebhead);
//敌机子弹移动
void enemyBulletMove(EnemyBullet* ebhead);
//产生爆炸
void produceBomb(Plane* plane, MBullet* mhead, EnemyPlane* ehead, EnemyBullet* ebhead, PlaneBomb* phead);
//子弹碰撞
void bulletCollision(MBullet* mhead, EnemyBullet* ebhead);
//计分
void score();
//游戏结束
void gameOver(Plane* plane, MBullet* mhead, EnemyPlane* ehead, EnemyBullet* ebhead, PlaneBomb* phead);

// 主方法
int main()
{
	// 声明并初始化
	Plane plane; // 主机对象
	plane.x = (BACKGROUND_WIDTH - MAIN_PLANE_WIDTH) / 2;
	plane.y = BACKGROUND_HEIGHT - MAIN_PLANE_HEIGHT;
	plane.survive = 1;

	MBullet* mhead = (MBullet*)malloc(sizeof(MBullet));   // 子弹列表的虚头
	mhead->next = NULL; 	//指针一定要初始化
	EnemyPlane* ehead = (EnemyPlane*)malloc(sizeof(EnemyPlane)); //敌机列表
	ehead->next = NULL;
	EnemyBullet* ebhead = (EnemyBullet*)malloc(sizeof(EnemyBullet)); //敌机子弹列表
	ebhead->next = NULL;
	PlaneBomb* phead = (PlaneBomb*)malloc(sizeof(PlaneBomb));
	phead->next = NULL;


	//初始化
	init();

	// 开始界面
	gameStartInterface();
	// 开始界面结束

	// 游戏界面
	BeginBatchDraw();  // 开始绘制
	while (1) {
		// 主机爆炸 || 爆炸列表不为空
		if (!mainPlaneBomb) {
			// 非阻塞的接收按键输入
			if (_kbhit()) {
				planeMoveAndFireBullet(_getch(), &plane, mhead);
			}

			produceBomb(&plane, mhead, ehead, ebhead, phead);
			//bulletCollision(mhead, ebhead);
			backMove();
			mainBulletMove(mhead);
			produceEnemyPlane(ehead);
			enemyPlaneMove(&plane, ehead);
			enemyPlaneOpenFire(ehead, ebhead);
			enemyBulletMove(ebhead);
			paint(&plane, mhead, ehead, ebhead, phead);
			Sleep(30);
		}
		else {
			gameOver(&plane, mhead, ehead, ebhead, phead);
		}
		
	}
	
	
	EndBatchDraw(); // 结束绘制
	closegraph(); // 关闭窗口
}


/*
	载入PNG图并去透明部分
*/
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD *dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD *draw = GetImageBuffer();
	DWORD *src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

/*
  背景移动
*/
void backMove() {
	// BACKGROUND_HEIGHT 必须为每次步长的整数倍
	BACKGROUND_1_Y += 1;
	BACKGROUND_2_Y += 1;
	if (BACKGROUND_1_Y == BACKGROUND_HEIGHT) {
		BACKGROUND_1_Y = 0;
		BACKGROUND_2_Y = -BACKGROUND_HEIGHT;
	}
}

/*
	绘制
*/
void paint(Plane* plane, MBullet* mhead, EnemyPlane* ehead, EnemyBullet* ebhead, PlaneBomb* phead) {
	//设置当前绘制设备
	//SetWorkingImage();

	// 清屏
	clearrectangle(0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);

	// 绘制背景
	putimage(BACKGROUND_1_X, BACKGROUND_1_Y, &BACK_1_IMG);
	putimage(BACKGROUND_2_X, BACKGROUND_2_Y, &BACK_2_IMG);

	// 绘制飞机(如果主机存活的话)
	if (plane->survive) {
		drawAlpha(&MAIN_PLANE_IMAGE, plane->x, plane->y);
	}

	// 绘制主机子弹
	MBullet* cur = mhead->next;
	while (cur) {
		drawAlpha(&MAIN_BULLET_IMAGE, cur->x, cur->y);
		cur = cur->next;
	}

	// 绘制敌机
	EnemyPlane* cur1 = ehead->next;
	while (cur1) {
		drawAlpha(&ENEMY_PLANE_IMG, cur1->x, cur1->y);
		cur1 = cur1->next;
	}

	//绘制敌机子弹
	EnemyBullet* cur2 = ebhead->next;
	while (cur2) {
		drawAlpha(&ENEMY_BULLET_IMG, cur2->x, cur2->y);
		cur2 = cur2->next;
	}

	//绘制爆炸
	PlaneBomb* cur3 = phead->next;
	PlaneBomb* prev3 = phead;
	while (cur3) {
		switch (cur3->life)
		{
		case 1:
			drawAlpha(&PLANE_BOMB_IMG_1, cur3->x, cur3->y);
			cur3->life++;
			cur3 = cur3->next;
			break;
		case 2:
			drawAlpha(&PLANE_BOMB_IMG_2, cur3->x, cur3->y);
			cur3->life++;
			cur3 = cur3->next;
			break;
		case 3:
			drawAlpha(&PLANE_BOMB_IMG_3, cur3->x, cur3->y);
			cur3->life++;
			cur3 = cur3->next;
			break;
		case 4:
			drawAlpha(&PLANE_BOMB_IMG_4, cur3->x, cur3->y);
			cur3->life++;
			cur3 = cur3->next;
			break;
		case 5:
			drawAlpha(&PLANE_BOMB_IMG_5, cur3->x, cur3->y);
			cur3->life++;
			cur3 = cur3->next;
			break;
		case 6:
			drawAlpha(&PLANE_BOMB_IMG_6, cur3->x, cur3->y);
			cur3->life++;
			cur3 = cur3->next;
			break;
		case 7:
			drawAlpha(&PLANE_BOMB_IMG_7, cur3->x, cur3->y);
			cur3->life++;
			cur3 = cur3->next;
			break;
		case 8:
			drawAlpha(&PLANE_BOMB_IMG_8, cur3->x, cur3->y);
			cur3->life++;
			cur3 = cur3->next;
			break;
		default:
			// 删除这个爆炸
			prev3->next = cur3->next;
			free(cur3);
			cur3 = prev3->next;
			break;
		}
	}

	// 绘制分数
	settextcolor(RGB(0, 205, 102));
	settextstyle(20, 0, _TEXT("楷书"));
	setbkmode(TRANSPARENT);
	char str[20];
	sprintf_s(str, 20, "YOUR SCORE: %d", myScore);
	CString cstr = str;
	LPCTSTR pstr = (LPCTSTR)cstr;
	outtextxy(0, 550, pstr);

	// 刷新图片
	FlushBatchDraw();
}

/*
	初始化(加载图片)
*/
void init() {
	//初始化窗口
	initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);

	//初始化背景图片
	loadimage(&BACK_1_IMG, _TEXT("背景\\背景_01.jpg"), BACKGROUND_WIDTH, BACKGROUND_HEIGHT, true);
	loadimage(&BACK_2_IMG, _TEXT("背景\\背景_01.jpg"), BACKGROUND_WIDTH, BACKGROUND_HEIGHT, true);

	//初始化飞机图片
	loadimage(&MAIN_PLANE_IMAGE, _TEXT("主机\\主机_01.png"), MAIN_PLANE_WIDTH, MAIN_PLANE_HEIGHT, true);
	
	//初始化子弹图片
	loadimage(&MAIN_BULLET_IMAGE, _TEXT("子弹\\子弹_01.png"), MAIN_BULLET_WIDTH, MAIN_BULLET_HEIGHT, true);
	
	//初始化敌机图片
	loadimage(&ENEMY_PLANE_IMG, _TEXT("敌机\\敌机_02.png"), ENEMY_PLANE_WIDTH, ENEMY_PLANE_HEIGHT, true);
	
	//初始化敌机子弹图片
	loadimage(&ENEMY_BULLET_IMG, _TEXT("子弹\\敌机子弹_02.png"));

	//初始化飞机爆炸图片
	loadimage(&PLANE_BOMB_IMG_1, _TEXT("爆炸\\dd1.png"), PLANE_BOMB_WIDTH, PLANE_BOMB_HEIGHT, true);
	loadimage(&PLANE_BOMB_IMG_2, _TEXT("爆炸\\dd2.png"), PLANE_BOMB_WIDTH, PLANE_BOMB_HEIGHT, true);
	loadimage(&PLANE_BOMB_IMG_3, _TEXT("爆炸\\dd3.png"), PLANE_BOMB_WIDTH, PLANE_BOMB_HEIGHT, true);
	loadimage(&PLANE_BOMB_IMG_4, _TEXT("爆炸\\dd4.png"), PLANE_BOMB_WIDTH, PLANE_BOMB_HEIGHT, true);
	loadimage(&PLANE_BOMB_IMG_5, _TEXT("爆炸\\dd5.png"), PLANE_BOMB_WIDTH, PLANE_BOMB_HEIGHT, true);
	loadimage(&PLANE_BOMB_IMG_6, _TEXT("爆炸\\dd6.png"), PLANE_BOMB_WIDTH, PLANE_BOMB_HEIGHT, true);
	loadimage(&PLANE_BOMB_IMG_7, _TEXT("爆炸\\dd7.png"), PLANE_BOMB_WIDTH, PLANE_BOMB_HEIGHT, true);
	loadimage(&PLANE_BOMB_IMG_8, _TEXT("爆炸\\dd8.png"), PLANE_BOMB_WIDTH, PLANE_BOMB_HEIGHT, true);
	
	//循环播放音乐
	mciSendString(_TEXT("open 背景音乐\\背景音乐_02.mp3 alias bkmusic"), NULL, 0, NULL);
	mciSendString(_TEXT("play bkmusic repeat"), NULL, 0, NULL);

	//初始化游戏开始界面图片
	loadimage(&START_GAME_IMG, _TEXT("开始界面\\界面.jpg"), BACKGROUND_WIDTH, BACKGROUND_HEIGHT, true);
}

/*
	飞机移动 & 发射子弹
*/
void planeMoveAndFireBullet(int ch, Plane* plane, MBullet* mhead) {
	switch (ch) {
		case 72 :
			plane->y -= 10;
			break;
		case 80 : 
			plane->y += 10;
			break;
		case 75 :
			plane->x -= 10;
			break;
		case 77 :
			plane->x += 10;
			break;
		case 65 :
			MBullet* cur = mhead;
			MBullet * p = (MBullet*)malloc(sizeof(MBullet));
			p->next = cur->next;
			cur->next = p;
			//p->width = 10;
			//p->height = 10;
			p->x = plane->x + (MAIN_PLANE_WIDTH - MAIN_BULLET_WIDTH) / 2;
			p->y = plane->y - MAIN_BULLET_HEIGHT + 20;
			
			break;
	}

	// 越界处理
	if (plane->x <= 0)
		plane->x = 0;
	else if (plane->x >= (BACKGROUND_WIDTH - MAIN_PLANE_WIDTH))
		plane->x = BACKGROUND_WIDTH - MAIN_PLANE_WIDTH;
	else if (plane->y <= 0)
		plane->y = 0;
	else if (plane->y >= (BACKGROUND_HEIGHT - MAIN_PLANE_HEIGHT))
		plane->y = BACKGROUND_HEIGHT - MAIN_PLANE_HEIGHT;
}

/*
	主机子弹移动
*/
void mainBulletMove(MBullet* mhead) {
	MBullet* cur = mhead->next;
	MBullet* prev = mhead;
	while (cur != NULL) {
		cur->y -= 20;
		
		// 越界处理
		if (cur->y <= 0) {
			prev->next = cur->next;
			free(cur);
			// cur = cur->next;  //已经释放的空间再访问就会报错
			cur = prev->next;
		}
		else {
			prev = cur;
			cur = cur->next;
		}	
	}
}

/*
	游戏开始界面
*/
void gameStartInterface() {
	// 清除
	clearrectangle(0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);
	
	// 绘制背景
	putimage(0, 0, &START_GAME_IMG);

	// 绘制提示语 “请按空格键开始游戏”
	settextcolor(RGB(128,34,34));
	settextstyle(30, 0, _TEXT("黑体"));
	setbkmode(TRANSPARENT);
	outtextxy(50, 265, _TEXT("请按空格键开始游戏"));

	while (1) {
		if (_kbhit()) {
			if (_getch() == 32)
				break;
		}
	}
}

/*
	敌机移动
*/
void enemyPlaneMove(Plane* plane, EnemyPlane* ehead) {
	EnemyPlane* cur = ehead->next;
	EnemyPlane* prev = ehead;
	while (cur) {
		if (cur->trajectory == 1) {
			int dx = 0;
			int dy = 250;
			int x = cur->x;
			int y = cur->y;
			double xx = (x - dx)*cos(1.5 * 3.1415 / 180) - (y - dy)*sin(1.5 * 3.1415 / 180) + dx;
			double yy = (y - dy)*cos(1.5 * 3.1415 / 180) + (x - dx)*sin(1.5 * 3.1415 / 180) + dy;
			cur->x = (int)xx;
			cur->y = (int)yy;
		}
		else if (cur->trajectory == 2) {
			cur->y += 3;
		}
		else { // cur->trajectory == 4
			cur->y += 3;
			cur->x -= 2;
		}

		if (cur->y >= WINDOW_HEIGHT) {
			prev->next = cur->next;
			free(cur);
			cur = prev->next;
			continue;
		}
		prev = cur;
		cur = cur->next;
	}
}

/*
	敌机产生
*/
void produceEnemyPlane(EnemyPlane* ehead) {
	// 计算当前敌机的数量
	int count = 0;
	int sum = 0;
	EnemyPlane* cur = ehead->next;
	while (cur) {
		count++;
		sum += cur->trajectory;
		cur = cur->next;
	}
		
	// 当前敌机超过3架, 不产生敌机
	if (count >= 3)
		return;
	
	// 生成一个随机数,确定新敌机产生的位置
	EnemyPlane* newEnemyPlane = (EnemyPlane*)malloc(sizeof(EnemyPlane));
	if (sum == 0) {
		newEnemyPlane->x = 0;
		newEnemyPlane->y = 0;
		newEnemyPlane->trajectory = 1;
	}
	else if (sum == 1) {
		newEnemyPlane->x = (WINDOW_WIDTH - ENEMY_PLANE_WIDTH) / 2;
		newEnemyPlane->y = 0;
		newEnemyPlane->trajectory = 2;
	}
	else if (sum == 2) {
		newEnemyPlane->x = WINDOW_WIDTH - ENEMY_PLANE_WIDTH;
		newEnemyPlane->y = 0;
		newEnemyPlane->trajectory = 4;
	}
	else if (sum == 3) {
		newEnemyPlane->x = WINDOW_WIDTH - ENEMY_PLANE_WIDTH;
		newEnemyPlane->y = 0;
		newEnemyPlane->trajectory = 4;
	}
	else if (sum == 4) {
		newEnemyPlane->x = 0;
		newEnemyPlane->y = 0;
		newEnemyPlane->trajectory = 1;
	
	}
	else if (sum == 5) {
		newEnemyPlane->x = (WINDOW_WIDTH - ENEMY_PLANE_WIDTH) / 2;
		newEnemyPlane->y = 0;
		newEnemyPlane->trajectory = 2;
	}
	else if (sum == 6) {
		newEnemyPlane->x = 0;
		newEnemyPlane->y = 0;
		newEnemyPlane->trajectory = 1;
	}
	newEnemyPlane->next = ehead->next;
	ehead->next = newEnemyPlane;
}

/*
	敌机发射子弹
*/
void enemyPlaneOpenFire(EnemyPlane* ehead, EnemyBullet* ebhead) {
	// 有敌机才会发射子弹
	EnemyPlane* curPlane = ehead->next;
	while (curPlane) {
		int randNum = rand() % 10000;
		if (randNum % 13 == 0) {
			EnemyBullet* p = (EnemyBullet*)malloc(sizeof(EnemyBullet));
			p->x = curPlane->x + (ENEMY_PLANE_WIDTH - ENEMY_BULLET_WIDTH) / 2;
			p->y = curPlane->y + ENEMY_PLANE_HEIGHT - 10;
			p->next = ebhead->next;
			ebhead->next = p;
		}
		curPlane = curPlane->next;
	}
}

/*
	敌机子弹移动
*/
void enemyBulletMove(EnemyBullet* ebhead) {
	EnemyBullet* cur = ebhead->next;
	EnemyBullet* prev = ebhead;
	while (cur != NULL) {
		cur->y += 5;

		// 越界处理
		if (cur->y >= WINDOW_HEIGHT) {
			prev->next = cur->next;
			free(cur);
			// cur = cur->next;  //已经释放的空间再访问就会报错
			cur = prev->next;
		}
		else {
			prev = cur;
			cur = cur->next;
		}
	}
}

/*
	产生爆炸
*/
void produceBomb(Plane* plane, MBullet* mhead, EnemyPlane* ehead, EnemyBullet* ebhead, PlaneBomb* phead) {
	// 主机子弹打到敌机产生爆炸
	MBullet* curBullet = mhead->next;
	MBullet* prevBullet = mhead;
	while (curBullet) {
		EnemyPlane* curEnemyPlane = ehead->next;
		EnemyPlane* prevEnemyPlane = ehead;
		int flag = 0;
		while (curEnemyPlane) {
			// 碰撞了
			if (curBullet->x >= curEnemyPlane->x
				&&
				curBullet->x + MAIN_BULLET_WIDTH <= curEnemyPlane->x + ENEMY_PLANE_WIDTH
				&&
				curBullet->y >= curEnemyPlane->y
				&&
				curBullet->y + MAIN_BULLET_HEIGHT <= curEnemyPlane->y + ENEMY_PLANE_HEIGHT
				) 
			{
				//加入爆炸
				PlaneBomb* p = (PlaneBomb*)malloc(sizeof(PlaneBomb));
				p->x = curEnemyPlane->x;
				p->y = curEnemyPlane->y;
				p->life = 1;
				p->next = phead->next;
				phead->next = p;

				// 删除飞机	
				prevEnemyPlane->next = curEnemyPlane->next;
				free(curEnemyPlane);
				curEnemyPlane = prevEnemyPlane->next;

				//计分
				score();

				//标志为1
				flag = 1;
				break;
			}
			else {
				prevEnemyPlane = curEnemyPlane;
				curEnemyPlane = curEnemyPlane->next;
			}
		}

		if (flag) {
			//删除子弹
			prevBullet->next = curBullet->next;
			free(curBullet);
			curBullet = prevBullet->next;
		}
		else {
			prevBullet = curBullet;
			curBullet = curBullet->next;
		}
	}


	// 主机与敌机相撞产生爆炸
	EnemyPlane* curEnemyPlane = ehead->next;
	EnemyPlane* prevEnemyPlane = ehead;
	while (curEnemyPlane) {
		if (plane->x + MAIN_PLANE_WIDTH >= curEnemyPlane->x
			&&
			plane->x <= curEnemyPlane->x + ENEMY_PLANE_WIDTH
			&&
			plane->y + MAIN_PLANE_HEIGHT >= curEnemyPlane->y
			&&
			plane->y <= curEnemyPlane->y + ENEMY_PLANE_HEIGHT)
		{
			//敌机爆炸
			PlaneBomb* p = (PlaneBomb*)malloc(sizeof(PlaneBomb));
			p->x = curEnemyPlane->x;
			p->y = curEnemyPlane->y;
			p->life = 1;
			p->next = phead->next;
			phead->next = p;

			//主机爆炸
			PlaneBomb* p1 = (PlaneBomb*)malloc(sizeof(PlaneBomb));
			p1->x = plane->x;
			p1->y = plane->y;
			p1->life = 1;
			p1->next = phead->next;
			phead->next = p1;

			//设置主机为死亡
			plane->survive = 0;
			plane->x = -MAIN_BULLET_WIDTH;
			plane->y = -MAIN_PLANE_HEIGHT;

			//游戏结束标志(主机爆炸)
			mainPlaneBomb = 1;

			//删除敌机
			prevEnemyPlane->next = curEnemyPlane->next;
			free(curEnemyPlane);
			curEnemyPlane = prevEnemyPlane->next;

			return;
		}
		else {
			prevEnemyPlane = curEnemyPlane;
			curEnemyPlane = curEnemyPlane->next;
		}
	
	}

	// 敌机子弹打到主机
	EnemyBullet* curEnemyBullet = ebhead->next;
	EnemyBullet* prevEnemyBullet = ebhead;
	while (curEnemyBullet) {
		if (curEnemyBullet->x >= plane->x
			&&
			curEnemyBullet->x <= plane->x + MAIN_PLANE_WIDTH
			&&
			curEnemyBullet->y >= plane->y
			&&
			curEnemyBullet->y <= plane->y + MAIN_PLANE_HEIGHT) {

			//主机爆炸
			PlaneBomb* p = (PlaneBomb*)malloc(sizeof(PlaneBomb));
			p->x = plane->x;
			p->y = plane->y;
			p->life = 1;
			p->next = phead->next;
			phead->next = p;

			//设置主机为死亡
			plane->survive = 0;
			plane->x = -MAIN_BULLET_WIDTH;
			plane->y = -MAIN_PLANE_HEIGHT;

			//游戏结束标志(主机爆炸)
			mainPlaneBomb = 1;

			//删除敌机子弹
			prevEnemyBullet->next = curEnemyBullet->next;
			free(curEnemyBullet);
			curEnemyBullet = prevEnemyBullet->next;
		}
		else {
			prevEnemyBullet = curEnemyBullet;
			curEnemyBullet = curEnemyBullet->next;
		}
	}


}

/*
	子弹碰撞
*/
void bulletCollision(MBullet* mhead, EnemyBullet* ebhead) {
	MBullet* curBullet = mhead->next;
	MBullet* prevBullet = mhead;
	while (curBullet) {
		EnemyBullet* curEnemyBullet = ebhead->next;
		EnemyBullet* prevEnemyBullet = ebhead;
		int flag = 0;
		while (curEnemyBullet) {
			// 碰撞了
			if (curBullet->x -4 <= curEnemyBullet->x
				&&
				curBullet->x + MAIN_BULLET_WIDTH + 6 >= curEnemyBullet->x + ENEMY_BULLET_WIDTH
				&&
				curBullet->y -4 <= curEnemyBullet->y
				&&
				curBullet->y + MAIN_BULLET_HEIGHT + 6 >= curEnemyBullet->y + ENEMY_BULLET_HEIGHT
				)
			{
				
				// 删除敌机子弹
				prevEnemyBullet->next = curEnemyBullet->next;
				free(curEnemyBullet);
				curEnemyBullet = prevEnemyBullet->next;

				//标志为1
				flag = 1;
				break;
			}
			else {
				prevEnemyBullet = curEnemyBullet;
				curEnemyBullet = curEnemyBullet->next;
			}
		}

		if (flag) {
			//删除子弹
			prevBullet->next = curBullet->next;
			free(curBullet);
			curBullet = prevBullet->next;
		}
		else {
			prevBullet = curBullet;
			curBullet = curBullet->next;
		}
	}
}

/*
	计分
*/
void score() {
	myScore += 100;
}

/*
	游戏结束界面
*/
void gameOver(Plane* plane, MBullet* mhead, EnemyPlane* ehead, EnemyBullet* ebhead, PlaneBomb* phead) {
	// 清除
	clearrectangle(0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT);

	// 绘制背景
	setfillcolor(BLACK);
	fillrectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// 绘制提示语
	// 提示语: YOUR SOCRE: XXX
	char str[20];
	sprintf_s(str, 20, "YOUR SCORE: %d", myScore);
	CString cstr = str;
	LPCTSTR pstr = (LPCTSTR)cstr;

	settextcolor(RGB(255, 69, 00));
	settextstyle(30, 0, _TEXT("黑体"));
	setbkmode(TRANSPARENT);

	outtextxy(50, 235, _TEXT("GAME           OVER"));
	outtextxy(50, 270, _TEXT("按空格键重新开始游戏"));
	outtextxy(50, 305, pstr);

	// 刷新图片
	FlushBatchDraw();

	while (1) {
		if (_kbhit()) {
			if (_getch() == 32)
			{
				plane->x = (BACKGROUND_WIDTH - MAIN_PLANE_WIDTH) / 2;
				plane->y = BACKGROUND_HEIGHT - MAIN_PLANE_HEIGHT;
				plane->survive = 1;

				mhead->next = NULL;
				ehead->next = NULL;
				ebhead->next = NULL;
				phead->next = NULL;

				myScore = 0; //分数重置为0
				mainPlaneBomb = 0; //复活
				break;
			}
		}
	}

}

