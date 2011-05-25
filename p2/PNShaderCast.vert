// ========================================================================= //
// Author: Thomas Kalbe                                                      //
// mailto:thomas.kalbe@gris.informatik.tu-darmstadt.de                      //
// GRIS - Graphisch Interaktive Systeme                                      //
// Technische Universität Darmstadt                                          //
// Fraunhoferstrasse 5                                                       //
// D-64283 Darmstadt, Germany                                                //
//                                                                           //
// Creation Date: 19.05.2011                                                 //
//                                                                           //
// PN-Triangles Shader, based on "Curved PN Triangles" by Vlachos, Peters,   //
//                                Boyd and Mitchell                          //
// http://alex.vlachos.com/graphics/CurvedPNTriangles.pdf                    //
// ========================================================================= //
varying vec3 Normal;
varying vec3 viewDir;

void main()
{

	vec3 phi = vec3(gl_MultiTexCoord6.xyz);
	
	vec3 p0 = gl_MultiTexCoord0.xyz;
	vec3 p1 = gl_MultiTexCoord1.xyz;
	vec3 p2 = gl_MultiTexCoord2.xyz;

	vec3 n0 = gl_MultiTexCoord3.xyz;
	vec3 n1 = gl_MultiTexCoord4.xyz;
	vec3 n2 = gl_MultiTexCoord5.xyz;

	// first step, 6 control points
	vec3 P01 = 1.0/3.0*(2.0*p0 + p1);
	vec3 P10 = 1.0/3.0*(2.0*p1 + p0);

	vec3 P12 = 1.0/3.0*(2.0*p1 + p2);
	vec3 P21 = 1.0/3.0*(2.0*p2 + p1);

	vec3 P20 = 1.0/3.0*(2.0*p2 + p0);
	vec3 P02 = 1.0/3.0*(2.0*p0 + p2);


	// second step, 3 control points


	vec3 b210 = P01 - dot(P01 - p0,n0)*n0;
	vec3 b120 = P10 - dot(P10 - p1,n1)*n1;

	vec3 b021 = P12 - dot(P12 - p1,n1)*n1;
	vec3 b012 = P21 - dot(P21 - p2,n2)*n2;

	vec3 b102 = P20 - dot(P20 - p2,n2)*n2;
	vec3 b201 = P02 - dot(P02 - p0,n0)*n0;

	vec3 b111 = 0.25*(b210 + b120 + b201 + b102 + b021 + b012) - 1.0/6.0*(p0 + p1 + p2);


	vec3 b200 = phi.x*p0   + phi.y*b210 + phi.z*b201;
	vec3 b110 = phi.x*b210 + phi.y*b120 + phi.z*b111;
	vec3 b101 = phi.x*b201 + phi.y*b111 + phi.z*b102;

	vec3 b020 = phi.x*b120 + phi.y*p1   + phi.z*b021;
	vec3 b011 = phi.x*b111 + phi.y*b021 + phi.z*b012;

	vec3 b002 = phi.x*b102 + phi.y*b012 + phi.z*p2;


	vec3 b100 = phi.x*b200 + phi.y*b110 + phi.z*b101;
	vec3 b010 = phi.x*b110 + phi.y*b020 + phi.z*b011;
	vec3 b001 = phi.x*b101 + phi.y*b011 + phi.z*b002;

	vec3 pos = phi.x*b100 + phi.y*b010 + phi.z*b001;

	// "Normalenmodell" 
//	vec3 n110 = normalize(n0 + n1);
//	vec3 n101 = normalize(n0 + n2);
//	vec3 n011 = normalize(n1 + n2);

	vec3 w = (b001-b100);
	vec3 z = (b010-b100);

//	vec3 n100 = phi.x*n0   + phi.y*n110 + phi.z*n101;
//	vec3 n010 = phi.x*n110 + phi.y*n1   + phi.z*n011;
//	vec3 n001 = phi.x*n101 + phi.y*n011 + phi.z*n2;


	Normal = normalize(cross(z,w));

	Normal = gl_NormalMatrix*Normal;
	viewDir = gl_ModelViewMatrixInverse[3].xyz;

	gl_Position =  gl_ModelViewProjectionMatrix * vec4(pos,1.0);

}