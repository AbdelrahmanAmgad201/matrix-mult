#include <stdio.h>
#include <stdlib.h>

void read_matrix_from_file(const char *filename, int ***matrix, int *rows, int *cols) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the first line to get the matrix dimensions
    if (fscanf(file, "row=%d col=%d", rows, cols) != 2) {
        fprintf(stderr, "Invalid file format\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the matrix
    *matrix = (int **)malloc(*rows * sizeof(int *));
    for (int i = 0; i < *rows; i++) {
        (*matrix)[i] = (int *)malloc(*cols * sizeof(int));
    }

    // Read the matrix values
    for (int i = 0; i < *rows; i++) {
        for (int j = 0; j < *cols; j++) {
            if (fscanf(file, "%d", &((*matrix)[i][j])) != 1) {
                fprintf(stderr, "Error reading matrix data\n");
                fclose(file);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);
}

void print_matrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

void free_matrix(int **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}
void write_matrix_to_file(const char *filename, int **matrix, int rows, int cols) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    // Write the matrix dimensions
    fprintf(file, "row=%d col=%d\n", rows, cols);

    // Write the matrix data
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d", matrix[i][j]);
            if (j < cols - 1) {
                fprintf(file, "\t");  // Use tabs for separation
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}
long solve_per_matrix(){
    return 0;
}
long solve_per_row(){
    return 0;
}
long solve_per_element(){
    return 0;
}
int main() {
    int **matrixA, **matrixB;
    int rowsA, colsA, rowsB, colsB;

    // Read matrices from files
    read_matrix_from_file("a.txt", &matrixA, &rowsA, &colsA);
    read_matrix_from_file("b.txt", &matrixB, &rowsB, &colsB);

    // Print matrices
    printf("Matrix A (%d x %d):\n", rowsA, colsA);
    print_matrix(matrixA, rowsA, colsA);

    printf("\nMatrix B (%d x %d):\n", rowsB, colsB);
    print_matrix(matrixB, rowsB, colsB);


    write_matrix_to_file("amgad.txt", matrixA, 10 , 5);

    // Free allocated memory
    free_matrix(matrixA, rowsA);
    free_matrix(matrixB, rowsB);

    

    return 0;
}
