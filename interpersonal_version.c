#include <stdio.h>
#include <string.h>
 
#define BOARD_SIZE 8                        //ボードサイズ
#define BOARD_H "|"                         //ボードの縦パーツ
#define BOARD_W "-"                         //ボードの横パーツ1
#define BOARD_W2 " "                        //ボードの横パーツ2
#define DEFAULT "\x1b[0m"                   //デフォルト色
#define MAGENTA "\x1b[35m"                  //マゼンダ
#define CYAN "\x1b[36m"                     //シアン
#define BACK_GREEN "\x1b[42m"               //背景緑
#define C_BLACK "\x1b[30m"                  //黒
#define C_WHITE "\x1b[37m"                  //白
#define BOLD "\x1b[1m"                      //太字
 
//-----プロトタイプ宣言-----
int new_board(int board[][8]);
int set_board(int board[][8]);
int input(int turn, int board[][8]);
int put_judge_vec(int x, int y, int turn, int board[][8]);
int put_judge(int x, int y, int turn, int i, int j, int board[][8]);
int turn_vec(int x, int y, int turn, int board[][8]);
void turnover(int x, int y, int i, int j, int turn, int board[][8]);
int end_judge(int turn, int board[][8]);
void result(int rupt, int board[][8]);
int again(void);
 
enum stone {
    BLANK,                                  //空き
    BLACK,                                  //黒石
    WHITE,                                  //白石
};
 
//======================================================
//main関数
//
//  endが終了判定、ruptが中断判定
//======================================================
int main(void)
{
    int board[BOARD_SIZE][BOARD_SIZE], turn = 0, end = 0, rupt = 0;
     
    new_board(board);
     
    while (end == 0 && rupt == 0) {
        set_board(board);
         
        if (turn == 0) printf("Bの番です。\n");
        else printf("Wの番です。\n");
         
        rupt = input(turn, board);
        turn = (turn + 1) % 2;
         
        switch (end_judge(turn, board)) {
            case 1:
                if (turn == 0) printf("Bの置く場所が無いので、Wの番です。\n");
                else printf("Wの置く場所が無いので、Bの番です。\n");
                turn = (turn + 1) % 2;
                break;
                 
            case 2:
                result(rupt, board);
                end = again();
                 
                if (end == 0) {
                    new_board(board);
                    turn = 0;
                }
                break;
                 
            default:
                break;
        }
        if (rupt == 1) result(rupt, board);
    }
    return 0;
}
 
//=======================================================
//ボードの初期化
//=======================================================
int new_board(int board[][8])
{
    int i, j;
     
    for (i = 0; i < BOARD_SIZE; i++)    //初期化
        for (j = 0; j < BOARD_SIZE; j++) board[i][j] = BLANK;
     
    board[3][3] = WHITE;                //----------------------
    board[3][4] = BLACK;                //黒石、白石を初期位置へ
    board[4][3] = BLACK;                //
    board[4][4] = WHITE;                //----------------------
     
    return 0;
}
 
//========================================================
//ボードの描写
//========================================================
int set_board(int board[][8])
{
    int i, j, k;
     
    printf("\n\n ");
     
    //-----A ~ Hの描写-----
    for (i = 0; i < BOARD_SIZE; i++) {
        printf("%2s%s%s%2c", BOARD_H, BOLD, MAGENTA, i + 65);
        printf(DEFAULT);
    }
    printf("%2s\n", BOARD_H);
     
    for (i = 0; i < BOARD_SIZE; i++) {
         
        //-----ボードの横パーツの描写-----
        for (j = 0; j < BOARD_SIZE * 4 + 4; j++) {
            if (i == 0 || j < 3 || j > BOARD_SIZE * 4) {
                if (j % 2 == 0) printf("%s", BOARD_W);
                else printf("%s", BOARD_W2);
                 
            }else {
                if (j % 2 == 0) printf("%s%s", BACK_GREEN, BOARD_W);
                else printf("%s%s", BACK_GREEN, BOARD_W2);
                printf(DEFAULT);
            }
        }
         
        //-----数字（1 ~ 8)までの描写-----
        printf("\n%s%s%d", CYAN, BOLD, i + 1);
        printf(DEFAULT);
        printf("%2s", BOARD_H);
         
        //-----空き、黒石、白石の描写-----
        for (j = 0; j < BOARD_SIZE; j++){
            switch(board[i][j]) {
                case BLANK:                         //空きの描写
                    if (j == BOARD_SIZE - 1) {
                        printf("%s%2s", BACK_GREEN, " ");
                        printf(DEFAULT);
                        printf("%2s", BOARD_H);
                         
                    }else {
                        printf("%s%2s%2s", BACK_GREEN, " ", BOARD_H);
                        printf(DEFAULT);
                    }
                    break;
                     
                case BLACK:                         //黒石の描写
                    if (j == BOARD_SIZE - 1) {
                        printf("%s%s%s%2s", C_BLACK, BACK_GREEN, BOLD, "B");
                        printf(DEFAULT);
                        printf("%2s", BOARD_H);
                         
                    }else {
                        printf("%s%s%s%2s", C_BLACK, BACK_GREEN, BOLD, "B");
                        printf(DEFAULT);
                        printf("%s%2s", BACK_GREEN, BOARD_H);
                        printf(DEFAULT);
                    }
                    break;
                     
                case WHITE:                         //白石の描写
                    if (j == BOARD_SIZE - 1) {
                        printf("%s%s%s%2s", C_WHITE, BACK_GREEN, BOLD, "W");
                        printf(DEFAULT);
                        printf("%2s", BOARD_H);
                         
                    }else {
                        printf("%s%s%s%2s", C_WHITE, BACK_GREEN, BOLD, "W");
                        printf(DEFAULT);
                        printf("%s%2s", BACK_GREEN, BOARD_H);
                        printf(DEFAULT);
                    }
                    break;
                     
                default:                            //エラー処理
                    printf("E");
                    break;
            }
        }
        printf("\n");
    }
     
    //-----1番下のボードの横パーツ描写-----
    for (i = 0; i < BOARD_SIZE * 4 + 4; i++) {
        if (i % 2 == 0) printf("%s", BOARD_W);
        else printf("%s", BOARD_W2);
    }
    printf("\n");
     
    return 0;
}
 
//=============================================================
//ユーザからの入力を受け取る
//=============================================================
int input(int turn, int board[][8])
{
    int y;
    char x, posi[5];
     
    printf("石を置く場所を %s横%s縦%s(例:%sA%s1%s) のように入力して下さい。\n"
           "終了する場合は N を入力して下さい。\n",
           MAGENTA, CYAN, DEFAULT, MAGENTA, CYAN, DEFAULT);
     
    while (1) {
        fgets(posi, sizeof(posi), stdin);
        sscanf(posi, "%c%d", &x, &y);
         
        if (x == 'N' || x == 'n') return 1;
        else if (x >= 65 && x <= 72 && y >= 1 && y <= 8) {
            if (put_judge_vec(x - 65, y - 1, turn, board) == 1) {
                turn_vec(x - 65, y - 1, turn, board);
                break;
 
            }else {
                printf("ひっくり返せる石がありません。もう一度入力して下さい。\n");
                if (posi[strlen(posi) - 1] != '\n') while (getchar() != '\n');      //バッファクリア
            }
             
        }else if (x >= 97 && x <= 104 && y >= 1 && y <= 8) {
            if (put_judge_vec(x - 97, y - 1, turn, board) == 1) {
                turn_vec(x - 97, y - 1, turn, board);
                break;
 
            }else {
                printf("ひっくり返せる石がありません。もう一度入力して下さい。\n");
                if (posi[strlen(posi) - 1] != '\n') while (getchar() != '\n');      //バッファクリア
            }
             
        }else {
            printf("値が不正です。もう一度入力して下さい。\n");
            if (posi[strlen(posi) - 1] != '\n') while (getchar() != '\n');          //バッファクリア
        }
    }
    return 0;
}
 
//==================================================================
//全方向の石を調べてひっくり返せるか判定
//
//  受け取ったx座標、y座標の全方向の石を調べる
//  i, jが方向をあらわす
//==================================================================
int put_judge_vec(int x, int y, int turn, int board[][8])
{
    int i, j;
 
    if (board[y][x] != BLANK) return 0;
     
    for (i = -1; i < 2; i++)
        for (j = -1; j < 2; j++)
            if (put_judge(x, y, turn, i, j, board) == 1) return 1;
     
    return 0;
}
 
//==============================================================
//一方向の石を調べてひっくり返せるか判定
//
//  x座標とy座標からの受け取ったi, jの一方向の石を調べる
//==============================================================
int put_judge(int x, int y, int turn, int i, int j, int board[][8])
{
    int flag = 0;
     
    while (1) {
        x += j;
        y += i;
         
        if (x < 0 || y < 0 || x > BOARD_SIZE - 1 || y > BOARD_SIZE - 1
            || board[y][x] == BLANK)
            return 0;
         
        else if (board[y][x] == (turn ? BLACK : WHITE)) {
            flag = 1;
            continue;
        }
         
        if (flag == 1) break;
         
        return 0;
    }
    return 1;
}
 
//===============================================================
//石を置き、周りのひっくり返せる石をひっくり返す
//
//  i, jが方向をあらわす
//===============================================================
int turn_vec(int x, int y, int turn, int board[][8])
{
    int i, j, flag;
     
    if (board[y][x] != BLANK) return 0;     //x座標、y座標が空きか判定
     
    //-----全方向について調べ、石をひっくり返す-----
    for (i = -1; i < 2; i++) {
        for (j = -1; j < 2; j++) {
             
            if (i == 0 && j == 0) continue;
            else if (put_judge(x, y, turn, i, j, board) == 1) {
                turnover(x, y, i, j, turn, board);
                flag = 1;
            }
        }
    }
     
    //-----石を置く-----
    if (flag == 1) {
        board[y][x] = (turn ? WHITE : BLACK);
        return 1;
    }
    return 0;
}
 
//===============================================================
//石をひっくり返す
//===============================================================
void turnover(int x, int y, int i, int j, int turn, int board[][8])
{
    while (1) {
        x += j;
        y += i;
         
        if (board[y][x] == (turn ? WHITE : BLACK)) break;
        else board[y][x] = (turn ? WHITE : BLACK);
    }
}
 
//================================================================
//終了判定
//================================================================
int end_judge(int turn, int board[][8])
{
    int i, j;
     
    //-----ターン者の置く場所があるかどうか-----
    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 0; j < BOARD_SIZE; j++)
            if (board[i][j] == BLANK && put_judge_vec(j, i, turn, board) == 1)
                return 0;
     
    turn = (turn + 1) % 2;      //ターン者交代
     
    //-----もう一人の置く場所があるかどうか-----
    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 0; j < BOARD_SIZE; j++)
            if (board[i][j] == BLANK && put_judge_vec(j, i, turn, board) == 1)
                return 1;
     
    return 2;
}
 
//====================================================================
//ゲーム結果出力
//
//  bが黒石の数、wが白石の数
//====================================================================
void result(int rupt, int board[][8])
{
     
    int i, j, b = 0, w = 0;
     
    //-----全部の石を数える-----
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
             
            switch (board[i][j]) {
                case BLACK:
                    b++;
                    break;
                     
                case WHITE:
                    w++;
                    break;
                     
                default:
                    break;
            }
        }
    }
    set_board(board);
     
    //-----結果の判定、出力-----
    if (rupt == 1) printf("ゲームが中断されました。\n\n");
    else if (b > w) {
        printf("%s%sGAME SET%s\n", "\x1b[5;35m", BOLD, DEFAULT);
        printf("B = %d個, W = %d個\nでBの勝ちです。\n\n", b, w);
         
    }else if (w > b) {
        printf("%s%sGAME SET%s\n", "\x1b[5;35m", BOLD, DEFAULT);
        printf("B = %d個, W = %d個\nでWの勝ちです。\n\n", b, w);
         
    }else {
        printf("%s%sGAME SET%s\n", "\x1b[5;35m", BOLD, DEFAULT);
        printf("B = %d個, W = %d個\nで引き分けです。\n\n", b, w);
    }
}
 
//=====================================================================
//コンティニュー画面
//=====================================================================
int again(void)
{
    char aga[8];
     
    do {
        printf("もう一度遊ぶ場合は %sY%s を入力して下さい。\n"
               "終了する場合は %sN%s を入力して下さい。\n", MAGENTA, DEFAULT, CYAN, DEFAULT);
         
        fgets(aga, sizeof(aga), stdin);
         
        if (strcmp(aga, "Y\n") != 0 && strcmp(aga, "y\n") != 0
            && strcmp(aga, "N\n") != 0 && strcmp(aga, "n\n") != 0)
            printf("値が不正です。もう一度入力して下さい。\n");
         
    }while (strcmp(aga, "Y\n") != 0 && strcmp(aga, "y\n") != 0
            && strcmp(aga, "N\n") != 0 && strcmp(aga, "n\n") != 0);
     
    if (strcmp(aga, "Y\n") == 0 || strcmp(aga, "y\n") == 0)
        return 0;
     
    return 1;
}