#include <NeuralNetwork.h>
#include <Arduino.h>

#include "model.h"

NeuralNetwork::NeuralNetwork() {
    this->error_reporter = &this->micro_error_reporter;

    this->model = tflite::GetModel(model_tflite);
    if (this->model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(this->error_reporter,
        "Model provided is schema version %d not equal "
        "to supported version %d.\n",
        this->model->version(), TFLITE_SCHEMA_VERSION);
    }
    
    //this->resolver = tflite::MicroMutableOpResolver<1>;

    const int tensor_arena_size = sizeof(1);//float[128][547][1]);
    uint8_t tensor_arena[tensor_arena_size];

    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, 2048, error_reporter);
    interpreter.AllocateTensors();

    // Obtain a pointer to the model's input tensor
    input = interpreter.input(0);
}

NeuralNetwork::~NeuralNetwork()
{
}

void NeuralNetwork::InputData(float data[128][547][1]) {
    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, 2048, error_reporter);
    float *inputLayer = interpreter.typed_input_tensor<float>(0);
    memcpy(inputLayer, *data,
            (128 * 547 * 1 * sizeof(float)));
}

int NeuralNetwork::ScanData() {
    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, 2048, error_reporter);
    TfLiteStatus invoke_status = interpreter.Invoke();
    if (invoke_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
        return -1;
    }

    TfLiteTensor* output = interpreter.output(0);
    return output->data.f[0];
}