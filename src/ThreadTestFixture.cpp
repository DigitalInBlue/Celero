///
/// \author	Ivan Shynkarenka
///
/// \copyright Copyright 2015 John Farrier
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
#include <celero/PimplImpl.h>

#include <iostream>
#include <algorithm>
#include <future>

#include <assert.h>

using namespace celero;

class ThreadTestFixture::Impl
{
	public:
		static thread_local uint64_t currentCallId;
		static thread_local uint64_t currentThreadId;
		std::vector<std::future<void>> futures;
};

uint64_t ThreadTestFixture::Impl::currentCallId = 0;
uint64_t ThreadTestFixture::Impl::currentThreadId = 0;

ThreadTestFixture::ThreadTestFixture()
{
}

ThreadTestFixture::~ThreadTestFixture()
{
}

void ThreadTestFixture::startThreads(uint64_t threads, uint64_t calls)
{
	uint64_t callsPerThread = calls / threads;
	for(uint64_t i = 0; i < threads; ++i)
	{
		this->pimpl->futures.push_back(std::async([this, i, callsPerThread]()
		{
			this->pimpl->currentThreadId = i + 1;
			for(auto operation = 0; operation < callsPerThread;)
			{
				this->pimpl->currentCallId = ++operation;
				this->UserBenchmark();
			}
		}));
	}
}

void ThreadTestFixture::stopThreads()
{
	// This part will be more effective after 
	// wait_for_all() will be avaliable for futures!
	for(auto& f : this->pimpl->futures)
	{
		f.wait();
	};
}

uint64_t ThreadTestFixture::run(uint64_t threads, uint64_t calls, int64_t experimentValue)
{
	// Set up the testing fixture.
	this->setUp(experimentValue);

	// Start working threads.
	this->startThreads(threads, calls);

	// Get the starting time.
	const auto startTime = celero::timer::GetSystemTime();

	this->onExperimentStart(experimentValue);

	// Stop working threads.
	this->stopThreads();

	this->onExperimentEnd();

	const auto endTime = celero::timer::GetSystemTime();

	// Tear down the testing fixture.
	this->tearDown();

	// Return the duration in microseconds for the given problem size.
	return (endTime - startTime);
}