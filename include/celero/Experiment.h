#ifndef H_CELERO_EXPERIMENT_H
#define H_CELERO_EXPERIMENT_H

// www.helleboreconsulting.com

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

#include <string>
#include <celero/Factory.h>
#include <celero/Statistics.h>

namespace celero
{
	class Benchmark;
	class Statistics;

	///
	/// \class Experiment
	///
	/// \author	John Farrier
	///
	class CELERO_EXPORT Experiment
	{
		public:
			class Result
			{
				public:
					Result(Experiment* x);

					///
					/// Gets a pointer to the owning experiment.
					///
					Experiment* getExperiment() const;

					///
					///
					///
					void setProblemSpaceValue(int64_t x);

					///
					///
					///
					int64_t getProblemSpaceValue() const;

					///
					///
					///
					Statistics* getStatistics();

					///
					/// Adds a run time sample during experiment execution.
					///
					void addRunTimeSample(const uint64_t x);

					///
					/// Returns the best run time sample observed.
					///
					uint64_t getRunTime() const;

					///
					/// \brief	Get the number of computed microseconds per operation.
					///
					///	An operation is defined as one operation of one run.
					///
					double getUsPerCall() const;

					///
					/// \brief	Get the number of computer operations per second.
					///
					///	An operation is defined as one operation of one run.
					///
					double getOpsPerSecond() const;

					///
					/// Calculate this experiments baseline value.
					///
					/// If this IS a baseline experiment, the function will return 1.0;
					/// Returns -1 on error.
					///
					double getBaselineMeasurement();

					///
					/// Sets a flag indicating if this result is complete.
					///
					void setComplete(bool x);

					///
					/// Gets a flag indicating if this result is complete.
					///
					bool getComplete() const;

				private:
					///
					/// Disable default constructor
					///
					Result();

					///
					/// \brief	Pimpl Idiom
					///
					class Impl;

					///
					/// \brief	Pimpl Idiom
					///
					Pimpl<Impl> pimpl;	
			};

			///
			///
			///
			Experiment(std::weak_ptr<Benchmark> benchmark);

			///
			///
			///
			Experiment(std::weak_ptr<Benchmark> benchmark, const std::string& name, uint64_t samples, uint64_t calls, double baselineTarget);

			///
			/// \brief	Default destructor.
			///
			~Experiment();

			///
			/// Gets a pointer to the owning Benchmark object.
			///
			std::shared_ptr<Benchmark> getBenchmark();

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
			void setCalls(uint64_t x);

			///
			///
			///
			uint64_t getCalls() const;

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
			void setFactory(std::shared_ptr<Factory> x);
			
			///
			/// Gets the factory used to create this experiment's test fixtures.
			///
			std::shared_ptr<Factory> getFactory() const;

			///
			///
			///
			void addProblemSpace(int64_t x);

			///
			/// There is one result for each problem space.
			///
			size_t getResultSize();

			///
			///
			///
			std::shared_ptr<Experiment::Result> getResult(size_t x);

			///
			///
			///
			std::shared_ptr<Experiment::Result> getResultByValue(int64_t x);

		private:
			///
			/// Hide the default constructor
			///
			Experiment();

			///
			/// Hide the copy constructor
			///
			Experiment(const Experiment&);

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
}

#endif
