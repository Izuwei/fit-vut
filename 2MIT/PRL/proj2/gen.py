#!/usr/bin/python3
import random
from sys import argv

try:
    mat1_rows, mat1_cols = (int(argv[1]), int(argv[2]))
    mat2_rows, mat2_cols = (int(argv[3]), int(argv[4]))
except:
    mat1_rows, mat1_cols = (2, 3)
    mat2_rows, mat2_cols = (3, 4)

mat3_rows, mat3_cols = mat1_rows, mat2_cols

assert(mat1_cols == mat2_rows)

def mat(rows, cols):
    return [[0 for j in range(cols)] for i in range(rows)]

def randfill(mat, rows, cols):
    for i in range(rows):
        for j in range(cols):
            mat[i][j] = random.randint(-100, 100)

def mat2str(mat, rows, cols):
    return '\n'.join(' '.join(str(mat[i][j]) for j in range(cols)) for i in range(rows))

mat1 = mat(mat1_rows, mat1_cols)
mat2 = mat(mat2_rows, mat2_cols)
mat3 = mat(mat3_rows, mat3_cols)

randfill(mat1, mat1_rows, mat1_cols)
randfill(mat2, mat2_rows, mat2_cols)
for i in range(mat3_rows):
    for j in range(mat3_cols):
        for k in range(mat1_cols):
            mat3[i][j] += mat1[i][k] * mat2[k][j]

f = open('mat1', 'w')
f.write(str(mat1_rows) + '\n')
f.write(mat2str(mat1, mat1_rows, mat1_cols) + '\n')
f.close()

f = open('mat2', 'w')
f.write(str(mat2_cols) + '\n')
f.write(mat2str(mat2, mat2_rows, mat2_cols) + '\n')
f.close()

f = open('mat3', 'w')
f.write(str(mat3_rows) + ':' + str(mat3_cols) + '\n')
f.write(mat2str(mat3, mat3_rows, mat3_cols) + '\n')
f.close()

