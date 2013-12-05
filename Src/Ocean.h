#ifndef OCEAN_H
#define OCEAN_H

#include <CGK/Math/Vector2.h>
#include <CGK/App/IDrawable.h>
#include <GL/GLEW.h>

class Ocean : public CGKAppIDrawable 
{
public:
    Ocean(const CGKVector2f& start, const CGKVector2f& end, float dx);
    virtual ~Ocean();

	void Draw();
	void Update();

private:
    GLuint vArrObj;
    GLuint vBuffer;
    GLuint program;

	int numVertices;
};

#endif /* end of include guard: OCEAN_H */
