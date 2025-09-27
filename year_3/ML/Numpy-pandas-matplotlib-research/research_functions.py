from collections import Counter
from typing import List


def are_multisets_equal(x: List[int], y: List[int]) -> bool:
    cnt1 = Counter(x)
    cnt2 = Counter(y)
    return cnt1 == cnt2


def max_prod_mod_3(x: List[int]) -> int:
    max = 0
    check = 0
    for i in range(len(x) - 1):
        if x[i] % 3 == 0 or x[i+1] % 3 == 0:
            if x[i]*x[i+1] > max:
                max = x[i]*x[i+1]
                check = 1
    if check:
        return max
    return -1


def convert_image(image: List[List[List[float]]], weights: List[float]) -> List[List[float]]:
    result = [[0.0 for i in range(len(image[0]))] for j in range(len(image))]
    for i in range (len(image)):
        for j in range (len(image[0])):
            for c in range (len(weights)):
                result[i][j] += image[i][j][c]*weights[c]

    return result

def rle_scalar(x: List[List[int]], y:  List[List[int]]) -> int:
    len1 = 0
    len2 = 0
    for i in range (len(x)):
        len1 += x[i][1]
    for i in range (len(y)):
        len2 += y[i][1]
    if len1 != len2:
        return -1
    len_v = len1
    i1 = 0
    i2 = 0
    result = 0
    len1 = x[0][1]
    len2 = y[0][1]
    for j in range (len_v):
        if len1 == 0:
            i1 += 1
            len1 = x[i1][1]
        if len2 == 0:
            i2 += 1
            len2 = y[i2][1]
        result += x[i1][0]*y[i2][0]
        len1 -= 1
        len2 -= 1
    return result

def cosine_distance(X: List[List[float]], Y: List[List[float]]) -> List[List[float]]:
    M = [[0.0 for i in range(len(Y))] for j in range(len(X))]
    for i in range (len(M)):
        for j in range (len(M[0])):
            if (all(r == 0  for r in X[i]) or all(r == 0 for r in Y[j])):
                M[i][j] = 1
            else:
                scalar = 0
                len1 = 0
                len2 = 0
                for z in range (len(X[0])):
                    scalar += X[i][z]*Y[j][z]
                    len1 += X[i][z]*X[i][z]
                    len2 += Y[j][z]*Y[j][z]
                len1 = len1 ** 0.5
                len2 = len2 ** 0.5
                M[i][j] = scalar / (len1*len2)
    return M


