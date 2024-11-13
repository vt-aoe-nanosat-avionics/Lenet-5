PROJECT = lenet_m4
BUILD_DIR = build

CFILES = target_m4/lenet.c
CCFILES = target_m4/tflm_wrapper.cc

# Edit these two lines as needed
DEVICE=stm32l496rgt3
OOCD_FILE = board/stm32l4-generic.cfg

# All lines below probably should not be edited
VPATH += $(TA_EXPT_DIR)
INCLUDES += $(patsubst %,-I%, . $(TA_EXPT_DIR))
OPENCM3_DIR=libopencm3


LD = $(PREFIX)g++
CXXSTD = -std=c++17
INCLUDES += -Itflite-micro/. -Itflite-micro/tensorflow/lite/micro/tools/make/downloads -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/gemmlowp 
INCLUDES += -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/flatbuffers/include -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/kissfft 
INCLUDES += -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/ruy -Itflite-micro/tensorflow/lite/micro/tools/make/downloads/cmsis/Cortex_DFP/Device/"ARMCM4"/Include 
INCLUDES += -Itflite-micro/tensorflow/lite/micro



include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk


	
