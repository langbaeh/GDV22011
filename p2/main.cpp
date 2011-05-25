// ========================================================================= //
// Author: Matthias Bein                                                     //
// mailto:matthias.bein@gris.informatik.tu-darmstadt.de                      //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Creation Date: 26.051.2010                                                 //
// ========================================================================= //

#include "main.h"
#include <fstream>
#include <iostream>


// Window size information
float wRatio;
int wSizeH=600, wSizeW=400;
GLdouble near = 0.1, far = 30;
// Camera information
Vec3f camPos(0.0f, 2.0f, -10.0f);       // camera position
Vec3f camDir(0.0f, 0.0f, 1.0f);         // camera lookat (always Z)
Vec3f camUp(0.0f, 1.0f, 0.0f);          // camera up direction (always Y)
float camAngleX=0.0f, camAngleY=0.0f;   // camera angles
// Light information
Vec3f lightPos(10.0f, 30.0f, -10.0f);
// Mouse information
int mouseX, mouseY, mouseButton;
float mouseSensitivy = 1.0f;
// Button information
bool b_r = false;

bool showPN = true;

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(wSizeH,wSizeW);
  glutCreateWindow("PN Triangles");
  
  glutIgnoreKeyRepeat(1);
  glutKeyboardFunc(keyPressed);
  glutMouseFunc(mousePressed);
  glutMotionFunc(mouseMoved);
  
  glutDisplayFunc(renderScene);
  glutReshapeFunc(changeSize);
  
  initialize();

  std::cout << "P(oint)N(ormal) Triangle demonstration (using vertex / fragment shaders)\n";
  std::cout << "Usage:\nesc: exit program\n  -: decrease LOD\n  +: increase LOD \n  x: toggle standard / PN-Triangle rendering\n";
  std::cout << "1: default shaders\n 2: casteljau shaders \n 3: vlachos shaders\n\n";
  std::cout << "mouse left: rotate\nmouse middle: move (pan)\nmouse right: zoom" << std::endl;

  glutMainLoop();
  
  return 0;
}

void initialize() {

  glewInit();
  if (!GLEW_ARB_vertex_shader || !GLEW_ARB_fragment_shader) 
  {
    std::cout << "Shaders are not supported or not ready!" << std::endl;
    exit(1);
  }
  glClearColor(1.0,1.0,1.0,0.0);
  // enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // shading model
  glShadeModel(GL_SMOOTH);  // GL_SMOOTH, GL_FLAT
  // size window
  changeSize(wSizeH,wSizeW);
  // lighting is used in this exercise
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  // load a triangle mesh 
  //  std::ifstream vin(MESHPATH);  
  mesh.load(std::cin);
  pntriangles.init();
  pntriangles.setMesh(mesh.vertices, mesh.normals, mesh.triangles); 
  std::cerr << "Read " << mesh.vertices.size() << " vertices " << std::endl;
  std::cerr << "Read " << mesh.triangles.size() << " triangles " << std::endl;
}

void changeSize(int w, int h) {
  // Prevent a division by zero, when window is too short
  if(h == 0) h = 1;
  float wRatio = 1.0* w / h;
  // Reset the coordinate system before modifying
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();	
  // Set the viewport to be the entire window
  glViewport(0, 0, w, h);
  // Set the correct perspective.
  gluPerspective(45,wRatio,near,far);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(camPos.x,             camPos.y,             camPos.z,               // Position
            camPos.x + camDir.x,  camPos.y + camDir.y,  camPos.z + camDir.z,    // Lookat
	    camUp.x,              camUp.y,              camUp.z);               // Up-direction
}

// Rendering

void renderScene(void) {
  // clear the screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  // apply camPos before rotation
  glLoadIdentity();  
  gluLookAt(camPos.x,             camPos.y,             camPos.z,               // Position
            camPos.x + camDir.x,  camPos.y + camDir.y,  camPos.z + camDir.z,    // Lookat
	    camUp.x,              camUp.y,              camUp.z);               // Up-direction
  // apply rotation
  glRotatef(camAngleX,0,1,0); // window x axis rotates around up vector
  glRotatef(camAngleY,1,0,0); // window y axis rotates around x
  
  glColor3f(1.0,0.0,0.0);

  if (showPN)
    pntriangles.draw();
  else 
    mesh.drawOutlined();

  // swap Buffers
  glFlush();
  glutSwapBuffers();
}



// Callbacks

void keyPressed(unsigned char key, int x, int y) {
  int ret = 0;
  switch (key) {
    // esc => exit
  case 27:
    exit(0);
    break;
    // v => reset view
  case 'v':
  case 'V':
    camPos.set(0.0f, 2.0f, -10.0f);
  camAngleX = 180.0f;
  camAngleY = 0.0f;
  break;
  case '+':
    pntriangles.increaseDepth();
    break;
  case '-':
    pntriangles.decreaseDepth();
    break;
  case 'x':
    showPN = !showPN;
    break;
  case '1':
    ret = pntriangles.changeShaders((char*)VERTEXSHADERPATH, (char*)FRAGMENTSHADERPATH);
    std::cerr << key << std::endl;
    std::cerr << ret << std::endl;
    break;
  case '2':
     ret = pntriangles.changeShaders((char*)"/gris/gris-f/homestud/skoch/workspace/uni/GDV22011/p2/PNShaderCast.vert", (char*) FRAGMENTSHADERPATH);
    if (ret == 0){
      pntriangles.changeShaders((char*)VERTEXSHADERPATH, (char*)FRAGMENTSHADERPATH);
    }
    std::cerr << key << std::endl;
    std::cerr << ret << std::endl;
    break;
  }
  glutPostRedisplay();
}

void mousePressed(int button, int state, int x, int y) {
  switch(button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        mouseButton = 1;
        mouseX = x;
        mouseY = y;
      }
      else mouseButton = 0;
      break;
    case GLUT_RIGHT_BUTTON:
      if (state == GLUT_DOWN) {
        mouseButton = 2;
        mouseX = x;
        mouseY = y;
      }
      else mouseButton = 0;
      break;
    case GLUT_MIDDLE_BUTTON:      
      if (state == GLUT_DOWN) {
        mouseButton = 3;
        mouseX = x;
        mouseY = y;
      }
      else mouseButton = 0;
      break;
  }
}

void mouseMoved(int x, int y) { 
  switch(mouseButton) {
  // 1 => rotate
  case 1:
    // update angle with relative movement
    camAngleX = fmod(camAngleX + (x-mouseX)*mouseSensitivy,360.0f);
    camAngleY -= (y-mouseY)*mouseSensitivy;
    // limit y angle by 85 degree
    if (camAngleY > 85) camAngleY = 85;
    if (camAngleY < -85) camAngleY = -85;
    break;
  // 2 => zoom
  case 2: 
    camPos -= Vec3f(0.0f,0.0f,0.1f)*(y-mouseY)*mouseSensitivy;
    break;
  // 3 => translate 
  case 3:
    // update camPos
    camPos += Vec3f(0.1f,0.0f,0.0f)*(x-mouseX)*mouseSensitivy;
    camPos += Vec3f(0.0f,0.1f,0.0f)*(y-mouseY)*mouseSensitivy;
    break;
  default: break;
  }  
  // update mouse for next relative movement
  mouseX = x;
  mouseY = y;
  // redraw if mouse moved, since there is no idleFunc defined
  glutPostRedisplay();
}
