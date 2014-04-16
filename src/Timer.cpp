///
/// \author	John Farrier
///
/// \copyright Copyright 2014 John Farrier 
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
/// 
/// http://www.apache.org/licenses/LICENSE-2.0
/// 
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///

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
