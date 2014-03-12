#include <celero/Timer.h>

#ifdef WIN32
	#include <Windows.h>
	LARGE_INTEGER QPCFrequency;
#else
	#include <sys/time.h>
#endif

uint64_t celero::timer::GetSystemTime()
{
	/// \todo	Replace celero::timer::GetSystemTime() with std::chrono
	#ifdef WIN32
		LARGE_INTEGER timeStorage;
		QueryPerformanceCounter(&timeStorage);
		return timeStorage.QuadPart;
	#else
		timeval timeStorage;
		gettimeofday(&timeStorage, nullptr);
		return timeStorage.tv_sec * 1000000 + timeStorage.tv_usec;
	#endif
}

double celero::timer::ConvertSystemTime(uint64_t x)
{
	/// \todo	Replace celero::timer::GetSystemTime(uint64_t x) with std::chrono
	#ifdef WIN32
		return static_cast<double>(x)/static_cast<double>(QPCFrequency.QuadPart);
	#else
		return x * 1.0e-6;
	#endif
}

void celero::timer::CachePerformanceFrequency()
{
	#ifdef WIN32
		QueryPerformanceFrequency(&QPCFrequency);
	#endif
}
