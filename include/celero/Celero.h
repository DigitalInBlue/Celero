#ifndef _CELERO_CELERO_H_
#define _CELERO_CELERO_H_

///
/// \namespace celero
///
/// \author	John Farrier
///
/// \copyright Copyright 2012 John Farrier 
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
/// Ideas from Nick Brunn's Hayai (https://github.com/nickbruun/hayai) were used and I likely owe him a beer.
///
/// Special thanks to the band "3" for providing the development soundtrack.
///
/// "Calls" refers to how many loops of the test function are measured as a time.
/// For very fast code, many calls would help amoratize measurement error.
/// 
/// "Samples" refers to how many sets of "Ops" will be performed.  Each "sample" is
/// a single measurement.  Set to 0 to have Celero decide how many samples are required
/// for a minimally significant answer.
///
/// It is highly encouraged to only run this code compiled in a "Release" mode to use all available optimizations.
///

#ifdef WIN32
	#include <process.h>
#endif

#include <celero/BenchmarkInfo.h>
#include <celero/TestFixture.h>
#include <celero/GenericFactory.h>
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
	/// \param samples The total number of times to execute the Test.  (Each test contains calls.)
	/// \param calls The total number of calls per Test.
	/// \param testFactory The factory implementation for the test.
	///
	/// \returns a pointer to a BenchmarkInfo instance representing the given test.
	///
	CELERO_EXPORT std::shared_ptr<BenchmarkInfo> RegisterTest(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<Factory> testFactory);

	///
	/// \brief	Adds a new test baseline to the list of test baseliness to be executed.
	///
	///	All test baselines must be registered prior to calling celer::Run().
	///
	/// \param groupName The name of the Test Group that the baseline is associated with.
	/// \param benchmarkName A unique name for a specific test baseline within a Test Group.
	/// \param samples The total number of times to execute the Test baseline.  (Each test contains calls.)
	/// \param calls The total number of calls per Test baseline.
	/// \param testFactory The factory implementation for the test baseline.
	///
	/// \returns a pointer to a BenchmarkInfo instance representing the given test.
	///
	CELERO_EXPORT std::shared_ptr<BenchmarkInfo> RegisterBaseline(const char* groupName, const char* benchmarkName, const uint64_t samples, const uint64_t calls, std::shared_ptr<Factory> testFactory);

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
#define CELERO_MAIN int main(int argc, char** argv) { celero::Run(argc, argv); return 0; }

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
#define BENCHMARK_IMPL(groupName, benchmarkName, fixtureName, samples, calls)		\
	class BENCHMARK_CLASS_NAME(groupName, benchmarkName) : public fixtureName	\
	{																			\
		public:																	\
			BENCHMARK_CLASS_NAME(groupName, benchmarkName)() : fixtureName() {}	\
																				\
		protected:																\
			virtual void UserBenchmark();										\
																				\
			virtual void setProblemSetSize(const size_t x) { info->setProblemSetSize(x); } \
																				\
		private:																\
			static const std::shared_ptr< ::celero::BenchmarkInfo> info;			\
	};																			\
																				\
	const std::shared_ptr< ::celero::BenchmarkInfo> BENCHMARK_CLASS_NAME(groupName, benchmarkName)::info = \
		::celero::RegisterTest(#groupName, #benchmarkName, samples, calls, std::make_shared< ::celero::GenericFactory<BENCHMARK_CLASS_NAME(groupName, benchmarkName)>>()); \
																				\
	void BENCHMARK_CLASS_NAME(groupName, benchmarkName)::UserBenchmark()

///
/// \define BENCHMARK_F
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark containing a test fixture.
///
/// Using the BENCHMARK_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BENCHMARK_F(groupName, benchmarkName, fixtureName, samples, calls) BENCHMARK_IMPL(groupName, benchmarkName, fixtureName, samples, calls)				

///
/// \define BENCHMARK
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark.
///
/// Using the BENCHMARK_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BENCHMARK(groupName, benchmarkName, samples, calls) BENCHMARK_IMPL(groupName,  benchmarkName, ::celero::TestFixture, samples, calls)

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
#define BASELINE_IMPL(groupName, baselineName, fixtureName, samples, calls)		\
	class BASELINE_CLASS_NAME(groupName, baselineName) : public fixtureName		\
	{																			\
		public:																	\
			BASELINE_CLASS_NAME(groupName, baselineName)() : fixtureName() {}	\
																				\
		protected:																\
			virtual void UserBenchmark();										\
																				\
			virtual void setProblemSetSize(const size_t x) { info->setProblemSetSize(x); } \
																				\
		private:																\
			static const std::shared_ptr< ::celero::BenchmarkInfo> info;			\
	};																			\
																				\
	const std::shared_ptr< ::celero::BenchmarkInfo> BASELINE_CLASS_NAME(groupName, baselineName)::info = \
		::celero::RegisterBaseline(#groupName, #baselineName, samples, calls, std::make_shared< ::celero::GenericFactory<BASELINE_CLASS_NAME(groupName, baselineName)>>()); \
																				\
	void BASELINE_CLASS_NAME(groupName, baselineName)::UserBenchmark()

///
/// \define BASELINE_F
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark containing a test fixture.
///
/// Using the BASELINE_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BASELINE_F(groupName, baselineName, fixtureName, samples, calls) BASELINE_IMPL(groupName, baselineName, fixtureName, samples, calls)				

///
/// \define BASELINE_F
///
/// \brief	A macro to place in user code to define a UserBenchmark function for a benchmark.
///
/// Using the BASELINE_ macro, this effectivly fills in a class's UserBenchmark() function.
///
#define BASELINE(groupName, baselineName, samples, calls) BASELINE_IMPL(groupName,  baselineName, ::celero::TestFixture, samples, calls)

#endif
