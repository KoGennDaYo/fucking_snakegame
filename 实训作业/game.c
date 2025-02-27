#include "game.h"

// ��ʼ����Ϸ״̬
void init_game(Game* game) {
    // ��ʼ����������ӣ�ʹ�õ�ǰʱ����Ϊ���ӣ�ȷ��ÿ����Ϸ�������
    srand(time(NULL));
    // ��ʼ���ߵĳ���Ϊ 3
    game->snake.length = 3;
    // �߳�ʼ���ƶ�����Ϊ����
    game->snake.dir = RIGHT;
    // ��ʼʱ��û���޵�״̬���޵�ʱ��Ϊ 0
    game->snake.invincible = 0;
    // ��ʼʱ��û������
    game->snake.dead = 0;
    // ��ʼ�÷���Ϊ 0
    game->score = 0;
    // ��Ϸ��ʼ�ؿ�Ϊ 1
    game->level = 1;
    // ��Ϸ��ʼ�ٶȣ���ֵԽ���ٶ�Խ��
    game->speed = 150;
    // ��Ϸ��ʼ״̬������ͣ״̬
    game->paused = 0;
    // ��Ϸ��ʼʱ��������״̬
    game->running = 1;

    // �߳�ʼλ�õ���ʼ x ���꣬λ����Ϸ��������
    int startX = WIDTH / 2;
    // �߳�ʼλ�õ���ʼ y ���꣬λ����Ϸ��������
    int startY = HEIGHT / 2;
    // ��ʼ���ߵ�����λ�ã�������������
    for (int i = 0; i < game->snake.length; i++) {
        game->snake.body[i].x = startX - i;
        game->snake.body[i].y = startY;
    }

    // ������Ϸ�е��ϰ���
    generate_obstacles(game);
    // ������Ϸ�е�ʳ��
    generate_food(game);
}

// ������Ϸ����
void draw_game(Game* game, HANDLE hOut) {
    // ���󻺳��������ɸ���˵����Ϣ�����ڴ洢��Ϸ������ַ���Ϣ
    CHAR_INFO buffer[(WIDTH + 2) * (HEIGHT + 15)];
    // ����Ҫ���Ƶľ������򣬼���Ϸ����ķ�Χ
    SMALL_RECT rect = { 0, 0, WIDTH + 1, HEIGHT + 20 };
    // ���建�����Ĵ�С
    COORD bufferSize = { WIDTH + 2, HEIGHT + 15 };
    // ���建��������ʼ����
    COORD bufferCoord = { 0, 0 };

    // ��ʼ�����������������������Ϊ��Ӧ���ַ�������
    for (int y = 0; y < HEIGHT + 15; y++) {
        for (int x = 0; x < WIDTH + 2; x++) {
            int index = y * (WIDTH + 2) + x;
            // ����Ǳ߽�λ�ã���ʾ '#' �ַ�
            if (y == 0 || y == HEIGHT + 1 || x == 0 || x == WIDTH + 1) {
                buffer[index].Char.AsciiChar = '#';
                buffer[index].Attributes = 0x07;
            }
            // ����λ����ʾ�ո�
            else {
                buffer[index].Char.AsciiChar = ' ';
                buffer[index].Attributes = 0x07;
            }
        }
    }

    // ������
    for (int i = 0; i < game->snake.length; i++) {
        int x = game->snake.body[i].x + 1;
        int y = game->snake.body[i].y + 1;
        int index = y * (WIDTH + 2) + x;
        // ��ͷ��ʾ '@' �ַ���������ʾ '*' �ַ�
        buffer[index].Char.AsciiChar = (i == 0) ? '@' : 'o';
        // ����ߴ����޵�״̬����ʾ��ͬ����ɫ����
        buffer[index].Attributes = (game->snake.invincible > 0) ? 0x0E : 0x0A;
    }

    // ����ʳ��
    for (int i = 0; i < MAX_FOOD; i++) {
        int x = game->foods[i].position.x + 1;
        int y = game->foods[i].position.y + 1;
        int index = y * (WIDTH + 2) + x;
        buffer[index].Char.AsciiChar = '$';
        // ����ʳ��������ʾ��ͬ����ɫ����
        buffer[index].Attributes =
            (game->foods[i].type == NORMAL) ? 0x0C :
            (game->foods[i].type == SPEED_UP) ? 0x09 : 0x0D;
    }

    // �����ϰ���
    for (int i = 0; i < game->obstacle_count; i++) {
        int x = game->obstacles[i].x + 1;
        int y = game->obstacles[i].y + 1;
        int index = y * (WIDTH + 2) + x;
        buffer[index].Char.AsciiChar = 'X';
        buffer[index].Attributes = 0x08;
    }

    // ������Ϸ��Ϣ����÷֡��ؿ����ٶȵ�
    char info[50];
    sprintf(info, "�÷�: %d  �Ѷ�: %d  �ٶ�: %d", game->score, game->level, 150 - game->speed + 50);
    for (int i = 0; info[i] != '\0'; i++) {
        int index = (HEIGHT + 2) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = info[i];
        buffer[index].Attributes = 0x07;
    }

    // �������˵������ʾ�û����ո���ͣ��Ϸ
    char instruction[] = "���¿ո�����ͣ��Ϸ.";
    for (int i = 0; instruction[i] != '\0'; i++) {
        int index = (HEIGHT + 3) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = instruction[i];
        buffer[index].Attributes = 0x07;
    }

    // ��Ӳ˵��е�ʳ��Ч��˵��
    char foodEffect1[] = "ʳ��Ч��:";
    for (int i = 0; foodEffect1[i] != '\0'; i++) {
        int index = (HEIGHT + 4) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = foodEffect1[i];
        buffer[index].Attributes = 0x07;
    }
    char foodEffect2[] = " - ��ͨʳ��: �ӷ֣�������Ч����";
    for (int i = 0; foodEffect2[i] != '\0'; i++) {
        int index = (HEIGHT + 5) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = foodEffect2[i];
        buffer[index].Attributes = 0x07;
    }
    char foodEffect3[] = " - ����ʳ�� (��ɫ): ������߼��١�";
    for (int i = 0; foodEffect3[i] != '\0'; i++) {
        int index = (HEIGHT + 6) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = foodEffect3[i];
        buffer[index].Attributes = 0x07;
    }
    char foodEffect4[] = " - �޵�ʳ�� (��ɫ): ������һ��ʱ���ڿ��Դ����ϰ��";
    for (int i = 0; foodEffect4[i] != '\0'; i++) {
        int index = (HEIGHT + 7) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = foodEffect4[i];
        buffer[index].Attributes = 0x07;
    }

    // ��Ӳ���˵��
    char controls1[] = "����˵��:";
    for (int i = 0; controls1[i] != '\0'; i++) {
        int index = (HEIGHT + 8) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = controls1[i];
        buffer[index].Attributes = 0x07;
    }
    char controls2[] = " - ʹ������̵ķ�����Կ����ƶ���";
    for (int i = 0; controls2[i] != '\0'; i++) {
        int index = (HEIGHT + 9) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = controls2[i];
        buffer[index].Attributes = 0x07;
    }
    char controls3[] = " - ��Ϸ����ʱ����R�����¿�ʼ��";
    for (int i = 0; controls3[i] != '\0'; i++) {
        int index = (HEIGHT + 10) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = controls3[i];
        buffer[index].Attributes = 0x07;
    }
    char controls4[] = " - ����Esc���˳���Ϸ��";
    for (int i = 0; controls4[i] != '\0'; i++) {
        int index = (HEIGHT + 11) * (WIDTH + 2) + i;
        buffer[index].Char.AsciiChar = controls4[i];
        buffer[index].Attributes = 0x07;
    }

    // �����Ϸ������ͣ״̬����ʾ��ͣ��ʾ��Ϣ
    if (game->paused) {
        char pauseMsg[] = "��Ϸ����ͣ�����¿ո�ʼ��";
        for (int i = 0; pauseMsg[i] != '\0'; i++) {
            int index = (HEIGHT + 12) * (WIDTH + 2) + i;
            buffer[index].Char.AsciiChar = pauseMsg[i];
            buffer[index].Attributes = 0x07;
        }
    }

    // �������������ʾ��Ϸ������ʾ��Ϣ
    if (game->snake.dead) {
        char gameOverMsg[] = "��Ϸ����������R���¿�ʼ��Esc�˳���Ϸ��";
        for (int i = 0; gameOverMsg[i] != '\0'; i++) {
            int index = (HEIGHT + 13) * (WIDTH + 2) + i;
            buffer[index].Char.AsciiChar = gameOverMsg[i];
            buffer[index].Attributes = 0x07;
        }
    }

    // ���������������������̨
    WriteConsoleOutput(hOut, buffer, bufferSize, bufferCoord, &rect);
    // ˢ�»�����
    FlushConsoleInputBuffer(hOut);
}

// �����û�����
void handle_input(Game* game) {
    // ����Ƿ��а�������
    if (_kbhit()) {
        // ��ȡ�û�����İ���
        int ch = _getch();
        // ����Ƿ���������ⰴ�������ȷ��� 0xE0���ٻ�ȡ���尴��ֵ
        if (ch == 0xE0) ch = _getch();
        // ��¼�µ��ƶ�����
        Direction newDir = game->snake.dir;
        switch (ch) {
            // �Ϸ�������������ƶ�
        case 72: newDir = UP; break;
            // �·�������������ƶ�
        case 80: newDir = DOWN; break;
            // ��������������ƶ�
        case 75: newDir = LEFT; break;
            // �ҷ�������������ƶ�
        case 77: newDir = RIGHT; break;
            // �ո������ͣ�������Ϸ
        case 32: game->paused = !game->paused; break;
            // ESC �����˳���Ϸ
        case 27: game->running = 0; break;
            // R ������Ϸ���������¿�ʼ��Ϸ
        case 'r':
        case 'R':
            if (game->snake.dead) {
                init_game(game);
            }
            break;
        }
        // ��ֹ�����������ƶ�
        if ((newDir == UP && game->snake.dir != DOWN) ||
            (newDir == DOWN && game->snake.dir != UP) ||
            (newDir == LEFT && game->snake.dir != RIGHT) ||
            (newDir == RIGHT && game->snake.dir != LEFT)) {
            game->snake.dir = newDir;
        }
    }
}

// ������Ϸ״̬
void update_game(Game* game) {
    // �����Ϸ��ͣ�����������������и���
    if (game->paused || game->snake.dead) return;
    // �����µ���ͷλ��
    Point newHead = game->snake.body[0];
    switch (game->snake.dir) {
    case UP: newHead.y--; break;
    case DOWN: newHead.y++; break;
    case LEFT: newHead.x--; break;
    case RIGHT: newHead.x++; break;
    }
    // �߽���ײ���
    if (newHead.x < 0 || newHead.x >= WIDTH || newHead.y < 0 || newHead.y >= HEIGHT) {
        game->snake.dead = 1;
        // ������Ϸ������Ч
        play_sound(3);
        return;
    }
    // �ϰ�����ײ���
    for (int i = 0; i < game->obstacle_count; i++) {
        if (newHead.x == game->obstacles[i].x && newHead.y == game->obstacles[i].y && !game->snake.invincible) {
            game->snake.dead = 1;
            // ������Ϸ������Ч
            play_sound(3);
            return;
        }
    }
    // ����ײ���
    for (int i = 1; i < game->snake.length; i++) {
        if (newHead.x == game->snake.body[i].x && newHead.y == game->snake.body[i].y && !game->snake.invincible) {
            game->snake.dead = 1;
            // ������Ϸ������Ч
            play_sound(3);
            return;
        }
    }
    int ateFood = 0;
    // ʳ����
    for (int i = 0; i < MAX_FOOD; i++) {
        if (newHead.x == game->foods[i].position.x && newHead.y == game->foods[i].position.y) {
            // ���ų�ʳ����Ч
            play_sound(1);
            // ����ʳ���������ӵ÷�
            game->score += (game->foods[i].type + 1) * 10;
            ateFood = 1;
            // ����ͬ����ʳ�������Ч��
            switch (game->foods[i].type) {
            case SPEED_UP:
                // ����
                game->speed = (game->speed > 50) ? game->speed - 20 : 50;
                break;
            case INVINCIBLE:
                // ����޵�״̬
                game->snake.invincible = 50;
                break;
            }
            // �����µ�ʳ��
            generate_food(game);
        }
    }
    if (!ateFood) {
        // δ�Ե�ʳ��ƶ�����
        for (int i = game->snake.length - 1; i > 0; i--) {
            game->snake.body[i] = game->snake.body[i - 1];
        }
    }
    else {
        if (game->snake.length < MAX_LENGTH) {
            // �Ե�ʳ���������
            game->snake.length++;
            for (int i = game->snake.length - 1; i > 0; i--) {
                game->snake.body[i] = game->snake.body[i - 1];
            }
        }
    }
    // ������ͷλ��
    game->snake.body[0] = newHead;
    // �ؿ��������
    if (game->score >= game->level * 100) {
        game->level++;
        // ���ݹؿ������ٶ�
        game->speed = 150 - (game->level * 10);
        if (game->speed < 50) game->speed = 50;
        // �����µ��ϰ���
        generate_obstacles(game);
    }
    // �޵�״̬��ʱ�ݼ�
    if (game->snake.invincible > 0) game->snake.invincible--;
}

// ����ʳ��
void generate_food(Game* game) {
    for (int i = 0; i < MAX_FOOD; i++) {
        int valid;
        do {
            valid = 1;
            // �������ʳ���λ��
            game->foods[i].position.x = rand() % WIDTH;
            game->foods[i].position.y = rand() % HEIGHT;
            // �������ʳ�������
            game->foods[i].type = rand() % 3;
            // ���ʳ���Ƿ��������ص�
            for (int j = 0; j < game->snake.length; j++) {
                if (game->snake.body[j].x == game->foods[i].position.x && game->snake.body[j].y == game->foods[i].position.y) {
                    valid = 0;
                    break;
                }
            }
            // ���ʳ���Ƿ����ϰ����ص�
            for (int j = 0; j < game->obstacle_count; j++) {
                if (game->obstacles[j].x == game->foods[i].position.x && game->obstacles[j].y == game->foods[i].position.y) {
                    valid = 0;
                    break;
                }
            }
            // ���ʳ���Ƿ�������ʳ���ص�
            for (int j = 0; j < i; j++) {
                if (game->foods[j].position.x == game->foods[i].position.x && game->foods[j].position.y == game->foods[i].position.y) {
                    valid = 0;
                    break;
                }
            }
        } while (!valid);
    }
}

// �����ϰ���
void generate_obstacles(Game* game) {
    // ��ʼ���ϰ�������Ϊ 0
    game->obstacle_count = 0;
    // ��ʼ�����Դ���Ϊ 0
    int attempts = 0;
    // ѭ�������ϰ��ֱ���ﵽ����������߳��Դ����ﵽ����
    while (game->obstacle_count < 20 && attempts < 1000) {
        // �������һ���µ��ϰ���λ�ã���Χ����Ϸ�����ڲ��ʵ�����ƫ��
        Point newObstacle = { rand() % (WIDTH - 4) + 2, rand() % (HEIGHT - 4) + 2 };
        // ������ϰ����Ƿ�����̫��
        int tooClose = 0;
        // ������ϰ����Ƿ�����̫��
        for (int j = 0; j < game->snake.length; j++) {
            // �������ϰ���������ĳ���ֵ� x �����
            int dx = newObstacle.x - game->snake.body[j].x;
            // �������ϰ���������ĳ���ֵ� y �����
            int dy = newObstacle.y - game->snake.body[j].y;
            // �������С�� 25��������ƽ�����жϾ��룩������Ϊ̫��
            if (dx * dx + dy * dy < 25) {
                tooClose = 1;
                break;
            }
        }
        // ������ϰ����Ƿ��Ѿ�����
        int exists = 0;
        // ������ϰ����Ƿ��Ѿ����������е��ϰ����б���
        for (int j = 0; j < game->obstacle_count; j++) {
            if (newObstacle.x == game->obstacles[j].x && newObstacle.y == game->obstacles[j].y) {
                exists = 1;
                break;
            }
        }
        // ������ϰ���Ȳ������ߣ�Ҳ�������������б��У�����ӵ��ϰ����б�
        if (!tooClose && !exists) {
            game->obstacles[game->obstacle_count++] = newObstacle;
        }
        // ���Դ����� 1
        attempts++;
    }
}

// ������Ч
void play_sound(int type) {
    switch (type) {
    case 1:
        // �򵥴�ӡ��ʾ��Ϣ�����滻Ϊʵ�ʵ���Ч���Ŵ���
        printf("Eat food sound!\n");
        break;
    case 3:
        // �򵥴�ӡ��ʾ��Ϣ�����滻Ϊʵ�ʵ���Ч���Ŵ���
        printf("Game over sound!\n");
        break;
    default:
        break;
    }
}