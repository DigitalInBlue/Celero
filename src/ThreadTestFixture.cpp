///
/// \author	Ivan Shynkarenka
///
/// \copyright Copyright 2015, 2016, 2017, 2018 John Farrier
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

#include <celero/ThreadTestFixture.h>

#include <assert.h>
#include <celero/PimplImpl.h>
#include <algorithm>
#include <future>
#include <iostream>

using namespace celero;

class ThreadTestFixture::Impl
{
public:
	static thread_local uint64_t currentCallId;
	static thread_local uint64_t currentThreadId;
	std::vector<std::future<void>> futures;
};

thread_local uint64_t ThreadTestFixture::Impl::currentCallId = 0;
thread_local uint64_t ThreadTestFixture::Impl::currentThreadId = 0;

ThreadTestFixture::ThreadTestFixture() : TestFixture()
{
}

ThreadTestFixture::~ThreadTestFixture()
{
}

void ThreadTestFixture::startThreads(uint64_t threads, uint64_t iterations)
{
	const uint64_t iterationsPerThread = iterations / threads;

	for(uint64_t i = 0; i < threads; ++i)
	{
		try
		{
			this->pimpl->futures.push_back(
				// std::async(std::launch::deferred,
				std::async(std::launch::async, [this, i, iterationsPerThread]() {
					this->pimpl->currentThreadId = i + 1;
					for(auto threadIterationCounter = size_t(0); threadIterationCounter < iterationsPerThread;)
					{
						this->pimpl->currentCallId = ++threadIterationCounter;
						this->UserBenchmark();
					}
				}));
		}
		catch(std::system_error& e)
		{
			std::cerr << "Exception. Error Code: " << e.code() << ", " << e.what() << std::endl;
		}
	}
}

void ThreadTestFixture::stopThreads()
{
	// This part will be more effective after
	// wait_for_all() will be avaliable for futures!
	for(auto& f : this->pimpl->futures)
	{
		if(f.valid() == true)
		{
			try
			{
				f.wait();
			}
			catch(std::system_error& e)
			{
				std::cerr << "Exception. Error Code: " << e.code() << ", " << e.what() << std::endl;
			}
		}
	};
}

uint64_t ThreadTestFixture::run(uint64_t threads, uint64_t calls, const celero::TestFixture::ExperimentValue& experimentValue)
{
	if(this->HardCodedMeasurement() == 0)
	{
		// Set up the testing fixture.
		this->setUp(experimentValue);

		// Get the starting time.
		const auto startTime = celero::timer::GetSystemTime();

		this->onExperimentStart(experimentValue);

		// Start working threads.
		this->startThreads(threads, calls);

		// Stop working threads.
		this->stopThreads();

		this->onExperimentEnd();

		const auto endTime = celero::timer::GetSystemTime();

		// Tear down the testing fixture.
		this->tearDown();

		// Return the duration in microseconds for the given problem size.
		return (endTime - startTime);
	}

	return this->HardCodedMeasurement();
}
