#ifndef H_CELERO_TOSTRING_H
#define H_CELERO_TOSTRING_H
#ifdef __CYGWIN__
///
/// \author	Daniel Rother
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

// Implement std::to_string() here because cygwin doesn't provide it on it's own...
#include <sstream>
#include <string>
#include <iostream>

namespace std {
	///
	/// \brief	Implementation of std::to_string
	///
	/// Implementation of std::to_string in cases where these function is not provided by libC
	/// e.g. Cygwin 2.850 (32 bit), gcc/libc 4.8.3-3
	///
	/// \author	Daniel Rother
	///
	template <typename T>
	string numberToString(T number)
	{
	   stringstream ss;
	   ss << number;
	   return ss.str();
	}

	string to_string (int val);
	string to_string (long val);
	string to_string (long long val);
	string to_string (unsigned val);
	string to_string (unsigned long val);
	string to_string (unsigned long long val);
	string to_string (float val);
	string to_string (double val);
	string to_string (long double val);
}

#endif
#endif