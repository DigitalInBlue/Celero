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
#include <celero/TestFixture.h>
#include <gtest/gtest.h>

namespace
{
	class Fixture : public celero::TestFixture
	{
	public:
		void onExperimentStart(const celero::TestFixture::ExperimentValue&) override
		{
			this->onStart = true;
		}

		void onExperimentEnd() override
		{
			this->onEnd = true;
		}

		void setUp(const celero::TestFixture::ExperimentValue&) override
		{
			this->onSetUp = true;
		}

		void tearDown() override
		{
			this->onTearDown = true;
		}

		bool onStart{false};
		bool onEnd{false};
		bool onSetUp{false};
		bool onTearDown{false};
	};
} // namespace

TEST(TestFixture, Run)
{
	constexpr uint64_t iterations = 10;
	Fixture fixture;
	EXPECT_NO_THROW(fixture.run(0, iterations, {}));

	EXPECT_TRUE(fixture.onStart);
	EXPECT_TRUE(fixture.onEnd);
	EXPECT_TRUE(fixture.onSetUp);
	EXPECT_TRUE(fixture.onTearDown);

	EXPECT_EQ(iterations, fixture.getExperimentIterations());
}
