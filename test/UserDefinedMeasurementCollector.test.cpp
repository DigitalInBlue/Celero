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

#include <celero/UserDefinedMeasurementCollector.h>
#include <gtest/gtest.h>

TEST(UserDefinedMeasurementCollector, getFields)
{
	auto fixture = std::make_shared<celero::TestFixture>();
	ASSERT_TRUE(fixture != nullptr);

	celero::UserDefinedMeasurementCollector collector(fixture);

	const auto fields = collector.getFields(fixture);
	EXPECT_TRUE(fields.empty());
}

TEST(UserDefinedMeasurementCollector, getAggregateValues)
{
	auto fixture = std::make_shared<celero::TestFixture>();
	ASSERT_TRUE(fixture != nullptr);

	celero::UserDefinedMeasurementCollector collector(fixture);

	const auto values = collector.getAggregateValues();
	EXPECT_TRUE(values.empty());
}