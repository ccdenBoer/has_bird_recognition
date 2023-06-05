#include <NeuralNetwork.h>
#include <RPC.h>
#include <SDRAM.h>
#include <stdlib.h>

//#include "mounttest.h"

NeuralNetwork::NeuralNetwork(uint8_t* model_data) {

    Serial.print("Starting NN init");
    this->error_reporter = &this->micro_error_reporter;
    Serial.print("error repotrer initialized");
    this->model = tflite::GetModel(model_data);
    if (this->model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(error_reporter,
        "Model provided is schema version %d not equal "
        "to supported version %d.\n",
        this->model->version(), TFLITE_SCHEMA_VERSION);
    }
    //print version
    Serial.print("Model provided is schema version: ");
    Serial.print(this->model->version());
    Serial.println();

    resolver.AddFullyConnected();
    resolver.AddConv2D();
    resolver.AddMaxPool2D();
    resolver.AddSoftmax();
    resolver.AddRelu();
    resolver.AddTanh();

    std::size_t size = 1024 * 1024 * 4;
    std::size_t malloc_size = size + 15;
    // auto size = 1378784;
    // /alignas(16) uint8_t tensor_arena[size];

    auto unaligned_tensor_arena = SDRAM.malloc(malloc_size);

    // allign to 16
    auto tensor_arena = (uint8_t*) std::align(16,size,unaligned_tensor_arena,malloc_size);


    if (tensor_arena == nullptr)
    {
        Serial.println("NeuralNetwork: Tensor alignment failed");
    }



    Serial.println("NeuralNetwork: created tensor Arena");
    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, size, error_reporter);
    Serial.println("NeuralNetwork: Interpreter constructor done");
    interpreter.AllocateTensors();
    Serial.println("NeuralNetwork: AllocateTensors done");
}

NeuralNetwork::~NeuralNetwork()
{
}

void NeuralNetwork::InputData(float** data) {
    tflite::MicroInterpreter interpreter = *this->interpreter;
    float *inputLayer = interpreter.typed_input_tensor<float>(0);
    memcpy(inputLayer, data,
            (128 * 547 * 1 * sizeof(float)));
}

int NeuralNetwork::Predict() {
    
    tflite::MicroInterpreter interpreter = *this->interpreter;
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