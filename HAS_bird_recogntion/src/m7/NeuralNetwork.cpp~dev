#include <NeuralNetwork.h>
#include <RPC.h>

// #include "model.h"

NeuralNetwork::NeuralNetwork(uint8_t* model_data) {
    Serial.println("NeuralNetwork: constructor");
    this->error_reporter = &this->micro_error_reporter;
    
    this->model = tflite::GetModel(model_data);
    Serial.println("NeuralNetwork: GetModel done");
    

    if (this->model->version() != TFLITE_SCHEMA_VERSION) {
        TF_LITE_REPORT_ERROR(this->error_reporter,
        "Model provided is schema version %d not equal"
        "to supported version %d.\n",
        this->model->version(), TFLITE_SCHEMA_VERSION);
    }
    //print version
    Serial.print("Model provided is schema version: ");
    Serial.print(this->model->version());
    Serial.println();

    auto size = 128*547*4;
    alignas(16) uint8_t tensor_arena[size];
    tflite::MicroInterpreter interpreter(model, resolver, tensor_arena, size, error_reporter);
    Serial.println("NeuralNetwork: Interpreter constructor done");
    interpreter.AllocateTensors();
    Serial.println("NeuralNetwork: AllocateTensors done");
    

    // Obtain a pointer to the model's input tensor
    input = interpreter.input(0);
}

NeuralNetwork::~NeuralNetwork()
{
}

void NeuralNetwork::InputData(float width[128], float height[2188], float channels[3]) {
  RPC.println(input->dims->size);
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