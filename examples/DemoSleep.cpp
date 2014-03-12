#include <celero/Celero.h>

#include <thread>
#include <chrono>

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN;

BASELINE(DemoSleep, Baseline, 0, 1)
{
	std::this_thread::sleep_for(std::chrono::microseconds(100000));
}

BENCHMARK(DemoSleep, HalfBaseline, 0, 1)
{
	std::this_thread::sleep_for(std::chrono::microseconds(50000));
}

BENCHMARK(DemoSleep, TwiceBaseline, 0, 1)
{
	std::this_thread::sleep_for(std::chrono::microseconds(200000));
}
