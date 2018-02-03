#ifndef H_CELERO_BENCHMARK_H
#define H_CELERO_BENCHMARK_H

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

#include <celero/Experiment.h>
#include <functional>
#include <memory>
#include <string>

namespace celero
{
	///
	/// \class Benchmark
	///
	/// \author	John Farrier
	///
	class CELERO_EXPORT Benchmark
	{
	public:
		///
		/// \brief	Default constructor
		///
		Benchmark();

		///
		/// \brief	Overloaded constructor.
		///
		/// \param name Name of the test group.
		///
		Benchmark(const std::string& name);

		///
		///
		///
		Benchmark(const Benchmark& other);

		///
		/// \brief	Default destructor.
		///
		~Benchmark();

		///
		/// Assignment Operator
		///
		Benchmark& operator=(const Benchmark& other);

		///
		/// The name to group all experiment under.
		///
		std::string getName() const;

		///
		///
		///
		void setBaseline(std::shared_ptr<Experiment> x);

		///
		/// Gets the baseline case associated this benchmark.
		///
		std::shared_ptr<Experiment> getBaseline() const;

		///
		///
		///
		void addExperiment(std::shared_ptr<Experiment> x);

		///
		/// Gets the test case associated with the given experiment index.
		///
		std::shared_ptr<Experiment> getExperiment(size_t experimentIndex);

		///
		/// Gets the test case associated with the given experiment name.
		///
		std::shared_ptr<Experiment> getExperiment(const std::string& experimentName);

		///
		/// Returns the total number of experiments per benchmark.
		///
		size_t getExperimentSize() const;

	private:
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

#endif
