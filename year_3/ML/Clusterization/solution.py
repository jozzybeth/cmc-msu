import numpy as np

import sklearn
import sklearn.metrics


def silhouette_score(x, labels):
    '''
    :param np.ndarray x: Непустой двумерный массив векторов-признаков
    :param np.ndarray labels: Непустой одномерный массив меток объектов
    :return float: Коэффициент силуэта для выборки x с метками labels
    '''

    uniq, count_clusters = np.unique(labels, return_counts=True)

    if len(uniq) <= 1:
        return 0

    matrix = sklearn.metrics.pairwise_distances(x)
    sum_elem = np.zeros((len(matrix), len(uniq)))
    size_cluster = np.zeros(len(matrix))
    mask = np.zeros((len(matrix), len(uniq)), dtype=bool)

    for i, cluster in enumerate(uniq):
        cluster_mask = labels == cluster
        size_cluster[cluster_mask] = np.sum(cluster_mask)
        sum_elem[:, i] = np.sum(matrix[cluster_mask], axis=0)
        mask[:, i] = cluster_mask

    one = size_cluster == 1
    s = sum_elem[mask]
    s[one] = 0
    s[~one] /= size_cluster[~one] - 1

    d = np.min((sum_elem / count_clusters)[~mask].reshape((len(matrix), len(uniq) - 1)), axis=1)
    d[one] = 0

    m = np.maximum(d, s)
    zero = m == 0

    sil = (d - s)[~zero] / m[~zero]
    sil_score = np.sum(sil) / len(matrix)

    return sil_score


def bcubed_score(true_labels, predicted_labels):
    '''
    :param np.ndarray true_labels: Непустой одномерный массив меток объектов
    :param np.ndarray predicted_labels: Непустой одномерный массив меток объектов
    :return float: B-Cubed для объектов с истинными метками true_labels и предсказанными метками predicted_labels
    '''

    true_uniq, true_index, true_counts = np.unique(true_labels, return_inverse=True, return_counts=True)
    pred_uniq, pred_index, pred_counts = np.unique(predicted_labels, return_inverse=True, return_counts=True)

    corr = (true_labels.reshape(1, len(true_labels)) == true_labels.reshape(len(true_labels), 1)) & \
           (predicted_labels.reshape(1, len(predicted_labels)) == predicted_labels.reshape(len(predicted_labels), 1))

    rec = np.mean(np.sum(corr, axis=0) / true_counts[true_index])
    prec = np.mean(np.sum(corr, axis=0) / pred_counts[pred_index])

    score = 0
    if rec + prec != 0:
        score = 2 * prec * rec / (rec + prec)

    return score
1