import numpy as np


def get_part_of_array(X: np.ndarray) -> np.ndarray:
    return X[::4, 120:500:5]
    


def sum_non_neg_diag(X: np.ndarray) -> int:
    d = X.diagonal()
    non_neg_d = d[d >= 0]
    if non_neg_d.size > 0:
        return np.sum(non_neg_d)
    return -1


def replace_values(X: np.ndarray) -> np.ndarray:
    Y = X.copy()
    M = np.mean(X, axis=0)
    mask = (Y < 0.25*M) | (Y > 1.5*M)
    return np.where(mask, -1, Y)
 
