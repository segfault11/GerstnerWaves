#ifndef OCEAN_H
#define OCEAN_H

#include <CGK/Math/Vector2.h>
#include <CGK/App/IDrawable.h>
#include <GL/GLEW.h>

struct GerstnerWave
{
	GerstnerWave(
		float waveLength,
		float amplitude,
		float angularFrequency,
		float phase,
		CGKVector2f direction
	)
	:
		WaveLength(waveLength),
		Amplitude(amplitude),
		AngularFrequency(angularFrequency),
		Phase(phase),
		Direction(direction)
	{

	};	

	float WaveLength;
	float Amplitude;
	float AngularFrequency;
	float Phase;
	CGKVector2f Direction;
};


class Ocean : public CGKAppIDrawable 
{
public:
    Ocean(const CGKVector2f& start, const CGKVector2f& end, float dx);
    virtual ~Ocean();

	void AddGerstnerWave(const GerstnerWave& wave);
	void Draw();
	void Update();

private:
    GLuint vArrObj;
    GLuint vBuffer;
    GLuint program;

	int numWaves;
	int numVertices;
	float dx;
};

#endif /* end of include guard: OCEAN_H */
