// Original code at: https://github.com/fenbf/benchmarkLibsTest

#pragma warning(push)
#pragma warning(disable : 4251) // warning C4251: celero::Result::pimpl': class 'celero::Pimpl<celero::Result::Impl>' needs to have dll-interface to
								// be used by clients of class 'celero::Result'
#include <celero\Celero.h>
#pragma warning(pop)

#include <emmintrin.h>
#include <omp.h>
#include <bitset>
#include <iostream>
#include <random>

CELERO_MAIN;

static const int SamplesCount = 1000;
static const int IterationsCount = 5;
static const int ThresholdValue = 127;
static const int MaximumDistribution = 255;

void Checker(bool val, bool reference, uint64_t i)
{
	if(val != reference)
	{
		std::cout << "Mismatch at index: " << i << "\n";
	}
}

bool GetBoolAt(int64_t pos, uint8_t* outputValues)
{
	const auto bytePos = pos / 8;
	const auto bitPos = pos % 8;
	return ((*(outputValues + bytePos)) & (1 << bitPos)) > 0;
}

///
/// \struct MyExperimentValue
///
/// \brief Simple extension of the ExperimentValue struct.
///
class MyExperimentValue : public celero::TestFixture::ExperimentValue
{
public:
	MyExperimentValue() : celero::TestFixture::ExperimentValue(){};
	virtual ~MyExperimentValue() = default;

	uint64_t NumBytes{0};
	uint64_t NumFullBytes{0};
};

class CompressBoolsFixture : public celero::TestFixture
{
public:
	virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
	{
		std::vector<celero::TestFixture::ExperimentValue> problemSpace;

		const auto stepCount = MaxArrayLength / BenchmarkSteps;

		for(int64_t i = 0; i < BenchmarkSteps; i++)
		{
			// ExperimentValues is part of the base class and allows us to specify
			// some values to control various test runs to end up building a nice graph.
			MyExperimentValue ev;
			ev.Value = stepCount + i * stepCount;
			ev.Iterations = int64_t(i + 1);
			ev.NumBytes = static_cast<unsigned int>((ev.Value + 7) / 8);
			ev.NumFullBytes = static_cast<unsigned int>((ev.Value) / 8);
			problemSpace.push_back(ev);
		}

		return problemSpace;
	}

	/// Before each run, build a vector of random integers.
	virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
	{
		this->arrayLength = static_cast<size_t>(experimentValue.Value);
		this->inputValues.reset(new int[arrayLength]);
		this->referenceValues.reset(new bool[arrayLength]);

		std::mt19937 gen(0); // Standard mersenne_twister_engine seeded with 0, constant
		std::uniform_int_distribution<> dist(0, MaximumDistribution);

		// set every byte, copute reference values
		for(int64_t i = 0; i < experimentValue.Value; ++i)
		{
			this->inputValues[i] = dist(gen);
			this->referenceValues[i] = this->inputValues[i] > ThresholdValue;
		}
	}

#ifdef _DEBUG
	static const int64_t MaxArrayLength{100};
	static const int64_t BenchmarkSteps{1};
#else
	static const int64_t MaxArrayLength{5000000};
	static const int64_t BenchmarkSteps{5};
#endif

	size_t arrayLength{0};
	std::unique_ptr<int[]> inputValues;
	std::unique_ptr<bool[]> referenceValues;
};

class NoPackingFixture : public CompressBoolsFixture
{
public:
	virtual void setUp(const celero::TestFixture::ExperimentValue& x) override
	{
		CompressBoolsFixture::setUp(x);
		this->outputValues.reset(new bool[static_cast<unsigned int>(arrayLength)]);
	}

	virtual void tearDown() override
	{
		for(size_t i = 0; i < arrayLength; ++i)
		{
			Checker(outputValues[i], referenceValues[i], i);
		}
	}

	std::unique_ptr<bool[]> outputValues;
};

BASELINE_F(CompressBoolsTest, NoPackingVersion, NoPackingFixture, SamplesCount, IterationsCount)
{
	for(size_t i = 0; i < this->arrayLength; ++i)
	{
		this->outputValues[i] = this->inputValues[i] > ThresholdValue;
	}
}

class StdBitsetFixture : public CompressBoolsFixture
{
public:
	virtual void tearDown() override
	{
		for(size_t i = 0; i < this->arrayLength; ++i)
		{
			Checker(this->outputBitset[i], this->referenceValues[i], i);
		}
	}

	std::bitset<MaxArrayLength> outputBitset;
};

// Run an automatic baseline.
// Celero will help make sure enough samples are taken to get a reasonable measurement
BENCHMARK_F(CompressBoolsTest, StdBitset, StdBitsetFixture, SamplesCount, IterationsCount)
{
	for(size_t i = 0; i < this->arrayLength; ++i)
	{
		this->outputBitset.set(i, this->inputValues[i] > ThresholdValue);
	}
}

class StdVectorFixture : public CompressBoolsFixture
{
public:
	virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
	{
		CompressBoolsFixture::setUp(experimentValue);
		this->outputVector.resize(static_cast<unsigned int>(experimentValue.Value));
	}

	virtual void tearDown() override
	{
		for(size_t i = 0; i < arrayLength; ++i)
		{
			Checker(this->outputVector[i], this->referenceValues[i], i);
		}
	}

	std::vector<bool> outputVector;
};

BENCHMARK_F(CompressBoolsTest, StdVector, StdVectorFixture, SamplesCount, IterationsCount)
{
	for(size_t i = 0; i < this->arrayLength; ++i)
	{
		this->outputVector[i] = this->inputValues[i] > ThresholdValue;
	}
}

class ManualVersionFixture : public CompressBoolsFixture
{
public:
	ManualVersionFixture() : numBytes(0), numFullBytes(0)
	{
	}

	virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
	{
		CompressBoolsFixture::setUp(experimentValue);

		auto ev = dynamic_cast<const MyExperimentValue&>(experimentValue);
		this->numBytes = ev.NumBytes;
		this->numFullBytes = ev.NumFullBytes;
		this->outputValues.reset(new uint8_t[this->numBytes]);
	}

	virtual void tearDown() override
	{
		for(size_t i = 0; i < this->arrayLength; ++i)
		{
			Checker(GetBoolAt(i, outputValues.get()), referenceValues[i], i);
		}
	}

	uint64_t numBytes{0};
	uint64_t numFullBytes{0};
	std::unique_ptr<uint8_t[]> outputValues;
};

BENCHMARK_F(CompressBoolsTest, SingleVarVersion, ManualVersionFixture, SamplesCount, IterationsCount)
{
	uint8_t OutByte = 0;
	int shiftCounter = 0;

	auto pInputData = inputValues.get();
	auto pOutputByte = outputValues.get();

	for(size_t i = 0; i < this->arrayLength; ++i)
	{
		if(*pInputData > ThresholdValue)
		{
			OutByte |= (1 << shiftCounter);
		}

		pInputData++;
		shiftCounter++;

		if(shiftCounter > 7)
		{
			*pOutputByte++ = OutByte;
			OutByte = 0;
			shiftCounter = 0;
		}
	}

	// our byte might be incomplete, so we need to handle this:
	if(this->arrayLength & 7)
	{
		*pOutputByte++ = OutByte;
	}
}

BENCHMARK_F(CompressBoolsTest, NotDepentendVersion, ManualVersionFixture, SamplesCount, IterationsCount)
{
	uint8_t Bits[8] = {0};
	const int64_t lenDivBy8 = (this->arrayLength / 8) * 8;

	auto pInputData = inputValues.get();
	auto pOutputByte = outputValues.get();

	for(int64_t j = 0; j < lenDivBy8; j += 8)
	{
		Bits[0] = pInputData[0] > ThresholdValue ? 0x01 : 0;
		Bits[1] = pInputData[1] > ThresholdValue ? 0x02 : 0;
		Bits[2] = pInputData[2] > ThresholdValue ? 0x04 : 0;
		Bits[3] = pInputData[3] > ThresholdValue ? 0x08 : 0;
		Bits[4] = pInputData[4] > ThresholdValue ? 0x10 : 0;
		Bits[5] = pInputData[5] > ThresholdValue ? 0x20 : 0;
		Bits[6] = pInputData[6] > ThresholdValue ? 0x40 : 0;
		Bits[7] = pInputData[7] > ThresholdValue ? 0x80 : 0;

		*pOutputByte++ = Bits[0] | Bits[1] | Bits[2] | Bits[3] | Bits[4] | Bits[5] | Bits[6] | Bits[7];
		pInputData += 8;
	}

	if(arrayLength & 7)
	{
		// note that we'll use max 7 elements, so max is Bits[6]... (otherwise we would get another full byte)
		auto RestW = arrayLength & 7;
		memset(Bits, 0, 8);

		for(unsigned long long i = 0; i < RestW; ++i)
		{
			Bits[i] = *pInputData > ThresholdValue ? 1 << i : 0;
			pInputData++;
		}

		*pOutputByte++ = Bits[0] | Bits[1] | Bits[2] | Bits[3] | Bits[4] | Bits[5] | Bits[6];
	}
}

struct bool8
{
	bool8() : val0(0), val1(0), val2(0), val3(0), val4(0), val5(0), val6(0), val7(0)
	{
	}

	bool8(bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7) :
		val0(v0),
		val1(v1),
		val2(v2),
		val3(v3),
		val4(v4),
		val5(v5),
		val6(v6),
		val7(v7)
	{
	}

	uint8_t val0 : 1;
	uint8_t val1 : 1;
	uint8_t val2 : 1;
	uint8_t val3 : 1;
	uint8_t val4 : 1;
	uint8_t val5 : 1;
	uint8_t val6 : 1;
	uint8_t val7 : 1;
};

class PackedStructFixture : public CompressBoolsFixture
{
public:
	virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
	{
		CompressBoolsFixture::setUp(experimentValue);

		this->numBytes = static_cast<unsigned int>((experimentValue.Value + 7) / 8);
		this->numFullBytes = static_cast<unsigned int>((experimentValue.Value) / 8);
		this->outputValues.reset(new bool8[numBytes]);
	}

	virtual void tearDown() override
	{
		for(size_t i = 0; i < arrayLength; ++i)
		{
			Checker(getBoolAt(i), referenceValues[i], i);
		}
	}

	bool getBoolAt(int64_t pos)
	{
		const auto bytePos = pos / 8;
		const auto bitPos = pos % 8;

		switch(bitPos)
		{
			case 0:
				return (*(outputValues.get() + bytePos)).val0;
			case 1:
				return (*(outputValues.get() + bytePos)).val1;
			case 2:
				return (*(outputValues.get() + bytePos)).val2;
			case 3:
				return (*(outputValues.get() + bytePos)).val3;
			case 4:
				return (*(outputValues.get() + bytePos)).val4;
			case 5:
				return (*(outputValues.get() + bytePos)).val5;
			case 6:
				return (*(outputValues.get() + bytePos)).val6;
			case 7:
				return (*(outputValues.get() + bytePos)).val7;
		}

		return (*(outputValues.get() + bytePos)).val7;
	}

	unsigned int numBytes;
	unsigned int numFullBytes;
	std::unique_ptr<bool8[]> outputValues;
};

BENCHMARK_F(CompressBoolsTest, PackedStructVersion, PackedStructFixture, SamplesCount, IterationsCount)
{
	const int64_t lenDivBy8 = (arrayLength / 8) * 8;

	auto pInputData = inputValues.get();
	auto pOutputByte = outputValues.get();

	bool8 out;

	for(int64_t j = 0; j < lenDivBy8; j += 8)
	{
		out.val0 = pInputData[0] > ThresholdValue;
		out.val1 = pInputData[1] > ThresholdValue;
		out.val2 = pInputData[2] > ThresholdValue;
		out.val3 = pInputData[3] > ThresholdValue;
		out.val4 = pInputData[4] > ThresholdValue;
		out.val5 = pInputData[5] > ThresholdValue;
		out.val6 = pInputData[6] > ThresholdValue;
		out.val7 = pInputData[7] > ThresholdValue;

		*pOutputByte++ = out;
		pInputData += 8;
	}

	if(arrayLength & 7)
	{
		auto RestW = arrayLength & 7;
		out = {false, false, false, false, false, false, false, false};

		if(RestW > 6)
		{
			out.val6 = pInputData[6] > ThresholdValue;
		}

		if(RestW > 5)
		{
			out.val5 = pInputData[5] > ThresholdValue;
		}

		if(RestW > 4)
		{
			out.val4 = pInputData[4] > ThresholdValue;
		}

		if(RestW > 3)
		{
			out.val3 = pInputData[3] > ThresholdValue;
		}

		if(RestW > 2)
		{
			out.val2 = pInputData[2] > ThresholdValue;
		}

		if(RestW > 1)
		{
			out.val1 = pInputData[1] > ThresholdValue;
		}

		if(RestW > 0)
		{
			out.val0 = pInputData[0] > ThresholdValue;
		}

		*pOutputByte++ = out;
	}
}

BENCHMARK_F(CompressBoolsTest, WithOpenMP, ManualVersionFixture, SamplesCount, IterationsCount)
{
	uint8_t Bits[8];
#pragma omp parallel for private(Bits)
	for(unsigned int i = 0; i < numFullBytes; ++i)
	{
		auto pInputData = inputValues.get() + i * 8;
		Bits[0] = pInputData[0] > ThresholdValue ? 0x01 : 0;
		Bits[1] = pInputData[1] > ThresholdValue ? 0x02 : 0;
		Bits[2] = pInputData[2] > ThresholdValue ? 0x04 : 0;
		Bits[3] = pInputData[3] > ThresholdValue ? 0x08 : 0;
		Bits[4] = pInputData[4] > ThresholdValue ? 0x10 : 0;
		Bits[5] = pInputData[5] > ThresholdValue ? 0x20 : 0;
		Bits[6] = pInputData[6] > ThresholdValue ? 0x40 : 0;
		Bits[7] = pInputData[7] > ThresholdValue ? 0x80 : 0;

		this->outputValues.get()[i] = Bits[0] | Bits[1] | Bits[2] | Bits[3] | Bits[4] | Bits[5] | Bits[6] | Bits[7];
	}

	if(numFullBytes < numBytes)
	{
		uint8_t Bits2[8] = {0};
		auto RestW = arrayLength & 7;
		auto pInputData = inputValues.get() + numFullBytes * 8;

		for(unsigned long long i = 0; i < RestW; ++i)
		{
			Bits2[i] = *pInputData > ThresholdValue ? 1 << i : 0;
			pInputData++;
		}

		this->outputValues.get()[numFullBytes] = Bits2[0] | Bits2[1] | Bits2[2] | Bits2[3] | Bits2[4] | Bits2[5] | Bits2[6];
	}
}

class SimdVersionFixture : public CompressBoolsFixture
{
public:
	virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
	{
		CompressBoolsFixture::setUp(experimentValue);

		this->numBytes = static_cast<unsigned int>((experimentValue.Value + 7) / 8);
		this->numFullBytes = static_cast<unsigned int>((experimentValue.Value) / 8);
		this->alignedOutputValues = (uint8_t*)_aligned_malloc(numBytes, 16);
		this->signedInputValues.reset(new int8_t[arrayLength]);

		for(size_t i = 0; i < this->arrayLength; ++i)
		{
			this->signedInputValues[i] = static_cast<int8_t>(this->inputValues[i] - 128);
		}
	}

	virtual void tearDown() override
	{
		for(size_t i = 0; i < this->arrayLength; ++i)
		{
			Checker(GetBoolAt(i, this->alignedOutputValues), this->referenceValues[i], i);
		}

		if(this->alignedOutputValues)
		{
			_aligned_free(this->alignedOutputValues);
			this->alignedOutputValues = nullptr;
		}
	}

	unsigned int numBytes;
	unsigned int numFullBytes;
	uint8_t* alignedOutputValues{nullptr};
	std::unique_ptr<int8_t[]> signedInputValues;
};

BENCHMARK_F(CompressBoolsTest, SimdVersion, SimdVersionFixture, SamplesCount, IterationsCount)
{
	uint16_t Bits[16] = {0};
	const size_t lenDiv16y16 = (arrayLength / 16) * 16; // full packs of 16 values...

	const __m128i sse127 = _mm_set1_epi8(127);
	const int8_t ConvertedThreshold = ThresholdValue - 128;
	const __m128i sseThresholds = _mm_set1_epi8(ConvertedThreshold);

	auto pInputData = signedInputValues.get();
	auto pOutputByte = alignedOutputValues;

	for(size_t j = 0; j < lenDiv16y16; j += 16)
	{
		const auto in16Values =
			_mm_set_epi8(pInputData[15], pInputData[14], pInputData[13], pInputData[12], pInputData[11], pInputData[10], pInputData[9], pInputData[8],
						 pInputData[7], pInputData[6], pInputData[5], pInputData[4], pInputData[3], pInputData[2], pInputData[1], pInputData[0]);
		const auto cmpRes = _mm_cmpgt_epi8(in16Values, sseThresholds);
		const auto packed = _mm_movemask_epi8(cmpRes);
		*((uint16_t*)pOutputByte) = static_cast<uint16_t>(packed);
		pOutputByte += 2;
		pInputData += 16;
	}

	if(arrayLength & 15)
	{
		auto RestW = arrayLength & 15;
		memset(Bits, 0, 16 * sizeof(uint16_t));

		for(size_t i = 0; i < RestW; ++i)
		{
			Bits[i] = *pInputData > ConvertedThreshold ? 1 << i : 0;
			pInputData++;
		}

		*pOutputByte++ = static_cast<uint8_t>(Bits[0] | Bits[1] | Bits[2] | Bits[3] | Bits[4] | Bits[5] | Bits[6] | Bits[7]);

		if(RestW > 8)
		{
			*pOutputByte++ = (Bits[8] | Bits[9] | Bits[10] | Bits[11] | Bits[12] | Bits[13] | Bits[14] | Bits[15]) >> 8;
		}
	}
}
