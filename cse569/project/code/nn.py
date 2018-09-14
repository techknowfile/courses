import numpy as np, math
import argparse
import pickle
import matplotlib
matplotlib.use('agg')
from matplotlib import pyplot as plt

import os, sys
from tensorflow.examples.tutorials.mnist import input_data


mnist = input_data.read_data_sets("MNIST_data/", one_hot = True)

alpha = 0.1
class NeuralNetwork:
    def __init__(self, *args, dropout=False):
        # Layers of the neural network (doesn't include an input layer)
        self.train_loss = []
        self.validation_loss = []
        self.layers = []

        self.image_count = 0

        current_wire = None
        for i, (layerType, units) in enumerate(*args):
            # For each layer defined, create the layer and attach it to the output wires
            # of the preceding layer

            if i == 0:
                # Create wires going from inputs to first hidden layer
                current_wire = Wire(units)
            else:
                new_layer = {
                    'sigmoid': sigmoidLayer(current_wire, units, dropout=dropout),
                    'softmax': softmaxLayer(current_wire, units, dropout=dropout),
                    'relu': reluLayer(current_wire, units, dropout=dropout)
                }.get(layerType, None)
                current_wire = new_layer.outputWire
                self.layers.append(new_layer)

    def train_and_validate(self, train_data, validation_data, epochs):
        global alpha
        alpha = 0.01
        for epoch in range(epochs):
            print("Epoch:", epoch)
            self.train(train_data)
            self.test(train_data, loss_list=self.train_loss)
            self.test(validation_data, loss_list=self.validation_loss)
            plt.clf()
            plt.plot(self.train_loss, label="Training set")
            plt.plot(self.validation_loss, label="Validation set")
            plt.legend(loc="upper right")
            plt.xlabel("Epoch")
            plt.ylabel("Cross-entropy loss")
            if not os.path.exists("images"):
                os.mkdir("images")
            plt.savefig('images/training_{:02d}.png'.format(self.image_count))
            self.image_count += 1
            alpha *= 1

    def train(self, data):
        for i, (input, label) in enumerate(zip(data[0], data[1])):
            output = self.forward_pass(input, training=True)
            errorGradient = self.calculate_error(output, label)  # TODO: Modify this when gradient calculation is resolved
            self.backward_pass(errorGradient)


    def test(self, data, loss_list=None):
            validation_loss = []
            correct_count = 0
            for i, (input, label) in enumerate(zip(data[0], data[1])):
                # print (input)
                output = self.forward_pass(input)
                validation_loss.append(-label.T.dot(np.log(output)))
                if np.argmax(output) == np.argmax(label):
                    correct_count += 1
            if not loss_list == None:
                loss_list.append(sum(validation_loss) / float(len(validation_loss)))
            print(correct_count / data[0].shape[0])
            print()

    def forward_pass(self, input, training=False):
        current_output = input
        for i, layer in enumerate( self.layers ):
            current_output = layer.forward(current_output, training=training)
        return current_output

    def calculate_error(self, prediction, label):
        return  label #TODO: Modify when gradient issue resolved

    def backward_pass(self, errorGradient):
        for i, layer in enumerate(self.layers[::-1]):
            layer.backward()

    def saveModel(self):
        pickle.dump( self, open ("model", "wb"))

    @staticmethod
    def load_model():
        return pickle.load(open("model", "rb"))


class Wire:
    def __init__(self, input_dimensions ):
        self.input_dimensions = input_dimensions

    def initialize( self, units, fn ):
        self.dropout_proba = 0.5
        self.units = units
        self.weights = fn(units, self.input_dimensions)
        self.gradients = None
        self.velocity = np.zeros(self.weights.shape)

    def generate_dropout_variables(self):
        self.dropout_vector = np.random.choice([0, 1], size=(self.units, ), p=[self.dropout_proba, 1-self.dropout_proba])
        self.dropout_matrix = np.diag(self.dropout_vector)


class Layer:
    def __init__( self, inputWire, units, dropout=False, weight_fn = lambda x, y: np.random.randn(x, y) / np.sqrt(x)):
        if not inputWire:
            raise TypeError("Must initialize layer with Wire")
        self.dropout = dropout
        self.inputWire = inputWire
        self.inputWire.initialize(units, fn=weight_fn)
        self.biases = np.zeros(self.inputWire.units)

        self.outputWire = Wire(units)

    def forward( self, x, training=False):
        raise NotImplementedError("Must implement forward-pass function -- forward( self,  )")
    def backward( self ):
        raise NotImplementedError("Must implement backward-pass function -- backward( self,  )" )

class sigmoidLayer( Layer ):
    def __init__(self, inputWire, units, dropout=False):
        super().__init__(inputWire, units, dropout)

    def forward(self, x, training=False):
        self.inputs = x
        # Add input value of "1" to the input array for the bias weight (w_0*x_0 + ...)
        self.z = np.dot(self.inputWire.weights, self.inputs) + self.biases

        self.outputs = self.sigmoid(self.z)
        if training and self.dropout:
            self.inputWire.generate_dropout_variables()
            # self.outputs = self.outputs.dot(self.inputWire.dropout_matrix)
            self.outputs = self.outputs*self.inputWire.dropout_vector / self.inputWire.dropout_proba
        return self.outputs

    def sigmoid( self, x ):
        # Calculate sigmoid on vector cells
        self.sig_values = 1 / (1 + np.exp(-x))
        return self.sig_values
        
    def backward( self ):
        dA_dz = self.sig_values * (1 - self.sig_values)
        if self.dropout:
            d_p_wrt_f = dA_dz*self.inputWire.dropout_vector
        self.dz = self.outputWire.gradients * dA_dz
        dA = np.dot(self.inputWire.weights.T, self.dz) # Multiply outgoing gradients by the dE/df of their respective node in this layer
        self.inputWire.gradients = dA
        # print("backward gradient:", d_E_wrt_x)

        self.update()

    def update( self ):
        global alpha
        # if self.dropout:
        #     self.inputWire.weights -= alpha*(np.outer(self.inputs, self.inputWire.dropout_matrix.dot(self.d_E_wrt_f)))
        # self.inputWire.velocity = self.inputWire.momentum*self.inputWire.velocity - alpha * (np.outer(self.inputs, self.d_E_wrt_f))
        # self.inputWire.weights += self.inputWire.velocity
        self.inputWire.weights -= alpha * np.dot(self.dz, self.inputs.T)
        self.biases -= self.outputWire.gradients


class reluLayer(Layer):
    def __init__(self, inputWire, units, dropout=False):
        super().__init__(inputWire, units, dropout, weight_fn=lambda x, y: np.random.randn(x, y) / np.sqrt(x/2))

    def forward(self, x, training=False):
        self.inputs = x
        # Add input value of "1" to the input array for the bias weight (w_0*x_0 + ...)
        self.inputs = np.insert(x, 0, 1.0, 0)
        self.z = self.inputWire.weights.T.dot(self.inputs)

        self.outputs = self.relu(self.z)
        if training and self.dropout:
            self.inputWire.generate_dropout_variables()
            # self.outputs = self.outputs.dot(self.inputWire.dropout_matrix)
            self.outputs = self.outputs * self.inputWire.dropout_vector / self.inputWire.dropout_proba
        return self.outputs

    def relu(self, x):
        # Calculate sigmoid on vector cells
        self.relu_values = np.maximum(0, x)
        return self.relu_values

    def backward(self):
        d_p_wrt_f = np.zeros(self.relu_values.shape)
        d_p_wrt_f[self.relu_values > 0] = 1
        if self.dropout:
            d_p_wrt_f = d_p_wrt_f * self.inputWire.dropout_vector
        self.d_E_wrt_f = np.sum(self.outputWire.gradients[1:], 1) * d_p_wrt_f
        d_E_wrt_x = self.inputWire.weights.dot(
            np.diag(self.d_E_wrt_f))  # Multiply outgoing gradients by the dE/df of their respective node in this layer
        self.inputWire.gradients = d_E_wrt_x
        # print("backward gradient:", d_E_wrt_x)

        self.update()

    def update(self):
        global alpha
        # if self.dropout:
        #     self.inputWire.weights -= alpha*(np.outer(self.inputs, self.inputWire.dropout_matrix.dot(self.d_E_wrt_f)))
        # self.inputWire.velocity = self.inputWire.momentum*self.inputWire.velocity - alpha * (np.outer(self.inputs, self.d_E_wrt_f))
        # self.inputWire.weights += self.inputWire.velocity
        self.inputWire.weights -= alpha * (np.outer(self.inputs, self.d_E_wrt_f))

class softmaxLayer( Layer ):
    def __init__( self, inputWire, units, dropout=False):
        super().__init__( inputWire, units, dropout )

    def forward(self, x, training=False):
        self.inputs = x
        self.z = np.dot(self.inputWire.weights, self.inputs) + self.biases
        self.outputs = self.softmax(self.z)
        return self.outputs

    def softmax( self, x ):
        # Calculate sigmoid on vector cells
        exponentials = np.exp(x - np.max(x))
        self.softmax_values = exponentials / np.sum(exponentials)
        return self.softmax_values
        
    def backward( self ):
        self.inputWire.gradients = None
        dA_dz = self.softmax_values * (1 - self.softmax_values)
        self.dz = self.outputWire.gradients * dA_dz # single incoming gradient
        dA = np.dot(self.inputWire.weights.T, self.dz)
        self.inputWire.gradients = dA

        self.update()

    def update( self ):
        global alpha
        # TODO: Refactor away from the outer() function to allow for mini-batch
        self.inputWire.weights -= alpha*(np.outer(self.inputs, self.d_E_wrt_f))

def main():
    EPOCHS = 1
    # MNIST
    train_size = 50000
    train_data = mnist.train.next_batch(train_size)
    test_size = int(train_size * .2)
    validation_data = mnist.validation.next_batch(test_size)

    nn_architecture = [("input", 784), ("sigmoid", 256), ("sigmoid", 256), ("softmax", 10)]
    nn = NeuralNetwork(nn_architecture, dropout=False)
    nn.train_and_validate(train_data, validation_data, EPOCHS)
    nn.saveModel()

    print("Unpickling model and revalidating")
    nn2 = nn.load_model()
    nn2.test(validation_data)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--alpha", help="Learning rate that controls the step size of gradient descent")
    main()
