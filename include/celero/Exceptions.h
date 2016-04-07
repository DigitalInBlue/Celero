#ifndef H_CELERO_EXCEPTIONS_H
#define H_CELERO_EXCEPTIONS_H

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

#include <cstdint>
#include <utility>

namespace celero
{
   class TestFixture;

   // Singleton storing exception settings (currently only one flag)
   struct ExceptionSettings
   {
      ExceptionSettings();

      // Flag indicating whether Celero should catch exceptions or not
      static bool catchExceptions();
      static void setCatchExceptions( bool catchExceptions );

   private:
      static ExceptionSettings & instance();

   private:
      bool catchExceptions_;
   };

   // Run test and catch SEH exceptions if they are supported by compiler
   std::pair<bool, uint64_t> RunAndCatchSEHExc( TestFixture & test,
                                                uint64_t threads, uint64_t calls,
                                                int64_t experimentValue );

   // Run test and catch all exceptions we can
   std::pair<bool, uint64_t> RunAndCatchExc( TestFixture & test,
                                             uint64_t threads, uint64_t calls,
                                             int64_t experimentValue );

}

#endif
