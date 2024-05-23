#pragma once
#ifdef abs
#define NEED_ABS_RESTORED abs
#undef abs
#endif


#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#ifdef NEED_ABS_RESTORED
#define abs NEED_ABS_RESTORED
#undef NEED_ABS_RESTORED
#endif

#define NUMBER_OF_CLASSES 7

class NeuralNetwork {
    private:
        const tflite::Model*        model;
        tflite::MicroInterpreter*   interpreter;
  		tflite::ErrorReporter* 		error_reporter;
        uint8_t*                    tensor_arena;
        tflite::AllOpsResolver      resolver;
        std::size_t                 tensor_arena_size;
        int*                        input_shape; //size 3: [HEIGHT, WIDTH, CHANNELS]
		size_t						input_size;
		void*						input_data;
    public:
        struct result_t{
            int predicted_class;
            float confidence;
            const char* class_name;
        };

        NeuralNetwork(uint8_t* model_data, int tensor_arena_size);
        ~NeuralNetwork();

        void InputData(float *data);

        result_t Predict();
};
