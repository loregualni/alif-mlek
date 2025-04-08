/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "BenchmarkHandler.hpp"

#include <cstdint>
#include <cstdlib>

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/benchmarks/micro_benchmark.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_profiler.h"
#include "tensorflow/lite/micro/micro_time.h"
#include "tensorflow/lite/micro/system_setup.h"

namespace arm {
namespace app {

    using BenchmarkRunner = tflite::MicroBenchmarkRunner<int8_t>;

    static const uint8_t* modelPtr;
    static size_t modelLen;
    static uint8_t* arenaPtr;
    static size_t arenaLen;
    static const tflite::MicroOpResolver *opResolver;
    static tflite::MicroProfiler profiler;

    void BenchmarkNIerations(int iterations,
                          const char* tag,
                          BenchmarkRunner& benchmark_runner,
                          tflite::MicroProfiler& profiler)
    {
        int32_t ticks = 0;
        for (int i = 0; i < iterations; ++i) {
            benchmark_runner.SetRandomInput(i);
            profiler.ClearEvents();
            benchmark_runner.RunSingleIteration();
            ticks += profiler.GetTotalTicks();
        }
        MicroPrintf("%s took %d ticks (%d ms)", tag, ticks, tflite::TicksToMs(ticks));
    }

    bool RunBenchmarkHandler(ApplicationContext& ctx)
    {
        tflite::InitializeTarget();

        modelPtr = ctx.Get<const uint8_t*>("modelPtr");
        modelLen = ctx.Get<size_t>("modelLen");
        arenaPtr = ctx.Get<uint8_t*>("arenaPtr");
        arenaLen = ctx.Get<size_t>("arenaLen");
        opResolver = ctx.Get<const tflite::MicroOpResolver*>("opResolver");

        uint32_t event_handle = profiler.BeginEvent("InitializeInferenceRunner");
        BenchmarkRunner benchmark_runner(modelPtr, opResolver, arenaPtr, arenaLen, &profiler);
        profiler.EndEvent(event_handle);
        profiler.Log();
        MicroPrintf("");

        BenchmarkNIerations(1, "BenchmarkNIerations(1)", benchmark_runner, profiler);
        profiler.Log();
        MicroPrintf("");

        BenchmarkNIerations(10, "BenchmarkNIerations(10)", benchmark_runner, profiler);
        MicroPrintf("");
        
        benchmark_runner.PrintAllocations();
        MicroPrintf("");

        return true;
    }

} // namespace app
} // namespace arm
