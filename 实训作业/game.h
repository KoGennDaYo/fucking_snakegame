#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 定义游戏区域的宽度
#define WIDTH 60
// 定义游戏区域的高度
#define HEIGHT 20
// 定义蛇的最大长度
#define MAX_LENGTH 100
// 定义食物的最大数量
#define MAX_FOOD 3

// 定义蛇的移动方向枚举类型
typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
// 定义食物的类型枚举类型
typedef enum { NORMAL, SPEED_UP, INVINCIBLE } FoodType;

// 定义点结构体，用于表示坐标位置
typedef struct {
    int x;  // x 坐标
    int y;  // y 坐标
} Point;

// 定义食物结构体
typedef struct {
    Point position;  // 食物的位置
    FoodType type;   // 食物的类型
} Food;

// 定义蛇结构体
typedef struct {
    Point body[MAX_LENGTH];  // 蛇的身体各部分的位置
    int length;              // 蛇的长度
    Direction dir;           // 蛇的移动方向
    int invincible;          // 蛇的无敌状态剩余时间
    int dead;                // 蛇是否死亡的标志
} Snake;

// 定义游戏结构体，包含游戏的所有状态信息
typedef struct {
    Snake snake;             // 蛇的状态
    Food foods[MAX_FOOD];    // 食物的状态
    Point obstacles[20];     // 障碍物的位置
    int obstacle_count;      // 障碍物的数量
    int score;               // 游戏得分
    int high_score;          // 游戏的最高得分
    int speed;               // 游戏的速度
    int level;               // 游戏的关卡
    int paused;              // 游戏是否暂停的标志
    int running;             // 游戏是否正在运行的标志
} Game;

// 函数声明

// 初始化游戏状态
void init_game(Game* game);
// 绘制游戏界面
void draw_game(Game* game, HANDLE hOut);
// 处理用户输入
void handle_input(Game* game);
// 更新游戏状态
void update_game(Game* game);
// 生成食物
void generate_food(Game* game);
// 生成障碍物
void generate_obstacles(Game* game);
// 播放音效
void play_sound(int type);

#endif