from tensorflow.examples.tutorials.mnist import input_data
from nn import *

mnist = input_data.read_data_sets("MNIST_data/", one_hot = True)
nn2 = NeuralNetwork.load_model()
test_images = mnist.test.images
test_labels = mnist.test.labels

train_data = mnist.train.next_batch(50000)
nn2.test(train_data)
