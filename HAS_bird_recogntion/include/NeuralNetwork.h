#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

class NeuralNetwork {
    private:
        const tflite::Model*        model;
        tflite::MicroInterpreter*   interpreter;
        TfLiteTensor*               input;
        tflite::ErrorReporter*      error_reporter;
        tflite::MicroErrorReporter  micro_error_reporter;
        tflite::AllOpsResolver      resolver;
    public:
        NeuralNetwork(uint8_t* model_data);
        ~NeuralNetwork();

        void    InputData(float **data);

        int     Predict();
};
