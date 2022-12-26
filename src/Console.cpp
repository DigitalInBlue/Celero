///
/// \author	John Farrier
///
/// \copyright Copyright 2015-2023 John Farrier
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

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#else
#include <iostream>
#endif

#ifdef _WIN32
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
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED);
#else
	std::cout << "\033[49m\033[31m";
#endif
}

void RedBold()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;31m";
#endif
}

void Green()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_GREEN);
#else
	std::cout << "\033[49m\033[32m";
#endif
}

void GreenBold()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;32m";
#endif
}

void Blue()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_BLUE);
#else
	std::cout << "\033[49m\033[34m";
#endif
}

void BlueBold()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;34m";
#endif
}

void Cyan()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_GREEN);
#else
	std::cout << "\033[49m\033[36m";
#endif
}

void CyanBold()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;36m";
#endif
}

void Yellow()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN);
#else
	std::cout << "\033[49m\033[33m";
#endif
}

void YellowBold()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;33m";
#endif
}

void White()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	std::cout << "\033[49m\033[37m";
#endif
}

void WhiteBold()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;37m";
#endif
}

void WhiteOnRed()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
	std::cout << "\033[41m\033[37m";
#endif
}

void WhiteOnRedBold()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[41m\033[1;37m";
#endif
}

void PurpleBold()
{
#ifdef _WIN32
	auto h = WinColor();
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#else
	std::cout << "\033[49m\033[1;38m";
#endif
}

void Default()
{
#ifdef _WIN32
	White();
#else
	std::cout << "\033[0m";
#endif
}

void celero::console::SetConsoleColor(const celero::console::ConsoleColor x)
{
	switch(x)
	{
		case console::ConsoleColor::Red:
			Red();
			break;
		case console::ConsoleColor::Red_Bold:
			RedBold();
			break;
		case console::ConsoleColor::Green:
			Green();
			break;
		case console::ConsoleColor::Green_Bold:
			GreenBold();
			break;
		case console::ConsoleColor::Blue:
			Blue();
			break;
		case console::ConsoleColor::Blue_Bold:
			BlueBold();
			break;
		case console::ConsoleColor::Cyan:
			Cyan();
			break;
		case console::ConsoleColor::Cyan_Bold:
			CyanBold();
			break;
		case console::ConsoleColor::Yellow:
			Yellow();
			break;
		case console::ConsoleColor::Yellow_Bold:
			YellowBold();
			break;
		case console::ConsoleColor::White:
			White();
			break;
		case console::ConsoleColor::White_Bold:
			WhiteBold();
			break;
		case console::ConsoleColor::WhiteOnRed:
			WhiteOnRed();
			break;
		case console::ConsoleColor::WhiteOnRed_Bold:
			WhiteOnRedBold();
			break;
		case console::ConsoleColor::Purple_Bold:
			PurpleBold();
			break;
		case console::ConsoleColor::Default:
		default:
			Default();
			break;
	}
}
