# Just small experiment about C++ threads CPU usage (AI assisted)

> I am thinking about C++17 program having separate thread printing hello every 100 miliseconds ten times and two approaches: one with simply waiting using "std::this_thread::sleep_for" and another with busy while loop checking the interval using "std::chrono::steady_clock::now()" and keeping track of time difference. What is more CPU friendly. Provide both programs and the way how to measure CPU usage of both on Linux.

Below are two complete C++17 examples that each launch a separate thread that prints "hello" every 100 milliseconds for ten times. The first example uses std::this_thread::sleep_for, and the second uses a busy‐waiting loop by repeatedly checking std::chrono::steady_clock::now().

* Example 1: Using std::this_thread::sleep_for (CPU friendly)

```c++
#include <iostream>
#include <thread>
#include <chrono>

void printHelloSleep() {
    for (int i = 0; i < 10; ++i) {
        std::cout << "hello" << std::endl;
        // Sleep for 100 milliseconds – the OS will block this thread:
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::thread t(printHelloSleep);
    t.join();
    return 0;
}
```

In this version the thread executes, prints “hello” then sleeps. While sleeping, the thread does not consume CPU time; the OS scheduler puts the thread aside.

* Example 2: Using a Busy-Wait Loop (CPU intensive)

```c++
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
        std::cout << "hello" << std::endl;
        // Schedule next print 100 milliseconds later.
        nextTimePoint += std::chrono::milliseconds(100);
    }
}

int main() {
    std::thread t(printHelloBusyWait);
    t.join();
    return 0;
}
```

Here, instead of blocking, the thread continuously checks the time until 100 milliseconds have elapsed. This loop uses processor cycles continuously and is therefore far less CPU friendly.

## Measuring CPU Usage on Linux

There are several ways to measure the CPU usage of these programs on Linux:

1. Using the time Command:
   • Compile the program (for example, busy waiting version):
         g++ -std=c++17 busy_wait.cpp -pthread -O2 -o busy_wait
   • Run it with:
         time ./busy_wait
   Look at the “user” CPU time reported. For the busy-wait version, the CPU time will be close to (or even exceed) the real elapsed time whereas the sleep version will show minimal user CPU time.

2. Using Top or Htop:
   • Start the program in one terminal. In another, run:
         top
     or
         htop
   • Observe the CPU usage of the process. The busy-wait loop will typically consume near 100% of one CPU core, while the sleep-based version will use very little CPU.

3. Using Perf:
   • If you have the “perf” tool installed, you can run:
         perf stat ./busy_wait
     or
         perf stat ./sleep_program
   This will show detailed statistics such as CPU cycles, instructions, and CPU time.

● Conclusion

The version using std::this_thread::sleep_for is more CPU friendly because it tells the OS to pause the thread’s execution, freeing up the CPU for other tasks. In contrast, the busy-wait loop continuously polls the clock and consumes CPU cycles even when no useful work is being done.

## Results 

**Note:** Run by actual human :-)

```
$ g++ sleep.cpp 
$ sudo perf stat ./a.out 
hello sleepy
hello sleepy
hello sleepy
hello sleepy
hello sleepy
hello sleepy
hello sleepy
hello sleepy
hello sleepy
hello sleepy

 Performance counter stats for './a.out':

              2.26 msec task-clock                       #    0.002 CPUs utilized             
                11      context-switches                 #    4.873 K/sec                     
                 0      cpu-migrations                   #    0.000 /sec                      
               136      page-faults                      #   60.245 K/sec                     
         6,469,337      cycles                           #    2.866 GHz                       
         4,222,736      instructions                     #    0.65  insn per cycle            
           741,926      branches                         #  328.656 M/sec                     
            29,293      branch-misses                    #    3.95% of all branches           

       1.003523246 seconds time elapsed

       0.000000000 seconds user
       0.002801000 seconds sys


$ g++ busy_wait.cpp 
$ sudo perf stat ./a.out 
hello busy
hello busy
hello busy
hello busy
hello busy
hello busy
hello busy
hello busy
hello busy
hello busy

 Performance counter stats for './a.out':

            901.59 msec task-clock                       #    0.999 CPUs utilized             
                15      context-switches                 #   16.637 /sec                      
                 2      cpu-migrations                   #    2.218 /sec                      
               138      page-faults                      #  153.064 /sec                      
     3,015,361,481      cycles                           #    3.345 GHz                       
     5,441,317,336      instructions                     #    1.80  insn per cycle            
       953,539,487      branches                         #    1.058 G/sec                     
         2,456,645      branch-misses                    #    0.26% of all branches           

       0.902288620 seconds time elapsed

       0.899836000 seconds user
       0.001999000 seconds sys
```
