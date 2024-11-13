
# Setting Up Virtual Environment
### (Only needs to be done for training)

```bash
sudo apt install python3.10-venv

python3 -m venv venv
source venv/bin/activate

pip install tensorflow-cpu
pip install matplotlib
```

# Compiling for x86
## 1. Compile Tensorflow Lite
```bash
cd tflite-micro
make -f tensorflow/lite/micro/tools/make/Makefile microlite
cd ..
```

## 2. Compoile Tensorflow Wrapper
```bash
g++ -std=c++17 -fno-rtti -fno-exceptions -fno-threadsafe-statics -Wnon-virtual-dtor -Werror -fno-unwind-tables -ffunction-sections -fdata-sections -fmessage-length=0 -DTF_LITE_STATIC_MEMORY -DTF_LITE_DISABLE_X86_NEON -Wsign-compare -Wdouble-promotion -Wunused-variable -Wunused-function -Wswitch -Wvla -Wall -Wextra -Wmissing-field-initializers -Wstrict-aliasing -Wno-unused-parameter -DKERNELS_OPTIMIZED_FOR_SPEED -DTF_LITE_USE_CTIME -O2 -Itflite-micro/. -Itflite-micro/tensorflow/lite/micro/tools/make/downloads -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/gemmlowp -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/kissfft -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/ruy -Itflite-micro/gen/linux_x86_64_default_gcc/genfiles/ -Itflite-micro/gen/linux_x86_64_default_gcc/genfiles/ -c tflm_wrapper.cc -o tflm_wrapper.o
```

## 3. Compile main C code
```bash
g++ target_x86/lenet.c target_x86/model_data.cc build/tflm_wrapper.o tflite-micro/gen/linux_x86_64_default_gcc/lib/libtensorflow-microlite.a -o lenet5.out
```


# Compiling for Cortex M4

## 1. Compile Libopencm3
```bash
cd libopencm3
make
cd ..
```

## 2. Compile Tensorflow Lite
```bash
cd tflite-micro
make -f tensorflow/lite/micro/tools/make/Makefile TARGET=cortex_m_generic TARGET_ARCH=cortex-m4+fp OPTIMIZED_KERNEL_DIR=cmsis_nn TARGET_TOOLCHAIN_ROOT=/usr/bin/ microlite
cd ..
```

## 3. Compile Tensorflow Wrapper
```bash
arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -std=c++17 -Wall -Wextra -fno-threadsafe-statics -Itflite-micro/. -Itflite-micro/tensorflow/lite/micro/tools/make/downloads -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/gemmlowp -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/kissfft -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/ruy -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/cmsis/Cortex_DFP/Device/"ARMCM4"/Include -Itflite-micro/tensorflow/lite/micro -ltensorflow-microlite -c target_m4/tflm_wrapper.cc -o build/tflm_wrapper.o
```

## 4. Compile and link main C code
```bash
arm-none-eabi-gcc -Os -std=c99 -ggdb3 -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-common -ffunction-sections -fdata-sections -Wextra -Wshadow -Wno-unused-variable -Wimplicit-function-declaration -Wredundant-decls -Wstrict-prototypes -Wmissing-prototypes  -MD -Wall -Wundef -I. -I. -Ilibopencm3/include -I. -I. -Ilibopencm3/include  -DSTM32L4 -DSTM32L496RGT3 -Ilibopencm3/include -o build/target_m4/lenet.o -c target_m4/lenet.c

arm-none-eabi-gcc -E -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32L4 -DSTM32L496RGT3 -D_ROM=1024K -D_RAM=256K -D_RAM2=64K -D_ROM_OFF=0x08000000 -D_RAM_OFF=0x20000000 -D_RAM2_OFF=0x10000000 -D_RAM3_OFF=0x20040000 -P -E libopencm3/ld/linker.ld.S -o generated.stm32l496rgt3.ld
```

## 5. Link
```bash
arm-none-eabi-g++ -Tgenerated.stm32l496rgt3.ld -Llibopencm3/lib -nostartfiles -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -specs=nosys.specs -Wl,--gc-sections -Llibopencm3/lib -Ltflite-micro/gen/cortex_m_generic_cortex-m4+fp_default_cmsis_nn_gcc/lib/ build/target_m4/lenet.o build/tflm_wrapper.o  -lopencm3_stm32l4  -ltensorflow-microlite -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group -o lenet_m4.elf
```

## 5. Create Bin
```bash
arm-none-eabi-objcopy -O binary build/lenet5.elf lenet5.bin
```

