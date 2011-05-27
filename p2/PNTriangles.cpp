#include "PNTriangles.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

GLuint programID;   // id of used shader program

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::cout << infoLog << std::endl;
		free(infoLog);
	}
}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		std::cout << infoLog << std::endl;
		free(infoLog);
	}
}

int readShaders(char* vertexShaderPath, char* fragmentShaderPath)
{
  //  std::cerr << vertexShaderPath << std::endl;
	// read vertex shader if possible
	std::ifstream vin(vertexShaderPath, std::ifstream::in | std::ios::binary);
	vin.seekg(0, std::ios::end);
	int lengthV = vin.tellg();
	vin.seekg(0, std::ios::beg);
	if (lengthV < 0) { std::cout << "readShaders(): no vertex shader found!" << std::endl; return 0; }
	char *vsource = new char[lengthV+1];
	vin.read(vsource, lengthV);
	vin.close();
	vsource[lengthV] = 0; // terminate char array

	// read fragment shader if possible
	std::ifstream fin(fragmentShaderPath, std::ifstream::in | std::ios::binary);
	fin.seekg(0, std::ios::end);
	int lengthF = fin.tellg();
	fin.seekg(0, std::ios::beg);
	if (lengthF < 0) { std::cout << "readShaders(): no fragment shader found!" << std::endl; return 0; }
	char *fsource = new char[lengthF+1];
	fin.read(fsource, lengthF);
	fin.close();
	fsource[lengthF] = 0; // terminate char array

	// create shaders, set source and compile
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsource, 0);
	glShaderSource(fragmentShader, 1, (const char **)&fsource, 0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// create a program, attach both shaders and link the program (shaders can be deleted now)
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// check of all was successfull and return the programID
	GLint success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		std::cout << "===== Vertex Shader =====" << std::endl << vsource << std::endl;
		std::cout << "===== Vertex Shader Info Log =====" << std::endl;
		printShaderInfoLog(vertexShader);
		std::cout << "===== Fragment Shader =====" << std::endl << fsource << std::endl;
		std::cout << "===== Fragment Shader Info Log =====" << std::endl;
		printShaderInfoLog(fragmentShader);
		std::cout << "===== Program Info Log =====" << std::endl;
		printProgramInfoLog(program);
		glDeleteProgram(program);
		program = 0;
	}
	return program;
}


void PNTriangles::addVertex(int u, int v, int w, int q)
{
  glMultiTexCoord3f(6,(float)u/q, (float)v/q,(float)w/q);
  glVertex3f((float)u/q, (float)v/q, (float)w/q);
  tessels.push_back(Tessel((float)u/q, (float)v/q,(float)w/q));
}

void PNTriangles::regularPatch(int level)
{
  int q = level;

  for (int l = q; l > 0; --l)
  {
    int u = l;
    int w = q-l;

    for (int k=0; k<2*l-1; ++k)
    {
      if (k%2==0) /* even triangle */
      {
        addVertex(u,  q-u-w,  w,q);
        addVertex(u-1,q-u-w+1,w,q);
        addVertex(u-1,q-u-w,  w+1,q);
        --u;
      }
      else  /* odd triangle */
      {
        addVertex(u,  q-u-w,  w,q);
        addVertex(u-1,q-u-w,  w+1,q);
        addVertex(u,  q-u-w-1,w+1,q);
      }
    }
  }
}

int PNTriangles::changeShaders(char* vertexShaderPath, char* fragmentShaderPath){
  programID = readShaders(vertexShaderPath, fragmentShaderPath);
  return programID;
}

void PNTriangles::init()
{
  // initialize shader programs
  programID = readShaders((char*)VERTEXSHADERPATH, (char*)FRAGMENTSHADERPATH);
  // build pre-tesselated display lists
  for (int i = 0; i < maxdepth; ++i)
    {
      DLids[i] = glGenLists(1);
      
      glNewList(DLids[i], GL_COMPILE);
      glBegin(GL_TRIANGLES);
      regularPatch(i+1);
      glEnd();
      glEndList();
      
    }
  
}

void PNTriangles::draw()
{
  //  std:: cerr<< "drawing " << programID << std::endl;
	glUseProgram(programID);
	GLuint VertexVBOID;
	glGenBuffers(1, &VertexVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);

	GLuint TesselVBOID;
	glGenBuffers(1, &TesselVBOID);
	glBindBuffer(GL_ARRAY_BUFFER, TesselVBOID);

	PNVertex pvertices[triangles.size()*3];
	//	ushort pindices[triangles.size()*3];

	Triangles::iterator it = triangles.begin();
	int i =0;
	while (it != triangles.end())
	{

		Vec3f &p0 = vertices[(*it).x];
		Vec3f &p1 = vertices[(*it).y];
		Vec3f &p2 = vertices[(*it).z];

		Vec3f &n0 = normals[(*it).x];
		Vec3f &n1 = normals[(*it).y];
		Vec3f &n2 = normals[(*it).z];
		++it;

		// insert triangles into VBO
		pvertices[i*3].x0 = p0[0];
		pvertices[i*3].y0 = p0[1];
		pvertices[i*3].z0 = p0[2];

		pvertices[i*3].nx0 = n0[0];
		pvertices[i*3].ny0 = n0[1];
		pvertices[i*3].nz0 = n0[2];

		pvertices[i*3+1].x1 = p1[0];
		pvertices[i*3+1].y1 = p1[1];
		pvertices[i*3+1].z1 = p1[2];

		pvertices[i*3+1].nx1 = n1[0];
		pvertices[i*3+1].ny1 = n1[1];
		pvertices[i*3+1].nz1 = n1[2];

		pvertices[i*3+2].x2 = p2[0];
		pvertices[i*3+2].y2 = p2[1];
		pvertices[i*3+2].z2 = p2[2];

		pvertices[i*3+2].nx2 = n2[0];
		pvertices[i*3+2].ny2 = n2[1];
		pvertices[i*3+2].nz2 = n2[2];
		
		//pindices[i*3] = i*3;
		//pindices[i*3+1] = i*3+1;
		//pindices[i*3+2] = i*3+2;

		
		++i;
		//		glMultiTexCoord3fv(0,&p0[0]);
		//		glMultiTexCoord3fv(1,&p1[0]);
		//		glMultiTexCoord3fv(2,&p2[0]);

		//		glMultiTexCoord3fv(3,&n0[0]);
		// glMultiTexCoord3fv(4,&n1[0]);
		//		glMultiTexCoord3fv(5,&n2[0]);

		//		glCallList(DLids[depth]);
		
	}
	int numT = triangles.size()*3;
	int sizeVertexBuf = sizeof(PNVertex)*numT;
	glBufferData(GL_ARRAY_BUFFER, sizeVertexBuf, &pvertices[0].x0, GL_STATIC_DRAW);


	//	int sizeIndexBuf = sizeof(ushort)*numT;
	//glBufferData(GL_ARRAY_BUFFER, sizeIndexBuf, pindices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glEnableVertexAttribArray(0); // submit positions on stream 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PNVertex), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1); // submit normals on stream 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PNVertex), BUFFER_OFFSET(12));
	glEnableVertexAttribArray(2); // submit normals on stream 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(PNVertex), BUFFER_OFFSET(24));
	glEnableVertexAttribArray(3); // submit normals on stream 3
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(PNVertex), BUFFER_OFFSET(36));
	glEnableVertexAttribArray(4); // submit normals on stream 4
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(PNVertex), BUFFER_OFFSET(48));
	glEnableVertexAttribArray(5); // submit normals on stream 5
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(PNVertex), BUFFER_OFFSET(60));


	glBindAttribLocation(programID, 0, "p0");
	glBindAttribLocation(programID, 1, "p1");
	glBindAttribLocation(programID, 2, "p2");
	glBindAttribLocation(programID, 3, "n0");
	glBindAttribLocation(programID, 4, "n1");
	glBindAttribLocation(programID, 5, "n2");


	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
	glDrawArrays(GL_TRIANGLES, 0, numT);

	int sizeTesselBuf = sizeof(Tessel)*tessels.size();
	glBufferData(GL_ARRAY_BUFFER, sizeTesselBuf, &tessels[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, TesselVBOID);

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Tessel), BUFFER_OFFSET(0) );
	glVertexAttribDivisor(6, 1);
	glBindAttribLocation(programID, 6, "phi");	
	
	glDrawArraysInstanced(GL_TRIANGLES,0, numT, tessels.size());

	glUseProgram(0);


	//	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);

	// draw domain subdiv triangle
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	glCallList(DLids[depth]);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void PNTriangles::setMesh(Vertices &verts, Normals &norms, Triangles &tris) {

	vertices.clear();
	vertices.resize(verts.size());
	normals.clear();
	normals.resize(norms.size());
	triangles.clear();
	triangles.resize(tris.size());

	memcpy(&vertices[0], &verts[0], 3*verts.size()*sizeof(GLfloat));
	memcpy(&normals[0], &norms[0], 3*norms.size()*sizeof(GLfloat));
	memcpy(&triangles[0], &tris[0], 3*tris.size()*sizeof(int));
}
