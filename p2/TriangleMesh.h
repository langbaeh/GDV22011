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
#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <vector>
#include "Vec3.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <fstream>
#include <iostream>


class TriangleMesh  {

  void calculateNormals();

public:
  typedef Vec3i Triangle;
  typedef Vec3f Normal;
  typedef Vec3f Vertex;

  typedef std::vector<Triangle> Triangles;
  typedef std::vector<Normal> Normals;
  typedef std::vector<Vertex> Vertices;

  TriangleMesh();
  ~TriangleMesh();

  void load(std::istream&);
  void clear();
  void draw();
  void drawOutlined();

  void flipNormals();

  Vertices vertices;
  Normals normals;
  Triangles triangles;

};


#endif

