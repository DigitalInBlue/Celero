#include <celero/Timer.h>

#ifdef WIN32
	#include <Windows.h>
#else
#endif

uint64_t celero::timer::GetSystemTime()
{
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

double celero::timer::GetSystemTime(uint64_t x)
{
	#ifdef WIN32
		static LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		return static_cast<double>(x)/static_cast<double>(frequency.QuadPart);
	#else
		return x * 1.0e-6;
	#endif
}
