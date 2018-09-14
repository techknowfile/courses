import tensorflow as tf
import numpy as np

x_ph = tf.placeholder(tf.float32, shape=(2, 1))

W_var = tf.Variable(np.array([[1,0], [0, 1], [1, 0], [0, 1]]), dtype=tf.float32)
b_var = tf.Variable(np.array([[1], [1], [-1], [-1]]), dtype=tf.float32)

y_tensor = tf.matmul(W_var, x_ph) + b_var

session = tf.Session()
session.run(tf.global_variables_initializer())

W_value = session.run(W_var)

print(W_value)
x_datum = np.array([[3], [7]])
y_value = session.run(y_tensor, feed_dict={x_ph: x_datum})

print(y_value)

W2_op = tf.assign(W_var, W_var*2)
y_value, W2_value = session.run([ y_tensor, W2_op ], feed_dict={x_ph: x_datum})
print(y_value)

y_value = session.run(y_tensor, feed_dict={x_ph: x_datum})
print(y_value)

y_value = session.run(y_tensor, feed_dict={x_ph: x_datum})
print(y_value)
