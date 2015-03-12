#include <celero/Celero.h>

CELERO_MAIN

class BaseFixture : public celero::TestFixture
{
	public:
		virtual std::vector<int64_t> getExperimentValues() const
		{
			std::vector<int64_t> bufferSizes;
			bufferSizes.push_back(32);
			bufferSizes.push_back(64);
			bufferSizes.push_back(128);
			bufferSizes.push_back(256);
			bufferSizes.push_back(512);
			bufferSizes.push_back(1024);
			bufferSizes.push_back(2048);
			bufferSizes.push_back(4096);
			return bufferSizes;
		}

		virtual int64_t getExperimentValueSize() const
		{
			return 1;
		}

		virtual void setUp(int64_t experimentValue)
		{
			for (auto i = 0; i < experimentValue; ++i)
				this->buffer.push_back(rand() % 256);
		}

		virtual void tearDown()
		{
			this->buffer.clear();
		}

		std::vector<char> buffer;
};

class StdFileFixture : public BaseFixture
{
	public:
		virtual void setUp(int64_t experimentValue)
		{
			BaseFixture::setUp(experimentValue);
			file = fopen("FileWrite.out", "wb");
		}

		virtual void tearDown()
		{
			fclose(file);
			BaseFixture::tearDown();
		}

		FILE* file;
};

BASELINE_F(FileWrite, fwrite, StdFileFixture, 1, 500000)
{
	celero::DoNotOptimizeAway(fwrite(buffer.data(), sizeof(char), buffer.size() * sizeof(char), file));
}

BENCHMARK_F(FileWrite, fwrite_with_fflush, StdFileFixture, 1, 500000)
{
	celero::DoNotOptimizeAway(fwrite(buffer.data(), sizeof(char), buffer.size() * sizeof(char), file));
	celero::DoNotOptimizeAway(fflush(file));
}
