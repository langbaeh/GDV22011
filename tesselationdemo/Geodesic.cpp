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
static void CreateCube();
static void CreateObject();
static void LoadEffect();
static void refreshBuffer(bool newBuffer);
static void updateBuffer();
void CreateModel();

typedef struct {
    GLuint Projection;
    GLuint Modelview;
    GLuint NormalMatrix;
    GLuint LightPosition;
    GLuint AmbientMaterial;
    GLuint DiffuseMaterial;
    GLuint TessLevel;
    GLuint TessLevelOuter;
    GLuint NormalModel;
    GLuint Tesselation;
    GLuint Wireframe;
    GLuint Tagg;
  GLuint ColNorm;
  GLuint DCol;
} ShaderUniforms;

static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static const GLuint NormalSlot = 1;
static const GLuint TagSlot = 2;
static const GLuint DeltaSlot = 3;
GLuint positions;

static GLuint ProgramHandle;
static Matrix4 ProjectionMatrix;
static Matrix4 ModelviewMatrix;
static Matrix3 NormalMatrix;
static ShaderUniforms Uniforms;
static float TessLevel = 10.0;;
static float TessLevelOuter;
static float NormalModel = 2.0;
static float Tesselation = 0.0;
static float Wireframe = 0.0;
static float Tagg = 0.0;
static float ColNorm = 0.0;
static float DCol = 0.0;

// storage

std::vector<float> Verts;
std::vector<float> Faces;
int nv;

// Camera information
Point3 eyePosition = P3MakeFromElems(0, 0, -10);
Point3 targetPosition = P3MakeFromElems(0, 0, 1.0);
Vector3 upVector = V3MakeFromElems(0, 1, 0);
float camAngleX=0.0f, camAngleY=0.0f;   // camera angles

// Button information
float mouseSensitivy = 0.001f;
bool b_r = false;
int mouseButton = 0;
int mouseX = 0;
int mouseY = 0;

// selection
int selID = 0;


void PezRender(GLuint fbo)
{
    glUniform1f(Uniforms.TessLevel, TessLevel);
    glUniform1f(Uniforms.TessLevelOuter, TessLevelOuter);
    glUniform1f(Uniforms.NormalModel, NormalModel);
    glUniform1f(Uniforms.Tesselation, Tesselation);
    glUniform1f(Uniforms.Wireframe, Wireframe);
    glUniform1f(Uniforms.Tagg, Tagg);
    glUniform1f(Uniforms.ColNorm, ColNorm);
    glUniform1f(Uniforms.DCol, DCol);
    
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
    TessLevel = 10.0;
    TessLevelOuter = 7;



    //    CreateIcosahedron();
    // CreateCube();
    //CreateObject();
        CreateModel();


    LoadEffect();

    Uniforms.Projection = glGetUniformLocation(ProgramHandle, "Projection");
    Uniforms.Modelview = glGetUniformLocation(ProgramHandle, "Modelview");
    Uniforms.NormalMatrix = glGetUniformLocation(ProgramHandle, "NormalMatrix");
    Uniforms.LightPosition = glGetUniformLocation(ProgramHandle, "LightPosition");
    Uniforms.AmbientMaterial = glGetUniformLocation(ProgramHandle, "AmbientMaterial");
    Uniforms.DiffuseMaterial = glGetUniformLocation(ProgramHandle, "DiffuseMaterial");
    Uniforms.TessLevel = glGetUniformLocation(ProgramHandle, "TessLevel");
    Uniforms.TessLevelOuter = glGetUniformLocation(ProgramHandle, "TessLevelOuter");
    Uniforms.NormalModel = glGetUniformLocation(ProgramHandle, "NormalModel");
    Uniforms.Tesselation = glGetUniformLocation(ProgramHandle, "Tesselation");
    Uniforms.Wireframe = glGetUniformLocation(ProgramHandle, "Wireframe");
    Uniforms.Tagg = glGetUniformLocation(ProgramHandle, "Tagg");
    Uniforms.ColNorm = glGetUniformLocation(ProgramHandle, "ColNorm");
    Uniforms.DCol = glGetUniformLocation(ProgramHandle, "DCol");

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

std::cerr << "Loading model from OFF file"  << std::endl;

    std::istream& in = std::cin;

 int nf,ne;
  char s[256];
  in >> s;
  bool noff = false;
  bool doff = false;


  if (s[0] == 'O' && s[1] == 'F' && s[2] == 'F'){
    ;
    std::cerr << "Computing new tags and deltas" <<std::endl;
  }
  else if (s[0] == 'N' && s[1] == 'O' && s[2] == 'F' && s[3] == 'F'){
    noff = true;
  }
  else if (s[0] == 'D' && s[1] == 'O' && s[2] == 'F' && s[3] == 'F'){
    doff = true;
    std::cerr << "Running with tags and deltas" <<std::endl;
  }
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

 
  //  float Verts[12*nv];
  Verts.clear();
  Verts.resize(12*nv);

  Vec3f vmin(10e6 );
  Vec3f vmax(-10e6 );


  for (int i = 0; i < nv; ++i)
  {
    in >> Verts[i*12+0];
    in >> Verts[i*12+1];
    in >> Verts[i*12+2];
    if (doff){
      in >> Verts[i*12+3];
      in >> Verts[i*12+4];
      in >> Verts[i*12+5];

      in >> Verts[i*12+6];
      in >> Verts[i*12+7];
      in >> Verts[i*12+8];

      in >> Verts[i*12+9];
      in >> Verts[i*12+10];
      in >> Verts[i*12+11];
    }
    else{
      Verts[i*12+3] = 0.0;
      Verts[i*12+4] = 0.0;
      Verts[i*12+5] = 0.0;
      Verts[i*12+6] = 0.0;
      Verts[i*12+7] = 0.0;
      Verts[i*12+8] = 0.0;
      Verts[i*12+9] = 0.0; 
      Verts[i*12+10] = 0.0;
      Verts[i*12+11] = 0.0;
    }




    vmin[0] = std::min(Verts[i*12+0], vmin[0]);
    vmin[1] = std::min(Verts[i*12+1], vmin[1]);
    vmin[2] = std::min(Verts[i*12+2], vmin[2]);

    vmax[0] = std::max(Verts[i*12+0], vmax[0]);
    vmax[1] = std::max(Verts[i*12+1], vmax[1]);
    vmax[2] = std::max(Verts[i*12+2], vmax[2]);
  }
    for (int i = 0; i < nv; ++i){

    std::cerr << i << " : "<<Verts[i*12+0] << " " << Verts[i*12+1] << " " <<  Verts[i*12+2] << std::endl;
    std::cerr << i << " : "<<Verts[i*12+3] << " " << Verts[i*12+4] << " " <<  Verts[i*12+5] << std::endl;
    std::cerr << i << " : "<<Verts[i*12+6] << " " << Verts[i*12+7] << " " <<  Verts[i*12+8] << std::endl;
    std::cerr << i << " : "<<Verts[i*12+9] << " " << Verts[i*12+10] << " " <<  Verts[i*12+11] << std::endl;
    }
  Vec3f mp = (vmin + vmax)*0.5;
  vmax -= mp;
  float maxp = std::max(vmax[0],vmax[1]);
  maxp = std::max(maxp,vmax[2]);
  for (int i = 0; i < nv; ++i)
    {
      Verts[i*12+0] -= mp[0];
      Verts[i*12+1] -= mp[1];
      Verts[i*12+2] -= mp[2];

      Verts[i*12+0] /= maxp;
      Verts[i*12+1] /= maxp;
      Verts[i*12+2] /= maxp;

    }
  //  int Faces[3*nf];
  Faces.clear();
  Faces.resize(3*nf);

 for (int i = 0; i < nf; ++i)
  {
    int three;
    in >> three;
    in >> Faces[i*3+2];
    in >> Faces[i*3+1];
    in >> Faces[i*3+0];
    //        std::cerr << Faces[i*3+0] << " " << Faces[i*3+1] << " " << Faces[i*3+2] << std::endl;
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
    v0 = Vec3f(Verts[id0*12+0],Verts[id0*12+1],Verts[id0*12+2] );
    v1 = Vec3f(Verts[id1*12+0],Verts[id1*12+1],Verts[id1*12+2] );
    v2 = Vec3f(Verts[id2*12+0],Verts[id2*12+1],Verts[id2*12+2] );

    vec1 = v1 - v0;
    vec2 = v2 - v0;

    normal = vec1 ^ vec2; // cross product


    //    std::cerr << id0 << " " << id1 << " " << id2 << std::endl;
    Verts[id0*12+3] += normal[0];
    Verts[id0*12+4] += normal[1];
    Verts[id0*12+5] += normal[2];

    Verts[id1*12+3] += normal[0];
    Verts[id1*12+4] += normal[1];
    Verts[id1*12+5] += normal[2];

    Verts[id2*12+3] += normal[0];
    Verts[id2*12+4] += normal[1];
    Verts[id2*12+5] += normal[2];
  }

  // Normalize normals.
  for (int i = 0; i < nv; ++i) {
    float s  = 0.0;


    s+= Verts[i*12+3]*Verts[i*12+3];
    s+= Verts[i*12+4]*Verts[i*12+4];
    s+= Verts[i*12+5]*Verts[i*12+5];

    s = -sqrt(s);
    Verts[i*12+3] /= s;
    Verts[i*12+4] /= s;
    Verts[i*12+5] /= s;

    //    std::cerr << Verts[i*6+3] << " "  << Verts[i*6+4] << " " << Verts[i*6+5] << std::endl;
  }


  float normDist[nv];
  for (int i = 0; i < nv; i++){
    normDist[i] = 0.0;
  }

  if (!doff){
    // compute distance of face normals to averaged vertex normals
    std::cerr<< "computing distances" << std::endl;
    for (int i = 0; i < nf; i++)
      {
	Vec3f vec1, vec2, normal;
	unsigned int id0, id1, id2;
	id0 = Faces[i*3+0];
	id1 = Faces[i*3+1];
	id2 = Faces[i*3+2];
	
	
	Vec3f v0, v1 ,v2;
	v0 = Vec3f(Verts[id0*12+0],Verts[id0*12+1],Verts[id0*12+2] );
	v1 = Vec3f(Verts[id1*12+0],Verts[id1*12+1],Verts[id1*12+2] );
	v2 = Vec3f(Verts[id2*12+0],Verts[id2*12+1],Verts[id2*12+2] );
	
	Vec3f n0, n1 ,n2;
	n0 = Vec3f(Verts[id0*12+3],Verts[id0*12+4],Verts[id0*12+5] );
	n1 = Vec3f(Verts[id1*12+3],Verts[id1*12+4],Verts[id1*12+5] );
	n2 = Vec3f(Verts[id2*12+3],Verts[id2*12+4],Verts[id2*12+5] );
	
	vec1 = v1 - v0;
	vec2 = v2 - v0;
	
	normal = vec1 ^ vec2; // cross product
	normal.normalize();
	//     std::cerr << normal[0] << " " << normal[1] << " " <<normal[2]  <<std::endl;
	// std::cerr << vec1[0] << " " << vec1[1] << " " <<vec1[2]  <<std::endl;
     // std::cerr << vec2[0] << " " << vec2[1] << " " << vec2[2]  <<std::endl;
	normDist[id0] += (normal* n0) + 1.0;
	normDist[id1] += (normal* n1) + 1.0;
	normDist[id2] += (normal* n2) + 1.0;
      }
    
    Vec3f strongEdge[nv];
    float edgestrengh[nv];
    // SLOW !!!
    for (int v = 0; v < nv; v++){
      edgestrengh[v]  = 100000.0;
      for (int i = 0; i < nf; i++){
	unsigned int id0, id1, id2;
	id0 = Faces[i*3+0];
	id1 = Faces[i*3+1];
	id2 = Faces[i*3+2];
	
	if (id0 == v || id1 == v || id2 == v){
	  Vec3f v0, v1 ,v2;
	  v0 = Vec3f(Verts[id0*12+0],Verts[id0*12+1],Verts[id0*12+2] );
	  v1 = Vec3f(Verts[id1*12+0],Verts[id1*12+1],Verts[id1*12+2] );
	  v2 = Vec3f(Verts[id2*12+0],Verts[id2*12+1],Verts[id2*12+2] );
	  Vec3f vec1 = v1 - v0;
	  Vec3f vec2 = v2 - v0;
	  
	  Vec3f normal = vec1 ^ vec2; // cross product
	  Vec3f n = Vec3f(Verts[v*12+3],Verts[v*12+4],Verts[v*12+5] );
	  float s = (normal* n) + 1.0;
	  
	  if (s < edgestrengh[v]){
	    edgestrengh[v] = s;
	    if (v == id1){
	      strongEdge[v] = vec1;
	    }else if (v == id2){
	      strongEdge[v] = vec2;
	    }else{
	      strongEdge[v] = v0-v1;
	    }
	  }
	}
	
	
      }
    }
    
    
    
    float DIST = 1.0;
    
    for (int i = 0; i < nf; i++)
      {
	Vec3f vec1, vec2, normal;
	unsigned int id0, id1, id2;
	id0 = Faces[i*3+0];
	id1 = Faces[i*3+1];
	id2 = Faces[i*3+2];
	
	
	Vec3f v0, v1 ,v2;
	v0 = Vec3f(Verts[id0*12+0],Verts[id0*12+1],Verts[id0*12+2] );
	v1 = Vec3f(Verts[id1*12+0],Verts[id1*12+1],Verts[id1*12+2] );
	v2 = Vec3f(Verts[id2*12+0],Verts[id2*12+1],Verts[id2*12+2] );
	
	Vec3f n0, n1 ,n2;
	n0 = Vec3f(Verts[id0*12+3],Verts[id0*12+4],Verts[id0*12+5] );
	n1 = Vec3f(Verts[id1*12+3],Verts[id1*12+4],Verts[id1*12+5] );
	n2 = Vec3f(Verts[id2*12+3],Verts[id2*12+4],Verts[id2*12+5] );
	
	Vec3f t0, t1, t2;
	t0 = Vec3f(Verts[id0*12+9],Verts[id0*12+10],Verts[id0*12+11] );
	t1 = Vec3f(Verts[id1*12+9],Verts[id1*12+10],Verts[id1*12+11] );
	t2 = Vec3f(Verts[id2*12+9],Verts[id2*12+10],Verts[id2*12+11] );
	
	
	
	//     std::cerr << normal[0] << " " << normal[1] << " " <<normal[2]  <<std::endl;
	// std::cerr << vec1[0] << " " << vec1[1] << " " <<vec1[2]  <<std::endl;
	// std::cerr << vec2[0] << " " << vec2[1] << " " << vec2[2]  <<std::endl;
	
	float t0l = t0.length();
	float t1l = t1.length();
	float t2l = t2.length();
	int new0, new1;
	new0 = -1;
	new1 = -1;
	//     std::cerr << t0l << " " << t1l << " " << t2l  <<std::endl;
	
	if (normDist[id0] > DIST && t0l == 0.0){
	  // not tagged yet
	  if (t1l > 0.0 && t2l > 0.0){
	    // two neighbours are already tagged, can't tag third
	    ;
	  }else{
	    // only one of the neighbors is tagged
	    new1 = id0;
	    if (normDist[id1] > normDist[id2] ){
	      new0 = id1;
	    }else{
	      new0 = id2;
	    }
	  }
	}else if (normDist[id1] > DIST && t1l == 0.0){
	  // not tagged yet
	  if (t0l > 0.0 && t2l > 0.0){
	 // two neighbours are already tagged, can't tag third
	    ;
	  }else{
	    // only one of the neighbors is tagged
	    new1 = id1;
	    if (normDist[id0] > normDist[id2] ){
	      new0 = id0;
	    }else{
	      new0 = id2;
	    }
	  }
	}else if (normDist[id2] > DIST && t2l == 0.0){
	  // not tagged yet
	  if (t1l > 0.0 && t0l > 0.0){
	    // two neighbours are already tagged, can't tag third
	    ;
	  }else{
	    // only one of the neighbors is tagged
	    new1 = id2;
	    if (normDist[id1] > normDist[id0] ){
	      new0 = id1;
	    }else{
	      new0 = id0;
	    }
	  }
	}
	
	if( normDist[new0] < normDist[new1]){
	  int b = new0;
	  new0 = new1;
	  new1 = b;
	  
	}
	
	if (new1 > 0 && new0 > 0 ){	 
	  Vec3f tn1 = Vec3f(Verts[new1*12+3],Verts[new1*12+4],Verts[new1*12+5] );
	  
	  
	  Vec3f delta = (tn1^strongEdge[new0]);
	  delta.normalize();
	  //	 std::cerr << "Tagging "  << new0 << " " << new1 << std::endl;
	  //std::cerr << delta[0] << " " << delta[1] << " " << delta[2]  << std::endl;
	  if (normDist[new0] > DIST){
	    Verts[new0*12+6] = delta[0];
	    Verts[new0*12+7] = delta[1];
	    Verts[new0*12+8] = delta[2];
	    
	    Verts[new0*12+9] = 1.0;
	    Verts[new0*12+10] = -1.0;
	    Verts[new0*12+11] = -1.0;
	  }
	  if (normDist[new0] > DIST){
	    Verts[new1*12+6] = delta[0];
	    Verts[new1*12+7] = delta[1];
	    Verts[new1*12+8] = delta[2];
	    
	    Verts[new1*12+9] = 1.0;
	    Verts[new1*12+10] = -1.0;
	    Verts[new1*12+11] = -1.0;
	  }
	}
      }
  }
  
  // crack prevention
  for (int i = 0; i < nv; i++){
            std::cerr << Verts[i*12+9] << " " << Verts[i*12+10] << " " <<  Verts[i*12+11] << std::endl;
	if ((fabs(Verts[i*12+9])+fabs(Verts[i*12+10])+fabs(Verts[i*12+11])) == 0.0){
      Verts[i*12+9] = 0.00000001;
    }
	//        std::cerr << Verts[i*12+9] << " " << Verts[i*12+10] << " " <<  Verts[i*12+11] << std::endl;
  }
  /*
  IndexCount = sizeof(Faces)/sizeof(Faces[0]);
  //  std::cerr << IndexCount << std::endl;
    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 12 * sizeof(float);
    // std::cerr <<  sizeof(Verts) << " " <<  sizeof(Verts)/(6*sizeof(float)) <<std::endl;
    //std::cerr <<  sizeof(Faces) << " " <<  sizeof(Faces)/(3*sizeof(int)) <<std::endl;
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(NormalSlot);
    glVertexAttribPointer(NormalSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 12));
    glEnableVertexAttribArray(DeltaSlot);
    glVertexAttribPointer(DeltaSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 24));
    glEnableVertexAttribArray(TagSlot);
    glVertexAttribPointer(TagSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 36));

    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
  */

  refreshBuffer(true);
}

/*
static void CreateObject()
{
    const int Faces[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
	0, 5, 6,
	0, 6, 1,

        7, 2, 1,
        7, 3, 2,
        7, 4, 3,
        7, 5, 4,
	7, 6, 5,
	7, 1, 6,

        };

    float s = sin(60.0/180.0*M_PI);
    float c = cos(60.0/180.0*M_PI);


    const float Verts[] = { // pos n delta tag
      0.0f, 0.0f, 2.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,

      -c, s, 0.0f,      -c, s, 0.0f,        0.0f,  10.0f, 20.0f, 1.0f, -1.0f, 0.0f,
      c,  s, 0.0f,       c, s, 0.0f,        0.0f, 10.0f, 20.0f,  1.0, -1.0f, 0.0f,

      2.0*c, 0.0f, 0.0f, 1.0, 0.0f, 0.0f,   0.0f, 10.0f, 20.0f,  1.0f, -1.0f, 0.0f,

      c, -s, 0.0f,       c, -s, 0.0f,       0.0f,  0.0f, 20.0f, 1.0f, -1.0f, 0.0f,
      -c, -s, 0.0f,      -c, -s, 0.0f,      0.0f, 0.0f, 20.0f,   1.0f, -1.0f, 0.0f,

      -2*c, 0.0f, 0.0f,  -1.0, 0.0f, 0.0f,  0.0f, 0.0f, 20.0f,    1.0f, -1.0f, 0.0f,

      0.0f, 0.0f, -2.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
};
 
    IndexCount = sizeof(Faces) / sizeof(Faces[0]);
    std::cerr << IndexCount << std::endl;

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 12 * sizeof(float);
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(NormalSlot);
    glVertexAttribPointer(NormalSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 12));
    glEnableVertexAttribArray(DeltaSlot);
    glVertexAttribPointer(DeltaSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 24));
    glEnableVertexAttribArray(TagSlot);
    glVertexAttribPointer(TagSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 36));

    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
}


/*
static void CreateCube()
{
    const int Faces[] = {
        0, 1, 2,
        1, 3, 2,
        4, 0, 2,
        6, 4, 2,
	3, 6, 2,

	7, 1, 5,
	4, 7, 5,
	1, 4, 5,
	
	4, 1, 0,
	1, 7, 3,
	7, 6, 3,
	7, 4, 6
        };

    const float Verts[] = {
      -1.4f, 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 
      -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,   -1.0f, 1.0f, -1.0f, 
      1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 0.0f, 
      1.0f, 1.0f, -1.0f,   1.0f, 1.0f, -1.0f,   0.0f, 0.0f, 0.0f, 
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,  
      -1.0f, -1.0f, -1.0f,-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f,   1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
      1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 0.0f,  
};
 
    IndexCount = sizeof(Faces) / sizeof(Faces[0]);
    std::cerr << IndexCount << std::endl;

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 12 * sizeof(float);
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(NormalSlot);
    glVertexAttribPointer(NormalSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 12));
    glEnableVertexAttribArray(TagSlot);
    glVertexAttribPointer(TagSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 24));

    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
}


/*static void CreateIcosahedron()
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
      0.000f,  0.000f, 1.000f,          0.000f,  0.000f,  1.000f, 0.0f,  0.000f,  1.0f,
      0.894f,  0.000f,  0.447f,         0.894f,  0.000f,  0.447f, 0.000f,  0.000f,  1.000f, 
         0.276f,  0.851f,  0.447f,      0.276f,  0.851f,  0.447f, 0.000f,  0.000f, 0.000f,
	 -0.724f,  0.526f,  0.447f,  	 -0.724f,  0.526f,  0.447f, 0.000f,  0.000f, 0.000f,
	 -0.724f, -0.526f,  0.447f,  	 -0.724f, -0.526f,  0.447f, 0.000f,  0.000f, 0.000f,
         0.276f, -0.851f,  0.447f,           0.276f, -0.851f,  0.447f, 0.000f,  0.000f, 0.000f,
         0.724f,  0.526f, -0.447f,           0.724f,  0.526f, -0.447f, 0.000f,  0.000f, 0.000f,
	 -0.276f,  0.851f, -0.447f,  	 -0.276f,  0.851f, -0.447f, 0.000f,  0.000f, 0.000f,
	 -0.894f,  0.000f, -0.447f,  	 -0.894f,  0.000f, -0.447f, 0.000f,  0.000f, 0.000f,
	 -0.276f, -0.851f, -0.447f,  	 -0.276f, -0.851f, -0.447f, 0.000f,  0.000f, 0.000f,
         0.724f, -0.526f, -0.447f,            0.724f, -0.526f, -0.447f, 0.000f,  0.000f, 0.000f, 
      0.000f,  0.000f, -1.000f,            0.000f,  0.000f, -1.000f, 0.000f,  0.000f, 0.000f, };
 
    IndexCount = sizeof(Faces) / sizeof(Faces[0]);
    std::cerr << IndexCount << std::endl;

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 9 * sizeof(float);
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(NormalSlot);
    glVertexAttribPointer(NormalSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 12));
    glEnableVertexAttribArray(TagSlot);
    glVertexAttribPointer(TagSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 24));

    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
}
*/


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
    const char* msg = "Can't find %s shader.  Does '../Geodesic.glsl' exist?\n";
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
    glBindAttribLocation(ProgramHandle, TagSlot, "Tag");
    glBindAttribLocation(ProgramHandle, DeltaSlot, "Delta");
    glLinkProgram(ProgramHandle);
    glGetProgramiv(ProgramHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(ProgramHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(linkSuccess, "Shader Link Errors:\n%s", compilerSpew);

    glUseProgram(ProgramHandle);
}

void PezUpdate(unsigned int elapsedMicroseconds)
{

    Vector3 rot = V3MakeFromElems(0.0, camAngleY, camAngleX);
    Matrix4 rotation = M4MakeRotationZYX(rot);
 

    Matrix4 lookAt = M4MakeLookAt(eyePosition, targetPosition, upVector);

    ModelviewMatrix = M4Mul(lookAt, rotation);
    NormalMatrix = M4GetUpper3x3(ModelviewMatrix);

    const int VK_LEFT = 0x25;
    const int VK_UP = 0x26;
    const int VK_RIGHT = 0x27;
    const int VK_DOWN = 0x28;


    /*
    if (PezIsPressing(VK_RIGHT)) TessLevelInner++;
    if (PezIsPressing(VK_LEFT))  TessLevelInner = TessLevelInner > 1 ? TessLevelInner - 1 : 1;
    if (PezIsPressing(VK_UP))    TessLevelOuter++;
    if (PezIsPressing(VK_DOWN))  TessLevelOuter = TessLevelOuter > 1 ? TessLevelOuter - 1 : 1;
    */
}


void PezHandleKey(char key){
  bool refresh = false;

  //std::cerr << "handling: " << key << std::endl;
  switch(key){
    // Normalmodels
  case '0': // Vlacho
    NormalModel = 0.0;
    break;
  case '1': // Casteljau
    NormalModel = 1.0;
    break;
  case '2': // basic
    NormalModel = 2.0;
    break;

  case 'w':
    Verts[selID*12+6] += 0.1;
    refresh = true;
    break;

  case 's':
    Verts[selID*12+6] -= 0.1;
    refresh = true;
    break;

  case 'e':
    Verts[selID*12+7] += 0.1;
    refresh = true;
    break;

  case 'd':
    Verts[selID*12+7] -= 0.1;
    refresh = true;
    break;
  case 'r':
    Verts[selID*12+8] += 0.1;
    refresh = true;
    break;

  case 'f':
    Verts[selID*12+8] -= 0.1;
    refresh = true;
    break;

  case 't':
    std::cerr << Verts[selID*12+9] << " " ;
    Verts[selID*12+9] += 0.1;
    if (Verts[selID*12+9] > 1000){
      Verts[selID*12+9] = 1000;
    }else{
      refresh = true;
    }
    std:: cerr << Verts[selID*12+9] << std::endl;
    break;

  case 'g':
    std::cerr << Verts[selID*12+9] << " " ;
    Verts[selID*12+9] -= 0.1;
    if (Verts[selID*12+9] < 0.0){
      Verts[selID*12+9] = 0.0;
    }else{
      refresh = true;
    }
    std:: cerr << Verts[selID*12+9] << std::endl;
    break;

  case 'y':
    Verts[selID*12+10] += 0.1;
    if (Verts[selID*12+10] > 1){
      Verts[selID*12+10] = 1;
    }else{
      refresh = true;
    }
    std::cerr << "tag changed at " << selID << std::endl;

    break;

  case 'h':
    Verts[selID*12+10] -= 0.1;
    if (Verts[selID*12+10] < -1.0){
      Verts[selID*12+10] = -1.0;
    }else{
      refresh = true;
    }

    break;

  case 'u':
    Verts[selID*12+11] += 0.1;
    if (Verts[selID*12+11] > 1){
      Verts[selID*12+11] = 1;
    }else{
      refresh = true;
    }


    break;

  case 'j':
    Verts[selID*12+11] -= 0.1;
    if (Verts[selID*12+11] < -1.0){
      Verts[selID*12+11] = -1.0;
    }else{
      refresh = true;
    }

    std::cerr << "tag changed at " << selID << std::endl;

    break;


  case 'n':
    selID = (selID-1)%nv;
    std::cerr << "Selected vertex " << selID << std::endl;
    refresh = true;
    break;

  case 'm':
    selID = (selID+1)%nv;
    std::cerr << "Selected vertex " << selID << std::endl;
    refresh = true;
    break;

  case 'i': // wireframe on/off
    if (Wireframe>0.0) Wireframe = 0.0;
    else Wireframe = 1.0;
    break;
  case 'k': // tesselation on/off
    if (Tesselation>0.0) Tesselation = 0.0;
    else Tesselation = 1.0;
    break;
  case 'p': // tagged on/off
    if (Tagg>0.0) { Tagg = 0.0; std::cerr << "tag off" << std::endl;}
    else {Tagg = 1.0; std::cerr << "tag on" << std::endl;}
    break;
  case ';': // color on/off
    if (DCol>0.0) { DCol = 0.0; std::cerr << "tag off" << std::endl;}
    else {DCol = 1.0; std::cerr << "tag on" << std::endl;}
    break;
  case 'l': // color on/off
    if (ColNorm>0.0) { ColNorm = 0.0; std::cerr << "tag off" << std::endl;}
    else {ColNorm = 1.0; std::cerr << "tag on" << std::endl;}
    break;
   
  case '.':
    TessLevel++;
    break;
  case ',':
    TessLevel = TessLevel > 1 ? TessLevel - 1 : 1;
      break;
  default:
    std::cerr << "can't handle key: " << key << std::endl;
    
  }

  if (refresh){
    updateBuffer();
    //    LoadEffect();
  }
}


void updateBuffer(){
  float* buf = (float*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);


  for (int i = 0; i < Verts.size(); i++){
    buf[i] = Verts[i];
  }


  glUnmapBuffer(GL_ARRAY_BUFFER);
}

void refreshBuffer(bool newBuffer){
  int sFaces[Faces.size()];
  float sVerts[Verts.size()];
  
  for (int i = 0; i < Verts.size(); i++){
    if (i%12== 0)
      std::cerr << std::endl;

    sVerts[i] = Verts[i];
    std::cerr << sVerts[i]  << " " ;
  }

  for (int i = 0; i < Faces.size(); i++){
    sFaces[i] = Faces[i];
  }
    IndexCount = sizeof(sFaces)/sizeof(sFaces[0]);
    std::cerr << IndexCount << std::endl;
    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLsizei stride = 12 * sizeof(float);
    if (newBuffer){
      // Create the VBO for positions:
      glGenBuffers(1, &positions);
    }

    glBindBuffer(GL_ARRAY_BUFFER, positions);

    glBufferData(GL_ARRAY_BUFFER, sizeof(sVerts), sVerts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(NormalSlot);
    glVertexAttribPointer(NormalSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 12));
    glEnableVertexAttribArray(DeltaSlot);
    glVertexAttribPointer(DeltaSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 24));
    glEnableVertexAttribArray(TagSlot);
    glVertexAttribPointer(TagSlot, 3, GL_FLOAT, GL_FALSE, stride, ((char *)NULL + 36));
 
    if (newBuffer){
    // Create the VBO for indices:
      GLuint indices;
      glGenBuffers(1, &indices);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sFaces), sFaces, GL_STATIC_DRAW);
    }

}








void PezHandleMouse(int x, int y, int action)
{
  switch(action){
  case PEZ_LEFT_DOWN:
    mouseButton = 1;
    mouseX = x;
    mouseY = y;
    break;
case PEZ_RIGHT_DOWN:
    mouseButton = 2;
    mouseX = x;
    mouseY = y;
    break;
case PEZ_MIDDLE_DOWN:
    mouseButton = 3;
    mouseX = x;
    mouseY = y;
    break;
  case PEZ_UP:
    mouseButton = 0;
    break; 
  case PEZ_MOVE:
    if(mouseButton == 1){ // rotate
      // update angle with relative movement
      camAngleX = fmod(camAngleX + (x-mouseX)*mouseSensitivy,360.0f);
      camAngleY -= (y-mouseY)*mouseSensitivy;
      // limit y angle by 85 degree
      if (camAngleY > 85) camAngleY = 85;
      if (camAngleY < -85) camAngleY = -85;
    }else if (mouseButton == 2){ // zoom
      Vector3 mv = V3MakeFromElems(0.0,0.0,0.1*(y-mouseY)*mouseSensitivy);
      eyePosition = P3SubV3(eyePosition,mv);
    }else if (mouseButton == 3){
      // update camPos
      Vector3 mv = V3MakeFromElems(0.1*mouseSensitivy*(x-mouseX),
				   0.1*mouseSensitivy*(y-mouseY),
				   0.0);
      eyePosition = P3AddV3(eyePosition,mv);
    }
    break;
  }
  
  



}
