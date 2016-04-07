///
/// \author	Peter Azmanov
///
/// \copyright Copyright 2016 Peter Azmanov
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

#include <celero/TestFixture.h>
#include <celero/Console.h>

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <iostream>
#include <iomanip>

//
// Macros and general logics below taken from Google Test code,
// see gtest/internal/gtest-port.h
//     gtest/src/gtest.cc
//

#ifndef CELERO_HAS_EXCEPTIONS
// The user didn't tell us whether exceptions are enabled, so we need
// to figure it out.
# if defined(_MSC_VER) || defined(__BORLANDC__)
// MSVC's and C++Builder's implementations of the STL use the _HAS_EXCEPTIONS
// macro to enable exceptions, so we'll do the same.
// Assumes that exceptions are enabled by default.
#  ifndef _HAS_EXCEPTIONS
#   define _HAS_EXCEPTIONS 1
#  endif  // _HAS_EXCEPTIONS
#  define CELERO_HAS_EXCEPTIONS _HAS_EXCEPTIONS
# elif defined(__GNUC__) && __EXCEPTIONS
// gcc defines __EXCEPTIONS to 1 iff exceptions are enabled.
#  define CELERO_HAS_EXCEPTIONS 1
# elif defined(__SUNPRO_CC)
// Sun Pro CC supports exceptions.  However, there is no compile-time way of
// detecting whether they are enabled or not.  Therefore, we assume that
// they are enabled unless the user tells us otherwise.
#  define CELERO_HAS_EXCEPTIONS 1
# elif defined(__IBMCPP__) && __EXCEPTIONS
// xlC defines __EXCEPTIONS to 1 iff exceptions are enabled.
#  define CELERO_HAS_EXCEPTIONS 1
# elif defined(__HP_aCC)
// Exception handling is in effect by default in HP aCC compiler. It has to
// be turned of by +noeh compiler option if desired.
#  define CELERO_HAS_EXCEPTIONS 1
# else
// For other compilers, we assume exceptions are disabled to be
// conservative.
#  define CELERO_HAS_EXCEPTIONS 0
# endif  // defined(_MSC_VER) || defined(__BORLANDC__)
#endif  // CELERO_HAS_EXCEPTIONS

// Determine whether the compiler supports Microsoft's Structured Exception
// Handling.  This is supported by several Windows compilers but generally
// does not exist on any other system.
#ifndef CELERO_HAS_SEH
// The user didn't tell us, so we need to figure it out.

# if defined(_MSC_VER) || defined(__BORLANDC__)
// These two compilers are known to support SEH.
#  define CELERO_HAS_SEH 1
# else
// Assume no SEH.
#  define CELERO_HAS_SEH 0
# endif

#endif  // CELERO_HAS_SEH

namespace celero
{
   ExceptionSettings::ExceptionSettings()
      : catchExceptions_(true)
   {}

   bool ExceptionSettings::catchExceptions()
   {
      return instance().catchExceptions_;
   }

   void ExceptionSettings::setCatchExceptions( bool catchExceptions )
   {
      instance().catchExceptions_ = catchExceptions;
   }

   ExceptionSettings & ExceptionSettings::instance()
   {
      static ExceptionSettings settings;
      return settings;
   }

#if CELERO_HAS_SEH
   namespace
   {
      int HandleSEH( DWORD exceptionCode )
      {
         // see https://support.microsoft.com/en-us/kb/185294
         DWORD const cppExceptionCode = 0xe06d7363;

         bool handle = true;
         if (exceptionCode == EXCEPTION_BREAKPOINT)
            handle = false;
         else if (exceptionCode == cppExceptionCode)
            handle = false;

         return handle ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
      }

#define EXC_CODE_TO_STR(code) \
   case code:                 \
      return #code;

      char const * ExceptionCodeToStr( DWORD exceptionCode )
      {
         switch (exceptionCode)
         {
         EXC_CODE_TO_STR(EXCEPTION_ACCESS_VIOLATION);
         EXC_CODE_TO_STR(EXCEPTION_ARRAY_BOUNDS_EXCEEDED);
         EXC_CODE_TO_STR(EXCEPTION_BREAKPOINT);
         EXC_CODE_TO_STR(EXCEPTION_DATATYPE_MISALIGNMENT);
         EXC_CODE_TO_STR(EXCEPTION_FLT_DENORMAL_OPERAND);
         EXC_CODE_TO_STR(EXCEPTION_FLT_DIVIDE_BY_ZERO);
         EXC_CODE_TO_STR(EXCEPTION_FLT_INEXACT_RESULT);
         EXC_CODE_TO_STR(EXCEPTION_FLT_INVALID_OPERATION);
         EXC_CODE_TO_STR(EXCEPTION_FLT_OVERFLOW);
         EXC_CODE_TO_STR(EXCEPTION_FLT_STACK_CHECK);
         EXC_CODE_TO_STR(EXCEPTION_FLT_UNDERFLOW);
         EXC_CODE_TO_STR(EXCEPTION_GUARD_PAGE);
         EXC_CODE_TO_STR(EXCEPTION_ILLEGAL_INSTRUCTION);
         EXC_CODE_TO_STR(EXCEPTION_IN_PAGE_ERROR);
         EXC_CODE_TO_STR(EXCEPTION_INT_DIVIDE_BY_ZERO);
         EXC_CODE_TO_STR(EXCEPTION_INT_OVERFLOW);
         EXC_CODE_TO_STR(EXCEPTION_INVALID_DISPOSITION);
         EXC_CODE_TO_STR(EXCEPTION_INVALID_HANDLE);
         EXC_CODE_TO_STR(EXCEPTION_NONCONTINUABLE_EXCEPTION);
         EXC_CODE_TO_STR(EXCEPTION_PRIV_INSTRUCTION);
         EXC_CODE_TO_STR(EXCEPTION_SINGLE_STEP);
         EXC_CODE_TO_STR(EXCEPTION_STACK_OVERFLOW);
         EXC_CODE_TO_STR(STATUS_UNWIND_CONSOLIDATE);
         default:
            return "Unknown code!";
         }
      }

#undef EXC_CODE_TO_STR

   }
#endif  // CELERO_HAS_SEH

   std::pair<bool, uint64_t> RunAndCatchSEHExc( TestFixture & test,
                                                uint64_t threads, uint64_t calls,
                                                int64_t experimentValue )
   {
#if CELERO_HAS_SEH
      __try
      {
         return std::make_pair(true, test.run(threads, calls, experimentValue));
      }
      __except (HandleSEH(GetExceptionCode()))
      {
         DWORD exceptionCode = GetExceptionCode();
         celero::console::SetConsoleColor(celero::console::ConsoleColor_Red);
         std::cout << "SEH exception " << ExceptionCodeToStr(exceptionCode) << std::endl;
         celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
         return std::make_pair(false, 0);
      }
#else // CELERO_HAS_SEH
      return std::make_pair(true, test.run(threads, calls, experimentValue));
#endif  // CELERO_HAS_SEH
   }

   std::pair<bool, uint64_t> RunAndCatchExc( TestFixture & test,
                                             uint64_t threads, uint64_t calls,
                                             int64_t experimentValue )
   {
      if (ExceptionSettings::catchExceptions())
      {
#if CELERO_HAS_EXCEPTIONS
         try
         {
            return RunAndCatchSEHExc(test, threads, calls, experimentValue);
         }
         catch (const std::exception& e)
         {
            celero::console::SetConsoleColor(celero::console::ConsoleColor_Red);
            std::cout << "C++ exception \"" << e.what() << "\"" << std::endl;
            celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
         }
         catch (...)
         {
            celero::console::SetConsoleColor(celero::console::ConsoleColor_Red);
            std::cout << "Unknown C++ exception" << std::endl;
            celero::console::SetConsoleColor(celero::console::ConsoleColor_Default);
         }
         return std::make_pair(false, 0);
#else // CELERO_HAS_EXCEPTIONS
         return RunAndCatchSEHExc(test, threads, calls, experimentValue);
#endif // CELERO_HAS_EXCEPTIONS
      }
      else
      {
         return std::make_pair(true, test.run(threads, calls, experimentValue));
      }
   }

}
