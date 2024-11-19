#ifndef TFLM_WRAPPER_H
#define TFLM_WRAPPER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the TensorFlow Lite Micro interpreter with a model.
void tflm_init(const uint8_t* model_data);

// Get the input tensor buffer.
float* tflm_get_input_buffer(int index);

// Get the output tensor buffer.
float* tflm_get_output_buffer(int index);

// Invoke the model.
void tflm_invoke(void);

#ifdef __cplusplus
}
#endif

#endif // TFLM_WRAPPER_H
