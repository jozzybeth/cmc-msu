import numpy as np
from sklearn.svm import SVC


def train_svm_and_predict(train_features, train_target, test_features):
    X = train_features[:360, [3, 4]]
    Y = train_target[:360]
    X_test = test_features[:, [3, 4]]
    model = SVC(C=1, kernel='rbf')
    model.fit(X, Y)
    return model.predict(X_test)
