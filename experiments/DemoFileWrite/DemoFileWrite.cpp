#include <celero/Celero.h>

CELERO_MAIN

class BaseFixture : public celero::TestFixture
{
	public:
        std::vector<int64_t> getExperimentValues() const override
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

        int64_t getExperimentValueResultScale() const override
		{
			return 1024 * 1024;
		}

        void setUp(int64_t experimentValue) override
		{
			for (auto i = 0; i < experimentValue; ++i)
				this->buffer.push_back(rand() % 256);
		}

        void tearDown() override
		{
			this->buffer.clear();
		}

		std::vector<char> buffer;
};

class StdFileFixture : public BaseFixture
{
	public:
        void setUp(int64_t experimentValue) override
		{
			BaseFixture::setUp(experimentValue);
			file = fopen("FileWrite.out", "wb");
		}

        void tearDown() override
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
