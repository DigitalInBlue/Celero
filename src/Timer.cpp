///
/// \author	John Farrier
///
/// \copyright Copyright 2015, 2016, 2017, 2018 John Farrier
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

#include <celero/Print.h>
#include <celero/Timer.h>
#include <iostream>

#ifdef WIN32
#include <Windows.h>
LARGE_INTEGER QPCFrequency;
#else
#include <chrono>
#endif

uint64_t celero::timer::GetSystemTime()
{
#ifdef WIN32
	LARGE_INTEGER timeStorage;
	QueryPerformanceCounter(&timeStorage);
	return static_cast<uint64_t>(timeStorage.QuadPart * 1000000) / static_cast<uint64_t>(QPCFrequency.QuadPart);
#else
	auto timePoint = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(timePoint.time_since_epoch()).count();
#endif
}

double celero::timer::CachePerformanceFrequency(bool quiet)
{
#ifdef WIN32
	QueryPerformanceFrequency(&QPCFrequency);
	auto precision = ((1.0 / static_cast<double>(QPCFrequency.QuadPart)) * 1000000.0);
#else
	auto precision =
		(static_cast<double>(std::chrono::high_resolution_clock::period::num) / static_cast<double>(std::chrono::high_resolution_clock::period::den))
		* 1000000.0;
#endif

	if(quiet == false)
	{
		std::cout << "Timer resolution: " << std::to_string(precision) << " us" << std::endl;
	}

	return precision;
}
