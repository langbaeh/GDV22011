cmake_minimum_required(VERSION 2.6)
project(PRAK1PNTriangles)

FIND_PACKAGE(GLUT)
FIND_PACKAGE(OpenGL)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h"
  )
  
SOURCE_GROUP(Shaders FILES PNShader.vert PNShader.frag PNShaderCast.vert PNShaderVlacho.vert )

include_directories( ${PROJECT_SOURCE_DIR})

add_executable(main main.cpp main.h Vec3.h TriangleMesh.h TriangleMesh.cpp PNTriangles.h PNTriangles.cpp PNShaderCast.vert PNShader.vert PNShader.frag PNShaderVlacho.vert )

IF (WIN32)
  set(GLEW_LIBRARY GLEW32)
ELSE (WIN32)
  set(GLEW_LIBRARY GLEW)
ENDIF(WIN32)

set(CORELIBS ${GLEW_LIBRARY} ${GLUT_LIBRARY} ${OPENGL_LIBRARIES})

target_link_libraries(main ${CORELIBS})
