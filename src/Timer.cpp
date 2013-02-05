#include <celero/Timer.h>

#ifdef WIN32
	#include <Windows.h>
#else
	#include <sys/time.h>
#endif

uint64_t celero::timer::GetSystemTime()
{
	/// \todo	Replace celero::timer::GetSystemTime() with std::chrono
	#ifdef WIN32
		LARGE_INTEGER timeStorage;
		QueryPerformanceCounter(&timeStorage);
		return static_cast<uint64_t>(timeStorage.QuadPart);
	#else
		timeval timeStorage;
		gettimeofday(&timeStorage, nullptr);
		return timeStorage.tv_sec * 1000000 + timeStorage.tv_usec;
	#endif
}

double celero::timer::GetSystemTime(uint64_t x)
{
	/// \todo	Replace celero::timer::GetSystemTime(uint64_t x) with std::chrono
	#ifdef WIN32
		static LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		return static_cast<double>(x)/static_cast<double>(frequency.QuadPart);
	#else
		return x * 1.0e-6;
	#endif
}
