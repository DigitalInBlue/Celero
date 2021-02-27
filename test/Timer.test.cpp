///
/// \author	Aaron Shelley
///
/// \copyright Copyright 2015, 2016, 2017 John Farrier
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
#include <gtest/gtest.h>

TEST(Timer, GetSystemTime)
{
	const auto time = celero::timer::GetSystemTime();
	EXPECT_NE(time, 0);
}

TEST(Timer, CachePerformanceFrequency)
{
	const auto freq = celero::timer::CachePerformanceFrequency(true);
	EXPECT_NE(freq, 0.0);
}
