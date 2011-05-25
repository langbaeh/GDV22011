// ========================================================================= //
// Author: Matthias Bein                                                     //
// mailto:matthias.bein@gris.informatik.tu-darmstadt.de                      //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universit�t Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Creation Date: 26.05.2010                                                 //
// ========================================================================= //

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <TriangleMesh.h>
#include <PNTriangles.h>
// Basics

int main(int argc, char **argv);

void initialize();

void changeSize(int w, int h);

// Rendering

void renderScene(void);

// Callbacks

void keyPressed(unsigned char key, int x, int y);

void mousePressed(int button, int state, int x, int y);

void mouseMoved(int x, int y);

TriangleMesh mesh; 
PNTriangles pntriangles;
