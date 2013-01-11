#ifndef _CELERO_CONSOLE_H_
#define _CELERO_CONSOLE_H_

#include <celero/Export.h>

namespace celero
{
	///
	/// \namespace console
		/// \author	John farrier
	///
	namespace console
	{
		///
		/// \enum ConsoleColor
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
	}
}

#endif
