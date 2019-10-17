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

#include <celero/Celero.h>
#include <celero/Experiment.h>
#include <celero/ExperimentResult.h>
#include <gtest/gtest.h>

const char* GroupNameString{"GroupName\0"};
const char* BenchmarkNameString = "BenchmarkName\0";
constexpr uint64_t Samples{0};
constexpr uint64_t Iterations{0};

// Build a class called "CeleroUserBenchmark_GroupName1_BenchmarkName1"
BENCHMARK(GroupName, BenchmarkName, Samples, Iterations){};

TEST(Celero, RegisterTest)
{
	std::shared_ptr<celero::Benchmark> benchmark;
	EXPECT_NO_THROW(benchmark = celero::RegisterTest(nullptr, nullptr, 0, 0, 0, nullptr));
	EXPECT_EQ(nullptr, benchmark);

	EXPECT_NO_THROW(benchmark = celero::RegisterTest(GroupNameString, nullptr, 0, 0, 0, nullptr));
	EXPECT_EQ(nullptr, benchmark);

	EXPECT_NO_THROW(benchmark = celero::RegisterTest(GroupNameString, BenchmarkNameString, 0, 0, 0, nullptr));
	EXPECT_NE(nullptr, benchmark);

	auto experimentFactory = std::make_shared<::celero::GenericFactory<CeleroUserBenchmark_GroupName_BenchmarkName>>();
	EXPECT_NO_THROW(benchmark = celero::RegisterTest(nullptr, nullptr, 0, 0, 0, experimentFactory));
	EXPECT_EQ(nullptr, benchmark);

	EXPECT_NO_THROW(benchmark = celero::RegisterTest(GroupNameString, nullptr, 0, 0, 0, experimentFactory));
	EXPECT_EQ(nullptr, benchmark);

	EXPECT_NO_THROW(benchmark = celero::RegisterTest(GroupNameString, BenchmarkNameString, 0, 0, 0, experimentFactory));
	EXPECT_NE(nullptr, benchmark);
}

TEST(Celero, RegisterBaseline)
{
	std::shared_ptr<celero::Benchmark> benchmark;
	EXPECT_NO_THROW(benchmark = celero::RegisterBaseline(nullptr, nullptr, 0, 0, 0, nullptr));
	EXPECT_EQ(nullptr, benchmark);

	EXPECT_NO_THROW(benchmark = celero::RegisterBaseline(GroupNameString, nullptr, 0, 0, 0, nullptr));
	EXPECT_EQ(nullptr, benchmark);

	EXPECT_NO_THROW(benchmark = celero::RegisterBaseline(GroupNameString, BenchmarkNameString, 0, 0, 0, nullptr));
	EXPECT_NE(nullptr, benchmark);

	auto experimentFactory = std::make_shared<::celero::GenericFactory<CeleroUserBenchmark_GroupName_BenchmarkName>>();
	EXPECT_NO_THROW(benchmark = celero::RegisterBaseline(nullptr, nullptr, 0, 0, 0, experimentFactory));
	EXPECT_EQ(nullptr, benchmark);

	EXPECT_NO_THROW(benchmark = celero::RegisterBaseline(GroupNameString, nullptr, 0, 0, 0, experimentFactory));
	EXPECT_EQ(nullptr, benchmark);

	EXPECT_NO_THROW(benchmark = celero::RegisterBaseline(GroupNameString, BenchmarkNameString, 0, 0, 0, experimentFactory));
	EXPECT_NE(nullptr, benchmark);
}

TEST(Celero, BuildDistribution)
{
	EXPECT_NO_THROW(celero::BuildDistribution(0, 0));
	
	const auto dist = celero::BuildDistribution(10, 10);
	EXPECT_EQ(size_t(10), dist.size());
}
