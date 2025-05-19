#include <iostream>
#include <thread>
#include <chrono>

void printHelloSleep() {
    for (int i = 0; i < 10; ++i) {
        std::cout << "hello sleepy" << std::endl;
        // Sleep for 100 milliseconds – the OS will block this thread:
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::thread t(printHelloSleep);
    t.join();
    return 0;
}
