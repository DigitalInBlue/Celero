// Original code at: https://github.com/fenbf/benchmarkLibsTest
#pragma once

static int randSeed = 10;
inline float randF()
{
	return 0.01f * (float)((randSeed++) % 100);
}

static const size_t UPDATES = 1000;
static const float DELTA_TIME = 1.0f / 60.0f;

template <int BufSize = 1>
class TParticle
{
private:
	float pos[4];
	float acc[4];
	float vel[4];
	float col[4];
	float rot;
	float time;
	float buf[BufSize];

public:
	void generate()
	{
		acc[0] = randF();
		acc[1] = randF();
		acc[2] = randF();
		acc[3] = randF();
		pos[0] = pos[1] = pos[2] = pos[3] = 0.0f;
		vel[0] = randF();
		vel[1] = randF();
		vel[2] = randF();
		vel[3] = vel[1] + vel[2];
		rot = 0.0f;
		time = 2.0f + randF();
	}

	void update(float dt)
	{
		vel[0] += acc[0] * dt;
		vel[1] += acc[1] * dt;
		vel[2] += acc[2] * dt;
		vel[3] += acc[3] * dt;
		pos[0] += vel[0] * dt;
		pos[1] += vel[1] * dt;
		pos[2] += vel[2] * dt;
		pos[3] += vel[3] * dt;
		col[0] = pos[0] * 0.001f;
		col[1] = pos[1] * 0.001f;
		col[2] = pos[2] * 0.001f;
		col[3] = pos[3] * 0.001f;
		rot += vel[3] * dt;
		time -= dt;

		for(int i = 0; i < BufSize; ++i)
		{
			buf[i] = vel[i % 4] + vel[i % 4] + vel[2] + pos[0] + pos[1] + pos[i % 4];
		}

		if(time < 0.0f)
		{
			generate();
		}
	}
};

using Particle = TParticle<1>;	 // sizeof 19*float = 76bytes
using Particle160 = TParticle<22>; // sizeof (18 + 22)*float = 160 bytes