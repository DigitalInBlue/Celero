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
#include <celero/Utilities.h>
#include <algorithm>
#include <cassert>

using namespace celero;

class Benchmark::Impl
{
public:
	Impl() : stats(), name(), baseline(), experiments()
	{
	}

	Impl(const std::string& x) : stats(), name(x), baseline(), experiments()
	{
	}

	Impl(const Benchmark& other) : stats(), name(other.pimpl->name), baseline(), experiments()
	{
	}

	void copy(const Benchmark& other)
	{
		stats = other.pimpl->stats;
		name = other.pimpl->name;
		baseline = other.pimpl->baseline;
		experiments = other.pimpl->experiments;
	}

	Statistics stats;

	/// Group name
	std::string name;

	std::shared_ptr<Experiment> baseline;
	std::vector<std::shared_ptr<Experiment>> experiments;
};

Benchmark::Benchmark() : pimpl()
{
}

Benchmark::Benchmark(const std::string& name) : pimpl(name)
{
}

Benchmark::Benchmark(const Benchmark& other) : pimpl(other)
{
}

Benchmark::~Benchmark()
{
}

Benchmark& Benchmark::operator=(const Benchmark& other)
{
	if(&other != this)
	{
		this->pimpl->copy(other);
	}

	return *this;
}

std::string Benchmark::getName() const
{
	return this->pimpl->name;
}

void Benchmark::setBaseline(std::shared_ptr<Experiment> x)
{
	this->pimpl->baseline = x;
}

std::shared_ptr<Experiment> Benchmark::getBaseline() const
{
	return this->pimpl->baseline;
}

void Benchmark::addExperiment(std::shared_ptr<Experiment> x)
{
	this->pimpl->experiments.push_back(x);
}

std::shared_ptr<Experiment> Benchmark::getExperiment(size_t x)
{
	// This is unsafe, but not user code.  I'll accept the risk.
	return this->pimpl->experiments[x];
}

std::shared_ptr<Experiment> Benchmark::getExperiment(const std::string& x)
{
	return *std::find_if(std::begin(this->pimpl->experiments), std::end(this->pimpl->experiments),
						 [x](decltype(*std::begin(this->pimpl->experiments)) i) -> bool { return (i->getName() == x); });
}

size_t Benchmark::getExperimentSize() const
{
	return this->pimpl->experiments.size();
}
