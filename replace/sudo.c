#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

// 檢查在 (row, col) 放置 num 是否符合數獨規則
int isValid(int row, int col, int num) {
    for (int x = 0; x < N; x++) {
        if (board[row][x] == num || board[x][col] == num) {
            return 0;
        }
    }

    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[startRow + i][startCol + j] == num) {
                return 0;
            }
        }
    }
    return 1;
}

// 遞迴解決數獨
int solveSudoku(int row, int col) {
    if (row == N - 1 && col == N) return 1;
    if (col == N) { row++; col = 0; }

    if (board[row][col] != 0) return solveSudoku(row, col + 1);

    for (int num = 1; num <= 9; num++) {
        if (isValid(row, col, num)) {
            board[row][col] = num;
            if (solveSudoku(row, col + 1)) return 1;
            board[row][col] = 0;
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
        if (board[row][col] == 0 && isValid(row, col, num)) {
            board[row][col] = num;
            filledCells--;
        }
    }
}

// 印出數獨盤面（簡單格式）
void printBoard() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
}

// 印出數獨盤面（美觀格式）
void printBoardPretty() {
    printf("\n +-------+-------+-------+\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (j % 3 == 0) printf(" | ");
            else printf(" ");
            if (board[i][j] == 0)
                printf("_");
            else
                printf("%d", board[i][j]);
        }
        printf(" |\n");
        if (i % 3 == 2) printf(" +-------+-------+-------+\n");
    }
}

// 儲存盤面到文字檔案
void saveBoardToFile(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("無法開啟檔案 %s\n", filename);
        return;
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fp, "%d ", board[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

// 儲存盤面到二進位檔案
void saveBoardToBinary(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        printf("無法開啟檔案 %s\n", filename);
        return;
    }
    fwrite(board, sizeof(int), N * N, fp);
    fclose(fp);
}

// 從文字檔案讀取盤面
void loadBoardFromFile(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("無法開啟檔案 %s\n", filename);
        return;
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fscanf(fp, "%d", &board[i][j]);
        }
    }
    fclose(fp);
}

// 從二進位檔案讀取盤面
void loadBoardFromBinary(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        printf("無法開啟檔案 %s\n", filename);
        return;
    }
    fread(board, sizeof(int), N * N, fp);
    fclose(fp);
}

void save_to_binary_file(int board[][9], int problem_id, const char* filename, int is_append) {
    FILE *fp;
    if (is_append) {
        // 檢查檔案是否存在
        fp = fopen(filename, "rb+");
        if (fp == NULL) {
            // 檔案不存在，建立新檔案
            fp = fopen(filename, "wb+");
            if (fp == NULL) {
                printf("無法建立檔案 %s\n", filename);
                return;
            }
            // 寫入新的標頭
            SudokuDataHeader header;
            header.numbers = 1;
            header.datasize = sizeof(SudokuProblem);
            fwrite(&header, sizeof(header), 1, fp);
        } else {
            // 檔案存在，更新標頭中的問題數量
            SudokuDataHeader header;
            fread(&header, sizeof(header), 1, fp);
            header.numbers++;
            // 回到檔案開頭更新標頭
            fseek(fp, 0, SEEK_SET);
            fwrite(&header, sizeof(header), 1, fp);
            // 移動到檔案末尾以添加新問題
            fseek(fp, 0, SEEK_END);
        }
    } else {
        // 建立新檔案
        fp = fopen(filename, "wb");
        if (fp == NULL) {
            printf("無法開啟檔案 %s 進行寫入\n", filename);
            return;
        }
        // 寫入標頭
        SudokuDataHeader header;
        header.numbers = 1;
        header.datasize = sizeof(SudokuProblem);
        fwrite(&header, sizeof(header), 1, fp);
    }

    // 建立並寫入問題
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

int main() {
    generateSudoku();
    printf("生成的數獨盤面:\n");
    printBoardPretty();

    // 儲存到二進位檔案（不附加，建立新檔案）
    save_to_binary_file(board, 1, "sudoku_problems.bin", 0);

    // 解題並輸出解答
    if (solveSudoku(0, 0)) {
        printf("\n解決後的數獨盤面:\n");
        printBoardPretty();
    } else {
        printf("\n無解。\n");
    }

    return 0;
}