///
/// \author	Peter Azmanov
///
/// \copyright Copyright 2016, 2017, 2018 John Farrier
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

#include <celero/Exceptions.h>

#include <celero/Console.h>
#include <celero/TestFixture.h>

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <iomanip>
#include <iostream>

//
// Macros and general logics below taken from Google Test code,
// see gtest/internal/gtest-port.h
//     gtest/src/gtest.cc
//

#ifndef CELERO_HAS_EXCEPTIONS
// The user didn't tell us whether exceptions are enabled, so we need
// to figure it out.
#if defined(_MSC_VER) || defined(__BORLANDC__)
// MSVC's and C++Builder's implementations of the STL use the _HAS_EXCEPTIONS
// macro to enable exceptions, so we'll do the same.
// Assumes that exceptions are enabled by default.
#ifndef _HAS_EXCEPTIONS
#define _HAS_EXCEPTIONS 1
#endif // _HAS_EXCEPTIONS
#define CELERO_HAS_EXCEPTIONS _HAS_EXCEPTIONS
#elif defined(__GNUC__) && __EXCEPTIONS
// gcc defines __EXCEPTIONS to 1 iff exceptions are enabled.
#define CELERO_HAS_EXCEPTIONS 1
#elif defined(__SUNPRO_CC)
// Sun Pro CC supports exceptions.  However, there is no compile-time way of
// detecting whether they are enabled or not.  Therefore, we assume that
// they are enabled unless the user tells us otherwise.
#define CELERO_HAS_EXCEPTIONS 1
#elif defined(__IBMCPP__) && __EXCEPTIONS
// xlC defines __EXCEPTIONS to 1 iff exceptions are enabled.
#define CELERO_HAS_EXCEPTIONS 1
#elif defined(__HP_aCC)
// Exception handling is in effect by default in HP aCC compiler. It has to
// be turned of by +noeh compiler option if desired.
#define CELERO_HAS_EXCEPTIONS 1
#else
// For other compilers, we assume exceptions are disabled to be
// conservative.
#define CELERO_HAS_EXCEPTIONS 0
#endif // defined(_MSC_VER) || defined(__BORLANDC__)
#endif // CELERO_HAS_EXCEPTIONS

// Determine whether the compiler supports Microsoft's Structured Exception
// Handling.  This is supported by several Windows compilers but generally
// does not exist on any other system.
#ifndef CELERO_HAS_SEH
// The user didn't tell us, so we need to figure it out.

#if defined(_MSC_VER) || defined(__BORLANDC__)
// These two compilers are known to support SEH.
#define CELERO_HAS_SEH 1
#else
// Assume no SEH.
#define CELERO_HAS_SEH 0
#endif

#endif // CELERO_HAS_SEH

namespace celero
{
	bool ExceptionSettings::GetCatchExceptions()
	{
		return ExceptionSettings::instance().catchExceptions;
	}

	void ExceptionSettings::SetCatchExceptions(bool x)
	{
		ExceptionSettings::instance().catchExceptions = x;
	}

	ExceptionSettings& ExceptionSettings::instance()
	{
		static ExceptionSettings settings;
		return settings;
	}

#if CELERO_HAS_SEH
	int HandleSEH(DWORD exceptionCode)
	{
		// see https://support.microsoft.com/en-us/kb/185294
		const DWORD cppExceptionCode = 0xe06d7363;

		if((exceptionCode == EXCEPTION_BREAKPOINT) || (exceptionCode == cppExceptionCode))
		{
			return EXCEPTION_EXECUTE_HANDLER;
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}

	const char* const ExceptionCodeToStr(DWORD exceptionCode)
	{
		switch(exceptionCode)
		{
			case EXCEPTION_ACCESS_VIOLATION:
				return "EXCEPTION_ACCESS_VIOLATION";

			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";

			case EXCEPTION_BREAKPOINT:
				return "EXCEPTION_BREAKPOINT";

			case EXCEPTION_DATATYPE_MISALIGNMENT:
				return "EXCEPTION_DATATYPE_MISALIGNMENT";

			case EXCEPTION_FLT_DENORMAL_OPERAND:
				return "EXCEPTION_FLT_DENORMAL_OPERAND";

			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				return "EXCEPTION_FLT_DIVIDE_BY_ZERO";

			case EXCEPTION_FLT_INEXACT_RESULT:
				return "EXCEPTION_FLT_INEXACT_RESULT";

			case EXCEPTION_FLT_INVALID_OPERATION:
				return "EXCEPTION_FLT_INVALID_OPERATION";

			case EXCEPTION_FLT_OVERFLOW:
				return "EXCEPTION_FLT_OVERFLOW";

			case EXCEPTION_FLT_STACK_CHECK:
				return "EXCEPTION_FLT_STACK_CHECK";

			case EXCEPTION_FLT_UNDERFLOW:
				return "EXCEPTION_FLT_UNDERFLOW";

			case EXCEPTION_GUARD_PAGE:
				return "EXCEPTION_GUARD_PAGE";

			case EXCEPTION_ILLEGAL_INSTRUCTION:
				return "EXCEPTION_ILLEGAL_INSTRUCTION";

			case EXCEPTION_IN_PAGE_ERROR:
				return "EXCEPTION_IN_PAGE_ERROR";

			case EXCEPTION_INT_DIVIDE_BY_ZERO:
				return "EXCEPTION_INT_DIVIDE_BY_ZERO";

			case EXCEPTION_INT_OVERFLOW:
				return "EXCEPTION_INT_OVERFLOW";

			case EXCEPTION_INVALID_DISPOSITION:
				return "EXCEPTION_INVALID_DISPOSITION";

			case EXCEPTION_INVALID_HANDLE:
				return "EXCEPTION_INVALID_HANDLE";

			case EXCEPTION_NONCONTINUABLE_EXCEPTION:
				return "EXCEPTION_NONCONTINUABLE_EXCEPTION";

			case EXCEPTION_PRIV_INSTRUCTION:
				return "EXCEPTION_PRIV_INSTRUCTION";

			case EXCEPTION_SINGLE_STEP:
				return "EXCEPTION_SINGLE_STEP";

			case EXCEPTION_STACK_OVERFLOW:
				return "EXCEPTION_STACK_OVERFLOW";

			case STATUS_UNWIND_CONSOLIDATE:
				return "STATUS_UNWIND_CONSOLIDATE";

			default:
				return "Unknown exception code.";
		}
	}
#endif // CELERO_HAS_SEH

	std::pair<bool, uint64_t> RunAndCatchSEHExc(TestFixture& test, uint64_t threads, uint64_t calls,
												const celero::TestFixture::ExperimentValue& experimentValue)
	{
#if CELERO_HAS_SEH
		__try
		{
			return std::make_pair(true, test.run(threads, calls, experimentValue));
		}
		__except(HandleSEH(GetExceptionCode()))
		{
			const auto exceptionCode = GetExceptionCode();
			celero::console::SetConsoleColor(celero::console::ConsoleColor_Red);
			std::cout << "SEH exception " << ExceptionCodeToStr(exceptionCode) << std::endl;
			celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
			return std::make_pair(false, 0);
		}
#else  // CELERO_HAS_SEH
		return std::make_pair(true, test.run(threads, calls, experimentValue));
#endif // CELERO_HAS_SEH
	}

	std::pair<bool, uint64_t> RunAndCatchExc(TestFixture& test, uint64_t threads, uint64_t calls,
											 const celero::TestFixture::ExperimentValue& experimentValue)
	{
		if(ExceptionSettings::GetCatchExceptions() == true)
		{
#if CELERO_HAS_EXCEPTIONS
			try
			{
				return RunAndCatchSEHExc(test, threads, calls, experimentValue);
			}
			catch(const std::exception& e)
			{
				celero::console::SetConsoleColor(celero::console::ConsoleColor_Red);
				std::cout << "C++ exception \"" << e.what() << "\"" << std::endl;
				celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
			}
			catch(...)
			{
				celero::console::SetConsoleColor(celero::console::ConsoleColor_Red);
				std::cout << "Unknown C++ exception" << std::endl;
				celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
			}

			return std::make_pair(false, 0);
#else  // CELERO_HAS_EXCEPTIONS
			return RunAndCatchSEHExc(test, threads, calls, experimentValue);
#endif // CELERO_HAS_EXCEPTIONS
		}
		else
		{
			return std::make_pair(true, test.run(threads, calls, experimentValue));
		}
	}
} // namespace celero
