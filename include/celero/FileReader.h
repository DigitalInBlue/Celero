#ifndef H_CELERO_FILEREADER_H
#define H_CELERO_FILEREADER_H

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

#include <locale>

namespace celero
{
	///
	/// \struct FileReader
	///
	/// A helper struct to aid in reading CSV files.
	///
	/// Classify commas as whitespace.
	///
	struct FieldReader : std::ctype<char>
	{
		FieldReader() : std::ctype<char>(FieldReader::GetTable())
		{
		}

		static std::ctype_base::mask const* GetTable()
		{
			static std::vector<std::ctype_base::mask> rc(table_size, std::ctype_base::mask());
			rc[','] = std::ctype_base::space;
			rc['\n'] = std::ctype_base::space;
			rc['\r'] = std::ctype_base::space;
			return &rc[0];
		}
	};
}

#endif
