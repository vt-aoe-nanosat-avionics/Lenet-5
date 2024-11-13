
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

## 3. Compile Main C File
```bash
make
```
