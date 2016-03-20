///
/// \author	John Farrier
///
/// \copyright Copyright 2016 John Farrier 
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

#include <iostream>
#include <algorithm>

#include <assert.h>

using namespace celero;

TestFixture::TestFixture()
{
}

TestFixture::~TestFixture()
{
}

void TestFixture::onExperimentStart(int64_t)
{
}

void TestFixture::onExperimentEnd()
{
}

void TestFixture::setUp(int64_t)
{
}

void TestFixture::tearDown()
{
}
	
uint64_t TestFixture::run(const uint64_t, const uint64_t iterations, int64_t experimentValue)
{
	if(this->HardCodedMeasurement() == 0)
	{
		// Set up the testing fixture.
		this->setUp(experimentValue);

		// Run the test body for each iterations.
		auto iterationCounter = iterations;

		// Get the starting time.
		const auto startTime = celero::timer::GetSystemTime();

		this->onExperimentStart(experimentValue);

		// Count down to zero
		while(iterationCounter--)
		{
			this->UserBenchmark();
		}

		this->onExperimentEnd();

		const auto endTime = celero::timer::GetSystemTime();

		// Tear down the testing fixture.
		this->tearDown();

		// Return the duration in microseconds for the given problem size.
		return (endTime - startTime);
	}

	return this->HardCodedMeasurement();
}

void TestFixture::UserBenchmark()
{
}

uint64_t TestFixture::HardCodedMeasurement() const
{
	return uint64_t(0);
}
