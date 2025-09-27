from typing import List
from copy import deepcopy


def get_part_of_array(X: List[List[float]]) -> List[List[float]]:
    start_col = 120
    end_col = 500
    rows = [X[i] for i in range(0, len(X), 4)]
    Y = [[row[i] for i in range (start_col,end_col) if (i - start_col) % 5 == 0] for row in rows]
    return Y


def sum_non_neg_diag(X: List[List[int]]) -> int:
    sum = 0
    col = len(X[0])
    flag = 0
    for i in range(0,len(X)):
        if i < col:
            if X[i][i] >= 0:
                sum += X[i][i]
                flag = 1
    if flag:
        return sum
    return -1



def replace_values(X: List[List[float]]) -> List[List[float]]:
    Y = deepcopy(X)
    for j in range (len(X[0])):
        M = 0
        for i in range(len(X)):
            M += X[i][j]
        M /= len(X)
        for i in range(len(X)):
            if X[i][j] > 1.5*M or X[i][j] < 0.25*M:
                Y[i][j] = -1
    return Y
        