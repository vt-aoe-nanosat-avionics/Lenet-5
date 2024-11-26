PROJECT = flight-cnn-blr
BUILD_DIR = build

TA_EXPT_DIR = ./target_m4/ta-expt
CFILES = target_m4/flight_cnn_blr.c
CFILES += target_m4/lenet.c
CFILES += bootloader.c taolst_protocol.c

CCFILES = target_m4/tflm_wrapper.cc
#CCFILES += target_m4/model_data.cc
CCFILES += target_x86/model_data.cc

# Edit these two lines as needed
DEVICE=stm32l496rgt3
OOCD_FILE = board/stm32l4-generic.cfg

# All lines below probably should not be edited
VPATH += $(TA_EXPT_DIR)
INCLUDES += $(patsubst %,-I%, . $(TA_EXPT_DIR))
INCLUDES += $(patsubst %,-I%, . ./target_m4)
OPENCM3_DIR=libopencm3

CXXSTD = -std=c++17
INCLUDES += -I. -Itensorflow/lite/micro/tools/make/downloads -Itensorflow/lite/micro/tools/make/downloads/gemmlowp 
INCLUDES += -Itensorflow/lite/micro/tools/make/downloads/flatbuffers/include -Itensorflow/lite/micro/tools/make/downloads/kissfft 
INCLUDES += -Itensorflow/lite/micro/tools/make/downloads/ruy -Itensorflow/lite/micro/tools/make/downloads/cmsis/Cortex_DFP/Device/"ARMCM4"/Include 
INCLUDES += -Itensorflow/lite/micro -Itensorflow/lite/micro/kernals



include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk


	
