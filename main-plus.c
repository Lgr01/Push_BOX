/***********************************************************
 * 文件名称    : main-plus.c
 * 作者        : NanGuiyan
 * 创建日期    : 2025/07/31
 * 功能描述    : 推箱子代码的规范版本。（用来养成良好的编程风格）,并开始学习如何从零完成一个完整的项目
 * 版权声明    : Copyright (c) 2025 your@email.com All Rights Reserved
 ***********************************************************/

 #include <stdio.h>
 #include <stdlib.h>

/* 定义常量 */
#define MAP_ROWS 8
#define MAP_COLS 8
#define MOVE_MAX 100

/* 跨平台输入处理 */
#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

 /* 游戏状态枚举 */
 typedef enum
 {
    GAME_CONTINUE,
    GAME_WIN,
    GAME_QUIT
 }GameState;

 /* 移动枚举方向 */
 typedef enum
 {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
 }MoveDirection;

 /* 地图元素枚举 */
 enum MapObjects
 {
    SPACE = 0,          //空地
    WALL = 1,           //墙壁
    TARGET = 3,         //目的地
    BOX = 4,            //箱子
    PLAYER = 6,         //人物
    BOX_TARGET = 7,     //箱子在目的地
    PLAYER_TARGET = 9   //人物在目的地

 };


/**
 *  初始化游戏地图
 *  @param map 地图指针
 */
void init_game_map(int map[MAP_ROWS][MAP_COLS])
{
    int initial[MAP_ROWS][MAP_COLS] = {
        {0,0,1,1,1,0,0,0},
        {0,0,1,3,1,0,0,0},
        {0,0,1,0,1,1,1,1},
        {1,1,1,4,0,4,3,1},
        {1,3,0,4,6,1,1,1},  // 玩家初始位置(4,4)，值为6（PLAYER）
        {1,1,1,1,4,1,0,0},
        {0,0,0,1,3,1,0,0},
        {0,0,0,1,1,1,0,0}
    };

    //复制初始地图
    for(int i = 0; i < MAP_ROWS; i++){
        for(int j = 0; j < MAP_COLS; j++)
        {
            map[i][j] = initial[i][j];
        }
    }

}

/**
 * 查找玩家位置：把玩家的行列坐标存入row和col
 * @param map   游戏地图
 * @param row   返回行坐标 
 * @param col   返回列坐标
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
    //玩家未找到处理
    *row = -1;
    *col = -1;

}

/**
 * 绘制游戏地图
 */
void draw_map(const int map[MAP_ROWS][MAP_COLS])
{
    printf("\n");
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            switch (map[i][j]) {
                case SPACE:         printf("  "); break;  // 空地
                case WALL:          printf("■ "); break;  // 墙壁
                case TARGET:        printf("○ "); break;  // 目的地
                case BOX:           printf("□ "); break;  // 箱子
                case PLAYER:        printf("♀ "); break;  // 玩家
                case PLAYER_TARGET: printf("♀ "); break;  // 玩家在目标
                case BOX_TARGET:    printf("★ "); break;  // 箱子在目标
                default:            printf("? "); break;  // 错误状态
            }
        }
        printf("\n");
    }
}


/**
 * 检查游戏胜利：判断箱子是否都在目的地
 * @param map:游戏地图
 * @return 1=游戏胜利 0=游戏未胜利
 */
int is_game_won(const int map[MAP_ROWS][MAP_COLS])
{
    for(int i = 0; i < MAP_ROWS; i++){
        for(int j = 0; j < MAP_COLS; j++){
            //如果存在箱子在空地，则游戏未结束
            if(map[i][j] == BOX){
                return 0;
            }
        }
    }
    return 1;
}


/**
 * 清屏函数（跨平台）
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
 * 打印游戏标题
 */
void print_title()
{
    clear_screen();
    printf("==================================\n");
    printf("           推箱子游戏1.0           \n");
    printf("==================================\n");
    printf("控制:W(上),S(下),A(左),D(右)\n");
}
/**
 * 跨平台获取字符
 * @return  获取的字符 
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
 * 处理输入并移动：接受按键，更新玩家/箱子位置，返回游戏状态
 * @param map 游戏地图
 * @param player_row 玩家所在行
 * @param player_col 玩家所在列
 * @param dir 移动方向
 * @return 0 = 移动失败 1 = 移动成功 
 */
int move_player(int map[MAP_ROWS][MAP_COLS], int *player_row, int *player_col, MoveDirection dir)
{
    //计算偏移
    int dr = 0, dc = 0;

    switch(dir){
        case DIR_UP:        dr = -1; break;
        case DIR_DOWN:      dr = +1; break;
        case DIR_LEFT:      dc = -1; break;
        case DIR_RIGHT:     dc = +1; break;
    }
    int player_new_row = *player_row + dr;
    int player_new_col = *player_col + dc;

    //边界检查
    if(player_new_col < 0 || player_new_col >= MAP_COLS || player_new_row < 0 || player_new_row >= MAP_ROWS){
        return 0; //移动失败
    }

    int *current = &map[*player_row][*player_col];
    int *next = &map[player_new_row][player_new_col];
    int *next_next = (player_new_col + dc >= 0 && player_new_col + dc < MAP_COLS &&
         player_new_row + dr >= 0 && player_new_row + dr< MAP_ROWS) ?
          &map[player_new_row + dr][player_new_col + dc] : NULL;
    
    //1.情况一：移动到目的地或空地
    if(SPACE == *next || TARGET == *next){
        //保留原始数据（空地或者时目的地）
        int original_value = (*current == PLAYER) ? SPACE : TARGET;

        *current = original_value;
        *next = (*next == SPACE) ? PLAYER : PLAYER_TARGET;

        *player_col = player_new_col;
        *player_row = player_new_row;
        return 1;
    }
    //2.情况二、推动箱子
    else if((BOX == *next || BOX_TARGET == *next) && next_next && (*next_next == SPACE || *next_next == TARGET)){
        //检查箱子能否被推动
        if(SPACE == *next_next || TARGET == *next_next){
            //保留原始数据
            int player_original = (*current == PLAYER) ? SPACE : TARGET;
            int box_original = (*next_next == SPACE) ? BOX : BOX_TARGET;

            //移动箱子
            *next_next = box_original;

            //移动玩家
            *next = (*next == BOX) ? PLAYER : PLAYER_TARGET;
            *current = player_original;

            *player_row = player_new_row;
            *player_col = player_new_col;
            return 1;
        }
    }
    return 0; //移动失败

}

/**
 * 游戏主循环
 * @param map 游戏地图
 */
GameState game_loop(int map[MAP_ROWS][MAP_COLS])
{   
    int player_row, player_col;
    int move_count = 0;

    while(move_count < MOVE_MAX){
        find_player(map, &player_row, &player_col);

        print_title();
        printf("移动次数:%d\n",move_count);
        printf("玩家位置：[%d,%d]\n",player_row,player_col);
        draw_map(map);

        int input = getch();
        int moved = 0;
        MoveDirection dir;
        
        //处理输入
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

            //检查游戏是否胜利
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
            printf("     恭喜！您成功完成了游戏！     \n");
            printf("==================================\n\n");
            draw_map(map);
            printf("\n");
            break;

        case GAME_QUIT:
            printf("游戏已退出！\n");
            break;

        case GAME_CONTINUE:
            printf("游戏结束（移动次数到达上限）\n");
            break;

    }
}

 int main()
 {
    int game_map[MAP_ROWS][MAP_COLS];
    int player_row, player_col;

    //1.初始化游戏
    init_game_map(game_map); //初始化地图
    find_player(game_map, &player_row, &player_col); //找玩家

    //2.游戏主循环
    GameState state = game_loop(game_map);
    
    //3.处理游戏状态
    print_result(game_map, state);

    //3.结束处理
    printf("\n感谢游玩！按任意键退出...\n");
    getch();
    return 0;
 }