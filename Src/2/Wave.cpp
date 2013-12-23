//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include <CGK/App/Application.h>
#include <CGK/Config/ConfigFile.h>
#include "Wave.h"
#include <CGK/OpenGL/Buffer.h>
#include <CGK/OpenGL/Program.h>
#include <CGK/MainCamera/MainCamera.h>
#include <vector>
#include <CGK/Math/Vector2.h>
//-----------------------------------------------------------------------------
#define TO_STRING(x)  #x
//-----------------------------------------------------------------------------
static const char* vertexShader =
	"#version 150\n"
	"float PI = 3.141592653;\n"
TO_STRING(
	uniform mat4 V;
	uniform mat4 P;

	in vec2 pos;
	
	void main()
	{
		gl_Position = vec4(pos.x, 0.3*sin(2*PI*pos.x), pos.y, 1.0);
	}

);
//-----------------------------------------------------------------------------
static const char* geometryShader =
    "#version 150\n"
	"float PI = 3.141592653;\n"
TO_STRING(
    uniform float dx;
	uniform mat4 V;
	uniform mat4 P;
    uniform float t;
    uniform vec2 dir = vec2(1.0, 1.0);
    
    struct Wave
    {
        float amplitude;
        float waveLength;
        float period;
        vec2 direction;
    };

    uniform Wave waves[6];
    uniform int numWaves;


    layout (points) in;
    layout (triangle_strip, max_vertices = 6) out;
   
    smooth out vec3 normal;
 
    vec4 disp(vec4 p)
    {
        float h = 0;
        float x = p.x;
        float z = p.z;    
    
        for (int i = 0; i < numWaves; i++)
        {
            vec2 d = normalize(waves[i].direction);
            vec2 x = vec2(p.x, p.z);
            float a = 2.0*PI*(dot(d, x)/waves[i].waveLength + t/waves[i].period);
            x += waves[i].amplitude*d.x*cos(a);
            h += waves[i].amplitude*sin(a); 
            z += waves[i].amplitude*d.y*cos(a);
        }
   
        return vec4(
            p.x, 
            h,
            p.z, 
            p.w
        );
    }

    vec3 nrm(vec4 p)
    {
        float hx = 0.0;
        float hy = 1.0;
        float hz = 0.0;
    
        for (int i = 0; i < numWaves; i++)
        {
            vec2 d = normalize(waves[i].direction);
            vec2 x = vec2(p.x, p.z);
            hx -= waves[i].amplitude*2.0*PI/waves[i].waveLength*d.x*cos(2.0*PI*(dot(d, x)/waves[i].waveLength + t/waves[i].period)); 
            hy -= waves[i].amplitude*2.0*PI/waves[i].waveLength*sin(2.0*PI*(dot(d, x)/waves[i].waveLength + t/waves[i].period)); 
            hz -= waves[i].amplitude*2.0*PI/waves[i].waveLength*d.y*cos(2.0*PI*(dot(d, x)/waves[i].waveLength + t/waves[i].period)); 
        }    

        return normalize(vec3(hx, hy, hz));
    }
    

    void main()
    {
        vec4 pos = gl_in[0].gl_Position;
        float x = pos.x;
        float z = pos.z;

        gl_Position = P*V*disp(vec4(x, 0.0, z, 1.0));
        normal = nrm(vec4(x, 0.0, z, 1.0));
        EmitVertex();                
        
        gl_Position = P*V*disp(vec4(x + dx, 0.0, z, 1.0));
        normal = nrm(vec4(x + dx, 0.0, z, 1.0));
        EmitVertex();                
        
        gl_Position = P*V*disp(vec4(x + dx, 0.0, z + dx, 1.0));
        normal = nrm(vec4(x + dx, 0.0, z + dx, 1.0));
        EmitVertex();                
        EndPrimitive();        
 
        gl_Position = P*V*disp(vec4(x, 0.0, z, 1.0));
        normal = nrm(vec4(x, 0.0, z, 1.0));
        EmitVertex();                
        
        gl_Position = P*V*disp(vec4(x, 0.0, z + dx, 1.0));
        normal = nrm(vec4(x, 0.0, z + dx, 1.0));
        EmitVertex();                
        
        gl_Position = P*V*disp(vec4(x + dx, 0.0, z + dx, 1.0));
        normal = nrm(vec4(x + dx, 0.0, z + dx, 1.0));
        EmitVertex();  
        EndPrimitive();              
    }
);
//-----------------------------------------------------------------------------
static const char* fragmentShader =
	"#version 150\n"
TO_STRING(

	out vec4 fragColor;
    smooth in vec3 normal;

	void main()
	{
        vec3 col = vec3(0.0, 0.3, 0.5);
        vec3 lig = normalize(vec3(1.0, 1.0, 1.0));

        vec3 fc = 0.2*col + 0.6*col*max(dot(lig, normal), 0.0);
        

		fragColor = vec4(fc, 1.0);
		//fragColor = vec4(abs(normal), 1.0);
	}
);
//-----------------------------------------------------------------------------
static float f(const CGKVector2f& p)
{
	return 0.0f;
}
//-----------------------------------------------------------------------------
Wave::Wave()
: numWaves(0)
{
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

	glBindAttribLocation(this->program, 0,"pos");
	glBindFragDataLocation(this->program, 0, "fragOut");
	CGKOpenGLProgramLink(this->program);
    glUseProgram(this->program);
    CGKOpenGLProgramUniform1f(this->program, "dx", 0.01);
    this->addWave(0.00425, 0.15, 5.0, CGKVector2f(-1.0, 1.0));
    this->addWave(0.01025, 0.45, 2.5, CGKVector2f(1.0, 0.0));
    this->addWave(0.01825, 2.0, 4.0, CGKVector2f(1.0, -2.0));
    this->addWave(0.01825, 0.4, 6.0, CGKVector2f(-0.0, -3.0));
    this->addWave(0.00325, 0.5, 3.0, CGKVector2f(-1.0, -1.0));
//    this->addWave(0.00125, 0.1, 2.0, CGKVector2f(-1.0, -1.0));
 //   this->addWave(0.0025, 0.1, 3.0, CGKVector2f(1.0, 0.0));

    glUseProgram(0);


	glGenBuffers(1, &this->buffer);
	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);

	float dx = 0.01;
	std::vector<CGKVector2f> v;

	for (unsigned int i = 0; i < 100; i++)
	{                          
	    for (unsigned int j = 0; j < 100; j++)
	    {                          
	        v.push_back(CGKVector2f(i*dx, j*dx));
	    }                                                         
	}

	std::cout << v.size() << std::endl;
	
	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(CGKVector2f)*v.size(), 
		v.data(),
	    GL_STATIC_DRAW
	);
		
	glGenVertexArrays(1, &this->vertexArray);
	glBindVertexArray(this->vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexArray);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
}
//-----------------------------------------------------------------------------
Wave::~Wave()
{

}
//-----------------------------------------------------------------------------
void Wave::Draw()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
	glUseProgram(this->program);

	CGKOpenGLProgramUniformMatrix4f(
		this->program, 
		"P",
		CGKMainCameraGetPerspective().GetData(),
		GL_TRUE
	);	
	
	CGKOpenGLProgramUniformMatrix4f(
		this->program, 
		"V",
		CGKMainCameraGetView().GetData(),
		GL_TRUE
	);	

	glBindVertexArray(this->vertexArray);	
	glDrawArrays(GL_POINTS, 0, 10000);


	CGKMainCameraRotateAroundYAxisAtPointWithAngle(
		CGKMainCameraGetFocus(),
		0.15f*CGKAppGetDeltaTime()/1000.0
	);
}
//-----------------------------------------------------------------------------
void Wave::Update()
{
    static float t = 0.0;
    
    glUseProgram(this->program);
    CGKOpenGLProgramUniform1f(this->program, "t", t);

    t += CGKAppGetDeltaTime()/1000.0f;
}
//-----------------------------------------------------------------------------
void Wave::addWave(
    float amplitude,
    float waveLength,
    float period,
    const CGKVector2f& dir
)
{
    glUseProgram(this->program);

    {
    std::stringstream s;
    s << "waves[" << this->numWaves << "].amplitude";
    CGKOpenGLProgramUniform1f(this->program, s.str().c_str(), amplitude); 
    }

    {
    std::stringstream s;
    s << "waves[" << this->numWaves << "].waveLength";
    CGKOpenGLProgramUniform1f(this->program, s.str().c_str(), waveLength); 
    }
    {
    std::stringstream s;
    s << "waves[" << this->numWaves << "].period";
    CGKOpenGLProgramUniform1f(this->program, s.str().c_str(), period); 
    }
    {
    std::stringstream s;
    s << "waves[" << this->numWaves << "].direction";
    CGKOpenGLProgramUniform2f(this->program, s.str().c_str(), dir[0], dir[1]); 
    }

    numWaves++;
    
    CGKOpenGLProgramUniform1i(this->program, "numWaves", this->numWaves);
};
//-----------------------------------------------------------------------------
