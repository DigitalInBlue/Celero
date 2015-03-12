///
/// \author	John Farrier
///
/// \copyright Copyright 2014 John Farrier
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

#include <iostream>
#include <algorithm>

#include <assert.h>

using namespace celero;

ThreadTestFixture::ThreadTestFixture()
{
}

ThreadTestFixture::~ThreadTestFixture()
{
}

void ThreadTestFixture::startThreads(uint64_t threads, uint64_t calls)
{
	uint64_t callsPerThread = calls / threads;
    for (uint64_t i = 0; i < threads; ++i)
    {
		this->threads.push_back(std::thread([this, i, callsPerThread]()
        {
			currentThreadId = i + 1;
			for (auto operation = 0; operation < callsPerThread;)
			{
				currentCallId = ++operation;
				this->UserBenchmark();
			}
        }));
    }
}

void ThreadTestFixture::stopThreads()
{
	for (auto& thread : this->threads)
    {
        thread.join();
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

	// Stop working threads.
	this->stopThreads();

	const auto endTime = celero::timer::GetSystemTime();

	// Tear down the testing fixture.
	this->tearDown();

	// Return the duration in microseconds for the given problem size.
	return (endTime - startTime);
}
