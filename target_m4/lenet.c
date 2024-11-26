#include <stdio.h>
#include <stdint.h>

#include "tflm_wrapper.h"

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/quadspi.h>
#include "IS25LP128F.h"

//#include "../target_x86/model_data.h"


//struct quadspi_command readCNN = {
//    .instruction.mode = QUADSPI_CCR_MODE_4LINE,
//    .instruction.instruction = IS25LP128F_CMD_FAST_READ,
//    .address.mode = QUADSPI_CCR_MODE_4LINE,
//    .address.size = QUADSPI_CCR_SIZE_32BIT,
//    .alternative_bytes.mode = QUADSPI_CCR_MODE_NONE,
//    .dummy_cycles = 6,
//    .data_mode = QUADSPI_CCR_MODE_4LINE
//};

struct quadspi_command enableQPI = {
    .instruction.mode = QUADSPI_CCR_MODE_1LINE,
    .instruction.instruction = IS25LP128F_CMD_ENTER_QPI_MODE,
    .alternative_bytes.mode = QUADSPI_CCR_MODE_NONE,
    .address.mode = QUADSPI_CCR_MODE_NONE,
    .dummy_cycles = 0,
    .data_mode = QUADSPI_CCR_MODE_NONE
};

struct quadspi_command enableWrite_single = {
    .instruction.mode = QUADSPI_CCR_MODE_1LINE,
    .instruction.instruction = IS25LP128F_CMD_WRITE_ENABLE,
    .alternative_bytes.mode = QUADSPI_CCR_MODE_NONE,
    .address.mode = QUADSPI_CCR_MODE_NONE,
    .dummy_cycles = 0,
    .data_mode = QUADSPI_CCR_MODE_NONE
};

//const unsigned char lenet_model_tflite[251384];
//unsigned int lenet_model_tflite_len = 251384;

int run_lenet5_cnn(void);

int main(void) {
    rcc_periph_reset_pulse(RST_USART1);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);
    gpio_mode_setup(GPIOA,GPIO_MODE_AF,GPIO_PUPD_NONE,GPIO9|GPIO10);
    gpio_set_af(GPIOA,GPIO_AF7,GPIO9);  // USART1_TX is alternate function 7
    gpio_set_af(GPIOA,GPIO_AF7,GPIO10); // USART1_RX is alternate function 7
    usart_set_baudrate(USART1,38400);
    usart_set_databits(USART1,8);
    usart_set_stopbits(USART1,USART_STOPBITS_1);
    usart_set_mode(USART1,USART_MODE_TX_RX);
    usart_set_parity(USART1,USART_PARITY_NONE);
    usart_set_flow_control(USART1,USART_FLOWCONTROL_NONE);

    usart_enable(USART1);

    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_QSPI);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO1 | GPIO2 | GPIO3 | GPIO4);
    gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH, GPIO1 | GPIO2 | GPIO3 | GPIO4);
    gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH, GPIO11);
    gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_VERYHIGH, GPIO3);
    gpio_set_af(GPIOC, GPIO_AF10, GPIO1 | GPIO2 | GPIO3 | GPIO4);
    gpio_set_af(GPIOC, GPIO_AF5, GPIO11);
    gpio_set_af(GPIOA, GPIO_AF10, GPIO3);

    quadspi_disable();
    quadspi_set_flash_size(23); // 128 Mbit = 16 Mbyte = 2^(n+1) // n = 23
    quadspi_set_cs_high_time(6);   // 1/2 clock cycle
    quadspi_set_prescaler(1);   // 1:80 prescaler
    quadspi_clear_flag(QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF);
    quadspi_select_flash(QUADSPI_FLASH_SEL_2);
    quadspi_set_threshold_level(7); // Set FIFO threshold level to 8 bytes
    quadspi_enable();

    quadspi_wait_while_busy();
    quadspi_write(&enableWrite_single, NULL, 0);

    quadspi_wait_while_busy();
    quadspi_write(&enableQPI, NULL, 0);

    //readCNN.address.address = 0x0000F004;
    //quadspi_wait_while_busy();
    //quadspi_read(&readCNN, lenet_model_tflite, lenet_model_tflite_len);

    quadspi_wait_while_busy();
    uint32_t ccr = 0;
    ccr = quadspi_prepare_funcion_mode(ccr, QUADSPI_CCR_FMODE_MEMMAP);
    ccr = quadspi_prepare_address_mode(ccr, QUADSPI_CCR_MODE_4LINE);
    ccr = quadspi_prepare_address_size(ccr, QUADSPI_CCR_SIZE_32BIT);
    ccr = quadspi_prepare_data_mode(ccr, QUADSPI_CCR_MODE_4LINE);
    ccr = quadspi_prepare_dummy_cycles(ccr, 6);
    ccr = quadspi_prepare_instruction_mode(ccr, QUADSPI_CCR_MODE_4LINE);
    ccr = quadspi_prepare_instruction(ccr, IS25LP128F_CMD_FAST_READ);
    ccr = quadspi_prepare_alternative_bytes_mode(ccr, QUADSPI_CCR_MODE_NONE);
    quadspi_write_ccr(ccr);

    run_lenet5_cnn();
    return 0;
}

int run_lenet5_cnn(void) {
    char string[9];
    unsigned char* lenet5_model_tflite = (unsigned char*)0x9000F004;
    unsigned int lenet5_model_tflite_len = *(unsigned int*)0x9000F000;

    sprintf(string, "%d", lenet5_model_tflite_len);
    for(int i = 0; i < 6; i++)
    {
        usart_send_blocking(USART1, string[i]);
    }
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');
    return 1;
    //read.address.address = 0x0000F004;
    //quadspi_wait_while_busy();
    //quadspi_read(&read, lenet_model_tflite, lenet_model_tflite_len);


    usart_send_blocking(USART1, 'S');
    // Initialize the TensorFlow Lite Micro interpreter.
    //tflm_init(lenet_model_tflite);
    usart_send_blocking(USART1, 'S');

    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');

    //read.address.address = 0x90000004;
    float* input = tflm_get_input_buffer(0);
    //float input[32*32];
    uint8_t* input_data = (uint8_t*)0x90000004;

    //if(input == NULL)
    //{
    //    usart_send_blocking(USART1, 'E');
    //    usart_send_blocking(USART1, 'R');
    //    usart_send_blocking(USART1, 'R');
    //    usart_send_blocking(USART1, '\r');
    //    usart_send_blocking(USART1, '\n');
    //    return 1;
    //}

    //quadspi_wait_while_busy();
    //quadspi_read(&read, input_data, 32*32);

    for (int i = 0; i < 32*32; i++) {
        input[i] = input_data[i];
    }

    for(int i = 0; i < 32; i++)
    {
        for(int j = 0; j < 32; j++)
        {
            uint8_t val = (uint8_t)input[i*32+j];
            if(val < 10)
            {
                sprintf(string, "%.3d", val);
                usart_send_blocking(USART1, ' ');
                usart_send_blocking(USART1, ' ');
                usart_send_blocking(USART1, string[0]);
            }
            else if(val < 100)
            {
                sprintf(string, "%.3d", val);
                usart_send_blocking(USART1, ' ');
                usart_send_blocking(USART1, string[0]);
                usart_send_blocking(USART1, string[1]);
            }
            else
            {
                sprintf(string, "%.3d", val);
                usart_send_blocking(USART1, string[0]);
                usart_send_blocking(USART1, string[1]);
                usart_send_blocking(USART1, string[2]);
            }
            usart_send_blocking(USART1, ' ');
        }
        usart_send_blocking(USART1, '\r');
        usart_send_blocking(USART1, '\n');
    }

    // Run inference.
    tflm_invoke();

    // Retrieve output predictions.
    float* output = tflm_get_output_buffer(0);

    sprintf(string, "Output: ");
    for(int j = 0; j < 8; j++)
    {
        usart_send_blocking(USART1, string[j]);
    }

    for(int i = 0; i < 10; i++) {
        sprintf(string, "0.%.6d", (int)(output[i]*1000000));
        usart_send_blocking(USART1, ' ');
        for(int j = 0; j < 8; j++)
        {
            usart_send_blocking(USART1, string[j]);
        }
    }
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');
    return 0;
}
