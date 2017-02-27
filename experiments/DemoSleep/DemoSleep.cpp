#include <celero/Celero.h>

#include <chrono>
#include <thread>

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

BASELINE(DemoSleep, Baseline, 60, 1)
{
	std::this_thread::sleep_for(std::chrono::microseconds(100000));
}

BENCHMARK(DemoSleep, HalfBaseline, 60, 1)
{
	std::this_thread::sleep_for(std::chrono::microseconds(50000));
}

BENCHMARK(DemoSleep, TwiceBaseline, 60, 1)
{
	std::this_thread::sleep_for(std::chrono::microseconds(200000));
}
