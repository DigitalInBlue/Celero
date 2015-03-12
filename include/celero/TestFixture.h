#ifndef H_CELERO_TESTFIXTURE_H
#define H_CELERO_TESTFIXTURE_H

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

#include <cstddef>
#include <stdint.h>

#include <celero/Timer.h>
#include <celero/Export.h>
#include <celero/ThreadLocal.h>

#include <vector>

namespace celero
{
	class Benchmark;

	///
	/// \class TestFixture
	///
	/// \author John Farrier
	///
	class CELERO_EXPORT TestFixture
	{
		public:
			///
			/// Default Constructor.
			///
			TestFixture();

			///
			/// Virtual destructor for inheritance.
			///
			virtual ~TestFixture();

			///
			/// Allows a test fixture to supply values to use for experiments.
			/// This is used to create multiple runs of the same experiment
			/// and varrying the data set size, for example.
			///
			/// It is only guaranteed that the constructor is called prior to this function being called.
			///
			virtual std::vector<int64_t> getExperimentValues() const { return std::vector<int64_t>(); };

			///
			/// Provide a size in bytes of each experiment value. If the value
			/// is greater than 0 then additional statistic value will be printed
			/// in output - [ xxxx MB/sec ]. For example for measure speed of
			/// file IO operations (megabytes per second).
			///
			/// It is only guaranteed that the constructor is called prior to this function being called.
			///
			virtual int64_t getExperimentValueSize() const { return 0; };

			///
			/// Set up the test fixture before benchmark execution.
			///
			/// \param experimentValue The value for the experiment.  This can be ignored if the test does not utilize experiment values.
			///
			virtual void setUp(int64_t experimentValue);
		
			///
			/// Called after test completion to destroy the fixture.
			///
			virtual void tearDown();
		
			///
			/// \param threads The number of working threads.
			/// \param calls The number of times to loop over the UserBenchmark function.
			/// \param experimentValue The experiment value to pass in setUp function.
			///
			/// \return Returns a pair of the number of microseconds the run took.
			///
			virtual uint64_t run(uint64_t threads, uint64_t calls, int64_t experimentValue);

			///
			/// Get the current call number starting from 1.
			///
			uint64_t getCallId() const { return currentCallId; };

			///
			/// Get the current thread Id starting from 1.
			///
			uint64_t getThreadId() const { return currentThreadId; };

		protected:
			static thread_local uint64_t currentCallId;
			static thread_local uint64_t currentThreadId;

			/// Executed for each operation the benchmarking test is run.
			virtual void UserBenchmark();
	};
}

#endif
