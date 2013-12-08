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
TO_STRING(
    in vec3 Position;

    void main()
    {
        gl_Position = vec4(Position, 1.0f);// P*V*vec4(x, y, z, 1.0f);
    }
);
//-----------------------------------------------------------------------------
static const char* geometryShader =
	"#version 150\n"
	"#define pi 3.141592653589793238462643383279\n"
TO_STRING(
	struct GerstnerWave
	{
		float WaveLength;
		float Amplitude;
		float Frequency;
		float Phase;
		vec2 Direction;
	};

	uniform GerstnerWave waves[32];
	uniform int numWaves;
	uniform float dx;
	uniform mat4 P;	
	uniform mat4 V;	
	uniform float t;

	layout (points) in;
	layout (triangle_strip, max_vertices = 6) out;
	
	vec4 disp(vec4 pos)
	{
		
		vec2 x0 = vec2(pos.x, pos.z);
		float x = x0.x;
		float y = 0.0f;
		float z = x0.y;

		for (int i = 0; i < numWaves; i++)
		{
			vec2 dir = waves[i].Direction;
			float k = 2.0f*pi/waves[i].WaveLength;		
			float A = waves[i].Amplitude;
			float omega = waves[i].Frequency;
			float phi = waves[i].Phase;		

			dir = normalize(dir);
	
			float a = dot(dir, x0)*k - omega*t + phi;
	
			x += dir.x*A*sin(a); 
			y += A*cos(a);
			z += dir.y*A*sin(a); 
		}
	
		return vec4(x, y, z, 1.0f);
	}
	
	vec4 displace(vec4 pos)
	{
		float x0 = pos.x;	
		float z0 = pos.z;	

		float x = x0;
		float y = 1.0f/16.0f*cos(-z0*4.0f*pi - 2.0f*pi*t);
		float z = z0 - 1.0f/16.0f*sin(-z0*4.0f*pi - 2.0f*pi*t);

		return vec4(x, y, z, 1.0f);
	}

	void main()
	{
		vec4 pos = gl_in[0].gl_Position;

		gl_Position = P*V*(disp(pos + vec4(0.0f, 0.0, 0.0f, 0.0f)));
		EmitVertex();	
		
		gl_Position = P*V*(disp(pos + vec4(dx, 0.0, 0.0f, 0.0f)));
		EmitVertex();		
		
		gl_Position = P*V*(disp(pos + vec4(0.0f, 0.0, dx, 0.0f)));
		EmitVertex();		
		EndPrimitive();

		gl_Position = P*V*(disp(pos + vec4(dx, 0.0, 0.0f, 0.0f)));
		EmitVertex();	
		
		gl_Position = P*V*(disp(pos + vec4(dx, 0.0, dx, 0.0f)));
		EmitVertex();		
		
		gl_Position = P*V*(disp(pos + vec4(0.0f, 0.0, dx, 0.0f)));
		EmitVertex();		
		EndPrimitive();
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
: numWaves(0), dx(dx)
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
        GL_GEOMETRY_SHADER, 
        geometryShader
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
void Ocean::AddGerstnerWave(const GerstnerWave& wave)
{
	if (this->numWaves >= 32) 
	{
	    std::cout << __FUNCTION__ << " cannot add more then 32 waves." << std::endl;
	}

	std::stringstream waveLength;
	std::stringstream amplitude;
	std::stringstream frequency;
	std::stringstream direction;
	std::stringstream phase;

	waveLength << "waves[" << this->numWaves << "].WaveLength";
	amplitude << "waves[" << this->numWaves << "].Amplitude";
	frequency << "waves[" << this->numWaves << "].Frequency";
	direction << "waves[" << this->numWaves << "].Direction";
	phase << "waves[" << this->numWaves << "].Phase";

	glUseProgram(this->program);

	CGKOpenGLProgramUniform1f(
		this->program, waveLength.str().c_str(), wave.WaveLength
	);

	CGKOpenGLProgramUniform1f(
		this->program, amplitude.str().c_str(), wave.Amplitude
	);

	CGKOpenGLProgramUniform1f(
		this->program, frequency.str().c_str(), wave.AngularFrequency
	);

	CGKOpenGLProgramUniform1f(
		this->program, phase.str().c_str(), wave.Phase
	);

	CGKOpenGLProgramUniform2f(
		this->program, direction.str().c_str(),
		wave.Direction[0], wave.Direction[1]
	);

	CGK_ASSERT( glGetError() == GL_NO_ERROR )
	this->numWaves++;
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
		CGKVector3f(0.0f, 0.25f, -0.25f), 
		CGKVector3f(0.0f, 0.0f, 1.0f), 
		CGKVector3f(0.0f, 1.0f, 0.0f)
	);

	glUseProgram(this->program);
	CGKOpenGLProgramUniformMatrix4f(this->program, "P", p.GetData(), GL_TRUE);
	CGKOpenGLProgramUniformMatrix4f(this->program, "V", v.GetData(), GL_TRUE);
	CGKOpenGLProgramUniform1f(this->program, "dx", dx);
	CGKOpenGLProgramUniform1i(this->program, "numWaves", this->numWaves);
	glBindVertexArray(this->vArrObj);
	glDrawArrays(GL_POINTS, 0, numVertices);
}
//-----------------------------------------------------------------------------
void Ocean::Update()
{
	static float t = 0.0f;

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glUseProgram(this->program);
	CGKOpenGLProgramUniform1f(this->program, "t", t);
	t += CGKAppGetDeltaTime()/1000.0f;
}
//-----------------------------------------------------------------------------
