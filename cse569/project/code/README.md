# Neural Network Framework (from scratch)
Generates a neural network model using a specified architecture, with support for dropout and hyperparameter specification

## Prerequisites
- Python 3.x
- numpy
- tensorflow
  note: tensorflow is only used to download and load the MNIST dataset

## Getting Started
1. Running "python nn.py" will train the neural network with 50,000 MNIST training images using stochastic gradient descent. After each epoch, the re-substitution accuracy and validation accuracy are displayed and plotted in an image stored in the images folder. After training is complete, the model will be saved to the "model" file in the main directory.
2. Running "python run_test.py" will calculate the testing accuracy of the model
3. Running "python run_train.py" will calculate the resubstitution error of the model

# Authors
* **Andrew Dudley**, **Sailik Sengupta**

## Acknowledgements
* Andrej Karpathy's "A Hacker's Guide to Neural Networks" for inspiring the structure of this framework
