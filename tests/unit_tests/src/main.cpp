
#include "../../sources.cpp"

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/MemoryLeakWarningPlugin.h"
#include "CppUTest/TestPlugin.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupportPlugin.h"
#include "Logging.h"

int main(int ac, char **av) {
#ifdef VI_CAPWAP_LOGGING_SPDLOG
    // Init spdlog before CppUTest leak tracking to avoid false leaks from lazy initialization
    spdlog::default_logger();
#endif
    MemoryLeakWarningPlugin::turnOnThreadSafeNewDeleteOverloads();
    MockSupportPlugin mockPlugin;
    TestRegistry::getCurrentRegistry()->installPlugin(&mockPlugin);
    return CommandLineTestRunner::RunAllTests(ac, av);
}
