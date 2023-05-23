#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

class NeuralNetwork {
    private:
        tflite::MicroErrorReporter  micro_error_reporter;
        tflite::ErrorReporter*      error_reporter;
        const tflite::Model*        model;
        tflite::AllOpsResolver      resolver;
        TfLiteTensor*               input;
        TfLiteTensor*               tensor;
        uint8_t                     tensor_arena[2048];
    public:
        NeuralNetwork(/* args */);
        ~NeuralNetwork();

        void    InputData(float width[128], float height[2188], float channels[3]);

        int     ScanData();
};
