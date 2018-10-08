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

#include <celero/Console.h>

using namespace celero;

#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#else
#include <curses.h>
#include <iostream>
#endif

#ifdef WIN32
auto WinColor() -> decltype(GetStdHandle(STD_OUTPUT_HANDLE))
{
	auto h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	GetConsoleScreenBufferInfo(h, &csbiInfo);
	return h;
}
#endif

void Red()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED);
#else
	std::cout << "\033[49m\033[31m";
#endif
}

void RedBold()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;31m";
#endif
}

void Green()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_GREEN);
#else
	std::cout << "\033[49m\033[32m";
#endif
}

void GreenBold()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;32m";
#endif
}

void Blue()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_BLUE);
#else
	std::cout << "\033[49m\033[34m";
#endif
}

void BlueBold()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;34m";
#endif
}

void Cyan()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_GREEN);
#else
	std::cout << "\033[49m\033[36m";
#endif
}

void CyanBold()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;36m";
#endif
}

void Yellow()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN);
#else
	std::cout << "\033[49m\033[33m";
#endif
}

void YellowBold()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;33m";
#endif
}

void White()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	std::cout << "\033[49m\033[37m";
#endif
}

void WhiteBold()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;37m";
#endif
}

void WhiteOnRed()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	std::cout << "\033[41m\033[37m";
#endif
}

void WhiteOnRedBold()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[41m\033[1;37m";
#endif
}

void PurpleBold()
{
#ifdef WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;38m";
#endif
}

void Default()
{
#ifdef WIN32
	White();
#else
	std::cout << "\033[0m";
#endif
}

void celero::console::SetConsoleColor(const celero::console::ConsoleColor x)
{
	switch(x)
	{
		case ConsoleColor_Red:
			Red();
			break;
		case ConsoleColor_Red_Bold:
			RedBold();
			break;
		case ConsoleColor_Green:
			Green();
			break;
		case ConsoleColor_Green_Bold:
			GreenBold();
			break;
		case ConsoleColor_Blue:
			Blue();
			break;
		case ConsoleColor_Blue_Bold:
			BlueBold();
			break;
		case ConsoleColor_Cyan:
			Cyan();
			break;
		case ConsoleColor_Cyan_Bold:
			CyanBold();
			break;
		case ConsoleColor_Yellow:
			Yellow();
			break;
		case ConsoleColor_Yellow_Bold:
			YellowBold();
			break;
		case ConsoleColor_White:
			White();
			break;
		case ConsoleColor_White_Bold:
			WhiteBold();
			break;
		case ConsoleColor_WhiteOnRed:
			WhiteOnRed();
			break;
		case ConsoleColor_WhiteOnRed_Bold:
			WhiteOnRedBold();
			break;
		case ConsoleColor_Purple_Bold:
			PurpleBold();
			break;
		case ConsoleColor_Default:
		default:
			Default();
			break;
	}
}
