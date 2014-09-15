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

#ifdef __CYGWIN__
#include <celero/ToString.h>

namespace std {
	string to_string (int val) {
		return numberToString<int>(val);
	}
	
	string to_string (long val) {
		return numberToString<long>(val);
	}
	
	string to_string (long long val) {
		return numberToString<long long>(val);
	}

	string to_string (unsigned val) {
		return numberToString<unsigned>(val);
	}

	string to_string (unsigned long val) {
		return numberToString<unsigned long>(val);
	}

	string to_string (unsigned long long val) {
		return numberToString<unsigned long long>(val);
	}

	string to_string (float val) {
		return numberToString<float>(val);
	}
	
	string to_string (double val) {
		return numberToString<double>(val);
	}
	
	string to_string (long double val) {
		return numberToString<long double>(val);
	}
}
#endif