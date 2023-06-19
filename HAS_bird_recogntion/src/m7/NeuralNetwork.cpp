#include <NeuralNetwork.h>
#include <SdramAllocator.h>
#include <cstdlib>
#include "tensorflow/lite/micro/system_setup.h"

NeuralNetwork::NeuralNetwork(uint8_t *model_data, int tensor_arena_size, int numberOfClasses) {
  tflite::InitializeTarget();
  Serial.print("Starting NN init");

  this->tensor_arena_size = tensor_arena_size;
  this->numberOfClasses = numberOfClasses;
  Serial.print("error reporter initialized");
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


  std::size_t size = this->tensor_arena_size;
  tensor_arena = Uint8Allocator.allocate(size);

  if (tensor_arena == nullptr) {
	Serial.println("NeuralNetwork: Tensor alignment failed");
  }

  Serial.println("NeuralNetwork: created tensor Arena");
  this->interpreter = new tflite::MicroInterpreter(model, resolver, tensor_arena, size);
  Serial.println("NeuralNetwork: Interpreter constructor done");
  this->interpreter->AllocateTensors();
  Serial.println("NeuralNetwork: AllocateTensors done");
  this->input_data = interpreter->typed_input_tensor<float>(0);

  //get input shape from model
  this->input_shape = interpreter->input_tensor(0)->dims->data;
  printf("NeuralNetwork: Input shape: %d, %d, %d\n", input_shape[0], input_shape[1], input_shape[2]);
  this->input_size = interpreter->input_tensor(0)->bytes;
  memset(this->input_data, 0, this->input_size);
  Serial.println("NeuralNetwork: Input zeroed out");
  //print size of input in bytes
  printf("NeuralNetwork: Input size in bytes: %d\n", this->input_size);
}

NeuralNetwork::~NeuralNetwork() {
  delete this->interpreter;
  delete this->model;
  Uint8Allocator.deallocate(tensor_arena, this->tensor_arena_size);
}

void NeuralNetwork::InputData(float *data) {
  Serial.println("NeuralNetwork: Start loading data");
  memcpy(this->input_data, data, this->input_size);
  Serial.println("NeuralNetwork: Input data done");
}

NeuralNetwork::result_t NeuralNetwork::Predict() {

  NeuralNetwork::result_t result = NeuralNetwork::result_t();
  const char *class_names[11] = {"Wilde Eend", "Tjiftjaf", "Koolmees", "Houtduif", "Huismus", "Merel", "Winterkoning", "Fitis", "Vink", "Spreeuw", "Geen Vogel"};

  Serial.println("NeuralNetwork: Started prediction");
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
  for (int i = 0; i < this->numberOfClasses; i++) {
	auto prediction = output->data.f[i];
	Serial.print("Output ");
	Serial.print(class_names[i]);
	Serial.print(": ");
	Serial.println(prediction);
	if (prediction > max_confidence) {
	  max_confidence = prediction;
	  index = i;
	}
  }
//    interpreter->Reset();
  result.confidence = max_confidence;
  result.predicted_class = index;
  result.class_name = class_names[index];
  return result;
}