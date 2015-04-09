#ifndef H_CELERO_TESTFIXTURE_H
#define H_CELERO_TESTFIXTURE_H

// www.helleboreconsulting.com

///
/// \author	John Farrier
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
			/// Provide a units result scale of each experiment value. If the value
			/// is greater than 0 then additional statistic value will be printed
			/// in output - [ xxxx units/sec ]. For example for measure speed of
			/// file IO operations method might return 1024 * 1024 to get megabytes 
            /// per second.
			///
			/// It is only guaranteed that the constructor is called prior to this function being called.
			///
            virtual double getExperimentValueResultScale() const { return 1.0; };

			///
			/// Allows the text fixture to run code that will be executed once
			/// immediately before the benchmark. Unlike setUp, the evaluation
			/// of this function IS included in the total experiment execution
			/// time.
			///
			/// \param experimentValue The value for the experiment.  This can be ignored if the test does not utilize experiment values.
			///
			virtual void onExperimentStart(int64_t experimentValue);

			///
			/// Allows the text fixture to run code that will be executed once
			/// immediately after the benchmark. Unlike tearDown, the evaluation
			/// of this function IS included in the total experiment execution
			/// time.
			///
			virtual void onExperimentEnd();

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
			///
			/// \param threads The number of working threads.
			/// \param iterations The number of times to loop over the UserBenchmark function.
			/// \param experimentValue The experiment value to pass in setUp function.
			///
			/// \return Returns a pair of the number of microseconds the run took.
			///
			virtual uint64_t run(uint64_t threads, uint64_t iterations, int64_t experimentValue);

		protected:
			/// Executed for each operation the benchmarking test is run.
			virtual void UserBenchmark();
	};
}

#endif
