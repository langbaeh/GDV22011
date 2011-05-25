// ========================================================================= //
// Author: Thomas Kalbe                                                      //
// mailto:thomas.kalbe@gris.informatik.tu-darmstadt.de                      //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Creation Date: 19.05.2011                                                 //
// ========================================================================= //
#ifndef PNTRIANGLES_DEFINED
#define PNTRIANGLES_DEFINED

#include <GL/glew.h>
#include <vector>
#include "Vec3.h"
#include "config.h"


class PNTriangles {

  typedef Vec3f Vertex;
  typedef Vec3f Normal;
  typedef Vec3i Triangle;

  typedef std::vector<Triangle> Triangles;
  typedef std::vector<Normal> Normals;
  typedef std::vector<Vertex> Vertices;

  void addVertex(int u, int v, int w, int q);
  void regularPatch(int level);

public:
  PNTriangles() { depth = 4; }

  void init();
  void draw();

  void setDepth(int d)   {  depth = d;  }
  void decreaseDepth() { if (depth >0) --depth; }
  void increaseDepth() { if (depth < maxdepth-1) ++depth; }

  void setMesh(Vertices &verts, Normals &norms, Triangles &tris);

  int depth;
  const static int maxdepth = 10;
  GLuint DLids[maxdepth];

  Vertices vertices;
  Normals normals;
  Triangles triangles;

};


#endif