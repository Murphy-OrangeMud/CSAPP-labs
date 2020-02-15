/* Cheng Zhiyi 1800017781 Vegetable
 * Use the trick of blocks of the matrices, and specially
 * deal with the diagonal positions.
  *
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
void block_trans_32_32(int M, int N, int A[N][M], int B[M][N]);
void block_trans_64_64(int M, int N, int A[N][M], int B[M][N]);
void block_trans_61_67(int M, int N, int A[N][M], int B[M][N]);
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    if (N == 32 && M == 32) 
    {
        block_trans_32_32(M, N, A, B);
    }
    else if (N == 64 && M == 64)
    {
        block_trans_64_64(M, N, A, B);
    }
    else
    {
        block_trans_61_67(M, N, A, B);
    }
    ENSURES(is_transpose(M, N, A, B));
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

    ENSURES(is_transpose(M, N, A, B));
}
/*
 * block_trans_32_32 - a transpose function using blocks for 32x32 matrices,
                       specially dealing with the diagonal positions, noticing
                       that the same position of A and B will share the same set
                       in the cache.
 */
char block_trans_32_32_desc[] = "Transpose using blocks for 32x32 matrices";
void block_trans_32_32(int M, int N, int A[N][M], int B[M][N])
{
    for (int i = 0; i < 32; i += 8) {
        for (int j = 0; j < 32; j += 8) {
            int ii, jj;
            if (i != j) {
                for (ii = i; ii < i + 8; ii++) {
                    for (jj = j; jj < j + 8; jj++) {
                        B[ii][jj] = A[jj][ii];
                    }
                }
            }
            else {
                for (ii = i; ii < i + 8; ii++) {
                    int tmp1 = A[ii][j];
                    int tmp2 = A[ii][j+1];
                    int tmp3 = A[ii][j+2];
                    int tmp4 = A[ii][j+3];
                    int tmp5 = A[ii][j+4];
                    int tmp6 = A[ii][j+5];
                    int tmp7 = A[ii][j+6];
                    int tmp8 = A[ii][j+7];
                    B[j][ii] = tmp1;
                    B[j+1][ii] = tmp2;
                    B[j+2][ii] = tmp3;
                    B[j+3][ii] = tmp4;
                    B[j+4][ii] = tmp5;
                    B[j+5][ii] = tmp6;
                    B[j+6][ii] = tmp7;
                    B[j+7][ii] = tmp8;
                }
            }
        }
    }
}
/*
 * block_trans_64_64 - a transpose function using 4x8 blocks for 64x64 matrices
                       seperately dealing with diagonal positions using 4x4 blocks, 
                       noticing that when at a distance of 256 ints, the values in
                       the same matrix will share the same set in the cache.
 */
char block_trans_64_64_desc[] = "Transpose using blocks for 64x64 matrices";
void block_trans_64_64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, ii, jj;
    int flag = 0;
    for (i = 0; i < 64; i += 8) {
        for (j = 0; j < 64; j += 4) {
            if (i != j && j != i + 4) {
                if (flag == 0) {
                    flag = 1;
                    for (ii = i; ii < i + 8; ii++) {
                        for (jj = j; jj < j + 4; jj++) {
                            B[ii][jj] = A[jj][ii];
                        }
                    }
                }
                else {
                    flag = 0;
                    for (ii = i + 7; ii >= i; ii--) {
                        for (jj = j; jj < j + 4; jj++) {
                            B[ii][jj] = A[jj][ii];
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < 64; i += 4) {
        for (int j = 0; j < 64; j += 4) {
            if (i == j || i == j + 4 || i == j - 4) {
                for (ii = i; ii < i + 4; ii++) {
                    int tmp1 = A[ii][j];
                    int tmp2 = A[ii][j+1];
                    int tmp3 = A[ii][j+2];
                    int tmp4 = A[ii][j+3];
                    B[j][ii] = tmp1;
                    B[j+1][ii] = tmp2;
                    B[j+2][ii] = tmp3;
                    B[j+3][ii] = tmp4;
                }
            }
        }
    }
}
/*
 * block_trans_61_67 - a transpose function using 6x6 blocks for 61x67 matrices
                       special case for diagonal positions and deal separately
                       with side positions.
 */
char block_trans_61_67_desc[] = "Transpose using blocks for 61x67 matrices";
void block_trans_61_67(int M, int N, int A[N][M], int B[M][N])
{
    int tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    int i, j, ii, jj;
    for (i = 0; i < 66; i += 6) {
        for (j = 0; j < 60; j += 6) {
            if (i != j && j != 54 && i != 60) {
                for (ii = i; ii < i + 6; ii++) {
                    for (jj = j; jj < j + 6; jj++) {
                        B[jj][ii] = A[ii][jj];
                    }
                }
            }
            else if (i != j && j == 54 && i != 60) {
                for (ii = i; ii < i + 6; ii++) {
                    for (jj = j; jj < j + 7; jj++) {
                        B[jj][ii] = A[ii][jj];
                    }
                }
            }
            else if (i != j && i == 60 && j != 54) {
                for (ii = i; ii < i + 7; ii++) {
                    for (jj = j; jj < j + 6; jj++) {
                        B[jj][ii] = A[ii][jj];
                    }
                }
            }
            else if (i == 60 && j == 54) {
                for (ii = i; ii < i + 7; ii++) {
                    for (jj = j; jj < j + 7; jj++) {
                        B[jj][ii] = A[ii][jj];
                    }
                }
            }
            else if (i == j && j != 54) {
                for (ii = i; ii < i + 6; ii++) {
                    tmp1 = A[ii][j];
                    tmp2 = A[ii][j+1];
                    tmp3 = A[ii][j+2];
                    tmp4 = A[ii][j+3];
                    tmp5 = A[ii][j+4];
                    tmp6 = A[ii][j+5];
                    B[j][ii] = tmp1;
                    B[j+1][ii] = tmp2;
                    B[j+2][ii] = tmp3;
                    B[j+3][ii] = tmp4;
                    B[j+4][ii] = tmp5;
                    B[j+5][ii] = tmp6;
                }
            }
            else if (i == j && j == 54) {
                for (ii = i; ii < i + 6; ii++) {
                    tmp1 = A[ii][j];
                    tmp2 = A[ii][j+1];
                    tmp3 = A[ii][j+2];
                    tmp4 = A[ii][j+3];
                    tmp5 = A[ii][j+4];
                    tmp6 = A[ii][j+5];
                    tmp7 = A[ii][j+6];
                    B[j][ii] = tmp1;
                    B[j+1][ii] = tmp2;
                    B[j+2][ii] = tmp3;
                    B[j+3][ii] = tmp4;
                    B[j+4][ii] = tmp5;
                    B[j+5][ii] = tmp6;
                    B[j+6][ii] = tmp7;
                }
            }
        }
    }
}
/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */

void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    //registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
