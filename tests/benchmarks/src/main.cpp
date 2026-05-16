
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"
#undef ANKERL_NANOBENCH_IMPLEMENT

#include "../../sources.cpp"

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestPlugin.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupportPlugin.h"
#include "Logging.h"

int main(int ac, char **av) {
    // Init spdlog before CppUTest leak tracking to avoid false leaks from lazy initialization
    spdlog::default_logger();
    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    MockSupportPlugin mockPlugin;
    TestRegistry::getCurrentRegistry()->installPlugin(&mockPlugin);
    return CommandLineTestRunner::RunAllTests(ac, av);
}
