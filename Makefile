PROJECT = lenet5
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
INCLUDES += -I. -Itensorflow  
INCLUDES += -Itensorflow/lite -Itensorflow/lite/c -Itensorflow/lite/core/api -Itensorflow/lite/core/c -Itensorflow/lite/kernals -Itensorflow/lite/kernals/internal -Itensorflow/lite/kernals/internal/optimized -Itensorflow/lite/kernals/internal/reference -Itensorflow/lite/kernals/internal/reference/integer_ops
INCLUDES += -Itensorflow/lite/micro/ -Itensorflow/lite/micro/arena_allocator -Itensorflow/lite/micro/kernels 
INCLUDES += -Itensorflow/lite/micro/memory_planner -Itensorflow/lite/micro/models -Itensorflow/lite/micro/tflite_bridge
INCLUDES += -Itensorflow/lite/schema



include $(OPENCM3_DIR)/mk/genlink-config.mk
include rules.mk
include $(OPENCM3_DIR)/mk/genlink-rules.mk


	
