#ifndef H_CELERO_BENCHMARKINFO_H
#define H_CELERO_BENCHMARKINFO_H

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

#include <celero/Export.h>
#include <celero/Pimpl.h>

namespace celero
{
	class Factory;

	///
	/// \class BenchmarkInfo
	///
	/// \author	John Farrier
	///
	class CELERO_EXPORT BenchmarkInfo
	{
		public:
			///
			/// \brief	Default constructor
			///
			BenchmarkInfo();

			///
			/// \brief	Overloaded constructor.
			///
			/// \param groupName Name of the test group.
			/// \param benchmarkName Name of the test.
			/// \param samples Number of samples for the test.
			/// \param calls Number of calls per run.
			/// \param testFactory Test factory implementation for the test.
			/// \param baselineTarget The highest baseline multiple you want for this test (used to pass/fail test.)
			///
			BenchmarkInfo(const std::string& groupName, const std::string& benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<Factory> testFactory, double baselineTarget = -1);
			
			///
			///
			///
			BenchmarkInfo(const BenchmarkInfo& other);

			///
			/// \brief	Default destructor.
			///
			~BenchmarkInfo();

			///
			///
			///
			void reset();

			///
			///
			///
			std::string getGroupName() const;

			///
			///
			///
			std::string getTestName() const;

			///
			///
			///
			void incrementSamples();

			///
			///
			///
			uint64_t getSamples() const;

			///
			///
			///
			uint64_t getOps() const;

			///
			///
			///
			std::shared_ptr<Factory> getFactory() const;

			///
			///
			///
			std::string get() const;

			///
			///
			///
			std::string getShort() const;

			///
			///
			///
			void setBaselineUnit(const double x);
			
			///
			///
			///
			double getBaselineUnit() const;

			///
			/// \brief	Compute the baseline measurement of an executed benchmark.
			///
			double getBaselineMeasurement() const;

			///
			/// \brief	References can be cleared out during complex runs.  This saves state for reporting purposes.
			///
			void saveBaselineMeasurement();

			///
			///
			///
			double getBaselineTarget() const;

			///
			/// \brief	Sets the test's best run time.
			///
			/// The function will only update the run time if x is smaller than the current value.
			///
			/// \param	x	An individual run's execution time in usec.
			///
			void setRunTime(const uint64_t x);

			///
			///
			///
			uint64_t getRunTime() const;

			///
			///
			///
			void incrementTotalRunTime(const uint64_t x);

			///
			///
			///
			uint64_t getTotalRunTime() const;

			///
			/// \brief	Get the number of computed microseconds per operation.
			///
			///	An operation is defined as one operation of one run.
			///
			double getUsPerOp() const;

			///
			/// \brief	Get the number of computer operations per second.
			///
			///	An operation is defined as one operation of one run.
			///
			double getOpsPerSecond() const;

			///
			///
			///
			void setProblemSetSize(const size_t x);
			
			///
			///
			///
			size_t getProblemSetSize() const;
			
			///
			///
			///
			size_t getProblemSetSizeIndex() const;
			
			///
			///
			///
			void incrementProblemSetSizeIndex();

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
}

#endif
