import numpy as np


class Preprocessor:

    def __init__(self):
        pass

    def fit(self, X, Y=None):
        pass

    def transform(self, X):
        pass

    def fit_transform(self, X, Y=None):
        pass


class MyOneHotEncoder(Preprocessor):

    def __init__(self, dtype=np.float64):
        super(Preprocessor).__init__()
        self.dtype = dtype

    def fit(self, X, Y=None):
        self.unique_features = {}
        for col in range(X.shape[1]):
            self.unique_features[col] = list(np.unique(X.iloc[:, col]))

    def transform(self, X):
        encoded_data = np.empty((0, sum(len(categories) for categories in self.unique_features.values())))
        for row in range(X.shape[0]):
            one_hot_row = []
            for col in range(X.shape[1]):
                categories = self.unique_features[col]
                one_hot = [1 if X.iloc[row, col] == category else 0 for category in categories]
                one_hot_row = np.hstack([one_hot_row, one_hot])
            encoded_data = np.vstack([encoded_data, one_hot_row])
        return encoded_data

    def fit_transform(self, X, Y=None):
        self.fit(X)
        return self.transform(X)

    def get_params(self, deep=True):
        return {"dtype": self.dtype}


class SimpleCounterEncoder:

    def __init__(self, dtype=np.float64):
        self.dtype = dtype

    def fit(self, X, Y=None):
        self.counters = {}
        for column in X.columns:
            self.counters[column] = {}
            for value in X[column].unique():
                mask = (X[column] == value)
                mean_y = Y[mask].mean()
                frequency = mask.mean()
                self.counters[column][value] = [mean_y, frequency]

    def transform(self, X, a=1e-5, b=1e-5):
        n_objects, n_features = X.shape
        result = np.zeros((n_objects, 3 * n_features))
        for i, column in enumerate(X.columns):
            for j, value in enumerate(X[column]):
                mean_target, fraction = self.counters[column][value]
                idx = 3 * i
                result[j, idx] = mean_target
                result[j, idx + 1] = fraction
                result[j, idx + 2] = (mean_target + a) / (fraction + b)
        return result

    def fit_transform(self, X, Y, a=1e-5, b=1e-5):
        self.fit(X, Y)
        return self.transform(X, a, b)

    def get_params(self, deep=True):
        return {"dtype": self.dtype}


def group_k_fold(size, n_splits=3, seed=1):
    idx = np.arange(size)
    np.random.seed(seed)
    idx = np.random.permutation(idx)
    n_ = size // n_splits
    for i in range(n_splits - 1):
        yield idx[i * n_: (i + 1) * n_], np.hstack((idx[:i * n_], idx[(i + 1) * n_:]))
    yield idx[(n_splits - 1) * n_:], idx[:(n_splits - 1) * n_]


class FoldCounters:

    def __init__(self, n_folds=3, dtype=np.float64):
        self.dtype = dtype
        self.n_folds = n_folds

    def fit(self, X, Y, seed=1):
        self.fold_counters = []
        for fold_idx, rest_idx in group_k_fold(X.shape[0], self.n_folds, seed):
            X_fold, Y_fold = X.iloc[rest_idx], Y.iloc[rest_idx]
            fold_counter = {}
            for column in X.columns:
                column_counter = {}
                unique_values = X_fold[column].unique()
                for value in unique_values:
                    mask = (X_fold[column] == value)
                    mean_target = Y_fold[mask].mean()
                    fraction = mask.mean()
                    column_counter[value] = [mean_target, fraction]
                fold_counter[column] = column_counter
            self.fold_counters.append((fold_idx, fold_counter))

    def transform(self, X, a=1e-5, b=1e-5):
        n_objects, n_features = X.shape
        result = np.zeros((n_objects, 3 * n_features))
        for fold_idx, fold_counter in self.fold_counters:
            for i, column in enumerate(X.columns):
                column_counters = fold_counter[column]
                for j in fold_idx:
                    value = X.iloc[j, i]
                    mean_target, fraction = column_counters[value]
                    idx = 3 * i
                    result[j, idx] = mean_target
                    result[j, idx + 1] = fraction
                    result[j, idx + 2] = (mean_target + a) / (fraction + b)

        return result

    def fit_transform(self, X, Y, a=1e-5, b=1e-5):
        self.fit(X, Y)
        return self.transform(X, a, b)


def weights(x, y):
    unique_values = np.unique(x)
    ohe_x = np.eye(len(unique_values))[x]
    weights = np.zeros(len(unique_values))
    lr = 1e-2
    n_iterations = 2000
    for _ in range(n_iterations):
        predictions = np.dot(ohe_x, weights)
        error = predictions - y
        grad = np.dot(ohe_x.T, error)
        weights -= lr * grad

    return weights
