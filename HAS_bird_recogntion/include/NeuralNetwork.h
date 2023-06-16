#ifdef abs
#define NEED_ABS_RESTORED abs
#undef abs
#endif

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"

#ifdef NEED_ABS_RESTORED
#define abs NEED_ABS_RESTORED
#undef NEED_ABS_RESTORED
#endif

class NeuralNetwork {
    private:
        const tflite::Model*        model;
        tflite::MicroInterpreter*   interpreter;
        uint8_t*                    tensor_arena;
        TfLiteTensor*               input;
        tflite::ErrorReporter*      error_reporter;
//        tflite::MicroErrorReporter  micro_error_reporter;
        tflite::AllOpsResolver      resolver;
        std::size_t                 tensor_arena_size;
        int                         numberOfClasses;
        int*                        input_shape; //size 3: [HEIGHT, WIDTH, CHANNELS]
        const char*                 class_names[];
    public:
        struct result_t{
            int predicted_class;
            float confidence;
            const char* class_name;
        };

        NeuralNetwork(uint8_t* model_data, int tensor_arena_size, int numberOfClasses, int input_shape[3]);
        ~NeuralNetwork();

        void InputData(float data[128][547][1]);

        result_t Predict();
};
