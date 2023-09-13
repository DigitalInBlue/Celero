#include <celero/Celero.h>

CELERO_MAIN

class BaseFixture : public celero::TestFixture
{
public:
	std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> getExperimentValues() const override
	{
		std::vector<std::shared_ptr<celero::TestFixture::ExperimentValue>> bufferSizes;
		bufferSizes.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(32));
		bufferSizes.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(64));
		bufferSizes.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(128));
		bufferSizes.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(256));
		bufferSizes.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(512));
		bufferSizes.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(1024));
		bufferSizes.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(2048));
		bufferSizes.push_back(std::make_shared<celero::TestFixture::ExperimentValue>(4096));
		return bufferSizes;
	}

	// Scale the experiment values to megabytes.
	double getExperimentValueResultScale() const override
	{
		return 1024.0 * 1024.0;
	}

	void setUp(const celero::TestFixture::ExperimentValue* const experimentValue) override
	{
		for(auto i = 0; i < experimentValue->Value; ++i)
		{
			this->buffer.push_back(std::abs(celero::Random()) % 256);
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
	void setUp(const celero::TestFixture::ExperimentValue* const x) override
	{
		BaseFixture::setUp(x);

		#ifdef WIN32
		fopen_s(&this->file , "FileWrite.out", "wb");
		#else
		this->file = fopen("FileWrite.out", "wb");
		#endif
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
