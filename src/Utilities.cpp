// www.helleboreconsulting.com

///
/// \author	John Farrier
///
/// \copyright Copyright 2015 John Farrier
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

#include <celero/Utilities.h>

template<>
void celero::DoNotOptimizeAway(std::function<void(void)>&& x)
{
	x();

	//
	// We must always do this test, but it will never pass.
	//
	if(std::chrono::system_clock::now() == std::chrono::time_point<std::chrono::system_clock>())
	{
		// This forces the value to never be optimized away
		// by taking a reference then using it.
		const auto* p = &x;
		putchar(*reinterpret_cast<const char*>(p));

		// If we do get here, kick out because something has gone wrong.
		std::abort();
	}
}
