#ifndef GAME_H
#define GAME_H

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// ������Ϸ����Ŀ��
#define WIDTH 60
// ������Ϸ����ĸ߶�
#define HEIGHT 20
// �����ߵ���󳤶�
#define MAX_LENGTH 100
// ����ʳ����������
#define MAX_FOOD 3

// �����ߵ��ƶ�����ö������
typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
// ����ʳ�������ö������
typedef enum { NORMAL, SPEED_UP, INVINCIBLE } FoodType;

// �����ṹ�壬���ڱ�ʾ����λ��
typedef struct {
    int x;  // x ����
    int y;  // y ����
} Point;

// ����ʳ��ṹ��
typedef struct {
    Point position;  // ʳ���λ��
    FoodType type;   // ʳ�������
} Food;

// �����߽ṹ��
typedef struct {
    Point body[MAX_LENGTH];  // �ߵ���������ֵ�λ��
    int length;              // �ߵĳ���
    Direction dir;           // �ߵ��ƶ�����
    int invincible;          // �ߵ��޵�״̬ʣ��ʱ��
    int dead;                // ���Ƿ������ı�־
} Snake;

// ������Ϸ�ṹ�壬������Ϸ������״̬��Ϣ
typedef struct {
    Snake snake;             // �ߵ�״̬
    Food foods[MAX_FOOD];    // ʳ���״̬
    Point obstacles[20];     // �ϰ����λ��
    int obstacle_count;      // �ϰ��������
    int score;               // ��Ϸ�÷�
    int high_score;          // ��Ϸ����ߵ÷�
    int speed;               // ��Ϸ���ٶ�
    int level;               // ��Ϸ�Ĺؿ�
    int paused;              // ��Ϸ�Ƿ���ͣ�ı�־
    int running;             // ��Ϸ�Ƿ��������еı�־
} Game;

// ��������

// ��ʼ����Ϸ״̬
void init_game(Game* game);
// ������Ϸ����
void draw_game(Game* game, HANDLE hOut);
// �����û�����
void handle_input(Game* game);
// ������Ϸ״̬
void update_game(Game* game);
// ����ʳ��
void generate_food(Game* game);
// �����ϰ���
void generate_obstacles(Game* game);
// ������Ч
void play_sound(int type);

#endif