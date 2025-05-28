#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 9

typedef struct {
    int numbers;    // 檔案內問題數量
    int datasize;   // 每個問題的大小
} SudokuDataHeader;

typedef struct {
    int id;
    int data[9][9];
} SudokuProblem;

int board[N][N];

// ======= 新增區塊：遊戲用全域變數 =======
int player_board[9][9];    // 玩家當前的盤面
int answer_board[9][9];    // 正確答案盤面
int original_board[9][9];  // 原始問題盤面
int error_count = 0;       // 錯誤計數

// 檢查在 (row, col) 放置 num 是否符合數獨規則（針對任意盤面）
int isValidBoard(int b[9][9], int row, int col, int num) {
    for (int x = 0; x < N; x++) {
        if (b[row][x] == num || b[x][col] == num) {
            return 0;
        }
    }
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (b[startRow + i][startCol + j] == num) {
                return 0;
            }
        }
    }
    return 1;
}

// 遞迴解決數獨（針對任意盤面）
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

// 生成隨機數獨盤面
void generateSudoku() {
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            board[i][j] = 0; // 初始化盤面
        }
    }
    int filledCells = 17; // 設定初始填入的數字數量
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

// 印出數獨盤面（美觀格式，支援任意盤面）
void printBoardPrettyCustom(int b[9][9]) {
    printf("\n +-------+-------+-------+\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (j % 3 == 0) printf(" | ");
            else printf(" ");
            if (b[i][j] == 0)
                printf("_");
            else
                printf("%d", b[i][j]);
        }
        printf(" |\n");
        if (i % 3 == 2) printf(" +-------+-------+-------+\n");
    }
}

// 儲存盤面到二進位檔案
void save_to_binary_file(int board[][9], int problem_id, const char* filename, int is_append) {
    FILE *fp;
    if (is_append) {
        fp = fopen(filename, "rb+");
        if (fp == NULL) {
            fp = fopen(filename, "wb+");
            if (fp == NULL) {
                printf("無法建立檔案 %s\n", filename);
                return;
            }
            SudokuDataHeader header;
            header.numbers = 1;
            header.datasize = sizeof(SudokuProblem);
            fwrite(&header, sizeof(header), 1, fp);
        } else {
            SudokuDataHeader header;
            fread(&header, sizeof(header), 1, fp);
            header.numbers++;
            fseek(fp, 0, SEEK_SET);
            fwrite(&header, sizeof(header), 1, fp);
            fseek(fp, 0, SEEK_END);
        }
    } else {
        fp = fopen(filename, "wb");
        if (fp == NULL) {
            printf("無法開啟檔案 %s 進行寫入\n", filename);
            return;
        }
        SudokuDataHeader header;
        header.numbers = 1;
        header.datasize = sizeof(SudokuProblem);
        fwrite(&header, sizeof(header), 1, fp);
    }
    SudokuProblem problem;
    problem.id = problem_id;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            problem.data[i][j] = board[i][j];
        }
    }
    fwrite(&problem, sizeof(problem), 1, fp);
    fclose(fp);
    printf("已成功儲存到二進位檔案 %s\n", filename);
}

// 計算數獨盤面中的空格數量
int count_empty_cells(int board[][9]) {
    int count = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) {
                count++;
            }
        }
    }
    return count;
}

// 檢查數獨解答是否合法（針對任意盤面）
int is_valid_solution(int b[9][9]) {
    int check[10];
    // 檢查每一行
    for (int i = 0; i < 9; i++) {
        for (int k = 1; k <= 9; k++) check[k] = 0;
        for (int j = 0; j < 9; j++) {
            int num = b[i][j];
            if (num < 1 || num > 9 || check[num]) return 0;
            check[num] = 1;
        }
    }
    // 檢查每一列
    for (int j = 0; j < 9; j++) {
        for (int k = 1; k <= 9; k++) check[k] = 0;
        for (int i = 0; i < 9; i++) {
            int num = b[i][j];
            if (num < 1 || num > 9 || check[num]) return 0;
            check[num] = 1;
        }
    }
    // 檢查每個3x3方格
    for (int blockRow = 0; blockRow < 3; blockRow++) {
        for (int blockCol = 0; blockCol < 3; blockCol++) {
            for (int k = 1; k <= 9; k++) check[k] = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int num = b[blockRow*3 + i][blockCol*3 + j];
                    if (num < 1 || num > 9 || check[num]) return 0;
                    check[num] = 1;
                }
            }
        }
    }
    return 1;
}

// 判斷遊戲是否完成
int is_complete() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (player_board[i][j] == 0)
                return 0;
        }
    }
    return 1;
}

// 處理玩家輸入
int handle_input() {
    int row, col, num;
    printf("請輸入座標與數字 (1-9)，例如 4 5 7 表示第4列第5行填7：");
    if (scanf("%d %d %d", &row, &col, &num) != 3) {
        printf("輸入格式錯誤！\n");
        return -1;
    }
    row--; col--;
    if (row < 0 || row > 8 || col < 0 || col > 8 || num < 1 || num > 9) {
        printf("座標或數字超出範圍，請輸入1~9。\n");
        return -1;
    }
    if (original_board[row][col] != 0) {
        printf("這個位置不能更改！\n");
        return -1;
    }
    if (player_board[row][col] == num) {
        printf("這個數字已經填過了！\n");
        return -1;
    }
    if (answer_board[row][col] != num) {
        printf("答案錯誤！\n");
        error_count++;
        printf("錯誤次數：%d\n", error_count);
        return 0;
    }
    player_board[row][col] = num;
    printf("填入成功！\n");
    return 1;
}

// 初始化遊戲盤面與答案
void init_game() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            original_board[i][j] = board[i][j];
            player_board[i][j] = board[i][j];
            answer_board[i][j] = board[i][j];
        }
    }
    // 用 answer_board 求解，不動原始盤面
    solveSudokuBoard(answer_board, 0, 0);
    error_count = 0;
    printf("遊戲初始化完成！\n");
}

// 從文字檔案讀取盤面
void loadBoardFromFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("無法開啟檔案 %s\n", filename);
        // 若失敗則自動產生隨機盤面
        generateSudoku();
        return;
    }
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            fscanf(fp, "%d", &board[i][j]);
    fclose(fp);
}

// 遊戲主流程（加上剩餘空格顯示）
void play() {
    init_game();
    printf("歡迎來到數獨遊戲！\n\n");
    printBoardPrettyCustom(player_board);

    while (!is_complete()) {
        printf("剩餘空格數：%d\n", count_empty_cells(player_board));
        int result = handle_input();
        printBoardPrettyCustom(player_board);

        // 新增：錯誤達5次遊戲結束
        if (error_count >= 5) {
            printf("錯誤5次，遊戲結束！\n");
            break;
        }

        if (result == 1 && is_complete()) {
            printf("恭喜你完成了數獨！\n");
            break;
        }
    }
    printf("遊戲結束，正確答案如下：\n");
    printBoardPrettyCustom(answer_board);
}

// 主程式（支援重玩與檔案載入）
int main() {
    char filename[100];
    int replay = 1;
    while (replay) {
        printf("請輸入數獨檔案名稱（如 sudoku.txt），或直接按 Enter 產生隨機盤面：");
        fgets(filename, sizeof(filename), stdin);
        if (filename[0] != '\n') {
            filename[strcspn(filename, "\n")] = 0;
            loadBoardFromFile(filename);
        } else {
            generateSudoku();
        }

        printf("生成的數獨盤面:\n");
        printBoardPrettyCustom(board);
        printf("空格數量：%d\n", count_empty_cells(board));

        save_to_binary_file(board, 1, "sudoku_problems.bin", 0);

        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                answer_board[i][j] = board[i][j];
        solveSudokuBoard(answer_board, 0, 0);

        printf("\n解決後的數獨盤面:\n");
        printBoardPrettyCustom(answer_board);
        printf("空格數量：%d\n", count_empty_cells(answer_board));

        if (is_valid_solution(answer_board)) {
            printf("此數獨解答合法！\n\n");
        } else {
            printf("此數獨解答不合法！\n\n");
        }

        play();

        printf("是否要重新開始？(1=是, 0=否)：");
        scanf("%d", &replay);
        getchar(); // 吃掉換行
    }
    return 0;
}