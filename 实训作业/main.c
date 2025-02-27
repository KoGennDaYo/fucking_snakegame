#include "game.h"
#include <stdio.h>
#include <windows.h>

// �������̨���ڵĿ�ȣ�������Ϸ�����ȼ��ϱ߽�
#define CONSOLE_WIDTH (WIDTH + 20)
// �������̨���ڵĸ߶ȣ�������Ϸ����߶ȼ��϶�����ʾ��Ϣ�Ŀռ�
#define CONSOLE_HEIGHT (HEIGHT + 20)

int main() {
    // ����һ�� Game �ṹ����������ڴ洢��Ϸ�ĸ���״̬��Ϣ
    Game game;
    // ��ȡ��׼����豸�ľ�������ں�������̨���������
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // ���� init_game ������ʼ����Ϸ�ĸ���״̬�����ߵ�λ�á����ȣ�ʳ����ϰ�������ɵ�
    init_game(&game);

    // ����һ�� SMALL_RECT �ṹ��������������ÿ���̨���ڵĴ�С��λ��
    SMALL_RECT windowSize = { 0, 0, CONSOLE_WIDTH - 1, CONSOLE_HEIGHT - 1 };
    // ����һ�� COORD �ṹ��������������ÿ���̨�������Ĵ�С
    COORD bufferSize = { CONSOLE_WIDTH, CONSOLE_HEIGHT };
    // ���ÿ���̨�������Ĵ�С��ȷ��������������Ϸ����������Ϣ
    SetConsoleScreenBufferSize(hOut, bufferSize);

    // ����һ�� CONSOLE_CURSOR_INFO �ṹ����������ڻ�ȡ�����ÿ���̨������Ϣ
    CONSOLE_CURSOR_INFO cursorInfo;
    // ��ȡ��ǰ����̨������Ϣ
    GetConsoleCursorInfo(hOut, &cursorInfo);
    // ���������Ϊ���ɼ�������Ӱ����Ϸ�������ʾ
    cursorInfo.bVisible = FALSE;
    // ���ÿ���̨������Ϣ��ʹ��겻�ɼ�
    SetConsoleCursorInfo(hOut, &cursorInfo);

    // ��Ϸ��ѭ����ֻҪ��Ϸ��������״̬���ͻ᲻��ѭ��ִ��
    while (game.running) {
        // ���� handle_input ���������û������룬�緽��������ߵ��ƶ�����ͣ��Ϸ��
        handle_input(&game);
        // �����Ϸû����ͣ����û������
        if (!game.paused && !game.snake.dead) {
            // ���� update_game ����������Ϸ��״̬�����ߵ��ƶ���ʳ��ļ�⡢��ײ����
            update_game(&game);
        }
        // ���� draw_game ����������Ϸ���棬�����ߡ�ʳ��ϰ���÷ֵ���Ϣ
        draw_game(&game, hOut);
        // �ó�����ͣһ��ʱ�䣬��ͣ��ʱ���� game.speed ������ʵ����Ϸ���ٶȿ���
        Sleep(game.speed);
    }

    // ��Ϸ���������� 0 ��ʾ���������˳�
    return 0;
}