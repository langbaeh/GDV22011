#include <stdlib.h>
#include <stdio.h>

#include <fstream>
#include <vector>
#include <iostream>

#include <pez.h>
#include <glew.h>
#include <glsw.h>
#include <vectormath.h>
#include "Vec3.h"


static void CreateIcosahedron();
static void LoadEffect();
void CreateModel();

typedef struct {
    GLuint Projection;
    GLuint Modelview;
    GLuint NormalMatrix;
    GLuint LightPosition;
    GLuint AmbientMaterial;
    GLuint DiffuseMaterial;
    GLuint TessLevelInner;
    GLuint TessLevelOuter;
} ShaderUniforms;

static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static const GLuint NormalSlot = 1;
static GLuint ProgramHandle;
static Matrix4 ProjectionMatrix;
static Matrix4 ModelviewMatrix;
static Matrix3 NormalMatrix;
static ShaderUniforms Uniforms;
static float TessLevelInner;
static float TessLevelOuter;

void PezRender(GLuint fbo)
{
    glUniform1f(Uniforms.TessLevelInner, TessLevelInner);
    glUniform1f(Uniforms.TessLevelOuter, TessLevelOuter);
    
    Vector4 lightPosition = V4MakeFromElems(0.25, 0.25, 1, 0);
    glUniform3fv(Uniforms.LightPosition, 1, &lightPosition.x);
    
    glUniformMatrix4fv(Uniforms.Projection, 1, 0, &ProjectionMatrix.col0.x);
    glUniformMatrix4fv(Uniforms.Modelview, 1, 0, &ModelviewMatrix.col0.x);

    Matrix3 nm = M3Transpose(NormalMatrix);
    float packed[9] = { nm.col0.x, nm.col1.x, nm.col2.x,
                        nm.col0.y, nm.col1.y, nm.col2.y,
                        nm.col0.z, nm.col1.z, nm.col2.z };
    glUniformMatrix3fv(Uniforms.NormalMatrix, 1, 0, &packed[0]);

    // Render the scene:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    glUniform3f(Uniforms.AmbientMaterial, 0.04f, 0.04f, 0.04f);
    glUniform3f(Uniforms.DiffuseMaterial, 0, 0.75, 0.75);
    glDrawElements(GL_PATCHES, IndexCount, GL_UNSIGNED_INT, 0);
}

const char* PezInitialize(int width, int height)
{
    TessLevelInner = 5;
    TessLevelOuter = 4;



    //CreateIcosahedron();
    CreateModel();


    LoadEffect();

    Uniforms.Projection = glGetUniformLocation(ProgramHandle, "Projection");
    Uniforms.Modelview = glGetUniformLocation(ProgramHandle, "Modelview");
    Uniforms.NormalMatrix = glGetUniformLocation(ProgramHandle, "NormalMatrix");
    Uniforms.LightPosition = glGetUniformLocation(ProgramHandle, "LightPosition");
    Uniforms.AmbientMaterial = glGetUniformLocation(ProgramHandle, "AmbientMaterial");
    Uniforms.DiffuseMaterial = glGetUniformLocation(ProgramHandle, "DiffuseMaterial");
    Uniforms.TessLevelInner = glGetUniformLocation(ProgramHandle, "TessLevelInner");
    Uniforms.TessLevelOuter = glGetUniformLocation(ProgramHandle, "TessLevelOuter");

    // Set up the projection matrix:
    const float HalfWidth = 0.6f;
    const float HalfHeight = HalfWidth * PEZ_VIEWPORT_HEIGHT / PEZ_VIEWPORT_WIDTH;
    ProjectionMatrix = M4MakeFrustum(-HalfWidth, +HalfWidth, -HalfHeight, +HalfHeight, 5, 150);

    // Initialize various state:
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.7f, 0.6f, 0.5f, 1.0f);

    return "Tessellation Demo";
}

void CreateModel()
{



    std::istream& in = std::cin;

 int nv,nf,ne;
  char s[256];
  in >> s;

  bool noff = false;

  if (s[0] == 'O' && s[1] == 'F' && s[2] == 'F')
    ;
  else if (s[0] == 'N' && s[1] == 'O' && s[2] == 'F' && s[3] == 'F')
    noff = true;
  else{
    std::cerr << "BAD OFF HEADER "  << std::endl;
    return;
  }

  in >> nv;
  in >> nf;
  in >> ne;

  if (nv <= 0 || nf <= 0){
    std::cerr << "number of verts or nf below zero "  << std::endl;
    return;
  }

  std::cerr << nv << " " << nf << " " << ne << std::endl;


  float Verts[6*nv];



  Vec3f vmin(10e6 );
  Vec3f vmax(-10e6 );


  for (int i = 0; i < nv; ++i)
  {
    in >> Verts[i*6+0];
    in >> Verts[i*6+1];
    in >> Verts[i*6+2];
    Verts[i*6+3] = 0.0;
    Verts[i*6+4] = 0.0;
    Verts[i*6+5] = 0.0;

    vmin[0] = std::min(Verts[i*3+0], vmin[0]);
    vmin[1] = std::min(Verts[i*3+1], vmin[1]);
    vmin[2] = std::min(Verts[i*3+2], vmin[2]);

    vmax[0] = std::max(Verts[i*3+0], vmax[0]);
    vmax[1] = std::max(Verts[i*3+1], vmax[1]);
    vmax[2] = std::max(Verts[i*3+2], vmax[2]);
    
  }

  Vec3f mp = (vmin + vmax)*0.5;
  vmax -= mp;
  float maxp = std::max(vmax[0],vmax[1]);
  maxp = std::max(maxp,vmax[2]);
  for (int i = 0; i < nv; ++i)
    {
      Verts[i*6+0] -= mp[0];
      Verts[i*6+1] -= mp[1];
      Verts[i*6+2] -= mp[2];

      Verts[i*6+0] /= maxp;
      Verts[i*6+1] /= maxp;
      Verts[i*6+2] /= maxp;

    }
  int Faces[3*nf];

 for (int i = 0; i < nf; ++i)
  {
    int three;
    in >> three;
    in >> Faces[i*3+0];
    in >> Faces[i*3+1];
    in >> Faces[i*3+2];
    //    std::cerr << Faces[i*3+0] << " " << Faces[i*3+1] << " " << Faces[i*3+2] << std::endl;
 unsigned int id0 = Faces[i*3+0];

  }

 

 for (int i = 0; i < nf; i++)
  {
    Vec3f vec1, vec2, normal;
    unsigned int id0, id1, id2;
    id0 = Faces[i*3+0];
    id1 = Faces[i*3+1];
    id2 = Faces[i*3+2];


    Vec3f v0, v1 ,v2;
    v0 = Vec3f(Verts[id0*6+0],Verts[id0*6+1],Verts[id0*6+2] );
    v1 = Vec3f(Verts[id1*6+0],Verts[id1*6+1],Verts[id1*6+2] );
    v2 = Vec3f(Verts[id2*6+0],Verts[id2*6+1],Verts[id2*6+2] );

    vec1 = v1 - v0;
    vec2 = v2 - v0;

    normal = vec1 ^ vec2; // cross product


    //    std::cerr << id0 << " " << id1 << " " << id2 << std::endl;
    Verts[id0*6+3] += normal[0];
    Verts[id0*6+4] += normal[1];
    Verts[id0*6+5] += normal[2];

    Verts[id1*6+3] += normal[0];
    Verts[id1*6+4] += normal[1];
    Verts[id1*6+5] += normal[2];

    Verts[id2*6+3] += normal[0];
    Verts[id2*6+4] += normal[1];
    Verts[id2*6+5] += normal[2];

  }

  // Normalize normals.
  for (int i = 0; i < nv; ++i) {
    float s  = 0.0;


    s+= Verts[i*6+3]*Verts[i*6+3];
    s+= Verts[i*6+4]*Verts[i*6+4];
    s+= Verts[i*6+5]*Verts[i*6+5];

    s = sqrt(s);
    Verts[i*6+3] /= s;
    Verts[i*6+4] /= s;
    Verts[i*6+5] /= s;
    std::cerr << Verts[i*6+3] << " "  << Verts[i*6+4] << " " << Verts[i*6+5] << std::endl;
  }

 
  
 
  IndexCount = sizeof(Faces)/sizeof(Faces[0]);
    std::cerr << IndexCount << std::endl;
    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 6 * sizeof(float);
    std::cerr <<  sizeof(Verts) << " " <<  sizeof(Verts)/(6*sizeof(float)) <<std::endl;
    std::cerr <<  sizeof(Faces) << " " <<  sizeof(Faces)/(3*sizeof(int)) <<std::endl;
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(NormalSlot);
    glVertexAttribPointer(NormalSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 12));

    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
}


static void CreateIcosahedron()
{
    const int Faces[] = {
        2, 1, 0,
        3, 2, 0,
        4, 3, 0,
        5, 4, 0,
        1, 5, 0,

        11, 6,  7,
        11, 7,  8,
        11, 8,  9,
        11, 9,  10,
        11, 10, 6,

        1, 2, 6,
        2, 3, 7,
        3, 4, 8,
        4, 5, 9,
        5, 1, 10,

        2,  7, 6,
        3,  8, 7,
        4,  9, 8,
        5, 10, 9,
        1, 6, 10 };

    const float Verts[] = {
         0.000f,  0.000f, 1.000f,          0.000f,  0.000f, 1.000f,
         0.894f,  0.000f,  0.447f,           0.894f,  0.000f,  0.447f, 
         0.276f,  0.851f,  0.447f,           0.276f,  0.851f,  0.447f, 
	 -0.724f,  0.526f,  0.447f,  	 -0.724f,  0.526f,  0.447f, 
	 -0.724f, -0.526f,  0.447f,  	 -0.724f, -0.526f,  0.447f, 
         0.276f, -0.851f,  0.447f,           0.276f, -0.851f,  0.447f, 
         0.724f,  0.526f, -0.447f,           0.724f,  0.526f, -0.447f, 
	 -0.276f,  0.851f, -0.447f,  	 -0.276f,  0.851f, -0.447f, 
	 -0.894f,  0.000f, -0.447f,  	 -0.894f,  0.000f, -0.447f, 
	 -0.276f, -0.851f, -0.447f,  	 -0.276f, -0.851f, -0.447f, 
         0.724f, -0.526f, -0.447f,            0.724f, -0.526f, -0.447f,  
         0.000f,  0.000f, -1.000f,            0.000f,  0.000f, -1.000f };
 
    IndexCount = sizeof(Faces) / sizeof(Faces[0]);
    std::cerr << IndexCount << std::endl;

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 6 * sizeof(float);
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(NormalSlot);
    glVertexAttribPointer(NormalSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 12));

    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
}

static void LoadEffect()
{
    GLint compileSuccess, linkSuccess;
    GLchar compilerSpew[256];

    glswInit();
    glswSetPath("../", ".glsl");
    glswAddDirectiveToken("*", "#version 400");

    const char* vsSource = glswGetShader("Geodesic.Vertex");
    const char* tcsSource = glswGetShader("Geodesic.TessControl");
    const char* tesSource = glswGetShader("Geodesic.TessEval");
    const char* gsSource = glswGetShader("Geodesic.Geometry");
    const char* fsSource = glswGetShader("Geodesic.Fragment");
    const char* msg = "Can't find %s shader.  Does '../BicubicPath.glsl' exist?\n";
    PezCheckCondition(vsSource != 0, msg, "vertex");
    PezCheckCondition(tcsSource != 0, msg, "tess control");
    PezCheckCondition(tesSource != 0, msg, "tess eval");
    PezCheckCondition(gsSource != 0, msg, "geometry");
    PezCheckCondition(fsSource != 0, msg, "fragment");

    GLuint vsHandle = glCreateShader(GL_VERTEX_SHADER);
    GLuint tcsHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
    GLuint tesHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
    GLuint gsHandle = glCreateShader(GL_GEOMETRY_SHADER);
    GLuint fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vsHandle, 1, &vsSource, 0);
    glCompileShader(vsHandle);
    glGetShaderiv(vsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(vsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Vertex Shader Errors:\n%s", compilerSpew);

    glShaderSource(tcsHandle, 1, &tcsSource, 0);
    glCompileShader(tcsHandle);
    glGetShaderiv(tcsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(tcsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Tess Control Shader Errors:\n%s", compilerSpew);

    glShaderSource(tesHandle, 1, &tesSource, 0);
    glCompileShader(tesHandle);
    glGetShaderiv(tesHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(tesHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Tess Eval Shader Errors:\n%s", compilerSpew);

    glShaderSource(gsHandle, 1, &gsSource, 0);
    glCompileShader(gsHandle);
    glGetShaderiv(gsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(gsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Geometry Shader Errors:\n%s", compilerSpew);

    glShaderSource(fsHandle, 1, &fsSource, 0);
    glCompileShader(fsHandle);
    glGetShaderiv(fsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(fsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Fragment Shader Errors:\n%s", compilerSpew);

    ProgramHandle = glCreateProgram();
    glAttachShader(ProgramHandle, vsHandle);
    glAttachShader(ProgramHandle, tcsHandle);
    glAttachShader(ProgramHandle, tesHandle);
    glAttachShader(ProgramHandle, gsHandle);
    glAttachShader(ProgramHandle, fsHandle);
    glBindAttribLocation(ProgramHandle, PositionSlot, "Position");
    glBindAttribLocation(ProgramHandle, NormalSlot, "Normal");
    glLinkProgram(ProgramHandle);
    glGetProgramiv(ProgramHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(ProgramHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(linkSuccess, "Shader Link Errors:\n%s", compilerSpew);

    glUseProgram(ProgramHandle);
}

void PezUpdate(unsigned int elapsedMicroseconds)
{

    const float RadiansPerMicrosecond = 0.0000005f;
    static float Theta = 0;
    Theta += elapsedMicroseconds * RadiansPerMicrosecond;
    Matrix4 rotation = M4MakeRotationX(Theta);
    Point3 eyePosition = P3MakeFromElems(0, 0, -10);

    Point3 targetPosition = P3MakeFromElems(0, 0, 0);
    Vector3 upVector = V3MakeFromElems(0, 1, 0);
    Matrix4 lookAt = M4MakeLookAt(eyePosition, targetPosition, upVector);
    ModelviewMatrix = M4Mul(lookAt, rotation);
    NormalMatrix = M4GetUpper3x3(ModelviewMatrix);

    const int VK_LEFT = 0x25;
    const int VK_UP = 0x26;
    const int VK_RIGHT = 0x27;
    const int VK_DOWN = 0x28;

    if (PezIsPressing(VK_RIGHT)) TessLevelInner++;
    if (PezIsPressing(VK_LEFT))  TessLevelInner = TessLevelInner > 1 ? TessLevelInner - 1 : 1;
    if (PezIsPressing(VK_UP))    TessLevelOuter++;
    if (PezIsPressing(VK_DOWN))  TessLevelOuter = TessLevelOuter > 1 ? TessLevelOuter - 1 : 1;
}

void PezHandleMouse(int x, int y, int action)
{
}
