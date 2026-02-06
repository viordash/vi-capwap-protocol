
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"
#undef ANKERL_NANOBENCH_IMPLEMENT

#include "../../sources.cpp"

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestPlugin.h"
#include "CppUTest/TestRegistry.h"
#include "CppUTestExt/MockSupportPlugin.h"

int main(int ac, char **av) {
    MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    MockSupportPlugin mockPlugin;
    TestRegistry::getCurrentRegistry()->installPlugin(&mockPlugin);
    return CommandLineTestRunner::RunAllTests(ac, av);
}
