import os

os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

import tensorflow as tf
import matplotlib.pyplot as plt
import numpy as np
import sys
from tensorflow.keras import datasets
from sklearn.metrics import precision_score, recall_score, f1_score, accuracy_score


(x_train,y_train),(x_test,y_test) = datasets.mnist.load_data()

x_train = tf.pad(x_train, [[0, 0], [2,2], [2,2]])#/255
x_test = tf.pad(x_test, [[0, 0], [2,2], [2,2]])#/255

x_train = tf.expand_dims(x_train, axis=3, name=None)
x_test = tf.expand_dims(x_test, axis=3, name=None)

x_test = x_test[:1000]
y_test = y_test[:1000]

model = tf.keras.models.load_model("models/model.keras")


interpreter = tf.lite.Interpreter(model_path="models/model.tflite")
interpreter.allocate_tensors()

# Get input and output tensors.
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

input_data = np.array(np.random.random_sample(input_details[0]['shape']), dtype=np.float32)

total = 0
correct = 0
predictions_keras = model.predict(x_test)
predictions_tflite = np.zeros(len(x_test))

for p in range(len(x_test)):
    for i in range(32):
        for j in range(32):
            input_data[0][i][j][0] = x_test[p][i][j]
    # Test model on random input data.
    input_shape = input_details[0]['shape']
    interpreter.set_tensor(input_details[0]['index'], input_data)

    interpreter.invoke()
    output_data = interpreter.get_tensor(output_details[0]['index'])
    predictions_tflite[p] = output_data.argmax(axis=1)[0]
    #print(predictions_tflite[p], end=' ')
    #print(y_test[p])
print('')

precicion_keras = precision_score(y_test, predictions_keras.argmax(axis=1), average=None)
precision_tflite = precision_score(y_test, predictions_tflite, average=None)
print('keras precision:\t[', *[round(x, 6) for x in precicion_keras], ']')
print('tflite precision:\t[', *[round(x, 6) for x in precision_tflite], ']')
print('')

recall_keras = recall_score(y_test, predictions_keras.argmax(axis=1), average=None)
recall_tflite = recall_score(y_test, predictions_tflite, average=None)
print('keras recall:\t\t[', *[round(x, 6) for x in recall_keras], ']')
print('tflite recall:\t\t[', *[round(x, 6) for x in recall_tflite], ']')
print('')

f1_keras = f1_score(y_test, predictions_keras.argmax(axis=1), average=None)
f1_tflite = f1_score(y_test, predictions_tflite, average=None)
print('keras f1:\t\t[', *[round(x, 6) for x in f1_keras], ']')
print('tflite f1:\t\t[', *[round(x, 6) for x in f1_tflite], ']')
print('')

accuracy_keras = accuracy_score(y_test, predictions_keras.argmax(axis=1))
accuracy_tflite = accuracy_score(y_test, predictions_tflite)
print('keras accuracy:\t\t' + str(accuracy_keras))
print('tflite accuracy:\t' + str(accuracy_tflite))
print('')





