///
/// \author	John Farrier
///
/// \copyright Copyright 2015-2023 John Farrier
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

#include <assert.h>
#include <celero/TestFixture.h>
#include <celero/UserDefinedMeasurement.h>
#include <algorithm>
#include <iostream>

using namespace celero;

TestFixture::TestFixture()
{
}

TestFixture::~TestFixture()
{
}

void TestFixture::onExperimentStart(const celero::TestFixture::ExperimentValue&)
{
}

void TestFixture::onExperimentEnd()
{
}

void TestFixture::setUp(const celero::TestFixture::ExperimentValue&)
{
}

void TestFixture::setExperimentIterations(uint64_t x)
{
	this->experimentIterations = x;
}

uint64_t TestFixture::getExperimentIterations() const
{
	return this->experimentIterations;
}

void TestFixture::setExperimentTime(uint64_t x)
{
	this->experimentTime = x;
}

uint64_t TestFixture::getExperimentTime() const
{
	return this->experimentTime;
}

void TestFixture::tearDown()
{
}

uint64_t TestFixture::run(const uint64_t, const uint64_t iterations, const celero::TestFixture::ExperimentValue& experimentValue)
{
	// This function constitutes one sample consisting of several iterations for a single experiment value.

	if(this->HardCodedMeasurement() == 0)
	{
		uint64_t totalTime = 0;

		// Set up the testing fixture.
		this->setUp(experimentValue);
		this->setExperimentIterations(iterations);

		// Run the test body for each iterations.
		auto iterationCounter = iterations;

		// Get the starting time.
		const auto startTime = celero::timer::GetSystemTime();

		// Count down to zero
		// Iterations are used when the benchmarks are very fast.
		// Do not start/stop the timer inside this loop.
		// The purpose of the loop is to help counter timer quantization/errors.
		while(iterationCounter--)
		{
			this->onExperimentStart(experimentValue);

			this->UserBenchmark();

			this->onExperimentEnd();
		}

		// See how long it took.
		totalTime += celero::timer::GetSystemTime() - startTime;

		this->setExperimentTime(totalTime);

		// Tear down the testing fixture.
		this->tearDown();

		// Return the duration in microseconds for the given problem size.
		return totalTime;
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

std::vector<std::shared_ptr<UserDefinedMeasurement>> TestFixture::getUserDefinedMeasurements() const
{
	return {};
}

std::vector<std::string> TestFixture::getUserDefinedMeasurementNames() const
{
	std::vector<std::string> names;
	const auto udms = this->getUserDefinedMeasurements();

	if(udms.empty() == false)
	{
		for(const auto& udm : udms)
		{
			names.emplace_back(udm->getName());
		}
	}

	return names;
}
