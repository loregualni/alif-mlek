#ifndef BENCHMARK_HANDLER_HPP
#define BENCHMARK_HANDLER_HPP

#include "AppContext.hpp"

namespace arm {
namespace app {

    /**
     * @brief       Handles the inference benchmark event.
     * @param[in]   ctx   Pointer to the application context.
     * @return      true or false based on execution success.
     **/
    bool RunBenchmarkHandler(ApplicationContext& ctx);

} /* namespace app */
} /* namespace arm */

#endif /* BENCHMARK_HANDLER_HPP */
