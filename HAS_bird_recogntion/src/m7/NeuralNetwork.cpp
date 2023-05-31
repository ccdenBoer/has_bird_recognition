#include <NeuralNetwork.h>
#include <Arduino.h>

#include "mounttest.h"

NeuralNetwork::NeuralNetwork() {
    Serial.print("Starting NN init");
    this->error_reporter = &this->micro_error_reporter;
    Serial.print("error repotrer initialized");
    this->model = tflite::GetModel(mounttest_tflite);
    if (this->model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(this->error_reporter,
        "Model provided is schema version %d not equal "
        "to supported version %d.\n",
        this->model->version(), TFLITE_SCHEMA_VERSION);
    }
    Serial.print("Model loaded");
    
    static tflite::MicroMutableOpResolver<1> resolver;
    resolver.AddFullyConnected();
    resolver.AddConv2D();
    resolver.AddMaxPool2D();
    resolver.AddSoftmax();
    resolver.AddRelu();
    resolver.AddTanh();

    const int tensor_arena_size = 2 * sizeof(float[128][547][1]);
    uint8_t tensor_arena[tensor_arena_size];
    Serial.print("initialized tensor arena");

    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, 2048, error_reporter);
    interpreter.AllocateTensors();
    Serial.print("Allocated tensor arena");

    // Obtain a pointer to the model's input tensor
    input = interpreter.input(0);
    Serial.print("Finished nn init");
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
    float* prediction = interpreter.typed_output_tensor<float>(0);
    float max_confidence = 0;
    int index = -1;
    for(int i = 0; i < output->dims->size; i++){
        if(prediction[i] > max_confidence){
            max_confidence = prediction[i];
            index = i;
        }
    }
    Serial.print(output->dims->size);
    return index;
}