#include "game.h"
#include <stdio.h>
#include <windows.h>

// 定义控制台窗口的宽度，基于游戏区域宽度加上边界
#define CONSOLE_WIDTH (WIDTH + 20)
// 定义控制台窗口的高度，基于游戏区域高度加上额外显示信息的空间
#define CONSOLE_HEIGHT (HEIGHT + 20)

int main() {
    // 定义一个 Game 结构体变量，用于存储游戏的各种状态信息
    Game game;
    // 获取标准输出设备的句柄，用于后续控制台的输出操作
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // 调用 init_game 函数初始化游戏的各种状态，如蛇的位置、长度，食物和障碍物的生成等
    init_game(&game);

    // 定义一个 SMALL_RECT 结构体变量，用于设置控制台窗口的大小和位置
    SMALL_RECT windowSize = { 0, 0, CONSOLE_WIDTH - 1, CONSOLE_HEIGHT - 1 };
    // 定义一个 COORD 结构体变量，用于设置控制台缓冲区的大小
    COORD bufferSize = { CONSOLE_WIDTH, CONSOLE_HEIGHT };
    // 设置控制台缓冲区的大小，确保能容纳整个游戏界面和相关信息
    SetConsoleScreenBufferSize(hOut, bufferSize);

    // 定义一个 CONSOLE_CURSOR_INFO 结构体变量，用于获取和设置控制台光标的信息
    CONSOLE_CURSOR_INFO cursorInfo;
    // 获取当前控制台光标的信息
    GetConsoleCursorInfo(hOut, &cursorInfo);
    // 将光标设置为不可见，避免影响游戏界面的显示
    cursorInfo.bVisible = FALSE;
    // 设置控制台光标的信息，使光标不可见
    SetConsoleCursorInfo(hOut, &cursorInfo);

    // 游戏主循环，只要游戏处于运行状态，就会不断循环执行
    while (game.running) {
        // 调用 handle_input 函数处理用户的输入，如方向键控制蛇的移动、暂停游戏等
        handle_input(&game);
        // 如果游戏没有暂停且蛇没有死亡
        if (!game.paused && !game.snake.dead) {
            // 调用 update_game 函数更新游戏的状态，如蛇的移动、食物的检测、碰撞检测等
            update_game(&game);
        }
        // 调用 draw_game 函数绘制游戏界面，包括蛇、食物、障碍物、得分等信息
        draw_game(&game, hOut);
        // 让程序暂停一段时间，暂停的时间由 game.speed 决定，实现游戏的速度控制
        Sleep(game.speed);
    }

    // 游戏结束，返回 0 表示程序正常退出
    return 0;
}