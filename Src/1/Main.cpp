#include <CGK/App/Application.h>
#include <CGK/Math/Vector2.h>
#include <cmath>
#include "Ocean.h"

int main(int argc, const char *argv[])
{
    CGKAppCreate("Ocean", 10, 10, 800, 600);
	Ocean ocean(CGKVector2f(-1.0, 0.0), CGKVector2f(1.0f, 2.0f), 0.025f);
	
	ocean.AddGerstnerWave(
		GerstnerWave(1.0f, 0.25f*1.0f/8.0f, 0.5*M_PI, 0.0f, CGKVector2f(1.0f, 0.0f))
	);
	
	ocean.AddGerstnerWave(
		GerstnerWave(1.0f, 0.25f*1.0f/8.0f, 0.2f*M_PI, 0.0f, CGKVector2f(0.0f, 1.0f))
	);
	
	ocean.AddGerstnerWave(
		GerstnerWave(0.25f, 1.0f/100.0f, 2.0f*M_PI, 1.0f, CGKVector2f(-1.0f, 1.0f))
	);

	CGKAppRegisterDrawable(ocean);
    CGKAppRun();
    CGKAppDestroy();

    return 0;
}

