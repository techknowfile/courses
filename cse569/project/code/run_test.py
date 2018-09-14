from tensorflow.examples.tutorials.mnist import input_data
from nn import *

mnist = input_data.read_data_sets("MNIST_data/", one_hot = True)
nn2 = NeuralNetwork.load_model()
test_images = mnist.test.images
test_labels = mnist.test.labels

test_data = (test_images, test_labels)
nn2.test(test_data)
