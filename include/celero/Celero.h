#ifndef H_CELERO_CELERO_H
#define H_CELERO_CELERO_H

///
/// \namespace celero
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
/// Special thanks to the bands "3" and "Coheed and Cambria" for providing the development soundtrack.
///
/// "Iterations" refers to how many loops of the test function are measured as a time.
/// For very fast code, many iterations would help amoratize measurement error.
///
/// "Samples" refers to how many sets of "Iterations" will be performed.  Each "sample" is
/// a single measurement.
///
/// It is highly encouraged to only run this code compiled in a "Release" mode to use all available optimizations.
///

#ifdef WIN32
#include <process.h>
#endif

#include <celero/Benchmark.h>
#include <celero/GenericFactory.h>
#include <celero/TestFixture.h>
#include <celero/ThreadTestFixture.h>
#include <celero/Utilities.h>

namespace celero
{
	///
	/// \brief	Adds a new test to the list of tests to be executed.
	///
	///	All tests must be registered prior to calling celer::Run().
	///
	/// \param groupName The name of the Test Group.  Used for retrieving the associated baseline.
	/// \param benchmarkName A unique name for a specific test within a Test Group.
	/// \param samples The total number of times to execute the Test.  (Each test contains iterations.)
	/// \param iterations The total number of iterations per Test.
	/// \param threads The total number of threads per Test sample.
	/// \param experimentFactory The factory implementation for the test.
	///
	/// \returns a pointer to a Benchmark instance representing the given test.
	///
	CELERO_EXPORT std::shared_ptr<Benchmark> RegisterTest(const char* groupName, const char* benchmarkName, const uint64_t samples,
														  const uint64_t iterations, const uint64_t threads,
														  std::shared_ptr<Factory> experimentFactory, const double target = -1);

	///
	/// \brief	Adds a new test baseline to the list of test baseliness to be executed.
	///
	///	All test baselines must be registered prior to calling celer::Run().
	///
	/// \param groupName The name of the Test Group that the baseline is associated with.
	/// \param benchmarkName A unique name for a specific test baseline within a Test Group.
	/// \param samples The total number of times to execute the Test baseline.  (Each sample contains one or more iterations.)
	/// \param iterations The total number of iterations per Test baseline sample.
	/// \param threads The total number of threads per Test baseline.
	/// \param experimentFactory The factory implementation for the test baseline.
	///
	/// \returns a pointer to a Benchmark instance representing the given test.
	///
	CELERO_EXPORT std::shared_ptr<Benchmark> RegisterBaseline(const char* groupName, const char* benchmarkName, const uint64_t samples,
															  const uint64_t iterations, const uint64_t threads,
															  std::shared_ptr<Factory> experimentFactory);

	///
	/// \brief	Builds a distribution of total system measurement error.
	///
	/// The result vector contains microseconds for each trivial timer sample.
	/// The purpose is to be able to characterize the generic distribution of results
	/// on a given system.
	///
	/// This is just an attempt to characterize the distribution, not quantify it.
	///
	CELERO_EXPORT std::vector<uint64_t> BuildDistribution(uint64_t numberOfSamples, uint64_t iterationsPerSample);

	///
	/// \brief	The main test executor.
	///
	CELERO_EXPORT void Run(int argc, char** argv);
}

///
/// \define CELERO_MAIN
///
///	\brief	A macro to build the most basic main() required to run the benchmark tests.
///
#define CELERO_MAIN                 \
	int main(int argc, char** argv) \
	{                               \
		celero::Run(argc, argv);    \
		return 0;                   \
	}

///
/// \define	BENCHMARK_CLASS_NAME
///
///	\brief	A macro to build a class name based on the test groupo and benchmark names.
///
#define BENCHMARK_CLASS_NAME(groupName, benchmarkName) CeleroUserBenchmark##_##groupName##_##benchmarkName

///
///	\define	BENCHMARK_IMPL
///
///	A macro to create a class of a unique name which can be used to register and execute a benchmark test.
///
#define BENCHMARK_IMPL(groupName, benchmarkName, fixtureName, samples, iterations, threads)                                   \
	class BENCHMARK_CLASS_NAME(groupName, benchmarkName) : public fixtureName                                                 \
	{                                                                                                                         \
	public:                                                                                                                   \
		BENCHMARK_CLASS_NAME(groupName, benchmarkName)() : fixtureName()                                                      \
		{                                                                                                                     \
		}                                                                                                                     \
                                                                                                                              \
	protected:                                                                                                                \
		virtual void UserBenchmark() override;                                                                                \
                                                                                                                              \
	private:                                                                                                                  \
		static const std::shared_ptr<::celero::Benchmark> info;                                                               \
	};                                                                                                                        \
                                                                                                                              \
	const std::shared_ptr<::celero::Benchmark> BENCHMARK_CLASS_NAME(groupName, benchmarkName)::info =                         \
		::celero::RegisterTest(#groupName, #benchmarkName, samples, iterations, threads,                                      \
							   std::make_shared<::celero::GenericFactory<BENCHMARK_CLASS_NAME(groupName, benchmarkName)>>()); \
                                                                                                                              \
	void BENCHMARK_CLASS_NAME(groupName, benchmarkName)::UserBenchmark()

///
///	\define	BENCHMARK_TEST_IMPL
///
///	A macro to create a class of a unique name which can be used to register and execute a benchmark test.
///
#define BENCHMARK_TEST_IMPL(groupName, benchmarkName, fixtureName, samples, iterations, threads, target)                              \
	class BENCHMARK_CLASS_NAME(groupName, benchmarkName) : public fixtureName                                                         \
	{                                                                                                                                 \
	public:                                                                                                                           \
		BENCHMARK_CLASS_NAME(groupName, benchmarkName)() : fixtureName()                                                              \
		{                                                                                                                             \
		}                                                                                                                             \
                                                                                                                                      \
	protected:                                                                                                                        \
		virtual void UserBenchmark() override;                                                                                        \
                                                                                                                                      \
	private:                                                                                                                          \
		static const std::shared_ptr<::celero::Benchmark> info;                                                                       \
	};                                                                                                                                \
                                                                                                                                      \
	const std::shared_ptr<::celero::Benchmark> BENCHMARK_CLASS_NAME(groupName, benchmarkName)::info =                                 \
		::celero::RegisterTest(#groupName, #benchmarkName, samples, iterations, threads,                                              \
							   std::make_shared<::celero::GenericFactory<BENCHMARK_CLASS_NAME(groupName, benchmarkName)>>(), target); \
                                                                                                                                      \
	void BENCHMARK_CLASS_NAME(groupName, benchmarkName)::UserBenchmark()

///
/// \define BENCHMARK_F
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark containing a test fixture.
///
/// Using the BENCHMARK_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BENCHMARK_F(groupName, benchmarkName, fixtureName, samples, iterations) \
	BENCHMARK_IMPL(groupName, benchmarkName, fixtureName, samples, iterations, 1)

///
/// \define BENCHMARK_T
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark containing a threaded test fixture.
///
/// Using the BENCHMARK_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BENCHMARK_T(groupName, benchmarkName, fixtureName, samples, iterations, threads) \
	BENCHMARK_IMPL(groupName, benchmarkName, fixtureName, samples, iterations, threads)

///
/// \define BENCHMARK_TEST_F
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark containing a test fixture.
///
/// Using the BENCHMARK_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BENCHMARK_TEST_F(groupName, benchmarkName, fixtureName, samples, iterations, target) \
	BENCHMARK_TEST_IMPL(groupName, benchmarkName, fixtureName, samples, iterations, 1, target)

///
/// \define BENCHMARK_TEST_T
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark containing a threaded test fixture.
///
/// Using the BENCHMARK_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BENCHMARK_TEST_T(groupName, benchmarkName, fixtureName, samples, iterations, threads, target) \
	BENCHMARK_TEST_IMPL(groupName, benchmarkName, fixtureName, samples, iterations, threads, target)

///
/// \define BENCHMARK
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark.
///
/// Using the BENCHMARK_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BENCHMARK(groupName, benchmarkName, samples, iterations) \
	BENCHMARK_IMPL(groupName, benchmarkName, ::celero::TestFixture, samples, iterations, 1)

///
/// \define BENCHMARK
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark.
///
/// Using the BENCHMARK_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BENCHMARK_TEST(groupName, benchmarkName, samples, iterations, target) \
	BENCHMARK_TEST_IMPL(groupName, benchmarkName, ::celero::TestFixture, samples, iterations, 1, target)

///
/// \define	BASELINE_CLASS_NAME
///
///	\brief	A macro to build a class name based on the test group and baseline names.
///
#define BASELINE_CLASS_NAME(groupName, baselineName) CeleroUserBaseline##_##groupName##_##baselineName

///
///	\define	BASELINE_IMPL
///
///	A macro to create a class of a unique name which can be used to register and execute a baseline benchmark test.
///
#define BASELINE_IMPL(groupName, baselineName, fixtureName, samples, iterations, threads, useconds)                             \
	class BASELINE_CLASS_NAME(groupName, baselineName) : public fixtureName                                                     \
	{                                                                                                                           \
	public:                                                                                                                     \
		BASELINE_CLASS_NAME(groupName, baselineName)() : fixtureName()                                                          \
		{                                                                                                                       \
		}                                                                                                                       \
                                                                                                                                \
	protected:                                                                                                                  \
		virtual void UserBenchmark() override;                                                                                  \
		virtual uint64_t HardCodedMeasurement() const override                                                                  \
		{                                                                                                                       \
			return uint64_t(useconds);                                                                                          \
		}                                                                                                                       \
                                                                                                                                \
	private:                                                                                                                    \
		static const std::shared_ptr<::celero::Benchmark> info;                                                                 \
	};                                                                                                                          \
                                                                                                                                \
	const std::shared_ptr<::celero::Benchmark> BASELINE_CLASS_NAME(groupName, baselineName)::info =                             \
		::celero::RegisterBaseline(#groupName, #baselineName, samples, iterations, threads,                                     \
								   std::make_shared<::celero::GenericFactory<BASELINE_CLASS_NAME(groupName, baselineName)>>()); \
                                                                                                                                \
	void BASELINE_CLASS_NAME(groupName, baselineName)::UserBenchmark()

///
/// \define BASELINE_F
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark containing a test fixture.
///
/// Using the BASELINE_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BASELINE_F(groupName, baselineName, fixtureName, samples, iterations) \
	BASELINE_IMPL(groupName, baselineName, fixtureName, samples, iterations, 1, 0)

///
/// \define BASELINE_T
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark containing a threaded test fixture.
///
/// Using the BASELINE_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BASELINE_T(groupName, baselineName, fixtureName, samples, iterations, threads) \
	BASELINE_IMPL(groupName, baselineName, fixtureName, samples, iterations, threads, 0)

///
/// \define BASELINE
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark.
///
/// Using the BASELINE_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BASELINE(groupName, baselineName, samples, iterations) \
	BASELINE_IMPL(groupName, baselineName, ::celero::TestFixture, samples, iterations, 1, 0)

///
/// \define BASELINE_FIXED
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark with a hard-coded timing.
///
/// This will NOT perform any timing measurments but will instead use the number of microseconds passed in as the measured time.
///
/// Using the BASELINE_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BASELINE_FIXED(groupName, baselineName, samples, iterations, useconds) \
	BASELINE_IMPL(groupName, baselineName, ::celero::TestFixture, samples, iterations, 1, useconds)
#define BASELINE_FIXED_F(groupName, baselineName, fixtureName, samples, iterations, useconds) \
	BASELINE_IMPL(groupName, baselineName, fixtureName, samples, iterations, 1, useconds)
#define BASELINE_FIXED_T(groupName, baselineName, fixtureName, samples, iterations, threads, useconds) \
	BASELINE_IMPL(groupName, baselineName, fixtureName, samples, iterations, threads, useconds)

#endif
