#pragma once

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

#include <celero/ExperimentResult.h>
#include <celero/Factory.h>
#include <celero/Statistics.h>

#include <string>

namespace celero
{
	class Benchmark;

	///
	/// \class Experiment
	///
	/// \author	John Farrier
	///
	class CELERO_EXPORT Experiment
	{
	public:
		///
		///
		///
		explicit Experiment(std::weak_ptr<celero::Benchmark> benchmark);

		///
		///
		///
		explicit Experiment(std::weak_ptr<celero::Benchmark> benchmark, const std::string& name, uint64_t samples, uint64_t iterations,
							uint64_t threads, double baselineTarget);

		///
		/// \brief	Default destructor.
		///
		~Experiment();

		Experiment(const Experiment&) = delete;
		Experiment& operator=(Experiment const& other) = delete;

		///
		/// Gets a pointer to the owning Benchmark object.
		///
		std::shared_ptr<celero::Benchmark> getBenchmark();

		///
		///
		///
		void setName(const std::string& x);

		///
		///
		///
		std::string getName() const;

		///
		///
		///
		void setSamples(uint64_t x);

		///
		///
		///
		uint64_t getSamples() const;

		///
		///
		///
		void setIterations(uint64_t x);

		///
		///
		///
		uint64_t getIterations() const;

		///
		///
		///
		void setThreads(uint64_t x);

		///
		///
		///
		uint64_t getThreads() const;

		///
		///
		///
		operator std::string() const;

		///
		///
		///
		std::string getShort() const;

		///
		///
		///
		void setBaselineTarget(double x);

		///
		///
		///
		double getBaselineTarget() const;

		///
		///
		///
		void incrementTotalRunTime(const uint64_t x);

		///
		///
		///
		uint64_t getTotalRunTime() const;

		///
		/// Used to set a flag indicating that this is a Baseline case, not a benchmark case.
		///
		void setIsBaselineCase(bool x);

		///
		/// Used to get a flag indicating that this is a Baseline case, not a benchmark case.
		///
		bool getIsBaselineCase() const;

		///
		/// Sets the factory used to create this experiment's test fixtures.
		///
		void setFactory(std::shared_ptr<celero::Factory> x);

		///
		/// Gets the factory used to create this experiment's test fixtures.
		///
		std::shared_ptr<celero::Factory> getFactory() const;

		///
		/// \param x	Can be interpreted in any way be the test fixture (i.e. index into an array, etc.)
		/// \param scale	Used to format unit results.
		///
		void addProblemSpace(std::shared_ptr<celero::TestFixture::ExperimentValue> x, double scale = 1.0);

		///
		/// There is one result for each problem space.
		///
		size_t getResultSize();

		///
		/// Get an ExperimentResult at a given index.
		///
		std::shared_ptr<celero::ExperimentResult> getResult(size_t x);

		///
		/// Get the ExperimentResult for the given problem space value.
		///
		std::shared_ptr<celero::ExperimentResult> getResultByValue(int64_t x);

	private:
		///
		/// Hide the default constructor
		///
		Experiment();

		///
		///
		/// \brief	Pimpl Idiom
		///
		class Impl;

		///
		/// \brief	Pimpl Idiom
		///
		Pimpl<Impl> pimpl;
	};
} // namespace celero

