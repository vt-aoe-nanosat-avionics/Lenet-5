import os

os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'

import tensorflow as tf
import matplotlib.pyplot as plt
from tensorflow.keras import datasets, layers, models, losses


model = models.load_model("models/model.keras")
model.summary()

layer1 = models.Sequential()
layer1.add(layers.Conv2D(6, 5, activation='tanh', input_shape=(32,32,1)))
for i in range(len(layer1.weights)):
    layer1.weights[i].assign(model.layers[0].weights[i])

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(layer1)
tflite_model = converter.convert()

# Save the model.
with open('models/layer0.tflite', 'wb') as f:
  f.write(tflite_model)


layer2 = models.Sequential()
layer2.add(layers.AveragePooling2D(2, input_shape=(28,28,6)))
for i in range(len(layer2.weights)):
    layer2.weights[i].assign(model.layers[1].weights[i])

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(layer2)
tflite_model = converter.convert()

# Save the model.
with open('models/layer1.tflite', 'wb') as f:
  f.write(tflite_model)


layer3 = models.Sequential()
layer3.add(layers.Activation('sigmoid', input_shape=(14,14,6)))
for i in range(len(layer3.weights)):
    layer3.weights[i].assign(model.layers[2].weights[i])

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(layer3)
tflite_model = converter.convert()

# Save the model.
with open('models/layer2.tflite', 'wb') as f:
  f.write(tflite_model)


layer4 = models.Sequential()
layer4.add(layers.Conv2D(16, 5, activation='tanh', input_shape=(14,14,6)))
for i in range(len(layer4.weights)):
    layer4.weights[i].assign(model.layers[3].weights[i])

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(layer4)
tflite_model = converter.convert()

# Save the model.
with open('models/layer3.tflite', 'wb') as f:
  f.write(tflite_model)


layer5 = models.Sequential()
layer5.add(layers.AveragePooling2D(2, input_shape=(10,10,16)))
for i in range(len(layer5.weights)):
    layer5.weights[i].assign(model.layers[4].weights[i])

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(layer5)
tflite_model = converter.convert()

# Save the model.
with open('models/layer4.tflite', 'wb') as f:
  f.write(tflite_model)


layer6 = models.Sequential()
layer6.add(layers.Activation('sigmoid', input_shape=(5,5,16)))
for i in range(len(layer6.weights)):
    layer6.weights[i].assign(model.layers[5].weights[i])

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(layer6)
tflite_model = converter.convert()

# Save the model.
with open('models/layer5.tflite', 'wb') as f:
  f.write(tflite_model)


layer7 = models.Sequential()
layer7.add(layers.Conv2D(120, 5, activation='tanh', input_shape=(5,5,16)))
for i in range(len(layer7.weights)):
    layer7.weights[i].assign(model.layers[6].weights[i])

layer7.add(layers.Flatten())

layer7.add(layers.Dense(84, activation='tanh'input_shape=(84)))
for i in range(len(layer7.layers[1].weights)):
    layer7.layers[1].weights[i].assign(model.layers[8].weights[i])

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(layer7)
tflite_model = converter.convert()

# Save the model.
with open('models/layer6.tflite', 'wb') as f:
  f.write(tflite_model)



layer8 = models.Sequential()
layer8.add(layers.Dense(10, activation='softmax'))
for i in range(len(layer8.weights)):
    layer8.weights[i].assign(model.layers[9].weights[i])

# Convert the model.
converter = tf.lite.TFLiteConverter.from_keras_model(layer8)
tflite_model = converter.convert()

# Save the model.
with open('models/layer7.tflite', 'wb') as f:
  f.write(tflite_model)





