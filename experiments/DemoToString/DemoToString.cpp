#include <celero/Celero.h>

#include <strstream>
#include <cstdlib>

///
/// This is the main(int argc, char** argv) for the entire celero program.
/// You can write your own, or use this macro to insert the standard one into the project.
///
CELERO_MAIN

const std::string uint8str[50] = { 
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", 
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49" };

struct itostr_helper 
{
	static unsigned out[10000];

	itostr_helper() 
	{
		for(int i = 0; i < 10000; i++) 
		{
			unsigned v = i;
			char * o = (char*) (out + i);
		
			o[3] = v % 10 + '0';
			o[2] = (v % 100) / 10 + '0';
			o[1] = static_cast<char>((v % 1000) / 100 + '0');
			o[0] = static_cast<char>((v % 10000) / 1000);
			
			if(o[0])
			{
				o[0] |= 0x30;
			}
			else if(o[1] != '0')
			{
				o[0] |= 0x20;
			}
			else if(o[2] != '0')
			{
				o[0] |= 0x10;
			}
			else
			{
				o[0] |= 0x00;
			}
		}
	}
};

unsigned itostr_helper::out[10000];

itostr_helper hlp_init;

template<typename T> std::string hopmanFast(T o)
{
	typedef itostr_helper hlp;

	unsigned blocks[3], *b = blocks + 2;
	blocks[0] = o < 0 ? ~o + 1 : o;
	blocks[2] = blocks[0] % 10000; blocks[0] /= 10000;
	blocks[2] = hlp::out[blocks[2]];

	if(blocks[0]) 
	{
		blocks[1] = blocks[0] % 10000; blocks[0] /= 10000;
		blocks[1] = hlp::out[blocks[1]];
		blocks[2] |= 0x30303030;
		b--;
	}

	if(blocks[0]) 
	{
		blocks[0] = hlp::out[blocks[0] % 10000];
		blocks[1] |= 0x30303030;
		b--;
	}

	char* f = ((char*) b);
	f += 3 - (*f >> 4);

	char* str = (char*) blocks;
	
	if(o < 0)
	{
		*--f = '-';
	}

	return std::string(f, (str + 12) - f);
}

BASELINE(DemoToString, Baseline, 128, 1000000)
{
	const uint8_t x = rand() % 42;
	celero::DoNotOptimizeAway(std::string("42"));
}

BENCHMARK(DemoToString, to_string, 128, 1000000)
{
	const uint8_t x = rand() % 42;
	celero::DoNotOptimizeAway(std::to_string(x));
}

BENCHMARK(DemoToString, strstream, 128, 1000000)
{
	const uint8_t x = rand() % 42;
	std::strstream ss; ss << x;
	celero::DoNotOptimizeAway(ss.str());
}

BENCHMARK(DemoToString, table, 128, 1000000)
{
	const uint8_t x = rand() % 42;
	celero::DoNotOptimizeAway(uint8str[x]);
}

/// http://ideone.com/GkPcy
BENCHMARK(DemoToString, hopmanFast, 128, 1000000)
{
	const uint8_t x = rand() % 42;
	celero::DoNotOptimizeAway(std::string(hopmanFast(x)));
}
