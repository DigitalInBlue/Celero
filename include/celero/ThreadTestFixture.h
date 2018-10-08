#ifndef H_CELERO_THREADTESTFIXTURE_H
#define H_CELERO_THREADTESTFIXTURE_H

///
/// \author	Ivan Shynkarenka
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

#include <celero/Pimpl.h>
#include <celero/TestFixture.h>

namespace celero
{
	class Benchmark;

	///
	/// \class ThreadTestFixture
	///
	/// \author Ivan Shynkarenka
	///
	class CELERO_EXPORT ThreadTestFixture : public TestFixture
	{
	public:
		///
		/// Default Constructor.
		///
		ThreadTestFixture();

		///
		/// Virtual destructor for inheritance.
		///
		virtual ~ThreadTestFixture();

		///
		/// Start threads before benchmark execution.
		///
		/// \param threads Count of working threads to start.
		/// \param calls The total number of times to loop over the UserBenchmark function.
		///
		virtual void startThreads(uint64_t threads, uint64_t calls);

		///
		/// Called after test completion to stop threads.
		///
		virtual void stopThreads();

		///
		/// \param threads The number of working threads.
		/// \param calls The total number of times to loop over the UserBenchmark function.
		/// \param experimentValue The experiment value to pass in setUp function.
		///
		/// \return Returns the number of microseconds the run took.
		///
		uint64_t run(uint64_t threads, uint64_t calls, const celero::TestFixture::ExperimentValue& experimentValue) override;

	private:
		class Impl;
		Pimpl<Impl> pimpl;
	};
} // namespace celero

#endif
