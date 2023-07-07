#include <NeuralNetwork.h>
#include <SdramAllocator.h>
#include <cstdlib>
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"

NeuralNetwork::NeuralNetwork(uint8_t *model_data, int tensor_arena_size, int numberOfClasses) {
  tflite::InitializeTarget();
  printf("Starting NN init\n");

  this->tensor_arena_size = tensor_arena_size;
  this->numberOfClasses = numberOfClasses;
  this->error_reporter = new tflite::MicroErrorReporter();
  printf("NeuralNetwork: Created error reporter\n");
  this->model = tflite::GetModel(model_data);
  if (this->model->version() != TFLITE_SCHEMA_VERSION) {
	TF_LITE_REPORT_ERROR(error_reporter,
						 "Model provided is schema version %d not equal "
						 "to supported version %d.\n",
						 this->model->version(), TFLITE_SCHEMA_VERSION);
  }
  //print version
  printf("Model provided is schema version:  %lu\n", this->model->version());

  std::size_t size = this->tensor_arena_size;
  tensor_arena = Uint8Allocator.allocate(size);

  if (tensor_arena == nullptr) {
	printf("NeuralNetwork: Tensor arena allocation failed\n");
  }

  printf("NeuralNetwork: Created tensor arena size: %d\n", size);
  this->interpreter = new tflite::MicroInterpreter(model, resolver, tensor_arena, size);
  printf("NeuralNetwork: Interpreter created\n");
  this->interpreter->AllocateTensors();
  printf("NeuralNetwork: AllocateTensors done\n");
  this->input_data = interpreter->typed_input_tensor<float>(0);

  //get input shape from model
  this->input_shape = interpreter->input_tensor(0)->dims->data;
  printf("NeuralNetwork: Input shape: %d, %d, %d\n", input_shape[0], input_shape[1], input_shape[2]);
  this->input_size = interpreter->input_tensor(0)->bytes;
  memset(this->input_data, 0, this->input_size);
  printf("NeuralNetwork: Input of %d bytes zero initialized\n", this->input_size);
}

NeuralNetwork::~NeuralNetwork() {
  delete this->interpreter;
  delete this->model;
  Uint8Allocator.deallocate(tensor_arena, this->tensor_arena_size);
}

void NeuralNetwork::InputData(float *data) {
  printf("NeuralNetwork: Input data of %d bytes\n", this->input_size);
  memcpy(this->input_data, data, this->input_size);
  printf("NeuralNetwork: Input data copied\n");
}

NeuralNetwork::result_t NeuralNetwork::Predict() {
//  calculate time between predictions
  static unsigned long lastTime = 0;
  unsigned long currentTime = millis();
  unsigned long timeBetweenPredictions = currentTime - lastTime;
  lastTime = currentTime;
  printf("NeuralNetwork: Time between predictions: %lu\n", timeBetweenPredictions);

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