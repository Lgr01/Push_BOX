/***********************************************************
 * �ļ�����    : main-plus.c
 * ����        : NanGuiyan
 * ��������    : 2025/07/31
 * ��������    : �����Ӵ���Ĺ淶�汾���������������õı�̷��,����ʼѧϰ��δ������һ����������Ŀ
 * ��Ȩ����    : Copyright (c) 2025 your@email.com All Rights Reserved
 ***********************************************************/

 #include <stdio.h>
 #include <stdlib.h>

/* ���峣�� */
#define MAP_ROWS 8
#define MAP_COLS 8
#define MOVE_MAX 100

/* ��ƽ̨���봦�� */
#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

 /* ��Ϸ״̬ö�� */
 typedef enum
 {
    GAME_CONTINUE,
    GAME_WIN,
    GAME_QUIT
 }GameState;

 /* �ƶ�ö�ٷ��� */
 typedef enum
 {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
 }MoveDirection;

 /* ��ͼԪ��ö�� */
 enum MapObjects
 {
    SPACE = 0,          //�յ�
    WALL = 1,           //ǽ��
    TARGET = 3,         //Ŀ�ĵ�
    BOX = 4,            //����
    PLAYER = 6,         //����
    BOX_TARGET = 7,     //������Ŀ�ĵ�
    PLAYER_TARGET = 9   //������Ŀ�ĵ�

 };


/**
 *  ��ʼ����Ϸ��ͼ
 *  @param map ��ͼָ��
 */
void init_game_map(int map[MAP_ROWS][MAP_COLS])
{
    int initial[MAP_ROWS][MAP_COLS] = {
        {0,0,1,1,1,0,0,0},
        {0,0,1,3,1,0,0,0},
        {0,0,1,0,1,1,1,1},
        {1,1,1,4,0,4,3,1},
        {1,3,0,4,6,1,1,1},  // ��ҳ�ʼλ��(4,4)��ֵΪ6��PLAYER��
        {1,1,1,1,4,1,0,0},
        {0,0,0,1,3,1,0,0},
        {0,0,0,1,1,1,0,0}
    };

    //���Ƴ�ʼ��ͼ
    for(int i = 0; i < MAP_ROWS; i++){
        for(int j = 0; j < MAP_COLS; j++)
        {
            map[i][j] = initial[i][j];
        }
    }

}

/**
 * �������λ�ã�����ҵ������������row��col
 * @param map   ��Ϸ��ͼ
 * @param row   ���������� 
 * @param col   ����������
 */
void find_player(const int map[MAP_ROWS][MAP_COLS], int* row, int* col)
{
    for(int i = 0; i < MAP_ROWS; i++){
        for(int j = 0; j < MAP_COLS; j++){
            if(map[i][j] == PLAYER || map[i][j] == PLAYER_TARGET){
                *row = i;
                *col = j;
                return;
            }
        }
    }
    //���δ�ҵ�����
    *row = -1;
    *col = -1;

}

/**
 * ������Ϸ��ͼ
 */
void draw_map(const int map[MAP_ROWS][MAP_COLS])
{
    printf("\n");
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            switch (map[i][j]) {
                case SPACE:         printf("  "); break;  // �յ�
                case WALL:          printf("�� "); break;  // ǽ��
                case TARGET:        printf("�� "); break;  // Ŀ�ĵ�
                case BOX:           printf("�� "); break;  // ����
                case PLAYER:        printf("�� "); break;  // ���
                case PLAYER_TARGET: printf("�� "); break;  // �����Ŀ��
                case BOX_TARGET:    printf("�� "); break;  // ������Ŀ��
                default:            printf("? "); break;  // ����״̬
            }
        }
        printf("\n");
    }
}


/**
 * �����Ϸʤ�����ж������Ƿ���Ŀ�ĵ�
 * @param map:��Ϸ��ͼ
 * @return 1=��Ϸʤ�� 0=��Ϸδʤ��
 */
int is_game_won(const int map[MAP_ROWS][MAP_COLS])
{
    for(int i = 0; i < MAP_ROWS; i++){
        for(int j = 0; j < MAP_COLS; j++){
            //������������ڿյأ�����Ϸδ����
            if(map[i][j] == BOX){
                return 0;
            }
        }
    }
    return 1;
}


/**
 * ������������ƽ̨��
 */
void clear_screen(void)
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

}

/**
 * ��ӡ��Ϸ����
 */
void print_title()
{
    clear_screen();
    printf("==================================\n");
    printf("           ��������Ϸ1.0           \n");
    printf("==================================\n");
    printf("����:W(��),S(��),A(��),D(��)\n");
}
/**
 * ��ƽ̨��ȡ�ַ�
 * @return  ��ȡ���ַ� 
 */
int getch(void)
{
    #ifdef _WIN32
        return _getch();
    #else
        struct termios oldt, newt;
        int ch;
        
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    #endif
}

/**
 * �������벢�ƶ������ܰ������������/����λ�ã�������Ϸ״̬
 * @param map ��Ϸ��ͼ
 * @param player_row ���������
 * @param player_col ���������
 * @param dir �ƶ�����
 * @return 0 = �ƶ�ʧ�� 1 = �ƶ��ɹ� 
 */
int move_player(int map[MAP_ROWS][MAP_COLS], int *player_row, int *player_col, MoveDirection dir)
{
    //����ƫ��
    int dr = 0, dc = 0;

    switch(dir){
        case DIR_UP:        dr = -1; break;
        case DIR_DOWN:      dr = +1; break;
        case DIR_LEFT:      dc = -1; break;
        case DIR_RIGHT:     dc = +1; break;
    }
    int player_new_row = *player_row + dr;
    int player_new_col = *player_col + dc;

    //�߽���
    if(player_new_col < 0 || player_new_col >= MAP_COLS || player_new_row < 0 || player_new_row >= MAP_ROWS){
        return 0; //�ƶ�ʧ��
    }

    int *current = &map[*player_row][*player_col];
    int *next = &map[player_new_row][player_new_col];
    int *next_next = (player_new_col + dc >= 0 && player_new_col + dc < MAP_COLS &&
         player_new_row + dr >= 0 && player_new_row + dr< MAP_ROWS) ?
          &map[player_new_row + dr][player_new_col + dc] : NULL;
    
    //1.���һ���ƶ���Ŀ�ĵػ�յ�
    if(SPACE == *next || TARGET == *next){
        //����ԭʼ���ݣ��յػ���ʱĿ�ĵأ�
        int original_value = (*current == PLAYER) ? SPACE : TARGET;

        *current = original_value;
        *next = (*next == SPACE) ? PLAYER : PLAYER_TARGET;

        *player_col = player_new_col;
        *player_row = player_new_row;
        return 1;
    }
    //2.��������ƶ�����
    else if((BOX == *next || BOX_TARGET == *next) && next_next && (*next_next == SPACE || *next_next == TARGET)){
        //��������ܷ��ƶ�
        if(SPACE == *next_next || TARGET == *next_next){
            //����ԭʼ����
            int player_original = (*current == PLAYER) ? SPACE : TARGET;
            int box_original = (*next_next == SPACE) ? BOX : BOX_TARGET;

            //�ƶ�����
            *next_next = box_original;

            //�ƶ����
            *next = (*next == BOX) ? PLAYER : PLAYER_TARGET;
            *current = player_original;

            *player_row = player_new_row;
            *player_col = player_new_col;
            return 1;
        }
    }
    return 0; //�ƶ�ʧ��

}

/**
 * ��Ϸ��ѭ��
 * @param map ��Ϸ��ͼ
 */
GameState game_loop(int map[MAP_ROWS][MAP_COLS])
{   
    int player_row, player_col;
    int move_count = 0;

    while(move_count < MOVE_MAX){
        find_player(map, &player_row, &player_col);

        print_title();
        printf("�ƶ�����:%d\n",move_count);
        printf("���λ�ã�[%d,%d]\n",player_row,player_col);
        draw_map(map);

        int input = getch();
        int moved = 0;
        MoveDirection dir;
        
        //��������
        switch(input){
            case 'w' : case 'W' : dir = DIR_UP;     moved = move_player(map, &player_row, &player_col, dir); break;
            case 's' : case 'S' : dir = DIR_DOWN;   moved = move_player(map, &player_row, &player_col, dir); break;
            case 'a' : case 'A' : dir = DIR_LEFT;   moved = move_player(map, &player_row, &player_col, dir); break;
            case 'd' : case 'D' : dir = DIR_RIGHT;   moved = move_player(map, &player_row, &player_col, dir); break;
            case 'q' : case 'Q' : return GAME_QUIT;
            default : break;
        }
        if(moved){
            move_count++;

            //�����Ϸ�Ƿ�ʤ��
            if(is_game_won(map)){
                return GAME_WIN;
            }
        }

    }
    return GAME_CONTINUE;

    
}
/**
 * 
 */
void print_result(const int map[MAP_ROWS][MAP_COLS], GameState state)
{   
    switch(state){
        case GAME_WIN:
            printf("==================================\n");
            printf("     ��ϲ�����ɹ��������Ϸ��     \n");
            printf("==================================\n\n");
            draw_map(map);
            printf("\n");
            break;

        case GAME_QUIT:
            printf("��Ϸ���˳���\n");
            break;

        case GAME_CONTINUE:
            printf("��Ϸ�������ƶ������������ޣ�\n");
            break;

    }
}

 int main()
 {
    int game_map[MAP_ROWS][MAP_COLS];
    int player_row, player_col;

    //1.��ʼ����Ϸ
    init_game_map(game_map); //��ʼ����ͼ
    find_player(game_map, &player_row, &player_col); //�����

    //2.��Ϸ��ѭ��
    GameState state = game_loop(game_map);
    
    //3.������Ϸ״̬
    print_result(game_map, state);

    //3.��������
    printf("\n��л���棡��������˳�...\n");
    getch();
    return 0;
 }