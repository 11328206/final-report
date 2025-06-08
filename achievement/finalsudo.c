#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 9

int board[N][N];
int player_board[N][N];
int answer_board[N][N];
int original_board[N][N];
int error_count = 0;

// 判斷在 (row, col) 放 num 是否合規
int isValidBoard(int b[9][9], int row, int col, int num) {
    for (int x = 0; x < N; x++)
        if (b[row][x] == num || b[x][col] == num) return 0;
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (b[startRow + i][startCol + j] == num) return 0;
    return 1;
}

// 遞迴解數獨
int solveSudokuBoard(int b[9][9], int row, int col) {
    if (row == N - 1 && col == N) return 1;
    if (col == N) { row++; col = 0; }
    if (b[row][col] != 0) return solveSudokuBoard(b, row, col + 1);
    for (int num = 1; num <= 9; num++) {
        if (isValidBoard(b, row, col, num)) {
            b[row][col] = num;
            if (solveSudokuBoard(b, row, col + 1)) return 1;
            b[row][col] = 0;
        }
    }
    return 0;
}

// 生成隨機盤面
void generateSudoku() {
    srand(time(NULL));
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            board[i][j] = 0;
    int filledCells = 17;
    while (filledCells > 0) {
        int row = rand() % 9;
        int col = rand() % 9;
        int num = rand() % 9 + 1;
        if (board[row][col] == 0 && isValidBoard(board, row, col, num)) {
            board[row][col] = num;
            filledCells--;
        }
    }
}

// 完全對齊且加粗的盤面顯示
void printBoardPrettyCustom(int b[9][9]) {
// filepath: c:\computerscience\sudo.c
    printf("\n    1 2 3   4 5 6   7 8 9\n");
    printf("  +-------+-------+-------+\n");
    for (int i = 0; i < 9; i++) {
        printf("%d |", i + 1);
        for (int j = 0; j < 9; j++) {
            if (original_board[i][j] != 0) {
                // 原始盤面數字：青色
                printf(" \033[36m%d\033[0m", original_board[i][j]);
            } else if (b[i][j] != 0) {
                if (b[i][j] == answer_board[i][j]) {
                    // 玩家正確填入：綠色
                    printf(" \033[32m%d\033[0m", b[i][j]);
                } else {
                    // 玩家錯誤填入：紅色
                    printf(" \033[31m%d\033[0m", b[i][j]);
                }
            } else {
                printf(" .");
            }
            if ((j + 1) % 3 == 0) printf(" |");
        }
        printf("\n");
        if ((i + 1) % 3 == 0)
            printf("  +-------+-------+-------+\n");
    }
}


// 初始化遊戲盤面與答案
void init_game() {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
            original_board[i][j] = board[i][j];
            player_board[i][j] = board[i][j];
            answer_board[i][j] = board[i][j];
        }
    solveSudokuBoard(answer_board, 0, 0);
    error_count = 0;
}

// 判斷遊戲是否完成
int is_complete() {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (player_board[i][j] == 0)
                return 0;
    return 1;
}

// 計算空格數
int count_empty_cells(int board[][9]) {
    int count = 0;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (board[i][j] == 0)
                count++;
    return count;
}

// 遊戲主流程
void play() {
    init_game();
    printf("歡迎來到數獨遊戲！\n\n");
    printBoardPrettyCustom(player_board);

    time_t start_time = time(NULL);
    const int TIME_LIMIT = 300; // 5分鐘

    while (!is_complete()) {
        time_t now = time(NULL);
        int elapsed = (int)(now - start_time);
        int remaining = TIME_LIMIT - elapsed;
        if (remaining <= 0) {
            printf("\033[31m\n時間到！超過五分鐘，挑戰失敗！\033[0m\n");
            break;
        }
        printf("剩餘時間：%d 分 %02d 秒\n", remaining / 60, remaining % 60);
        printf("請輸入座標與數字 (1-9)，例如 4 5 7 表示第4列第5行填7：");
        int row, col, num;
        if (scanf("%d %d %d", &row, &col, &num) != 3) {
            printf("\033[31m輸入格式錯誤！\033[0m\n");
            while(getchar()!='\n');
            continue;
        }
        row--; col--;
        if (row < 0 || row > 8 || col < 0 || num < 1 || num > 9) {
            printf("\033[31m座標或數字超出範圍，請輸入1~9。\033[0m\n");
            continue;
        }
        if (original_board[row][col] != 0) {
            printf("\033[33m這個位置不能更改！\033[0m\n");
            continue;
        }
        if (player_board[row][col] == num) {
            printf("\033[33m這個數字已經填過了！\033[0m\n");
            continue;
        }
        if (answer_board[row][col] != num) {
            printf("\033[31m答案錯誤！\033[0m\n");
            error_count++;
            printf("\033[33m錯誤次數：%d\033[0m\n", error_count);
            if (error_count >= 5) {
                printf("\033[31m錯誤5次，遊戲結束！\033[0m\n");
                break;
            }
            continue;
        }
        player_board[row][col] = num;
        printf("\033[32m填入成功！\033[0m\n");
        printf("剩餘空格數：%d\n", count_empty_cells(player_board));
        printBoardPrettyCustom(player_board);
        if (is_complete()) {
            printf("\033[36m恭喜你完成了數獨！\033[0m\n");
            break;
        }
    }
    printf("遊戲結束，正確答案如下：\n");
    printBoardPrettyCustom(answer_board);
}

// 主程式
int main() {
    int replay = 1;
    while (replay) {
        generateSudoku();
        printf("生成的數獨盤面:\n");
        printBoardPrettyCustom(board);
        printf("空格數量：%d\n", count_empty_cells(board));
        play();
        printf("是否要重新開始？(1=是, 0=否)：");
        scanf("%d", &replay);
        getchar();
    }
    return 0;
}