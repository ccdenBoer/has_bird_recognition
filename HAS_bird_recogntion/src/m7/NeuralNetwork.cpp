#include <NeuralNetwork.h>
#include <RPC.h>
#include <SDRAM.h>
#include <stdlib.h>

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

    auto unaligned_tensor_arena = SDRAM.malloc(malloc_size);

    // align to 16
    auto tensor_arena = (uint8_t*) std::align(16,size,unaligned_tensor_arena,malloc_size);


    if (tensor_arena == nullptr)
    {
        Serial.println("NeuralNetwork: Tensor alignment failed");
    }



    Serial.println("NeuralNetwork: created tensor Arena");
    this->interpreter = new tflite::MicroInterpreter(model, resolver, tensor_arena, size, error_reporter);
    Serial.println("NeuralNetwork: Interpreter constructor done");
    this->interpreter->AllocateTensors();
    Serial.println("NeuralNetwork: AllocateTensors done");
}

NeuralNetwork::~NeuralNetwork()
{
    delete this->interpreter;
    delete this->model;
}

void NeuralNetwork::InputData(float** data) {
    Serial.println("NeuralNetwork: Start loading data");
    tflite::MicroInterpreter* interpreter = this->interpreter;
    Serial.println("NeuralNetwork: Loaded interpreter");
    float* inputLayer = interpreter->typed_input_tensor<float>(0);
    //auto input = interpreter.input(0);
    Serial.println("NeuralNetwork: fetched input layer");
    memcpy(inputLayer, data,
            (128 * 547 * 1 * sizeof(float)));
    Serial.println("NeuralNetwork: Input data done");
    free(*data);
}

int NeuralNetwork::Predict() {
    Serial.println("Started prediction");
    tflite::MicroInterpreter* interpreter = this->interpreter;
    Serial.println("Loaded interpreter");
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
        return -1;
    }
    Serial.println("Invoke success");
    auto output = interpreter->output(0);
    Serial.println("Fetched output");
    //float* prediction = interpreter->typed_output_tensor<float>(0);
    float max_confidence = 0;
    int index = -1;
    Serial.println("Compute prediction");
    for(int i = 0; i < 11; i++){
        auto prediction = output->data.f[i];
        Serial.print("Output ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(prediction);
        if(prediction > max_confidence){
            max_confidence = prediction;
            index = i;
        }
    }
    return index;
}