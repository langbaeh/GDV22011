#include "TriangleMesh.h"


TriangleMesh::TriangleMesh()
{
}

TriangleMesh::~TriangleMesh()
{
  clear();
}

void TriangleMesh::load(std::istream& in)
{

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

  clear();

  // make room for vertices
  vertices.resize(nv);

  Vec3f vmin(10e6);
  Vec3f vmax(-10e6);

  for (int i = 0; i < nv; ++i)
  {
    in >> vertices[i][0];
    in >> vertices[i][1];
    in >> vertices[i][2];
    vmin[0] = std::min(vertices[i][0], vmin[0]);
    vmin[1] = std::min(vertices[i][1], vmin[1]);
    vmin[2] = std::min(vertices[i][2], vmin[2]);

    vmax[0] = std::max(vertices[i][0], vmax[0]);
    vmax[1] = std::max(vertices[i][1], vmax[1]);
    vmax[2] = std::max(vertices[i][2], vmax[2]);

    if (noff)
    {
      in >> normals[i][0];
      in >> normals[i][1];
      in >> normals[i][2];
    }
  }

  Vec3f mp = (vmin + vmax)*0.5;
  vmax -= mp;
  float maxp = std::max(vmax[0],vmax[1]);
  maxp = std::max(maxp,vmax[2]);
  for (int i = 0; i < nv; ++i)
  {
    vertices[i] -= mp;
    vertices[i] /= maxp;
  }

  triangles.resize(nf);

  for (int i = 0; i < nf; ++i)
  {
    int three;
    in >> three;
    in >> triangles[i][0];
    in >> triangles[i][1];
    in >> triangles[i][2];
  }

  if (!noff)
	  calculateNormals();
}

void TriangleMesh::clear()
{
  vertices.clear();
  triangles.clear();
  normals.clear();
}


void TriangleMesh::draw()
{
  if (triangles.size() == 0) return;

  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  glVertexPointer(3, GL_FLOAT,0, &vertices[0]);
  glNormalPointer(GL_FLOAT,0, &normals[0]);
  glDrawElements(GL_TRIANGLES, 3*triangles.size(), GL_UNSIGNED_INT, &triangles[0]);

  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

}

void TriangleMesh::drawOutlined()
{
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0,1.0);
  draw();
  glDisable(GL_POLYGON_OFFSET_FILL);

  glDisable(GL_LIGHTING);
  glColor3f(0,0,0);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  draw();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glEnable(GL_LIGHTING);

}

void TriangleMesh::calculateNormals()
{
  normals.resize(vertices.size());

  for (Triangles::iterator it = triangles.begin(); it != triangles.end(); ++it)
  {
    Vec3f vec1, vec2, normal;
    unsigned int id0, id1, id2;
    id0 = (*it)[0];
    id1 = (*it)[1];
    id2 = (*it)[2];

    vec1 = vertices[id1] - vertices[id0];
    vec2 = vertices[id2] - vertices[id0];

    normal = vec1 ^ vec2; // cross product

    normals[id0] += normal;
    normals[id1] += normal;
    normals[id2] += normal;
  }

  // Normalize normals.
  for (Normals::iterator nit=normals.begin(); nit!=normals.end(); ++nit) {
    (*nit).normalize();
  }
}

void TriangleMesh::flipNormals()
{
  for (Normals::iterator it = normals.begin(); it != normals.end(); ++it) {
    (*it) *= -1.0;
  }
}
