///
/// \author	John Farrier
///
/// \copyright Copyright 2019 John Farrier
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

#include <celero/Distribution.h>
#include <gtest/gtest.h>

TEST(Distribution, BuildDistribution)
{
	std::vector<uint64_t> dist;
	EXPECT_NO_THROW(dist = celero::BuildDistribution(0, 0));

	EXPECT_NO_THROW(dist = celero::BuildDistribution(10, 0));
	EXPECT_EQ(size_t(10), dist.size());
	
	EXPECT_NO_THROW(dist = celero::BuildDistribution(10, 10));
	EXPECT_EQ(size_t(10), dist.size());
}

TEST(Distribution, RunDistribution)
{
	const uint64_t t0 = 0;
	const uint64_t t10 = 10;
	const uint64_t t100 = 100;

	EXPECT_NO_THROW(celero::RunDistribution(t0));
	EXPECT_NO_THROW(celero::RunDistribution(t10));
	EXPECT_NO_THROW(celero::RunDistribution(t100));
}

TEST(Distribution, BuildDistributionEdgeCases)
{
	// Test extreme edge cases for the BuildDistribution function
	std::vector<uint64_t> dist;

	// Test with large numbers
	const int ArbitraryDistributionSize = 1000000;
	EXPECT_NO_THROW(dist = celero::BuildDistribution(ArbitraryDistributionSize, 0));
	EXPECT_EQ(size_t(ArbitraryDistributionSize), dist.size());

	// Test with both parameters set to 0
	EXPECT_NO_THROW(dist = celero::BuildDistribution(0, 0));
	EXPECT_EQ(size_t(0), dist.size());
}

TEST(Distribution, ValidateBuildDistributionOutput)
{
	// Validate that the output is correct for given input
	std::vector<uint64_t> dist;

	dist = celero::BuildDistribution(10, 5);
	EXPECT_EQ(size_t(10), dist.size());

	for(auto& value : dist)
	{
		// Ensure values are within expected range (0 to 5)
		EXPECT_LE(value, 5);
		EXPECT_GE(value, 0);
	}
}
