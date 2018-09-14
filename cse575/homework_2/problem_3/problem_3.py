from sklearn.datasets import fetch_mldata
from matplotlib import pyplot as plt
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

class knn_classifier:
    """"""
    def __init__(self, train_data, train_targets):
        self.train_data = train_data
        self.train_targets = train_targets
        self.distance = None
        self.recalculate_distance = True # Lazy way of not recalculating
                                      # distances if no new train_data
                                      # have been seen. Only done this
                                      # way to speed up completion of
                                      # homework assignment
        self.train_distance = None
        self.recalculate_train_distance = True

    def predict(self, test_data, test_targets, k_list):
        """"""
        results = []
        for i, datum in enumerate(test_data):
            self.distance = np.linalg.norm(datum.astype('float64') - train_data, axis=-1)

            datum_results = [] # Classification for each k
            for k in k_list:
                correct_count = 0
                # classifications = []
                min_idx = np.argpartition(self.distance, k-1)[:k]
                knn_targets = train_targets[min_idx]
                values, counts = np.unique(knn_targets, return_counts=True)
                classification = values[np.argmax(counts)]
                # classifications.append(classification)
                # if classification == test_targets[i]:
                #     correct_count += 1

                datum_results.append(classification)
            results.append(datum_results)

        classifications_per_k = list(map(list, zip(*results)))
        accuracies_per_k = [sum([classification[i] == test_targets[i] for i in range(len(classification))])/len(test_targets) for classification in classifications_per_k]

        return classifications_per_k, accuracies_per_k

model = knn_classifier(train_data, train_targets)

k_list = [1, 9, 19, 29, 39, 49, 59, 69, 79, 89, 99]
test_classifications, test_accuracies = model.predict(test_data, test_targets, k_list)
train_classifications, train_accuracies = model.predict(train_data, train_targets, k_list)

# train_accuracy = map(lambda x: x[1], train_result)

plt.plot(k_list, [1-accuracy for accuracy in test_accuracies], label='Test error')
plt.plot(k_list, [1-accuracy for accuracy in train_accuracies], label='Train error')
# plt.plot(k_list, train_accuracy, label='Train accuracy')
plt.legend()
plt.xlabel('K')
plt.ylabel('Error rate')
plt.xticks(k_list)
plt.show()
