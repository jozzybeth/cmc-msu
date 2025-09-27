import numpy as np


def evaluate_measures(sample):
    _, counts = np.unique(sample, return_counts=True)
    p = counts / len(sample)
    measures = {'gini': float(1 - np.sum(p ** 2)), 'entropy': float(-np.sum(p * np.log(p))), 'error': float(1 - np.max(p))}
    return measures
