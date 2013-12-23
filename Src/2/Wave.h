#ifndef WAVE_H
#define WAVE_H

#include <CGK/Math/Vector3.h>
#include <CGK/Math/Vector2.h>
#include <GL/glew.h>
#include <CGK/App/IDrawable.h>

class Wave : public CGKAppIDrawable
{
public:
    Wave();
    virtual ~Wave();
	void Draw();
    void Update();    

private:
    void addWave(
        float amplitude, 
        float waveLength, 
        float period, 
        const CGKVector2f& direction
    );

    unsigned int numWaves;

    GLuint program;
    GLuint buffer;
    GLuint vertexArray;

    CGKVector3f* positions;
    CGKVector3f* normals;
};

#endif /* end of include guard: WAVE_H */
