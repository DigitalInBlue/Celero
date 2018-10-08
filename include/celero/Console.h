#ifndef H_CELERO_CONSOLE_H
#define H_CELERO_CONSOLE_H

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

#include <celero/Export.h>

namespace celero
{
	///
	/// \namespace console
	///
	/// \author	John farrier
	///
	namespace console
	{
		///
		/// \enum ConsoleColor
		///
		/// \author	John farrier
		///
		enum ConsoleColor
		{
			ConsoleColor_Default,
			ConsoleColor_Red,
			ConsoleColor_Red_Bold,
			ConsoleColor_Green,
			ConsoleColor_Green_Bold,
			ConsoleColor_Blue,
			ConsoleColor_Blue_Bold,
			ConsoleColor_Cyan,
			ConsoleColor_Cyan_Bold,
			ConsoleColor_Yellow,
			ConsoleColor_Yellow_Bold,
			ConsoleColor_White,
			ConsoleColor_White_Bold,
			ConsoleColor_WhiteOnRed,
			ConsoleColor_WhiteOnRed_Bold,
			ConsoleColor_Purple_Bold
		};

		///
		/// Set the color of std::out on the console.
		///
		CELERO_EXPORT void SetConsoleColor(const celero::console::ConsoleColor x);
	} // namespace console
} // namespace celero

#endif
