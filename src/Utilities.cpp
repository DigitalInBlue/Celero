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
#include <celero/Utilities.h>

#ifdef WIN32
#include <Windows.h>

#include <PowrProf.h>
#endif

#ifdef max
#undef max
#endif

#include <limits>
#include <random>

template <>
void celero::DoNotOptimizeAway(std::function<void(void)>&& x)
{
	x();

	// We must always do this test, but it will never pass.
	static auto ttid = std::this_thread::get_id();
	if(ttid == std::thread::id())
	{
		// This forces the value to never be optimized away
		// by taking a reference then using it.
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));

		// If we do get here, kick out because something has gone wrong.
		std::abort();
	}
}

int celero::Random()
{
	// http://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution

	// Will be used to obtain a seed for the random number engine
	static std::random_device rd;

	// Standard mersenne_twister_engine seeded with rd()
	static std::mt19937 gen(rd());

	static std::uniform_int_distribution<> dis(std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max());

	return dis(gen);
}
