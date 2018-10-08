#include <celero/Celero.h>

#include <atomic>
#include <mutex>

CELERO_MAIN

class BaseFixture : public celero::ThreadTestFixture
{
public:
	int counter;

	void setUp(const celero::TestFixture::ExperimentValue&) override
	{
		this->counter = 0;
	}
};

class AtomicFixture : public celero::ThreadTestFixture
{
public:
	std::atomic<int> counter;

	void setUp(const celero::TestFixture::ExperimentValue&) override
	{
		this->counter = 0;
	}
};

class MutexFixture : public celero::ThreadTestFixture
{
public:
	int counter;
	std::mutex mutex;

	void setUp(const celero::TestFixture::ExperimentValue&) override
	{
		this->counter = 0;
	}
};

BASELINE_T(DemoMultithread, Baseline, BaseFixture, 10, 10000000, 8)
{
	++counter;
}

BENCHMARK_T(DemoMultithread, Atomic, AtomicFixture, 10, 10000000, 8)
{
	++counter;
}

BENCHMARK_T(DemoMultithread, Mutex, MutexFixture, 10, 10000000, 8)
{
	std::lock_guard<std::mutex> lock(mutex);
	++counter;
}
