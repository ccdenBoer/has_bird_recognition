#include <NeuralNetwork.h>
#include <RPC.h>
#include <SDRAM.h>
#include <stdlib.h>

NeuralNetwork::NeuralNetwork(uint8_t* model_data, int tensor_arena_size, int numberOfClasses, int input_shape[3]) {

    Serial.print("Starting NN init");

    this->tensor_arena_size = tensor_arena_size;
    this->numberOfClasses = numberOfClasses; 
    this->input_shape = input_shape;
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

    std::size_t size = this->tensor_arena_size;
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

void NeuralNetwork::InputData(float data[128][547][1]) {
    Serial.println("NeuralNetwork: Start loading data");
    tflite::MicroInterpreter* interpreter = this->interpreter;
    Serial.println("NeuralNetwork: Loaded interpreter");
    float* inputLayer = interpreter->typed_input_tensor<float>(0);
    //auto input = interpreter.input(0);
    Serial.println("NeuralNetwork: fetched input layer");
    int* input_shape = this->input_shape; 
    // for(int i = 0; i < input_shape[0]; i++){
    //     for(int j = 0; j < input_shape[1]; j++){
    //         for(int k = 0; k < input_shape[2]; k++){
    //             *(inputLayer) = (float) data[i][j][k];
    //             inputLayer++;
    //         }
    //     }
    // }
    memcpy(inputLayer, data,
            (input_shape[0] * input_shape[1] * input_shape[2] * sizeof(float)));
    Serial.println("NeuralNetwork: Input data done");
    free(*data);
}

NeuralNetwork::result_t NeuralNetwork::Predict() {

    NeuralNetwork::result_t result = NeuralNetwork::result_t();
    char* class_names[11] = {"Bird 1", "Bird 2", "Bird 3", "Bird 4", "Bird 5", "Bird 6", "Bird 7", "Bird 8", "Bird 9", "Bird 10", "No bird"};

    Serial.println("NeuralNetwork: Started prediction");
    tflite::MicroInterpreter* interpreter = this->interpreter;
    Serial.println("NeuralNetwork: Loaded interpreter");
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed\n");
        result.confidence = -1;
        result.predicted_class = -1;
        return result;
    }
    Serial.println("NeuralNetwork: Invoke success");
    auto output = interpreter->output(0);
    Serial.println("NeuralNetwork: Fetched output");
    //float* prediction = interpreter->typed_output_tensor<float>(0);
    float max_confidence = 0;
    int index = -1;
    Serial.println("NeuralNetwork: Compute prediction");
    for(int i = 0; i < this->numberOfClasses; i++){
        auto prediction = output->data.f[i];
        Serial.print("Output ");
        Serial.print(class_names[i]);
        Serial.print(": ");
        Serial.println(prediction);
        if(prediction > max_confidence){
            max_confidence = prediction;
            index = i;
        }
    }
    interpreter->ResetVariableTensors();
    result.confidence = max_confidence;
    result.predicted_class = index;
    result.class_name = class_names[index];
    return result;
}