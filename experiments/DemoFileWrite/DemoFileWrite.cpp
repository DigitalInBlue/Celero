#include <celero/Celero.h>

CELERO_MAIN

class BaseFixture : public celero::TestFixture
{
	public:
		std::vector<std::pair<int64_t, uint64_t>> getExperimentValues() const override
		{
			std::vector<std::pair<int64_t, uint64_t>> bufferSizes;
			bufferSizes.push_back(std::pair<int64_t, uint64_t>(32, 0));
			bufferSizes.push_back(std::pair<int64_t, uint64_t>(64, 0));
			bufferSizes.push_back(std::pair<int64_t, uint64_t>(128, 0));
			bufferSizes.push_back(std::pair<int64_t, uint64_t>(256, 0));
			bufferSizes.push_back(std::pair<int64_t, uint64_t>(512, 0));
			bufferSizes.push_back(std::pair<int64_t, uint64_t>(1024, 0));
			bufferSizes.push_back(std::pair<int64_t, uint64_t>(2048, 0));
			bufferSizes.push_back(std::pair<int64_t, uint64_t>(4096, 0));
			return bufferSizes;
		}

		// Scale the experiment values to megabytes.
		double getExperimentValueResultScale() const override
		{
			return 1024.0 * 1024.0;
		}

		void setUp(int64_t experimentValue) override
		{
			for(auto i = 0; i < experimentValue; ++i)
			{
				this->buffer.push_back(rand() % 256);
			}
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
			this->file = fopen("FileWrite.out", "wb");
		}

		void tearDown() override
		{
			fclose(this->file);
			BaseFixture::tearDown();
		}

		FILE* file;
};

BASELINE_F(FileWrite, fwrite, StdFileFixture, 30, 5000)
{
	fwrite(buffer.data(), sizeof(char), buffer.size() * sizeof(char), file);
}

BENCHMARK_F(FileWrite, fwrite_with_fflush, StdFileFixture, 30, 5000)
{
	fwrite(buffer.data(), sizeof(char), buffer.size() * sizeof(char), file);
	fflush(file);
}
