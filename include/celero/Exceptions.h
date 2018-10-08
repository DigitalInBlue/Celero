#ifndef H_CELERO_EXCEPTIONS_H
#define H_CELERO_EXCEPTIONS_H

///
/// \author	Peter Azmanov
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

#include <celero/TestFixture.h>
#include <cstdint>
#include <utility>

namespace celero
{
	///
	/// A Singleton storing exception settings (currently only one flag)
	///
	class ExceptionSettings
	{
	public:
		///
		/// Get a flag indicating whether Celero should catch exceptions or not
		///
		static bool GetCatchExceptions();

		///
		/// Set a flag indicating whether Celero should catch exceptions or not
		///
		static void SetCatchExceptions(bool catchExceptions);

	private:
		static ExceptionSettings& instance();

	private:
		bool catchExceptions{true};
	};

	///
	/// Run test and catch SEH exceptions if they are supported by compiler
	///
	std::pair<bool, uint64_t> RunAndCatchSEHExc(TestFixture& test, uint64_t threads, uint64_t calls,
												const celero::TestFixture::ExperimentValue& experimentValue);

	///
	/// Run test and catch all exceptions we can
	///
	std::pair<bool, uint64_t> RunAndCatchExc(TestFixture& test, uint64_t threads, uint64_t calls,
											 const celero::TestFixture::ExperimentValue& experimentValue);
} // namespace celero

#endif
