import numpy as np
import typing
from collections import defaultdict


def kfold_split(num_objects: int,
                num_folds: int) -> list[tuple[np.ndarray, np.ndarray]]:
    index = np.array(list(range(0, num_objects)))
    result: list[tuple[np.ndarray, np.ndarray]] = []
    i = 0
    a = num_objects // num_folds
    for j in range(num_folds):
        if (j == num_folds - 1):
            array2 = index[i:]
        else:
            array2 = index[i:i+a]
        array1 = index[~np.isin(index, array2)]
        result.append((array1, array2))
        i += a
    return result


def knn_cv_score(X: np.ndarray, y: np.ndarray, parameters: dict[str, list],
                 score_function: callable,
                 folds: list[tuple[np.ndarray, np.ndarray]],
                 knn_class: object) -> dict[str, float]:
    result = {}
    for n_neighbors in parameters['n_neighbors']:
        for metric in parameters['metrics']:
            for weight in parameters['weights']:
                for normalizer, normalizer_name in parameters['normalizers']:
                    scores = []
                    for train, test in folds:
                        X_train = X[train]
                        X_test = X[test]
                        y_train = y[train]
                        y_test = y[test]
                        if normalizer:
                            normalizer.fit(X_train)
                            X_train = normalizer.transform(X_train)
                            X_test = normalizer.transform(X_test)
                        model = knn_class(n_neighbors=n_neighbors, metric=metric, weights=weight)
                        model.fit(X_train, y_train)
                        y_pred = model.predict(X_test)
                        score = score_function(y_test, y_pred)
                        scores.append(score)
                    mean_score = np.mean(scores)
                    result[(normalizer_name, n_neighbors, metric, weight)] = mean_score
    return result
