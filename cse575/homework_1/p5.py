import warnings
from matplotlib import pyplot as plt
warnings.filterwarnings('ignore', category=RuntimeWarning)
import os
import math
from collections import defaultdict
import numpy as np
import pandas as pd
from IPython.display import display
from IPython.core.pylabtools import figsize, getfigs

epochs = 1000
folds = 5
bank_data_csv = './data_banknote_authentication.txt'

def sigmoid(z):
    return 1/(1 + np.exp(-z))


class LogisticRegression:
    """LogisticRegression"""
    def __init__(self, step_size=0.01):
        self.weights = None
        self.step_size = step_size

    def train(self, X, y):
        X = X.copy()
        X['bias_input'] = 1
        self.weights = np.zeros(X.shape[1])
        X = X.as_matrix()
        y = y.as_matrix()
        for i in range(epochs):
            sig_minus_y = (sigmoid(X @ self.weights) - y)
            grad_of_J = np.matmul(X.T, sig_minus_y)/len(X.T)
            self.weights-= self.step_size*grad_of_J

    def test(self, X, y):
        # TODO: check that model is trained
        X = X.copy()
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

    def predict(self, x, y):
        pass

class NaiveBayesClassifier:
    """NaiveBayesClassifier"""
    def train(self, X, y):
        """train

        :param X:
        :param y:
        """
        attributes = list(X)
        self.X_train = X
        self.y_train = y

        X = X.groupby(y).agg(['mean', 'std'])
        self.nb_param_dict = defaultdict(lambda: defaultdict(lambda: defaultdict(float)))

        # Learn priors
        class_counts = y.value_counts()
        total = class_counts.sum()
        self.nb_param_dict["classes"] = class_counts.index.tolist()
        self.nb_param_dict["attributes"] = attributes
        class_counts = class_counts.to_dict()
        for key in class_counts:
            self.nb_param_dict[("prior", key)] = class_counts[key]/total

        # Learn class conditionals
        for attr, stat in X:
            for i in range(len(X[(attr, stat)])):
                # For each class value, save the attribute class conditional parameters into dict
                self.nb_param_dict[attr][stat].update({i: X[(attr, stat)].iloc[i]})
        # print(self.nb_param_dict)
        # print("TESTING", df[df[4] == 1][0].mean())
        # print(self.nb_param_dict[0]["mean"][1])

    def test(self, X, y):
        """test

        :param X:
        :param y:
        """
        classifications = []
        for index, row in X.iterrows():
            probabilities = []
            for c in self.nb_param_dict["classes"]:
                prior = self.nb_param_dict[("prior", c)]
                probability = prior
                for attr in row.index:
                    x = row[attr]
                    probability*=self.normal_likelihood(x, attr, c)
                probabilities.append(probability)
            classification_index, classification_prob = max(enumerate(probabilities), key=lambda p: p[1])
            classifications.append(self.nb_param_dict["classes"][classification_index])
        classifications = np.array(classifications)
        stats = dict(zip(*np.unique(np.equal(classifications, y), return_counts=True)))
        accuracy = stats[True]/len(y)
        return classifications, accuracy
    def normal_likelihood(self, x, attr, c):
        mean = self.nb_param_dict[attr]["mean"][c]
        variance = self.nb_param_dict[attr]["std"][c]
        likelihood = math.e**(-(x-mean)**2/(2*variance))/math.sqrt(2*math.pi*variance)
        return likelihood

    def generate_data(self, n, c):
        """generate_data

        :param n: number of data points to sample
        :param c: target class
        """
        mus = [self.nb_param_dict[attr]["mean"][c] for attr in self.nb_param_dict["attributes"]]
        sigmas = [self.nb_param_dict[attr]["std"][c] for attr in self.nb_param_dict["attributes"]]
        data = np.random.normal(mus, sigmas, [n, len(mus)])
        df = pd.DataFrame(data)
        self.generated_data = df
        return df

def cross_validation(estimator, X, y, n_folds, return_models=False):
    """cross_validation

    :param estimator: function to fit the data
    :param X: A DataFrame containing the dataset you wish to fit
    :param y: A DataFrame containing the corresponding labels for supervised learning 
    :param n_folds: Number of folds for cross validation
    """

    scores = []
    models = []
    X_splits = [X.sample(frac=1/n_folds) for _ in range(n_folds)]
    y_splits = [y.iloc[X_split.index] for X_split in X_splits]
    for i in range(n_folds):
        X_train = pd.concat(X_splits[:i] + X_splits[i+1:])
        X_val = X_splits[i]
        y_train = pd.concat(y_splits[:i] + y_splits[i+1:])
        y_val = y_splits[i]

        classifier = estimator()
        classifier.train(X_train, y_train)
        results, accuracy = classifier.test(X_val, y_val)
        models += [classifier]
        scores += [accuracy]

    print(scores)
    return (scores, models) if return_models else scores,


def main():
    """main"""
    df = pd.read_csv(bank_data_csv, header=None)

    X = df.iloc[:, :4]
    y = df.iloc[:, 4]

    # Train and test logistic regression model
    scores, = cross_validation(LogisticRegression, X, y, folds)
    print(sum(scores)/len(scores))

    
    # plot_learning_curves(df)

    # # Train and test Naive Bayes classification model
    # scores, models = cross_validation(NaiveBayesClassifier, X, y, folds, return_models=True)
    # print("Average scores:", sum(scores)/len(scores))
    # for fold, model in enumerate(models):
    #     print("===============================================")
    #     print("Naive Bayes -- Fold " + str(fold+1))
    #     print("-----------------------------------------------")
    #     print("Learned parameters")
    #     print(model.X_train[model.y_train == 1].agg(['mean', 'var']))
    #     gen_df = model.generate_data(400, 1) # Generate 400 samples of class 1 from model
    #     gen_df_params = gen_df.agg(['mean', 'var'])
    #     print("-----------------------------------------------")
    #     print("Generated data parameters")
    #     print(gen_df_params)
    #     print("-----------------------------------------------")
    

def plot_learning_curves(df):
    # Plot learning curves
    training_percentages=[0.01, 0.02, 0.05, 0.1, 0.625, 1]
    num_of_runs = 5

    train = df.sample(frac=0.9)
    test = df.drop(train.index)
    X_test = test.iloc[:, :4]
    y_test = test.iloc[:, 4]
    X_train = train.iloc[:, :4]
    y_train = train.iloc[:, 4]

    results = {"lr": [], "lr_training": [], "nb": [], "nb_training": []}

    for percentage in training_percentages:
        results_per_run = {"lr": [], "lr_training": [], "nb": [],
                           "nb_training": []}
        for _ in range(num_of_runs):
            train_frac = train.sample(frac=percentage, replace=True)
            X_train_frac = train_frac.iloc[:, :4]
            y_train_frac = train_frac.iloc[:, 4]

            # NaiveBayesClassifier
            nb_classifier = NaiveBayesClassifier()
            nb_classifier.train(X_train_frac, y_train_frac)
            # Test set
            run_results, accuracy = nb_classifier.test(X_test, y_test)
            results_per_run["nb"].append(accuracy)
            # Training set
            run_results, accuracy = nb_classifier.test(X_train, y_train)
            results_per_run["nb_training"].append(accuracy)

            # Logistic Regression
            lr_classifier = LogisticRegression()
            lr_classifier.train(X_train_frac, y_train_frac)
            # Test set
            run_results, accuracy = lr_classifier.test(X_test, y_test)
            results_per_run["lr"].append(accuracy)
            # Training set
            run_results, accuracy = lr_classifier.test(X_train, y_train)
            results_per_run["lr_training"].append(accuracy)

        results["lr"].append(sum(results_per_run["lr"])/num_of_runs)
        results["lr_training"].append(sum(results_per_run["lr_training"])/num_of_runs)
        results["nb"].append(sum(results_per_run["nb"])/num_of_runs)
        results["nb_training"].append(sum(results_per_run["nb_training"])/num_of_runs)

    print(results["lr"], results["nb"])
    plt.plot(training_percentages, results["lr"], "r-+", linewidth=2,
             label="Logistic regression - Test accuracy")
    plt.plot(training_percentages, results["lr_training"], "r:", linewidth=2,
             label="Logistic regression - Training accuracy")
    plt.plot(training_percentages, results["nb"], "b-", linewidth=3,
             label="Naive Bayes - Test accuracy")
    plt.plot(training_percentages, results["nb_training"], "b:", linewidth=3,
             label="Naive Bayes - Training accuracy")
    plt.legend()
    plt.ylabel('Accuracy')
    plt.xlabel('Percentage of training set used')
    plt.show()

if __name__ == '__main__':
    main()
