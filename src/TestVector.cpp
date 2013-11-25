#include <celero/TestVector.h>
#include <celero/PimplImpl.h>
#include <celero/BenchmarkInfo.h>

#include <mutex>
#include <vector>
#include <algorithm>

using namespace celero;

///
/// \class Impl
///
class celero::TestVector::Impl
{
	public:
		Impl() : 
			testVectorMutex(),
			testVector(),
			baselineVectorMutex(),
			baselineVector()
		{
		}

		mutable std::mutex testVectorMutex;
		std::vector<std::shared_ptr<BenchmarkInfo>> testVector;

		mutable std::mutex baselineVectorMutex;
		std::vector<std::shared_ptr<BenchmarkInfo>> baselineVector;
};

TestVector::TestVector() : pimpl()
{
}

TestVector& TestVector::Instance()
{
	static TestVector singleton;
	return singleton;
}

void TestVector::pushBackTest(std::shared_ptr<BenchmarkInfo> x)
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->testVectorMutex);
	this->pimpl->testVector.push_back(x);
}

size_t TestVector::getTestSize() const
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->testVectorMutex);
	return this->pimpl->testVector.size();
}

size_t TestVector::getTestSize(const std::string& groupName) const
{
	size_t testSize = 0;
	std::lock_guard<std::mutex> mutexLock(this->pimpl->testVectorMutex);
	for(auto test : this->pimpl->testVector)
	{
		if(test->getGroupName() == groupName)
		{
			testSize++;
		}
	}

	return testSize;
}

void TestVector::forEachTest(std::function<void(std::shared_ptr<BenchmarkInfo>)> f)
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->testVectorMutex);
	std::for_each(this->pimpl->testVector.begin(), this->pimpl->testVector.end(), f);
}

void TestVector::pushBackBaseline(std::shared_ptr<BenchmarkInfo> x)
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->baselineVectorMutex);
	this->pimpl->baselineVector.push_back(x);
}

size_t TestVector::getBaselineSize() const
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->baselineVectorMutex);
	return this->pimpl->baselineVector.size();
}

size_t TestVector::getBaselineSize(const std::string& groupName) const
{
	size_t testSize = 0;
	std::lock_guard<std::mutex> mutexLock(this->pimpl->baselineVectorMutex);

	for(auto test : this->pimpl->baselineVector)
	{
		if(test->getGroupName() == groupName)
		{
			testSize++;
		}
	}

	return testSize;
}

void TestVector::forEachBaseline(std::function<void(std::shared_ptr<BenchmarkInfo>)> f)
{
	std::lock_guard<std::mutex> mutexLock(this->pimpl->baselineVectorMutex);
	std::for_each(this->pimpl->baselineVector.begin(), this->pimpl->baselineVector.end(), f);
}

std::shared_ptr<BenchmarkInfo> TestVector::getBaseline(const std::string& groupName)
{
	// std::lock_guard<std::mutex> mutexLock(this->pimpl->baselineVectorMutex);
	auto it = std::find_if(this->pimpl->baselineVector.begin(), this->pimpl->baselineVector.end(), 
		[&groupName](std::shared_ptr<BenchmarkInfo> td)
		{
			return (td->getGroupName() == groupName);
		});

	return *it;
}

