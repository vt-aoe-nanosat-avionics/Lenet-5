from tensorflow.keras import datasets
import tensorflow as tf
import matplotlib.pyplot as plt
import sys

file = int(sys.argv[1])

(x_train,y_train),(x_test,y_test) = datasets.mnist.load_data()

x_train = tf.pad(x_train, [[0, 0], [2,2], [2,2]])#/255
x_test = tf.pad(x_test, [[0, 0], [2,2], [2,2]])#/255

x_train = tf.expand_dims(x_train, axis=3, name=None)
x_test = tf.expand_dims(x_test, axis=3, name=None)

data = []

for i in range(32):
    for j in range(32):
        data.append(int(x_test[file][i][j]))
        #print("%3d" % int(x_test[file][i][j]), end=' ')
    #print('\n', end='')

with open('mnistData', 'wb') as f:
  f.write(bytearray(data))

#print(y_test[file])
fig = plt.figure()
plt.imshow(x_test[file], cmap='gray')
plt.show()#block=False)