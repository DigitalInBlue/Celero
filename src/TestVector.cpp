///
/// \author	John Farrier
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

#include <celero/Benchmark.h>
#include <celero/PimplImpl.h>
#include <celero/TestVector.h>
#include <algorithm>
#include <mutex>
#include <vector>

using namespace celero;

///
/// \class Impl
///
class celero::TestVector::Impl
{
public:
	Impl() : testVectorMutex(), testVector()
	{
	}

	mutable std::mutex testVectorMutex;
	std::vector<std::shared_ptr<Benchmark>> testVector;
};

TestVector::TestVector() : pimpl()
{
}

TestVector& TestVector::Instance()
{
	static TestVector singleton;
	return singleton;
}

void TestVector::push_back(std::shared_ptr<Benchmark> x)
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->testVectorMutex);
	this->pimpl->testVector.push_back(x);
}

size_t TestVector::size() const
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->testVectorMutex);
	return this->pimpl->testVector.size();
}

std::shared_ptr<Benchmark> TestVector::operator[](size_t x)
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->testVectorMutex);
	return this->pimpl->testVector[x];
}

std::shared_ptr<Benchmark> TestVector::operator[](const std::string& x)
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->testVectorMutex);

	const auto found = std::find_if(std::begin(this->pimpl->testVector), std::end(this->pimpl->testVector),
									[x](std::shared_ptr<Benchmark> const& bmark) -> bool { return (bmark->getName() == x); });

	if(found != std::end(this->pimpl->testVector))
	{
		return *found;
	}

	return nullptr;
}
