#include <chrono>
#include <thread>
#include <cstdio>
#include <functional>
namespace mstack{
class timer
{
public:
    template <class callable, class... arguments>
    timer(int after, callable&& f, arguments&&... args)
    {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
        std::thread([after, task]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }).detach();
    }
};
};