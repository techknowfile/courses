import os
import math
from collections import defaultdict
import numpy as np
import pandas as pd

epochs = 1000
folds = 5
bank_data_csv = './data_banknote_authentication.txt'

def sigmoid(z):
    return 1/(1 + np.exp(-z))


class logisticRegression:
    def __init__(self, step_size=0.01):
        self.weights = None
        self.step_size = step_size

    def train(self, X, y):
        X['bias_input'] = 1
        self.weights = np.zeros(X.shape[1])
        X = X.as_matrix()
        y = y.as_matrix()
        for i in range(epochs):
            sig_minus_y = np.diag((sigmoid(X @ self.weights) - y))
            grad_of_J = np.mean(np.matmul(sig_minus_y, X), axis=0)
            self.weights-= self.step_size*grad_of_J

    def test(self, X, y):
        # TODO: check that model is trained
        X['bias_input'] = 1
        X = X.as_matrix()
        y = y.as_matrix()
        # p is the predicted values (classifications)
        p = sigmoid(X @ self.weights)
        p[p > 0.5] = 1
        p[p <= 0.5] = 0
        stats = dict(zip(*np.unique(np.equal(p, y), return_counts=True)))
        accuracy = stats[True]/len(X)
        return p, accuracy

class NaiveBayesClassifier:
    def __init__(self, index=4):
        self.class_attr_index = index

    def train(self, X, y):
        X = X.groupby(X[y == 1]).agg(['mean', 'std'])
        self.nb_param_dict = defaultdict(lambda: defaultdict(lambda: defaultdict(float)))

        # Learn priors
        class_counts = df[self.class_attr_index].value_counts()
        total = class_counts.sum()
        self.nb_param_dict["classes"] = class_counts.index.tolist()
        class_counts = class_counts.to_dict()
        for key in class_counts:
            self.nb_param_dict[("prior", key)] = class_counts[key]/total

        # Learn class conditionals
        for attr, stat in X:
            if attr is not self.class_attr_index:
                for i in range(len(X[(attr, stat)])):
                    # For each class value, save the attribute class conditional parameters into dict
                    self.nb_param_dict[attr][stat].update({i: X[(attr, stat)].iloc[i]})
        print(self.nb_param_dict)
        print("TESTING", df[df[4] == 1][0].mean())
        print(self.nb_param_dict[0]["mean"][1])

    def test(self, X, y):
        classifications = []
        for index, row in X.iterrows():
            probabilities = []
            for c in self.nb_param_dict["classes"]:
                prior = self.nb_param_dict[("prior", c)]
                print("class:", c, "prior:", prior)
                probability = prior
                for attr in row.index:
                    x = row[attr]
                    probability*=self.normal_likelihood(x, attr, c)
                probabilities.append(probability)
            # print(probabilities)
            print("probabilities:", probabilities)
            classification_index, classification_prob = max(enumerate(probabilities), key=lambda p: p[1])
            # print("predication:", self.nb_param_dict["classes"][classification_index], "true label:", y[index])
            classifications.append(self.nb_param_dict["classes"][classification_index])
        classifications = np.array(classifications)
        print(y)
        stats = dict(zip(*np.unique(np.equal(classifications, y), return_counts=True)))
        accuracy = stats[True]/len(X)
        print(accuracy)
   
    def normal_likelihood(self, x, attr, c):
        mean = self.nb_param_dict[attr]["mean"][c]
        variance = self.nb_param_dict[attr]["std"][c]
        likelihood = math.e**(-(x-mean)**2/(2*variance))/math.sqrt(2*math.pi*variance)
        print(x, attr, c, likelihood)
        return likelihood


def cross_validation(X, y, n_folds):
    """cross_validation

    :param estimator: function to fit the data
    :param X: A DataFrame containing the dataset you wish to fit
    :param y: A DataFrame containing the corresponding labels for supervised learning 
    :param n_folds: Number of folds for cross validation
    """

    scores = []
    X_splits = [X.sample(frac=1/n_folds) for _ in range(n_folds)]
    y_splits = [y.iloc[X_split.index] for X_split in X_splits]
    for i in range(n_folds):
        X_train = pd.concat(X_splits[:i] + X_splits[i+1:])
        X_val = X_splits[i]
        y_train = pd.concat(y_splits[:i] + y_splits[i+1:])
        y_val = y_splits[i]

        lr_classifier = logisticRegression()
        lr_classifier.train(X_train, y_train)
        results, accuracy = lr_classifier.test(X_val, y_val)

        scores += [accuracy]

    return scores

def main():
    df = pd.read_csv(bank_data_csv, header=None)

    X = df.iloc[:, :4]
    y = df.iloc[:, 4]

    # Train and test logistic regression model
    # scores = cross_validation(X, y, folds)
    # print(scores)
    # print(sum(scores)/len(scores))

    # Train and test Naive Bayes classification model
    nb = NaiveBayesClassifier()
    nb.train(X, y)
    nb.test(X, y)

if __name__ == '__main__':
    main()
