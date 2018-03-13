#ifndef H_CELERO_TESTFIXTURE_H
#define H_CELERO_TESTFIXTURE_H

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

#include <celero/Export.h>
#include <celero/Timer.h>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include <celero/ThreadLocal.h>

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

		enum class Constants : int64_t
		{
#ifdef _MSC_VER
#if(_MSC_VER < 1900)
			NoProblemSpaceValue = -9223372036854775807
#else
			NoProblemSpaceValue = std::numeric_limits<int64_t>::min()
#endif
#else
			NoProblemSpaceValue = std::numeric_limits<int64_t>::min()
#endif
		};

		///
		/// \class ExperimentValue
		///
		/// You can derive from this type to add your own information to the experiment value set.
		///
		class ExperimentValue
		{
		public:
			ExperimentValue() = default;
			ExperimentValue(int64_t v) : Value(v){};
			ExperimentValue(int64_t v, int64_t i) : Value(v), Iterations(i){};

			virtual ~ExperimentValue() = default;

			/// An arbitrary integer value which can be used or translated for use by the test fixture.
			int64_t Value{0};

			/// The number of iterations to do with this test value.  0 (default) indicates that the default number of iterations set up for the test
			/// case should be used.
			int64_t Iterations{0};
		};

		///
		/// Allows a test fixture to supply values to use for experiments.
		///
		/// This is used to create multiple runs of the same experiment
		/// and varrying the data set size, for example.  The second value
		/// of the pair is an optional override for the number of iterations
		/// to be used.  If zero is specified, then the default number of
		/// iterations is used.
		///
		/// It is only guaranteed that the constructor is called prior to this function being called.
		///
		virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const
		{
			return std::vector<celero::TestFixture::ExperimentValue>();
		};

		///
		/// Provide a units result scale of each experiment value.
		///
		/// If the value is greater than 0 then additional statistic value will be printed
		/// in output - [ xxxx units/sec ]. For example for measure speed of
		/// file IO operations method might return 1024 * 1024 to get megabytes
		/// per second.
		///
		/// It is only guaranteed that the constructor is called prior to this function being called.
		///
		virtual double getExperimentValueResultScale() const
		{
			return 1.0;
		};

		///
		/// Allows the text fixture to run code that will be executed once immediately before the benchmark.
		///
		/// Unlike setUp, the evaluation of this function IS included in the total experiment execution
		/// time.
		///
		/// \param x The value for the experiment.  This can be ignored if the test does not utilize experiment values.
		///
		virtual void onExperimentStart(const celero::TestFixture::ExperimentValue& x);

		///
		/// Allows the text fixture to run code that will be executed once immediately after the benchmark.
		/// Unlike tearDown, the evaluation of this function IS included in the total experiment execution
		/// time.
		///
		virtual void onExperimentEnd();

		///
		/// Set up the test fixture before benchmark execution.
		///
		/// This code is NOT included in the benchmark timing.
		/// It is executed once before all iterations are executed and between each Sample.
		/// Your experiment should NOT rely on "setUp" methods to be called before EACH experiment run, only between each sample.
		///
		/// \param x The celero::TestFixture::ExperimentValue for the experiment.  This can be ignored if the test does not utilize experiment values.
		///
		virtual void setUp(const celero::TestFixture::ExperimentValue& x);

		///
		/// Called after test completion to destroy the fixture.
		///
		/// This code is NOT included in the benchmark timing.
		/// It is executed once after all iterations are executed and between each Sample.
		/// Your experiment should NOT rely on "tearDown" methods to be called after EACH experiment run, only between each sample.
		///
		virtual void tearDown();

		///
		///
		/// \param threads The number of working threads.
		/// \param iterations The number of times to loop over the UserBenchmark function.
		/// \param experimentValue The experiment value to pass in setUp function.
		///
		/// \return Returns the number of microseconds the run took.
		///
		virtual uint64_t run(uint64_t threads, uint64_t iterations, const celero::TestFixture::ExperimentValue& experimentValue);

	protected:
		/// Executed for each operation the benchmarking test is run.
		virtual void UserBenchmark();

		///
		/// Only used for baseline cases.  Used to define a hard-coded execution time vs. actually making a measurement.
		///
		virtual uint64_t HardCodedMeasurement() const;
	};
} // namespace celero

#endif
