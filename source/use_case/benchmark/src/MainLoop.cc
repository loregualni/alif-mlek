/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "hal.h"                    /* Brings in platform definitions. */
#include "TestModel.hpp"            /* Model class for running inference. */
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "BenchmarkHandler.hpp"     /* Handlers for benchmarking models. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */
#include "MicroMutableAllOpsResolver.hpp"

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace benchmark {
#if defined(DYNAMIC_MODEL_BASE) && defined(DYNAMIC_MODEL_SIZE)

static uint8_t* GetModelPointer()
{
    info("Model pointer: 0x%08x\n", DYNAMIC_MODEL_BASE);
    return reinterpret_cast<uint8_t*>(DYNAMIC_MODEL_BASE);
}

static size_t GetModelLen()
{
    /* TODO: Can we get the actual model size here somehow?
     * Currently we return the reserved space. It is possible to do
     * so by reading the memory pattern but it will not be reliable. */
    return static_cast<size_t>(DYNAMIC_MODEL_SIZE);
}

#else /* defined(DYNAMIC_MODEL_BASE) && defined(DYNAMIC_MODEL_SIZE) */

extern uint8_t* GetModelPointer();
extern size_t GetModelLen();

#endif /* defined(DYNAMIC_MODEL_BASE) && defined(DYNAMIC_MODEL_SIZE) */
    }  /* namespace benchmark */
} /* namespace app */
} /* namespace arm */

static void ArmEthosBenchmark()
{
    arm::app::TestModel model;  /* Model wrapper object. */

    /* Load the model. */
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::benchmark::GetModelPointer(),
                    arm::app::benchmark::GetModelLen())) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"benchmark"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("imgIndex", 0);

    /* Loop. */
    if (RunInferenceHandler(caseContext)) {
        info("Inference completed.\n");
    } else {
        printf_err("Inference failed.\n");
    }
}

static void TFLiteBenchmark()
{
    static const auto opResolver = arm::app::CreateAllOpsResolver();

    arm::app::ApplicationContext benchContext;
    benchContext.Set<const uint8_t*>("modelPtr", arm::app::benchmark::GetModelPointer());
    benchContext.Set<size_t>("modelLen", arm::app::benchmark::GetModelLen());
    benchContext.Set<uint8_t*>("arenaPtr", arm::app::tensorArena);
    benchContext.Set<size_t>("arenaLen", sizeof(arm::app::tensorArena));
    benchContext.Set<const tflite::MicroOpResolver*>("opResolver", &opResolver);

    /* Loop. */
    if (RunBenchmarkHandler(benchContext)) {
        info("Benchmark completed.\n");
    } else {
        printf_err("Benchmark failed.\n");
    }
}

void MainLoop()
{
    printf("============================ Arm Ethos-U Benchmark =============================\n\n");
    ArmEthosBenchmark();
    printf("\n");
    printf("============================ TFLite Micro Benchmark ============================\n\n");
    TFLiteBenchmark();
    printf("\n");
    printf("================================================================================\n\n");
}
