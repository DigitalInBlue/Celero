#include <celero/Celero.h>

CELERO_MAIN

class BaseFixture : public celero::TestFixture
{
public:
	std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
	{
		std::vector<celero::TestFixture::ExperimentValue> bufferSizes;
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

	// Scale the experiment values to megabytes.
	double getExperimentValueResultScale() const override
	{
		return 1024.0 * 1024.0;
	}

	void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
	{
		for(auto i = 0; i < experimentValue.Value; ++i)
		{
			this->buffer.push_back(celero::Random() % 256);
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
	void setUp(const celero::TestFixture::ExperimentValue& x) override
	{
		BaseFixture::setUp(x);
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
