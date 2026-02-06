#include <chrono>
#include <iomanip>
#include <sstream>

const char *log_time() {
    using namespace std::chrono;

    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = system_clock::to_time_t(now);
    std::tm bt;
    localtime_r(&timer, &bt);

    static thread_local char time_str[64];
    snprintf(time_str,
             sizeof(time_str),
             "%02d %02d:%02d:%02d.%03d",
             bt.tm_mday,
             bt.tm_hour,
             bt.tm_min,
             bt.tm_sec,
             (int)ms.count());
    return time_str;
}
