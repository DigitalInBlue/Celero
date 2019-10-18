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
#include <celero/TestVector.h>
#include <gtest/gtest.h>

TEST(TestVector, push_back)
{
	auto& vector = celero::TestVector::Instance();
	EXPECT_NO_THROW(vector.clear());
	EXPECT_NO_THROW(vector.push_back(std::make_shared<celero::Benchmark>()));
	EXPECT_EQ(vector.size(), size_t{1});
}

TEST(TestVector, operator_index)
{
	auto& vector = celero::TestVector::Instance();
	EXPECT_NO_THROW(vector.clear());

	auto benchmark = std::make_shared<celero::Benchmark>();
	EXPECT_NO_THROW(vector.push_back(benchmark));
	EXPECT_TRUE(vector[0] == benchmark);

	benchmark = std::make_shared<celero::Benchmark>();
	EXPECT_NO_THROW(vector.push_back(benchmark));
	EXPECT_TRUE(vector[1] == benchmark);

	benchmark = std::make_shared<celero::Benchmark>();
	EXPECT_NO_THROW(vector.push_back(benchmark));
	EXPECT_TRUE(vector[2] == benchmark);

	EXPECT_EQ(vector.size(), size_t{3});
}

TEST(TestVector, operator_name)
{
	auto& vector = celero::TestVector::Instance();
	EXPECT_NO_THROW(vector.clear());

	auto benchmark = std::make_shared<celero::Benchmark>("0");
	EXPECT_NO_THROW(vector.push_back(benchmark));
	EXPECT_TRUE(vector["0"] == benchmark);

	benchmark = std::make_shared<celero::Benchmark>("1");
	EXPECT_NO_THROW(vector.push_back(benchmark));
	EXPECT_TRUE(vector["1"] == benchmark);

	benchmark = std::make_shared<celero::Benchmark>("2");
	EXPECT_NO_THROW(vector.push_back(benchmark));
	EXPECT_TRUE(vector["2"] == benchmark);

	EXPECT_TRUE(vector["badname"] == nullptr);

	EXPECT_EQ(vector.size(), size_t{3});
}
