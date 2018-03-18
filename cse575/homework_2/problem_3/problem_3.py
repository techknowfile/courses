from sklearn.datasets import fetch_mldata
import numpy as np
mnist = fetch_mldata('MNIST original', data_home='./')

shuffle_indices = np.random.permutation(len(mnist.data))

np.random.seed(seed=0)

data = mnist.data[shuffle_indices]/255
targets = mnist.target[shuffle_indices]

train_data = data[:6000]
train_targets = targets[:6000]
test_data = data[-1000:]
test_targets = targets[-1000:]

def knn(train_data, train_targets, test_data, test_targets, k):
    correct_count = 0
    import math
    for i, datum in enumerate(test_data):
        distance = np.linalg.norm(datum - train_data, axis=-1)
        distance2 = np.linalg.norm(train_data-datum, axis=-1)
        min_idx = np.argpartition(distance, k-1)[:k]
        knn_targets = train_targets[min_idx]
        values, counts = np.unique(knn_targets, return_counts=True)
        classification = values[np.argmax(counts)]
        if classification == test_targets[i]:
            correct_count += 1

    print(correct_count/len(test_targets))

# def knn(train_data, train_targets, test_data, test_targets, k):
#     correct_count = 0
#     import math
#     for i, datum in enumerate(test_data):
#         distance = np.array([])
#         for j, train_datum in enumerate(train_data):
#             distance = np.append(distance, np.linalg.norm(datum-train_datum, axis=-1))
#         min_idx = np.argpartition(distance, k-1)[:k]
#         knn_targets = train_targets[min_idx]
#         values, counts = np.unique(knn_targets, return_counts=True)
#         classification = values[np.argmax(counts)]
#         if classification == test_targets[i]:
#             correct_count += 1

#     print(correct_count/len(test_targets))

# def knn(train_data, train_targets, test_data, test_targets, k):
#     correct_count = 0
#     import math
#     for i, datum in enumerate(test_data):
#         distance = np.array([])
#         for j, train_datum in enumerate(train_data):
#             cur_sum = 0.0
#             for k in range(len(datum)):
#                 cur_sum += (datum[k]-train_datum[k])**2
#             distance = np.append(distance, math.sqrt(cur_sum))
#         min_idx = np.argpartition(distance, k-1)[:k]
#         knn_targets = train_targets[min_idx]
#         values, counts = np.unique(knn_targets, return_counts=True)
#         classification = values[np.argmax(counts)]
#         if classification == test_targets[i]:
#             correct_count += 1

#     print(correct_count/len(test_targets))


knn(train_data, train_targets, test_data[:100], test_targets[:100], 13)
