
#include "tflm_wrapper.h"

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/quadspi.h>
#include "ta-expt/IS25LP128F.h"

#include "model_data.h"
#include "lenet.h"


struct quadspi_command readCNN = {
.instruction.mode = QUADSPI_CCR_MODE_4LINE,
.instruction.instruction = IS25LP128F_CMD_FAST_READ,
.address.mode = QUADSPI_CCR_MODE_4LINE,
.address.size = QUADSPI_CCR_SIZE_32BIT,
.alternative_bytes.mode = QUADSPI_CCR_MODE_NONE,
.dummy_cycles = 6,
.data_mode = QUADSPI_CCR_MODE_4LINE
};

void init_lenet(void) {
    readCNN.address.address = 0x0000F004;
    quadspi_wait_while_busy();
    quadspi_read(&readCNN, lenet_model_tflite, lenet_model_tflite_len);

    // Initialize the TensorFlow Lite Micro interpreter.
    tflm_init(lenet_model_tflite);
}

float* run_lenet(void) {
    readCNN.address.address = 0x00000004;
    float* input = tflm_get_input_buffer(0);
    //float input[32*32];
    uint8_t input_data[32*32];

    if(input == NULL)
    {
        return NULL;
    }

    quadspi_wait_while_busy();
    quadspi_read(&readCNN, input_data, 32*32);

    for (int i = 0; i < 32*32; i++) {
        input[i] = input_data[i];
    }

    // Run inference.
    tflm_invoke();

    // Retrieve output predictions.
    float* output = tflm_get_output_buffer(0);

    return output;
}
