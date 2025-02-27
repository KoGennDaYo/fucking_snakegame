#include "game.h"

// 初始化游戏状态
void init_game(Game* game) {
    // 初始化随机数种子，使用当前时间作为种子，确保每次游戏的随机性
    srand(time(NULL));
    // 初始化蛇的长度为 3
    game->snake.length = 3;
    // 蛇初始的移动方向为向右
    game->snake.dir = RIGHT;
    // 初始时蛇没有无敌状态，无敌时间为 0
    game->snake.invincible = 0;
    // 初始时蛇没有死亡
    game->snake.dead = 0;
    // 初始得分设为 0
    game->score = 0;
    // 游戏初始关卡为 1
    game->level = 1;
    // 游戏初始速度，数值越大速度越慢
    game->speed = 150;
    // 游戏初始状态不是暂停状态
    game->paused = 0;
    // 游戏开始时处于运行状态
    game->running = 1;

    // 蛇初始位置的起始 x 坐标，位于游戏区域中心
    int startX = WIDTH / 2;
    // 蛇初始位置的起始 y 坐标，位于游戏区域中心
    int startY = HEIGHT / 2;
    // 初始化蛇的身体位置，蛇身依次排列
    for (int i = 0; i < game->snake.length; i++) {
        game->snake.body[i].x = startX - i;
        game->snake.body[i].y = startY;
    }

    // 生成游戏中的障碍物
    generate_obstacles(game);
    // 生成游戏中的食物
    generate_food(game);
}

// 绘制游戏界面
void draw_game(Game* game, HANDLE hOut) {
    // 扩大缓冲区以容纳更多说明信息，用于存储游戏界面的字符信息
    CHAR_INFO buffer[(WIDTH + 2) * (HEIGHT + 15)];
    // 定义要绘制的矩形区域，即游戏界面的范围
    SMALL_RECT rect = { 0, 0, WIDTH + 1, HEIGHT + 20 };
    // 定义缓冲区的大小
    COORD bufferSize = { WIDTH + 2, HEIGHT + 15 };
    // 定义缓冲区的起始坐标
    COORD bufferCoord = { 0, 0 };

    // 初始化缓冲区，将整个区域填充为相应的字符和属性
    for (int y = 0; y < HEIGHT + 15; y++) {
        for (int x = 0; x < WIDTH + 2; x++) {
            int index = y * (WIDTH + 2) + x;
            // 如果是边界位置，显示 '#' 字符
            if (y == 0 || y == HEIGHT + 1 || x == 0 || x == WIDTH + 1) {
                buffer[index].Char.AsciiChar = '#';
                buffer[index].Attributes = 0x07;
            }
            // 其他位置显示空格
            else {
                buffer[index].Char.AsciiChar = ' ';
                buffer[index].Attributes = 0x07;
            }
        }
    }

    // 绘制蛇
    for (int i = 0; i < game->snake.length; i++) {
        int x = game->snake.body[i].x + 1;
        int y = game->snake.body[i].y + 1;
        int index = y * (WIDTH + 2) + x;
        // 蛇头显示 '@' 字符，身体显示 '*' 字符
        buffer[index].Char.AsciiChar = (i == 0) ? '@' : 'o';
        // 如果蛇处于无敌状态，显示不同的颜色属性
        buffer[index].Attributes = (game->snake.invincible > 0) ? 0x0E : 0x0A;
    }

    // 绘制食物
    for (int i = 0; i < MAX_FOOD; i++) {
        int x = game->foods[i].position.x + 1;
        int y = game->foods[i].position.y + 1;
        int index = y * (WIDTH + 2) + x;
        buffer[index].Char.AsciiChar = '$';
        // 根据食物类型显示不同的颜色属性
        buffer[index].Attributes =
            (game->foods[i].type == NORMAL) ? 0x0C :
            (game->foods[i].type == SPEED_UP) ? 0x09 : 0x0D;
    }

    // 绘制障碍物
    for (int i = 0; i < game->obstacle_count; i++) {
        int x = game->obstacles[i].x + 1;
        int y = game->obstacles[i].y + 1;
        int index = y * (WIDTH + 2) + x;
        buffer[index].Char.AsciiChar = 'X';
        buffer[index].Attributes = 0x08;
    }

    // 绘制游戏信息，如得分、关卡、速度等
    char info[50];
    sprintf(info, "得分: %d  难度: %d  速度: %d", game->score, game->level, 150 - game->speed + 50);
    for (int i = 0; info[i] != '\0'; i++) {
        int index = (HEIGHT + 2) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = info[i];
        buffer[index].Attributes = 0x07;
    }

    // 添加文字说明，提示用户按空格暂停游戏
    char instruction[] = "按下空格以暂停游戏.";
    for (int i = 0; instruction[i] != '\0'; i++) {
        int index = (HEIGHT + 3) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = instruction[i];
        buffer[index].Attributes = 0x07;
    }

    // 添加菜单中的食物效果说明
    char foodEffect1[] = "食物效果:";
    for (int i = 0; foodEffect1[i] != '\0'; i++) {
        int index = (HEIGHT + 4) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = foodEffect1[i];
        buffer[index].Attributes = 0x07;
    }
    char foodEffect2[] = " - 普通食物: 加分，无特殊效果。";
    for (int i = 0; foodEffect2[i] != '\0'; i++) {
        int index = (HEIGHT + 5) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = foodEffect2[i];
        buffer[index].Attributes = 0x07;
    }
    char foodEffect3[] = " - 加速食物 (蓝色): 给你的蛇加速。";
    for (int i = 0; foodEffect3[i] != '\0'; i++) {
        int index = (HEIGHT + 6) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = foodEffect3[i];
        buffer[index].Attributes = 0x07;
    }
    char foodEffect4[] = " - 无敌食物 (紫色): 让蛇在一定时间内可以穿过障碍物。";
    for (int i = 0; foodEffect4[i] != '\0'; i++) {
        int index = (HEIGHT + 7) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = foodEffect4[i];
        buffer[index].Attributes = 0x07;
    }

    // 添加操作说明
    char controls1[] = "操作说明:";
    for (int i = 0; controls1[i] != '\0'; i++) {
        int index = (HEIGHT + 8) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = controls1[i];
        buffer[index].Attributes = 0x07;
    }
    char controls2[] = " - 使用你键盘的方向键以控制移动。";
    for (int i = 0; controls2[i] != '\0'; i++) {
        int index = (HEIGHT + 9) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = controls2[i];
        buffer[index].Attributes = 0x07;
    }
    char controls3[] = " - 游戏结束时按下R键重新开始。";
    for (int i = 0; controls3[i] != '\0'; i++) {
        int index = (HEIGHT + 10) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = controls3[i];
        buffer[index].Attributes = 0x07;
    }
    char controls4[] = " - 按下Esc以退出游戏。";
    for (int i = 0; controls4[i] != '\0'; i++) {
        int index = (HEIGHT + 11) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = controls4[i];
        buffer[index].Attributes = 0x07;
    }

    // 如果游戏处于暂停状态，显示暂停提示信息
    if (game->paused) {
        char pauseMsg[] = "游戏已暂停，按下空格开始。";
        for (int i = 0; pauseMsg[i] != '\0'; i++) {
            int index = (HEIGHT + 12) * (WIDTH + 2) + i;
            buffer[index].Char.AsciiChar = pauseMsg[i];
            buffer[index].Attributes = 0x07;
        }
    }

    // 如果蛇死亡，显示游戏结束提示信息
    if (game->snake.dead) {
        char gameOverMsg[] = "游戏结束！按下R重新开始或Esc退出游戏。";
        for (int i = 0; gameOverMsg[i] != '\0'; i++) {
            int index = (HEIGHT + 13) * (WIDTH + 2) + i;
            buffer[index].Char.AsciiChar = gameOverMsg[i];
            buffer[index].Attributes = 0x07;
        }
    }

    // 将缓冲区内容输出到控制台
    WriteConsoleOutput(hOut, buffer, bufferSize, bufferCoord, &rect);
    // 刷新缓冲区
    FlushConsoleInputBuffer(hOut);
}

// 处理用户输入
void handle_input(Game* game) {
    // 检查是否有按键输入
    if (_kbhit()) {
        // 获取用户输入的按键
        int ch = _getch();
        // 如果是方向键等特殊按键，会先返回 0xE0，再获取具体按键值
        if (ch == 0xE0) ch = _getch();
        // 记录新的移动方向
        Direction newDir = game->snake.dir;
        switch (ch) {
            // 上方向键，蛇向上移动
        case 72: newDir = UP; break;
            // 下方向键，蛇向下移动
        case 80: newDir = DOWN; break;
            // 左方向键，蛇向左移动
        case 75: newDir = LEFT; break;
            // 右方向键，蛇向右移动
        case 77: newDir = RIGHT; break;
            // 空格键，暂停或继续游戏
        case 32: game->paused = !game->paused; break;
            // ESC 键，退出游戏
        case 27: game->running = 0; break;
            // R 键，游戏结束后重新开始游戏
        case 'r':
        case 'R':
            if (game->snake.dead) {
                init_game(game);
            }
            break;
        }
        // 禁止蛇立即反向移动
        if ((newDir == UP && game->snake.dir != DOWN) ||
            (newDir == DOWN && game->snake.dir != UP) ||
            (newDir == LEFT && game->snake.dir != RIGHT) ||
            (newDir == RIGHT && game->snake.dir != LEFT)) {
            game->snake.dir = newDir;
        }
    }
}

// 更新游戏状态
void update_game(Game* game) {
    // 如果游戏暂停或蛇已死亡，不进行更新
    if (game->paused || game->snake.dead) return;
    // 计算新的蛇头位置
    Point newHead = game->snake.body[0];
    switch (game->snake.dir) {
    case UP: newHead.y--; break;
    case DOWN: newHead.y++; break;
    case LEFT: newHead.x--; break;
    case RIGHT: newHead.x++; break;
    }
    // 边界碰撞检测
    if (newHead.x < 0 || newHead.x >= WIDTH || newHead.y < 0 || newHead.y >= HEIGHT) {
        game->snake.dead = 1;
        // 播放游戏结束音效
        play_sound(3);
        return;
    }
    // 障碍物碰撞检测
    for (int i = 0; i < game->obstacle_count; i++) {
        if (newHead.x == game->obstacles[i].x && newHead.y == game->obstacles[i].y && !game->snake.invincible) {
            game->snake.dead = 1;
            // 播放游戏结束音效
            play_sound(3);
            return;
        }
    }
    // 自碰撞检测
    for (int i = 1; i < game->snake.length; i++) {
        if (newHead.x == game->snake.body[i].x && newHead.y == game->snake.body[i].y && !game->snake.invincible) {
            game->snake.dead = 1;
            // 播放游戏结束音效
            play_sound(3);
            return;
        }
    }
    int ateFood = 0;
    // 食物检测
    for (int i = 0; i < MAX_FOOD; i++) {
        if (newHead.x == game->foods[i].position.x && newHead.y == game->foods[i].position.y) {
            // 播放吃食物音效
            play_sound(1);
            // 根据食物类型增加得分
            game->score += (game->foods[i].type + 1) * 10;
            ateFood = 1;
            // 处理不同类型食物的特殊效果
            switch (game->foods[i].type) {
            case SPEED_UP:
                // 加速
                game->speed = (game->speed > 50) ? game->speed - 20 : 50;
                break;
            case INVINCIBLE:
                // 获得无敌状态
                game->snake.invincible = 50;
                break;
            }
            // 生成新的食物
            generate_food(game);
        }
    }
    if (!ateFood) {
        // 未吃到食物，移动蛇身
        for (int i = game->snake.length - 1; i > 0; i--) {
            game->snake.body[i] = game->snake.body[i - 1];
        }
    }
    else {
        if (game->snake.length < MAX_LENGTH) {
            // 吃到食物，蛇身增长
            game->snake.length++;
            for (int i = game->snake.length - 1; i > 0; i--) {
                game->snake.body[i] = game->snake.body[i - 1];
            }
        }
    }
    // 更新蛇头位置
    game->snake.body[0] = newHead;
    // 关卡升级检测
    if (game->score >= game->level * 100) {
        game->level++;
        // 根据关卡提升速度
        game->speed = 150 - (game->level * 10);
        if (game->speed < 50) game->speed = 50;
        // 生成新的障碍物
        generate_obstacles(game);
    }
    // 无敌状态计时递减
    if (game->snake.invincible > 0) game->snake.invincible--;
}

// 生成食物
void generate_food(Game* game) {
    for (int i = 0; i < MAX_FOOD; i++) {
        int valid;
        do {
            valid = 1;
            // 随机生成食物的位置
            game->foods[i].position.x = rand() % WIDTH;
            game->foods[i].position.y = rand() % HEIGHT;
            // 随机生成食物的类型
            game->foods[i].type = rand() % 3;
            // 检查食物是否与蛇身重叠
            for (int j = 0; j < game->snake.length; j++) {
                if (game->snake.body[j].x == game->foods[i].position.x && game->snake.body[j].y == game->foods[i].position.y) {
                    valid = 0;
                    break;
                }
            }
            // 检查食物是否与障碍物重叠
            for (int j = 0; j < game->obstacle_count; j++) {
                if (game->obstacles[j].x == game->foods[i].position.x && game->obstacles[j].y == game->foods[i].position.y) {
                    valid = 0;
                    break;
                }
            }
            // 检查食物是否与其他食物重叠
            for (int j = 0; j < i; j++) {
                if (game->foods[j].position.x == game->foods[i].position.x && game->foods[j].position.y == game->foods[i].position.y) {
                    valid = 0;
                    break;
                }
            }
        } while (!valid);
    }
}

// 生成障碍物
void generate_obstacles(Game* game) {
    // 初始化障碍物数量为 0
    game->obstacle_count = 0;
    // 初始化尝试次数为 0
    int attempts = 0;
    // 循环生成障碍物，直到达到最大数量或者尝试次数达到上限
    while (game->obstacle_count < 20 && attempts < 1000) {
        // 随机生成一个新的障碍物位置，范围在游戏区域内并适当向内偏移
        Point newObstacle = { rand() % (WIDTH - 4) + 2, rand() % (HEIGHT - 4) + 2 };
        // 标记新障碍物是否离蛇太近
        int tooClose = 0;
        // 检查新障碍物是否离蛇太近
        for (int j = 0; j < game->snake.length; j++) {
            // 计算新障碍物与蛇身某部分的 x 轴距离
            int dx = newObstacle.x - game->snake.body[j].x;
            // 计算新障碍物与蛇身某部分的 y 轴距离
            int dy = newObstacle.y - game->snake.body[j].y;
            // 如果距离小于 25（这里用平方和判断距离），则标记为太近
            if (dx * dx + dy * dy < 25) {
                tooClose = 1;
                break;
            }
        }
        // 标记新障碍物是否已经存在
        int exists = 0;
        // 检查新障碍物是否已经存在于现有的障碍物列表中
        for (int j = 0; j < game->obstacle_count; j++) {
            if (newObstacle.x == game->obstacles[j].x && newObstacle.y == game->obstacles[j].y) {
                exists = 1;
                break;
            }
        }
        // 如果新障碍物既不靠近蛇，也不存在于现有列表中，则添加到障碍物列表
        if (!tooClose && !exists) {
            game->obstacles[game->obstacle_count++] = newObstacle;
        }
        // 尝试次数加 1
        attempts++;
    }
}

// 播放音效
void play_sound(int type) {
    switch (type) {
    case 1:
        // 简单打印提示信息，可替换为实际的音效播放代码
        printf("Eat food sound!\n");
        break;
    case 3:
        // 简单打印提示信息，可替换为实际的音效播放代码
        printf("Game over sound!\n");
        break;
    default:
        break;
    }
}