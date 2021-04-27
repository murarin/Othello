#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
   
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
int person_main(char mode);
int cpu_main(char mode);
int new_board(int board[][BOARD_SIZE]);
int set_board(int board[][BOARD_SIZE]);
int input(int turn, int board[][BOARD_SIZE]);
int put_judge_vec(int x, int y, int turn, int board[][BOARD_SIZE]);
int put_judge(int x, int y, int turn, int i, int j, int board[][BOARD_SIZE]);
int turn_vec(int x, int y, int turn, int board[][BOARD_SIZE]);
void turnover(int x, int y, int i, int j, int turn, int board[][BOARD_SIZE]);
int end_judge(int turn, int board[][BOARD_SIZE]);
void result(int rupt, int board[][BOARD_SIZE], char mode, char be_person[], char af_person[]);
int again(void);
void random_ai(int turn, int board[][BOARD_SIZE], int *cpu_x, int *cpu_y);
void alpha_beta_ai(int turn, int board[][BOARD_SIZE], int *cpu_x, int *cpu_y);
int re_high_val(int put_ok[][2], int count, int turn, int board0[][BOARD_SIZE], int beta, int flag);
int re_val(int board[][BOARD_SIZE], int turn);
int put_search(int put_ok[][2], int turn, int board[][BOARD_SIZE]);
  
enum stone {
    BLANK,                                  //空き
    BLACK,                                  //黒石
    WHITE,                                  //白石
};
   
//======================================================
//main関数
//  1P vs 2P または 1P vs CPU のモード選択
//
//  endは終了判定
//======================================================
int main(void)
{
    int end = 0;
    char mode[8];
  
    while (end == 0) {
        printf("\nモードを数字で入力してください。\n"
                "%s1P vs 2P%sで遊ぶ場合は %s0%s を、%s1P vs CPU%sで遊ぶ場合は %s1%s を入力してください。\n"
                 "終了する場合は N を入力してください。\n", 
                  CYAN, DEFAULT, CYAN, DEFAULT, MAGENTA, DEFAULT, MAGENTA, DEFAULT);
  
        do {
            fgets(mode, sizeof(mode), stdin);
  
            if (strcmp(mode, "0\n") != 0 && strcmp(mode, "1\n") != 0
                && strcmp(mode, "n\n") != 0 && strcmp(mode, "N\n") != 0) {
                printf("値が不正です。もう一度入力してください。\n");
                if (mode[strlen(mode) - 1] != '\n') while (getchar() != '\n');  //バッファクリア
  
            }else break;
  
        }while (1);
  
        if (mode[0] == '0') end = person_main(mode[0]);
        else if (mode[0] == '1') end = cpu_main(mode[0]);
        else {
            printf("終了します。\n\n");
            break;
        }
    }
    return 0;
}
  
//===========================================
//1P vs 2Pのmain関数
//
//  ruptが中断判定、endが終了判定
//===========================================
int person_main(char mode)
{
    //------------------------------------------
    //変数の宣言
    //  ボード、ターン、置く場所が無いかどうか
    //------------------------------------------
    int board[BOARD_SIZE][BOARD_SIZE], turn = 0, no_put = 0;
  
    new_board(board);
       
    while (1) {
  
        set_board(board);
  
        if (no_put == 1) printf("Bの置く場所がありません。\n");
        else if (no_put == 2) printf("Wの置く場所がありません。\n");
        no_put = 0;
  
        if (turn == 0) printf("Bの番です。\n");
        else printf("Wの番です。\n");
           
        if (input(turn, board) == 1) {
            result(1, board, mode, NULL, NULL);
            return 1;
        }
        turn = (turn + 1) % 2;
          
        //-----終了判定-----
        switch (end_judge(turn, board)) {
            case 1:                             //どちらかの置く場所が無い場合
                if (turn == 0) no_put = 1;
                else no_put = 2;
                turn = (turn + 1) % 2;
                break;
                   
            case 2:                             //どちらも置く場所が無い場合
                result(0, board, mode, NULL, NULL);
                return again();
                   
            default:                            //特に何もなければ継続
                break;
        }
    }
    return 0;
}
  
//============================================================
//1P vs CPUのmain関数
//
//  cpu_xとcpu_yがCPUの置いた石の座標
//  ruptが中断判定、end終了判定
//============================================================
int cpu_main(char mode)
{
    //------------------------------------------------------------------------------------------------
    //変数の宣言
    //  for文用、ボード、ターン、CPUの置いたx座標、CPUの置いたy座標、置ける場所が無い、AIの置いた場所
    //  先攻か後攻か、先攻の人、後攻の人、ボードの座標のABCDEFGH、難易度
    //------------------------------------------------------------------------------------------------
    int i, board[BOARD_SIZE][BOARD_SIZE], turn = 0, cpu_x, cpu_y, no_put = 0, put_ai;
    char senkou[4], be_person[16], af_person[16], str[8] = "ABCDEFGH", diff[4];
  
    printf("\n難易度を選択してください。\n"
            "%sEASYモード%sの場合は %s0%s を、%sNORMALモード%sの場合は %s1%s を入力してください。\n"
             "ゲームを終了する場合は N を入力してください。\n", 
              CYAN, DEFAULT, CYAN, DEFAULT, MAGENTA, DEFAULT, MAGENTA, DEFAULT);
  
    do {
        fgets(diff, sizeof(diff), stdin);
  
        if (strcmp(diff, "0\n") != 0 && strcmp(diff, "1\n") != 0
             && strcmp(diff, "N\n") != 0 && strcmp(diff, "n\n") != 0) {
            printf("値が不正です。もう一度入力してください。\n");
            if (diff[strlen(diff) - 1] != '\n') while (getchar() != '\n');
  
        }else break;
  
    }while (1);
  
    if (diff[0] == 'n' || diff[0] == 'N') {
        printf("終了します。\n\n");
        return 1;
    }
  
    printf("\n%s先手%sを選ぶ場合は %s0%s を、%s後手%sを選ぶ場合は %s1%s を入力してください。\n"
            "ゲームを終了する場合は N を入力してください。\n", 
             CYAN, DEFAULT, CYAN, DEFAULT, MAGENTA, DEFAULT, MAGENTA, DEFAULT);
  
    do {
        fgets(senkou, sizeof(senkou), stdin);
  
        if (strcmp(senkou, "0\n") != 0 && strcmp(senkou, "1\n") != 0
            && strcmp(senkou, "n\n") != 0 && strcmp(senkou, "N\n") != 0) {
            printf("値が不正です。もう一度入力してください。\n");
            if (senkou[strlen(senkou) - 1] != '\n') while (getchar() != '\n');   //バッファクリア
  
        }else break;
  
    }while (1);
  
    if (senkou[0] == 'n' || senkou[0] == 'N') {
        printf("終了します。\n\n");
        return 1;
      
    }else if (senkou[0] == '0') {               //'0'が入力されたらユーザが先攻
        turn = 0;
        strcpy(be_person, "あなた(B)");
        strcpy(af_person, "CPU");
  
    }else {                                     //CPUが先攻
        turn = 0;
        strcpy(be_person, "CPU");
        strcpy(af_person, "あなた(W)");
    }
  
    new_board(board);
  
    while (1) {                                 //終了と中断されていない間
  
        set_board(board);
  
        if (put_ai == 1) {                      //CPUが置いたら置いた場所を出力
            printf("CPUは %s%c%s%s%d%s に置きました。\n", 
             MAGENTA, str[cpu_x], DEFAULT, CYAN, cpu_y + 1, DEFAULT);
            put_ai = 0;
        }
  
        if (no_put == 1) printf("%s の置く場所がありません。\n", be_person);
        else if (no_put == 2) printf("%s の置く場所がありません。\n", af_person);
        no_put = 0;
    
        if (turn == 0 && strcmp(be_person, "あなた(B)") == 0) {         //ユーザ(黒の場合)のターン
            printf("%s の番です。\n", be_person);
  
            if (input(turn, board) == 1) {
                result(1, board, mode, be_person, af_person);
                return 1;
            }
            turn = (turn + 1) % 2;
              
        }else if (turn == 1 && strcmp(af_person, "あなた(W)") == 0){    //ユーザ(白の場合)のターン
            printf("%s の番です。\n", af_person);
            if (input(turn, board) == 1) {
                result(1, board, mode, be_person, af_person);
                return 1;
            }
            turn = (turn + 1) % 2;
  
        }else {                                                         //AIのターン
            if (diff[0] == '0') {                                       //かんたんモード
                random_ai(turn, board, &cpu_x, &cpu_y);
                turn = (turn + 1) % 2;
                put_ai = 1;
  
            }else {                                                     //普通モード
                alpha_beta_ai(turn, board, &cpu_x, &cpu_y);
                turn = (turn + 1) % 2;
                put_ai = 1;
            }
        }
  
        //-----終了判定-----
        switch (end_judge(turn, board)) {
            case 1:                                                     //どちらかの置く場所が無い場合
            if (turn == 0) no_put = 1;
            else no_put = 2;
            turn = (turn + 1) % 2;
            break;
  
            case 2:                                                     //どちらも置く場所が無い場合
                result(0, board, mode, be_person, af_person);
                return again();
  
            default :                                                   //特に何もなければ継続
                break;
        }
    }
    return 0;
}
  
//=======================================================
//ボードの初期化
//=======================================================
int new_board(int board[][BOARD_SIZE])
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
int set_board(int board[][BOARD_SIZE])
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
int input(int turn, int board[][BOARD_SIZE])
{
    int y;
    char x, posi[5];
       
    printf("石を置く場所を %s横%s縦%s(例:%sA%s1%s) のように入力して下さい。\n"
           "ゲームを終了する場合は N を入力して下さい。\n",
           MAGENTA, CYAN, DEFAULT, MAGENTA, CYAN, DEFAULT);
       
    while (1) {
        fgets(posi, sizeof(posi), stdin);
        sscanf(posi, "%c%d", &x, &y);
  
        //=====入力文字の判定======
        //-----英字が大文字の場合-----
        if (x == 'n' || x == 'N') return 1;                         //終了
        else if (x >= 65 && x <= 72 && y >= 1 && y <= 8) {          //座標内なら
            if (put_judge_vec(x - 65, y - 1, turn, board) == 1) {   //ひっくり返せるか判定
                turn_vec(x - 65, y - 1, turn, board);               //ひっくり返せるならひっくり返す
                break;
   
            }else {                                                 //座標内でもひっくり返せる石が無い
                printf("ひっくり返せる石がありません。もう一度入力して下さい。\n");
                if (posi[strlen(posi) - 1] != '\n') while (getchar() != '\n');      //バッファクリア
            }
  
        //-----英字が小文字の場合-----     
        }else if (x >= 97 && x <= 104 && y >= 1 && y <= 8) {
            if (put_judge_vec(x - 97, y - 1, turn, board) == 1) {
                turn_vec(x - 97, y - 1, turn, board);
                break;
   
            }else {
                printf("ひっくり返せる石がありません。もう一度入力して下さい。\n");
                if (posi[strlen(posi) - 1] != '\n') while (getchar() != '\n');      //バッファクリア
            }
               
        }else {
            printf("値が不正です。もう一度入力して下さい。\n");     //座標外
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
int put_judge_vec(int x, int y, int turn, int board[][BOARD_SIZE])
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
int put_judge(int x, int y, int turn, int i, int j, int board[][BOARD_SIZE])
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
int turn_vec(int x, int y, int turn, int board[][BOARD_SIZE])
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
void turnover(int x, int y, int i, int j, int turn, int board[][BOARD_SIZE])
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
int end_judge(int turn, int board[][BOARD_SIZE])
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
void result(int rupt, int board[][BOARD_SIZE], char mode, char be_person[], char af_person[])
{
       
    int i, j, b = 0, w = 0;
       
    //-----全部の石を数える-----
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
               
            switch (board[i][j]) {
                case BLACK:                     //黒石の数
                    b++;
                    break;
                       
                case WHITE:                     //白石の数
                    w++;
                    break;
                       
                default:
                    break;
            }
        }
    }
    set_board(board);
       
    //-----結果の判定、出力-----
    if (rupt == 1) {                            //中断された場合の結果
        printf("ゲームが中断されました。\n\n");
        return;
    }
  
    printf("%s%sGAME SET%s\n", "\x1b[5;35m", BOLD, DEFAULT);
  
    //-----モードの判定-----
    if (mode == '0') {                          //モードが1P vs 2Pの場合の結果
        printf("B = %d個, W = %d個\n", b, w);
        if (b > w) printf("で B の勝ちです。\n\n");
        else if (w > b) printf("で W の勝ちです。\n\n");
        else printf("で 引き分け です。\n\n");
  
    }else {                                     //モードが1P vs CPUの場合の結果
        printf("%s = %d個, %s = %d個\n", be_person, b, af_person, w);
        if (w < b) printf("で %s の勝ちです。\n\n", be_person);
        else if (w > b) printf("で %s の勝ちです。\n\n", af_person);
        else printf("で 引き分け です。\n\n");
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
       
    if (strcmp(aga, "N\n") == 0 || strcmp(aga, "n\n") == 0) {
        printf("終了します。\n");
        return 1;
     }
  
    return 0;
}
  
//======================================================
//RANDOM_AIの基本部分
//
//  置ける場所の中からランダムで石を置く
//  put_okは置ける場所の座標
//======================================================
void random_ai(int turn, int board[][BOARD_SIZE], int *cpu_x, int *cpu_y)
{
    //----------------------------------------------------------------
    //変数の宣言
    //  int for文用、for文用、ランダム数、置ける場所の数、置ける場所の座標
    //-----------------------------------------------------------------
    int i, j, y, count = 0, put_ok[64][2];
  
    srand(time(NULL));
  
    count = put_search(put_ok, turn, board);
  
    y = rand() % count;                                 //ランダムで座標選択
    turn_vec(put_ok[y][0], put_ok[y][1], turn, board);  //ひっくり返す
      
    *cpu_x = put_ok[y][0];                              //CPUの置いたx座標
    *cpu_y = put_ok[y][1];                              //CPUの置いたy座標
}
  
//===========================================================================
//αβ(アルファベータ)法AIの基本部分
//
//  自分が置けるすべての場所から、4ターン目までの評価値を算出し、
//      評価値が最も高い（今後自分が最も有利になると思われる）場所に置く
//===========================================================================
void alpha_beta_ai(int turn, int board[][BOARD_SIZE], int *cpu_x, int *cpu_y)
{
    //--------------------------------------------------------------------
    //変数の宣言
    //  int 現在のターンの評価値、1つ後のターンの評価値、2つ後のターンの評価値
    //  int あるターンの置ける場所、一番評価値が高い置ける場所、あるターンの置ける場所の数
    //  int 関数に渡された時点での初期ボード、1つ後のターンの初期ボード
    //  int for文用、for文用、相手のターン
    //-------------------------------------------------------------------
    int val0, val1, val2;
    int put_oks[4][64][2], put[1][2], count[4];
    int board0[BOARD_SIZE][BOARD_SIZE], board1[BOARD_SIZE][BOARD_SIZE];
    int i, j, turn1 = (turn + 1) % 2;
 
    memcpy(board0, board, sizeof(board0));
    count[0] = put_search(put_oks[0], turn, board0);                //初期ボードで置ける場所
  
    for (i = 0; i < count[0]; i++) {
        turn_vec(put_oks[0][i][0], put_oks[0][i][1], turn, board0);
  
        memcpy(board1, board0, sizeof(board1));                     
  
        count[1] = put_search(put_oks[1], turn1, board1);           //1つ掘り下げた場所の置ける場所
  
        for (j = 0; j < count[1]; j++) {
            turn_vec(put_oks[1][j][0], put_oks[1][j][1], turn1, board1);
  
            count[2] = put_search(put_oks[2], turn, board1);        //2つ掘り下げた場所の置ける場所
  
            if (j == 0) val1 = re_high_val(put_oks[2], count[2], turn, board1, val1, j);//葉の評価値の最大値
  
            else {
                val2 = re_high_val(put_oks[2], count[2], turn, board1, val1, j);        //葉の評価値の最大値
  
                if (val2 == 1000) {                                 //ベータカット
                    memcpy(board1, board0, sizeof(board1));
                    continue;
  
                }else if (val1 > val2) val1 = val2;                 //最小値の格納
            }
  
            if (i != 0 && val0 >= val1) break;                      //アルファカット
  
            memcpy(board1, board0, sizeof(board1));
        }
  
        if (i == 0 || val0 < val1) {                                //最も評価値が高い置ける場所
                val0 = val1;
                put[0][0] = put_oks[0][i][0];
                put[0][1] = put_oks[0][i][1];
        }
        memcpy(board0, board, sizeof(board0));
    }
    turn_vec(put[0][0], put[0][1], turn, board);                    //CPUが選んだ場所をひっくり返す
  
    //-----最終的にCPUが選んだ置く場所-----
    *cpu_x = put[0][0];
    *cpu_y = put[0][1];
}
  
//============================================================================
//葉の評価値を算出し、最大値を返す関数
//============================================================================
int re_high_val(int put_ok[][2], int count, int turn, int board0[][BOARD_SIZE], int beta, int flag)
{
    int i, j, val[2], board1[BOARD_SIZE][BOARD_SIZE];   //for文用、for文用、評価値、葉のボード
    int turn1 = (turn + 1) % 2;                         //相手のターン
  
    memcpy(board1, board0, sizeof(board1));
  
    for (i = 0; i < count; i++) {
        turn_vec(put_ok[i][0], put_ok[i][1], turn1, board1);        //4ターン目（葉）
  
        val[1] = re_val(board1, turn);                              //評価値の算出
  
        if (val[1] >= beta && flag != 0) return 1000;               //ベータカット
        else if (i == 0 || val[0] < val[1]) val[0] = val[1];        //最大値の格納
  
        memcpy(board1, board0, sizeof(board1));
    }
  
    return val[0];
}
  
//================================================
//評価値を算出する関数
//===============================================
int re_val(int board[][BOARD_SIZE], int turn)
{
    int i, j, val = 0;                                              //for文用、for文用、評価値
  
    turn++;                                                         //黒1白2に対し、先攻0後攻1のため
  
    int val_board[BOARD_SIZE][BOARD_SIZE] = {                       //評価値ボード
        {120, -20, 20, 5, 5, 20, -20, 120},
        {-20, -40, -5, -5, -5, -5, -40, -20},
        {20, -5, 15, 3, 3, 15, -5, 20},
        {5, -5, 3, 3, 3, 3, -5, 5},
        {5, -5, 3, 3, 3, 3, -5, 5},
        {20, -5, 15, 3, 3, 15, -5, 20},
        {-20, -40, -5, -5, -5, -5, -40, -20},
        {120, -20, 20, 5, 5, 20, -20, 120}
    };
  
    //-----全位置を調べ、評価値を算出-----
    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == turn) val += val_board[i][j];
            else if (board[i][j] != 0) val -= val_board[i][j];
        }
    return val;
}
  
//===============================================================
//置ける場所を探す関数
//==============================================================
int put_search(int put_ok[][2], int turn, int board[][BOARD_SIZE])
{
    int i, j, count = 0;                                //for文用、for文用、置ける場所の数
      
    //-----全位置を調べ、置ける場所を探す-----
    for (i = 0; i < BOARD_SIZE; i++)
        for (j = 0; j < BOARD_SIZE; j++)
            if (put_judge_vec(j, i, turn, board) == 1) {
                put_ok[count][0] = j;                  //置くことのできるx座標を格納
                put_ok[count][1] = i;                  //置くことのできるy座標を格納
                count++;
            }
  
    return count;
}