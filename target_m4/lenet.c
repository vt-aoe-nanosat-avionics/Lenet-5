#include <stdio.h>
#include <stdint.h>

#include "tflm_wrapper.h"

#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/quadspi.h>
#include "IS25LP128F.h"


struct quadspi_command read = {
.instruction.mode = QUADSPI_CCR_MODE_4LINE,
.instruction.instruction = IS25LP128F_CMD_FAST_READ,
.address.mode = QUADSPI_CCR_MODE_4LINE,
.address.size = QUADSPI_CCR_SIZE_32BIT,
.alternative_bytes.mode = QUADSPI_CCR_MODE_NONE,
.dummy_cycles = 6,
.data_mode = QUADSPI_CCR_MODE_4LINE
};

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

void run_lenet5_cnn(void);

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
    quadspi_set_prescaler(2);   // 1:80 prescaler
    quadspi_clear_flag(QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF);
    quadspi_select_flash(QUADSPI_FLASH_SEL_2);
    quadspi_set_threshold_level(7); // Set FIFO threshold level to 8 bytes
    quadspi_enable();

    quadspi_wait_while_busy();
    quadspi_write(&enableWrite_single, NULL, 0);

    quadspi_wait_while_busy();
    quadspi_write(&enableQPI, NULL, 0);

    usart_send_blocking(USART1, '1');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');

    run_lenet5_cnn();
    return 0;
}

void run_lenet5_cnn(void) {

    uint8_t data[4];
    uint8_t lenet5_model_data[251388];

    usart_send_blocking(USART1, '2');
    usart_send_blocking(USART1, 'a');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');

    read.address.address = 0x000F0000;
    quadspi_wait_while_busy();
    quadspi_read(&read, data, 4);

    usart_send_blocking(USART1, '2');
    usart_send_blocking(USART1, 'b');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');

    uint8_t model_size_1 = (uint8_t)(data[0]);
    uint8_t model_size_2 = (uint8_t)(data[1]);
    uint8_t model_size_3 = (uint8_t)(data[2]);
    uint8_t model_size_4 = (uint8_t)(data[3]);
    uint32_t model_size = (model_size_1<<24)|(model_size_2<<16)|(model_size_3<<8)|(model_size_4<<0);
    //test

    usart_send_blocking(USART1, '2');
    usart_send_blocking(USART1, 'c');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');

    read.address.address = 0x000F0004;
    quadspi_wait_while_busy();
    quadspi_read(&read, lenet5_model_data, model_size);

    usart_send_blocking(USART1, '2');
    usart_send_blocking(USART1, 'd');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');


    // Initialize the TensorFlow Lite Micro interpreter.
    tflm_init(lenet5_model_data);

    read.address.address = 0x00000000;
    float* input = tflm_get_input_buffer(0);
    //float input[32*32];
    uint8_t input_data[32*32];

    usart_send_blocking(USART1, '3');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');

    quadspi_wait_while_busy();
    quadspi_read(&read, input_data, 32*32);

    for (int i = 0; i < 32*32; i++) {
        input[i] = (float)input_data[i];
    }

    usart_send_blocking(USART1, '4');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');

    // Run inference.
    tflm_invoke();

    usart_send_blocking(USART1, '5');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');
    
    // Retrieve output predictions.
    const float* output = tflm_get_output_buffer(0);
    //float output[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    usart_send_blocking(USART1, '6');
    usart_send_blocking(USART1, '\r');
    usart_send_blocking(USART1, '\n');
    

    usart_send_blocking(USART1, (uint8_t)output[0]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[1]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[2]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[3]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[4]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[5]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[6]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[7]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[8]);
    usart_send_blocking(USART1, ' ');
    usart_send_blocking(USART1, (uint8_t)output[9]);
    usart_send_blocking(USART1, '\n');
    usart_send_blocking(USART1, '\r');
}
