#include <iostream>
#include <thread>
#include <chrono>

void printHelloBusyWait() {
    using Clock = std::chrono::steady_clock;
    auto nextTimePoint = Clock::now();

    for (int i = 0; i < 10; ++i) {
        // Busy wait until the current time reaches the scheduled point
        while (Clock::now() < nextTimePoint) {
            // Do nothing (spinning)
        }
        std::cout << "hello busy" << std::endl;
        // Schedule next print 100 milliseconds later.
        nextTimePoint += std::chrono::milliseconds(100);
    }
}

int main() {
    std::thread t(printHelloBusyWait);
    t.join();
    return 0;
}
