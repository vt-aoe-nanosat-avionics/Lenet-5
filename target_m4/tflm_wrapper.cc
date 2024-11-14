#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "tflm_wrapper.h"


namespace {
    tflite::ErrorReporter* error_reporter = nullptr;
    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;
    TfLiteTensor* output = nullptr;

    constexpr int kTensorArenaSize = 41 * 1024; //  41 KB
    __attribute__((aligned(16))) uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

//void *__dso_handle = NULL;

extern "C" int tflm_init(const uint8_t* model_data) {
    tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;

    model = tflite::GetModel(model_data);

    static tflite::MicroMutableOpResolver<7> micro_op_resolver;
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddAveragePool2D();
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddSoftmax();
    micro_op_resolver.AddTanh();
    micro_op_resolver.AddLogistic();

    static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize);
    interpreter = &static_interpreter;

    interpreter->AllocateTensors();
    return 0;
}

extern "C" float* tflm_get_input_buffer(int index) {
    if (!interpreter) return nullptr;
    input = interpreter->input(index);
    return input ? input->data.f : nullptr;
}

extern "C" const float* tflm_get_output_buffer(int index) {
    if (!interpreter) return nullptr;
    output = interpreter->output(index);
    return output ? output->data.f : nullptr;
}

extern "C" void tflm_invoke() {
    if (interpreter) {
        interpreter->Invoke();
    }
}