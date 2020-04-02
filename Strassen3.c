/* Create macros so that the matrices are stored in column-major order */
#include <stdlib.h>
#include <stdio.h>

#define A(i, j) a->arr[ (i)*a->size + (j) ]
#define B(i, j) b->arr[ (i)*b->size + (j) ]
#define C(i, j) c->arr[ (i)*c->size + (j) ]
#define D(i, j) d->arr[ (i)*d->size + (j) ]
#define R(i, j) r->arr[ (i)*r->size + (j) ]
#define R1(i, j) r1->arr[ (i)*r1->size + (j) ]
#define R2(i, j) r2->arr[ (i)*r2->size + (j) ]
#define R3(i, j) r3->arr[ (i)*r3->size + (j) ]

const int MIN_SIZE = 4;

typedef struct {
    double *arr;
    int size;
    int index;
} Matrix;

Matrix *make_matrix(int size) {
    Matrix *new = malloc(sizeof(Matrix));
    new->size = size;
    new->index = 0;
    new->arr = (double *) malloc(size * size * sizeof(double));
    return new;
}

Matrix *to_matrix(double *a, int size) {
    Matrix *new = malloc(sizeof(Matrix));
    new->size = size;
    new->arr = a;
    return new;
}

void free_matrix(Matrix *a) {
    free(a->arr);
    free(a);
}

void print_mat(Matrix *a) {
    for (int i = 0; i < a->size; i++) {
        for (int j = 0; j < a->size; j++) {
            printf("%f\t", A(i, j));
        }
        printf("\n");
    }
    printf("\n");
}

Matrix *sum_matrix(Matrix *a, Matrix *b) {
    Matrix *c = make_matrix(a->size);

    for (int i = 0; i < c->size; i++) {
        for (int j = 0; j < c->size; j++) {
            C(i, j) = A(i, j) + B(i, j);
        }
    }

    return c;
}

Matrix *subtract_matrix(Matrix *a, Matrix *b) {
    Matrix *c = make_matrix(a->size);

    for (int i = 0; i < c->size; i++) {
        for (int j = 0; j < c->size; j++) {
            C(i, j) = A(i, j) - B(i, j);
        }
    }

    return c;
}

Matrix *mult_matrix(Matrix *a, Matrix *b) {
    Matrix *c = make_matrix(a->size);

    for (int i = 0; i < c->size; i++) {
        for (int j = 0; j < c->size; j++) {
            for (int p = 0; p < c->size; p++) {
                C(i, j) += A(i, p) * B(p, j);
            }
        }
    }

    return c;
}

Matrix *compute_c11(Matrix *a, Matrix *b, Matrix *c, Matrix *d) {
    Matrix *r = make_matrix(a->size);

    for (int i = 0; i < c->size; i++) {
        for (int j = 0; j < c->size; j++) {
            R(i, j) = A(i, j) + B(i, j) - C(i, j) + D(i, j);
        }
    }

    return r;
}

Matrix *compute_c22(Matrix *a, Matrix *b, Matrix *c, Matrix *d) {
    Matrix *r = make_matrix(a->size);

    for (int i = 0; i < c->size; i++) {
        for (int j = 0; j < c->size; j++) {
            R(i, j) = A(i, j) - B(i, j) + C(i, j) + D(i, j);
        }
    }

    return r;
}

Matrix *subdivide(Matrix *a, int start_row, int start_col) {
    int size = a->size / 2;

    if (size < MIN_SIZE) {
        printf("Trying to divide matrix smaller than MIN_SIZE = %d\n", MIN_SIZE);
        exit(1);
    }

    Matrix *new = malloc(sizeof(Matrix));
    new->size = size;
    new->arr = (double *) malloc(size * size * sizeof(double));

    int end_row = start_row + size;
    int end_col = start_col + size;
    int new_index = 0;
    for (int i = start_row; i < end_row; i++) {
        for (int j = start_col; j < end_col; j++) {
            new->arr[new_index++] = A(i, j);
        }
    }
    return new;
}

Matrix *merge(Matrix *a, Matrix *b, Matrix *c, Matrix *d) {
    int size = a->size * 2;
    int half_size = size/2;
    Matrix *r = malloc(sizeof(Matrix));
    r->size = size;
    r->arr = (double *) malloc(size * size * sizeof(double));

    // c11
    int index = 0;
    for (int i = 0; i < half_size; i++) {
        for (int j = 0; j < half_size; j++) {
            R(i, j) = a->arr[index++];
        }
    }

    // c12
    index = 0;
    for (int i = 0; i < half_size; i++) {
        for (int j = half_size; j < size; j++) {
            R(i, j) = b->arr[index++];
        }
    }

    // c21
    index = 0;
    for (int i = half_size; i < size; i++) {
        for (int j = 0; j < half_size; j++) {
            R(i, j) = c->arr[index++];
        }
    }

    // c22
    index = 0;
    for (int i = half_size; i < size; i++) {
        for (int j = half_size; j < size; j++) {
            R(i, j) = d->arr[index++];
        }
    }

    return r;
}

void multiple_subdivides(Matrix *a, Matrix *b,
                      Matrix *a11, Matrix *a12, Matrix *a21, Matrix *a22,
                      Matrix *b11, Matrix *b12, Matrix *b21, Matrix *b22)
{
    int size = a->size;
    int half_size = size/2;

    for (size_t i = 0; i < size; i++) {
      for (size_t j = 0; j < size; j++) {
        if (i < half_size && j < half_size) {
          a11->arr[a11->index++] = A(i, j);
          b11->arr[b11->index++] = B(i, j);
        } else if (i < half_size && j >= half_size) {
          a12->arr[a12->index++] = A(i, j);
          b12->arr[b12->index++] = B(i, j);
        } else if (i >= half_size && j < half_size) {
          a21->arr[a21->index++] = A(i, j);
          b21->arr[b21->index++] = B(i, j);
        } else {
          a22->arr[a22->index++] = A(i, j);
          b22->arr[b22->index++] = B(i, j);
        }
      }
    }
}

void sum_matrices(Matrix *a, Matrix *b, Matrix *c, Matrix *d,
                  Matrix *r1, Matrix *r2, Matrix *r3)
{
  for (int i = 0; i < a->size; i++) {
      for (int j = 0; j < a->size; j++) {
          double _a = A(i, j);
          double _b = B(i, j);
          double _c = C(i, j);
          double _d = D(i, j);
          R1(i, j) = _a + _b;
          R2(i, j) = _c + _b;
          R3(i, j) = _a + _d;
      }
  }
}

Matrix *Strassen_MMult(Matrix *matrix_a, Matrix *matrix_b) {
    int size = matrix_a->size;
    int half_size = size/2;
    // Base case
    if (size <= MIN_SIZE) {
        return mult_matrix(matrix_a, matrix_b);
    }

    // Sub-divide
    Matrix *a11 = make_matrix(half_size);
    Matrix *a12 = make_matrix(half_size);
    Matrix *a21 = make_matrix(half_size);
    Matrix *a22 = make_matrix(half_size);
    Matrix *b11 = make_matrix(half_size);
    Matrix *b12 = make_matrix(half_size);
    Matrix *b21 = make_matrix(half_size);
    Matrix *b22 = make_matrix(half_size);
    multiple_subdivides(matrix_a, matrix_b, a11, a12, a21, a22, b11, b12, b21, b22);

    Matrix *a11_p_a22 = make_matrix(half_size);
    Matrix *a21_p_a22 = make_matrix(half_size);
    Matrix *a11_p_a12 = make_matrix(half_size);

    Matrix *b11_p_b22 = make_matrix(half_size);
    Matrix *b11_p_b12 = make_matrix(half_size);
    Matrix *b21_p_b22 = make_matrix(half_size);

    sum_matrices(a11, a22, a21, a12, a11_p_a22, a21_p_a22, a11_p_a12);
    sum_matrices(b11, b22, b21, a12, b11_p_b22, b21_p_b22, b11_p_b12);

    Matrix *a12_s_a22 = subtract_matrix(a12, a22);
    Matrix *a21_s_a11 = subtract_matrix(a21, a11);
    Matrix *b12_s_b22 = subtract_matrix(b12, b22);
    Matrix *b21_s_b11 = subtract_matrix(b21, b11);


    // Relation recursion
    Matrix *p1 = Strassen_MMult(a11_p_a22, b11_p_b22);
    Matrix *p2 = Strassen_MMult(b11, a21_p_a22);
    Matrix *p3 = Strassen_MMult(a11, b12_s_b22);
    Matrix *p4 = Strassen_MMult(a22, b21_s_b11);
    Matrix *p5 = Strassen_MMult(a11_p_a12, b22);
    Matrix *p6 = Strassen_MMult(a21_s_a11, b11_p_b12);
    Matrix *p7 = Strassen_MMult(a12_s_a22, b21_p_b22);

    free_matrix(a11);
    free_matrix(a12);
    free_matrix(a21);
    free_matrix(a22);
    free_matrix(b11);
    free_matrix(b12);
    free_matrix(b21);
    free_matrix(b22);

    free_matrix(a11_p_a22);
    free_matrix(b11_p_b22);
    free_matrix(a21_p_a22);
    free_matrix(b12_s_b22);
    free_matrix(b21_s_b11);
    free_matrix(a11_p_a12);
    free_matrix(a21_s_a11);
    free_matrix(b11_p_b12);
    free_matrix(a12_s_a22);
    free_matrix(b21_p_b22);

    // Merge
    Matrix *c11 = compute_c11(p1, p4, p5, p7);
    Matrix *c12 = sum_matrix(p3, p5);
    Matrix *c21 = sum_matrix(p2, p4);
    Matrix *c22 = compute_c22(p1, p2, p3, p6);

    free_matrix(p1);
    free_matrix(p2);
    free_matrix(p3);
    free_matrix(p4);
    free_matrix(p5);
    free_matrix(p6);
    free_matrix(p7);

    Matrix *r = merge(c11, c12, c21, c22);

    free_matrix(c11);
    free_matrix(c12);
    free_matrix(c21);
    free_matrix(c22);

    return r;
}

void MY_MMult(int m, int n, int k, double *a, int lda,
              double *b, int ldb,
              double *c_r, int ldc) {
    Matrix *matrix_a = to_matrix(a, lda);
    Matrix *matrix_b = to_matrix(b, ldb);

    Matrix *c = Strassen_MMult(matrix_a, matrix_b);

    for (int i = 0; i < c->size; i++) {
        for (int j = 0; j < c->size; j++) {
            c_r[i * ldc + j] = C(i, j);
        }
    }
}
