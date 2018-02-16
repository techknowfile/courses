import numpy as np
X = np.array([[1., 0., 1.], [0., -1., 1.], [0., 1., 1.], [-1., 0., 1.]])
y = np.array([1., 1., 0., 0.])
w = np.array([200000.0, -200000.0, 0.])


def sigmoid(z):
    return 1/(1 + np.exp(-z))

for i in range(1000000000):
    z = X @ w
    sig_minus_y = sigmoid(z) - y

    grad_of_J = X.T @ sig_minus_y / X.shape[1]
    # print(grad_of_J)
    w -= 0.01*grad_of_J
    if i % 100000 == 0:
        print(w)
