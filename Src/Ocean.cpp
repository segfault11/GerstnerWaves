//-----------------------------------------------------------------------------
#include "Ocean.h"
#include <iostream>
#include <CGK/Error/Error.h>
#include <CGK/Math/Matrix4.h>
#include <CGK/Math/Vector3.h>
#include <CGK/OpenGL/Program.h>
#include <CGK/App/Application.h>
//-----------------------------------------------------------------------------
// Shader definitions
//-----------------------------------------------------------------------------
#define TO_STRING(x) #x
//-----------------------------------------------------------------------------
static const char* vertexShader =
    "#version 150\n"
	"#define pi 3.141592653589793238462643383279\n"
TO_STRING(
    uniform mat4 P;
    uniform mat4 V;
	uniform float t;

    in vec3 Position;

    void main()
    {
		float x0 = Position.x;	
		float z0 = Position.z;	

		float x = x0;
		float y = 1.0f/16.0f*cos(-z0*4.0f*pi - 2.0f*pi*t);
		float z = z0 - 1.0f/16.0f*sin(-z0*4.0f*pi - 2.0f*pi*t);

        gl_Position = P*V*vec4(x, y, z, 1.0f);
    }
);
//-----------------------------------------------------------------------------
static const char* fragmentShader =
    "#version 150\n"
TO_STRING(
    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
);
//-----------------------------------------------------------------------------
Ocean::Ocean(const CGKVector2f& start, const CGKVector2f& end, float dx)
{
    CGK_ASSERT( dx > 0 )
    CGK_ASSERT( start[0] < end[0] )
    CGK_ASSERT( start[1] < end[1] )
    
    int nx = (end[0] - start[0])/dx + 1;
    int ny = (end[1] - start[1])/dx + 1;
	
	this->numVertices = nx*ny;

    CGKVector3f* vertices = new CGKVector3f[nx*ny];
   
    for (int i = 0; i < nx; i++)
    {
        for (int j = 0; j < ny; j++)
        {
            int idx = j*nx + i;
            float x = start[0] + i*dx;
            float z = start[1] + j*dx;
        
            vertices[idx][0] = x;
            vertices[idx][1] = 0.0f;
            vertices[idx][2] = z;
        }
    } 
   
    glGenBuffers(1, &this->vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->vBuffer);
    
    glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(CGKVector3f)*nx*ny, 
        &vertices[0], 
        GL_STATIC_DRAW
    );

    glGenVertexArrays(1, &this->vArrObj);
    glBindVertexArray(this->vArrObj);
    glBindBuffer(GL_ARRAY_BUFFER, this->vBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    this->program = glCreateProgram();
    
    CGKOpenGLProgramAttachShaderFromSource(
        this->program, 
        GL_VERTEX_SHADER, 
        vertexShader
    );

    CGKOpenGLProgramAttachShaderFromSource(
        this->program, 
        GL_FRAGMENT_SHADER, 
        fragmentShader
    );
    
    glBindAttribLocation(this->program, 0, "Position");
    glBindFragDataLocation(this->program, 0, "FragData");

    CGKOpenGLProgramLink(this->program);

    delete[] vertices; 
}
//-----------------------------------------------------------------------------
Ocean::~Ocean()
{
    glDeleteBuffers(1, &this->vBuffer);
    glDeleteVertexArrays(1, &this->vArrObj);
    glDeleteProgram(this->program);
}
//-----------------------------------------------------------------------------
void Ocean::Draw()
{
	CGKMatrix4f p;

	float aspect = static_cast<float>(CGKAppGetScreenWidth())/
		CGKAppGetScreenHeight(); 

	p.MakePerspective(60.0f, aspect, 0.01f, 10.0f);

	CGKMatrix4f v;

	v.MakeView(
		CGKVector3f(2.0f, 0.0f, 1.0f), 
		CGKVector3f(0.0f, 0.0f, 1.0f), 
		CGKVector3f(0.0f, 1.0f, 0.0f)
	);

	glUseProgram(this->program);
	CGKOpenGLProgramUniformMatrix4f(this->program, "P", p.GetData(), GL_TRUE);
	CGKOpenGLProgramUniformMatrix4f(this->program, "V", v.GetData(), GL_TRUE);
	glBindVertexArray(this->vArrObj);
	glDrawArrays(GL_POINTS, 0, numVertices);
}
//-----------------------------------------------------------------------------
void Ocean::Update()
{
	static float t = 0.0f;

	glUseProgram(this->program);
	CGKOpenGLProgramUniform1f(this->program, "t", t);
	t += CGKAppGetDeltaTime()/1000.0f;
}
//-----------------------------------------------------------------------------
