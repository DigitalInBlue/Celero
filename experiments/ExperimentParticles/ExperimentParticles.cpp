// Original code at: https://github.com/fenbf/benchmarkLibsTest

#include <celero/Celero.h>
#include "Particles.h"

CELERO_MAIN;

class ParticlesFixture : public celero::TestFixture
{
public:
	virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override
	{
		std::vector<celero::TestFixture::ExperimentValue> problemSpace;
		const auto totalNumberOfTests = 10;

		for(auto i = 0; i < totalNumberOfTests; i++)
		{
			// ExperimentValues is part of the base class and allows us to specify
			// some values to control various test runs to end up building a nice graph.
			problemSpace.push_back({1000 + i * 1000, static_cast<int64_t>(totalNumberOfTests - i)});
		}

		return problemSpace;
	}
};

class ParticlesObjVectorFixture : public ParticlesFixture
{
public:
	/// Before each run, build a vector of random integers.
	virtual void setUp(const celero::TestFixture::ExperimentValue& x) override
	{
		this->particles = std::vector<Particle>(x.Value);

		for(auto& p : this->particles)
		{
			p.generate();
		}
	}

	/// After each run, clear the vector
	virtual void tearDown() override
	{
		this->particles.clear();
	}

	std::vector<Particle> particles;
};

static const int SamplesCount = 100;
static const int IterationsCount = 1;

// Run an automatic baseline.
// Celero will help make sure enough samples are taken to get a reasonable measurement
BASELINE_F(ParticlesTest, ObjVector, ParticlesObjVectorFixture, SamplesCount, IterationsCount)
{
	for(size_t u = 0; u < UPDATES; ++u)
	{
		for(auto& p : particles)
		{
			p.update(DELTA_TIME);
		}
	}
}

class ParticlesPtrVectorFixture : public ParticlesFixture
{
public:
	virtual bool randomizeAddresses()
	{
		return true;
	}

	/// Before each run, build a vector of random integers.
	virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override
	{
		this->particles = std::vector<std::shared_ptr<Particle>>(experimentValue.Value);

		for(auto& p : this->particles)
		{
			p = std::make_shared<Particle>();
		}

		if(this->randomizeAddresses() == true)
		{
			for(int64_t i = 0; i < experimentValue.Value / 2; ++i)
			{
				const auto a = celero::Random() % experimentValue.Value;
				const auto b = celero::Random() % experimentValue.Value;

				if(a != b)
				{
					std::swap(this->particles[a], this->particles[b]);
				}
			}
		}

		for(auto& p : this->particles)
		{
			p->generate();
		}
	}

	/// After each run, clear the vector
	virtual void tearDown() override
	{
		this->particles.clear();
	}

	std::vector<std::shared_ptr<Particle>> particles;
};

BENCHMARK_F(ParticlesTest, PtrVector, ParticlesPtrVectorFixture, SamplesCount, IterationsCount)
{
	for(size_t u = 0; u < UPDATES; ++u)
	{
		for(auto& p : this->particles)
		{
			p->update(DELTA_TIME);
		}
	}
}

class ParticlesPtrVectorNoRandFixture : public ParticlesPtrVectorFixture
{
public:
	virtual bool randomizeAddresses()
	{
		return false;
	}
};

BENCHMARK_F(ParticlesTest, PtrVectorNoRand, ParticlesPtrVectorNoRandFixture, SamplesCount, IterationsCount)
{
	for(size_t u = 0; u < UPDATES; ++u)
	{
		for(auto& p : this->particles)
		{
			p->update(DELTA_TIME);
		}
	}
}
