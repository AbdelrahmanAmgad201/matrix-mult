#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {
    int row, col;
    int **A, **B, **C;
    int common_dim;
} ThreadData;

void read_matrix_from_file(const char *filename, int ***matrix, int *rows, int *cols) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fscanf(file, "row=%d col=%d", rows, cols); 

    *matrix = (int **)malloc(*rows * sizeof(int *));
    for (int i = 0; i < *rows; i++) {
        (*matrix)[i] = (int *)malloc(*cols * sizeof(int));
    }

    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            fscanf(file, "%d", &((*matrix)[i][j])); 
        }
    }
    fclose(file);
}

void write_matrix_to_file(const char *filename, int **matrix, int rows, int cols) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }
    fprintf(file, "row=%d col=%d\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i][j]);
            if (j < cols - 1) fprintf(file, "\t");
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void free_matrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) free(matrix[i]);
    free(matrix);
}

int **solve_per_matrix(int **A, int **B, int rowsA, int colsA, int rowsB, int colsB) {

    int **C = (int **)malloc(rowsA * sizeof(int *));
    for (int i = 0; i < rowsA; i++) {
        C[i] = (int *)malloc(colsB * sizeof(int));
        for (int j = 0; j < colsB; j++) {
            C[i][j] = 0;
            for (int k = 0; k < colsA; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

void *multiply_row(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int j = 0; j < data->col; j++) {
        data->C[data->row][j] = 0;
        for (int k = 0; k < data->common_dim; k++) {
            data->C[data->row][j] += data->A[data->row][k] * data->B[k][j];
        }
    }
    return NULL;
}


int **solve_per_row(int **A, int **B, int rowsA, int colsA, int rowsB, int colsB) {

    int **C = (int **)malloc(rowsA * sizeof(int *));
    pthread_t threads[rowsA];
    ThreadData data[rowsA];

    for (int i = 0; i < rowsA; i++) {
        C[i] = (int *)malloc(colsB * sizeof(int));
        data[i] = (ThreadData){i, colsB, A, B, C, colsA};
        pthread_create(&threads[i], NULL, multiply_row, &data[i]);
    }

    for (int i = 0; i < rowsA; i++) pthread_join(threads[i], NULL);
    return C;
}

void *multiply_element(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    data->C[data->row][data->col] = 0;
    for (int k = 0; k < data->common_dim; k++) {
        data->C[data->row][data->col] += data->A[data->row][k] * data->B[k][data->col];
    }
    return NULL;
}

int **solve_per_element(int **A, int **B, int rowsA, int colsA, int rowsB, int colsB) {
    int **C = (int **)malloc(rowsA * sizeof(int *));
    pthread_t threads[rowsA][colsB];
    ThreadData data[rowsA][colsB];

    for (int i = 0; i < rowsA; i++) {
        C[i] = (int *)malloc(colsB * sizeof(int));
        for (int j = 0; j < colsB; j++) {
            data[i][j] = (ThreadData){i, j, A, B, C, colsA};
            pthread_create(&threads[i][j], NULL, multiply_element, &data[i][j]);
        }
    }

    for (int i = 0; i < rowsA; i++)
        for (int j = 0; j < colsB; j++)
            pthread_join(threads[i][j], NULL);

    return C;
}

long matrix_mult(int **A, int **B, int rowsA, int colsA, int rowsB, int colsB, int multType, const char *filename) {
    struct timeval start, stop;
    gettimeofday(&start, NULL);

    int **C = NULL;
    switch (multType) {
        case 1: C = solve_per_matrix(A, B, rowsA, colsA, rowsB, colsB); break;
        case 2: C = solve_per_row(A, B, rowsA, colsA, rowsB, colsB); break;
        case 3: C = solve_per_element(A, B, rowsA, colsA, rowsB, colsB); break;
    }

    gettimeofday(&stop, NULL);
    long elapsed_time = (stop.tv_sec - start.tv_sec) * 1000000 + (stop.tv_usec - start.tv_usec);

    write_matrix_to_file(filename, C, rowsA, colsB);
    free_matrix(C, rowsA);

    return elapsed_time;
}

int main() {
    int **matrixA, **matrixB;
    int rowsA, colsA, rowsB, colsB;

    read_matrix_from_file("a.txt", &matrixA, &rowsA, &colsA);
    read_matrix_from_file("b.txt", &matrixB, &rowsB, &colsB);

    printf("No Threading Time: %ld µs\n", matrix_mult(matrixA, matrixB, rowsA, colsA, rowsB, colsB, 1, "c_per_matrix.txt"));
    printf("Thread per Row Time: %ld µs\n", matrix_mult(matrixA, matrixB, rowsA, colsA, rowsB, colsB, 2, "c_per_row.txt"));
    printf("Thread per Element Time: %ld µs\n", matrix_mult(matrixA, matrixB, rowsA, colsA, rowsB, colsB, 3, "c_per_element.txt"));

    free_matrix(matrixA, rowsA);
    free_matrix(matrixB, rowsB);

    return 0;
}
