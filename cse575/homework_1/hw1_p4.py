import numpy as np
X = np.array([[1., 0., 1.], [0., -1., 1.], [0., 1., 1.], [-1., 0., 1.]])
y = np.array([1., 1., 0., 0.])
w = np.array([1., 1., 0.])


def sigmoid(z):
    return 1/(1 + np.exp(-z))

for i in range(1000000000):
    z = X @ w
    sig_minus_y = sigmoid(z) - y
    sig_minus_y = np.diag(sig_minus_y)

    grad_of_J = np.mean(np.matmul(sig_minus_y, X), axis=0)
    # print(grad_of_J)
    w -= 0.01*grad_of_J
    if i % 1000 == 0:
        print(w)
