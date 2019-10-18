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
#include <celero/Statistics.h>
#include <gtest/gtest.h>

TEST(Statistics, getMinMax)
{
	constexpr auto total{10};
	celero::Statistics<int> stats;
	for(auto i = 0; i <= total; i++)
	{
		stats.addSample(i);
	}

	EXPECT_EQ(0, stats.getMin());
	EXPECT_EQ(total, stats.getMax());
}

TEST(Statistics, getMean)
{
	constexpr auto total{10};
	constexpr auto sample{10};
	celero::Statistics<int> stats;

	for(auto i = 0; i < total; i++)
	{
		stats.addSample(sample);
	}

	EXPECT_EQ(sample, stats.getMean());
	EXPECT_EQ(sample, stats.getMin());
	EXPECT_EQ(sample, stats.getMax());
}

TEST(Statistics, reset)
{
	constexpr auto total{10};
	constexpr auto sample{10};
	celero::Statistics<int> stats;

	for(auto i = 0; i < total; i++)
	{
		stats.addSample(sample);
	}

	EXPECT_EQ(sample, stats.getMean());
	EXPECT_EQ(sample, stats.getMin());
	EXPECT_EQ(sample, stats.getMax());
	EXPECT_EQ(size_t{total}, stats.getSize());
	EXPECT_NO_THROW(stats.reset());

	EXPECT_NE(sample, stats.getMean());
	EXPECT_NE(sample, stats.getMin());
	EXPECT_NE(sample, stats.getMax());
	EXPECT_EQ(size_t{0}, stats.getSize());
}
