


-- Vertex

in vec3 Position;
in vec3 Normal;
out vec3 vPosition;
out vec3 vNormal;

void main()
{
    vPosition = Position.xyz;
    vNormal = Normal.xyz;

}

-- TessControl

layout(vertices = 3) out;
in vec3 vPosition[];
in vec3 vNormal[];
out vec3 tcPosition[];
out vec3 tcNormal[];
uniform float TessLevelInner;
uniform float TessLevelOuter;

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = vPosition[ID];
    tcNormal[ID] = vNormal[ID];
       


    int m = (ID%3);
    int i = ID-m;	

    float d01 = sqrt(1.0-(dot(tcNormal[i], tcNormal[i+1])+1.0)/2.0);
    float d02 = sqrt(1.0-(dot(tcNormal[i], tcNormal[i+2])+1.0)/2.0);
    float d12 = sqrt(1.0-(dot(tcNormal[i+1], tcNormal[i+2])+1.0)/2.0);


       gl_TessLevelOuter[2] = ceil(d01*10);
       gl_TessLevelOuter[0] = ceil(d12*10);
       gl_TessLevelOuter[1] = ceil(d02*10);


    if (ID == 0) {
        gl_TessLevelInner[0] = max(gl_TessLevelOuter[2],max(gl_TessLevelOuter[1],gl_TessLevelOuter[0]));    
    }
}

-- TessEval

layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[];
in vec3 tcNormal[];
out vec3 tePosition;
out vec3 tePatchDistance;
out vec3 teNormal;
uniform mat4 Projection;
uniform mat4 Modelview;

void main()
{

    vec3 p0 = tcPosition[0];
    vec3 p1 = tcPosition[1];
    vec3 p2 = tcPosition[2];

    vec3 n0 = tcNormal[0];
    vec3 n1 = tcNormal[1];
    vec3 n2 = tcNormal[2];

    vec3 phi = gl_TessCoord.xyz;

	vec3 P01 = 1.0/3.0*(2.0*p0 + p1);
	vec3 P10 = 1.0/3.0*(2.0*p1 + p0);

	vec3 P12 = 1.0/3.0*(2.0*p1 + p2);
	vec3 P21 = 1.0/3.0*(2.0*p2 + p1);

	vec3 P20 = 1.0/3.0*(2.0*p2 + p0);
	vec3 P02 = 1.0/3.0*(2.0*p0 + p2);

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

	tePosition = phi.x*b100 + phi.y*b010 + phi.z*b001;


// Vlacho
	vec3 n200f = n0*phi.x*phi.x;
	vec3 n020f = n1*phi.y*phi.y;
	vec3 n002f = n2*phi.z*phi.z;

	vec3 p1p0 = p1-p0;
	vec3 p2p0 = p2-p0;
	vec3 p2p1 = p2-p1;
	
	vec3 n110f = phi.x*phi.y*(n0+n1-2.0* (dot(p1p0, n0+n1)/dot(p1p0,p1p0))* p1p0);
	vec3 n101f = phi.x*phi.z*(n0+n2-2.0* (dot(p2p0, n0+n2)/dot(p2p0,p2p0))* p2p0);
	vec3 n011f = phi.y*phi.z*(n1+n2-2.0* (dot(p2p1, n1+n2)/dot(p2p1,p2p1))* p2p1);


	teNormal = normalize(n200f+n020f+n002f+n110f+n101f+n011f);
// castelau

	vec3 w = (b001-b100);
	vec3 z = (b010-b100);


	teNormal = normalize(cross(z,w));

    tePatchDistance = gl_TessCoord;


    gl_Position = Projection * Modelview * vec4(tePosition, 1);

}

-- Geometry

uniform mat4 Modelview;
uniform mat3 NormalMatrix;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec3 tePosition[3];
in vec3 tePatchDistance[3];
in vec3 teNormal[3];
out vec3 gPatchDistance;
out vec3 gTriDistance;
out vec3 gNormal;

void main()
{
//    vec3 A = tePosition[2] - tePosition[0];
//    vec3 B = tePosition[1] - tePosition[0];

//    gFacetNormal = NormalMatrix * normalize(cross(A, B));

//    gFacetNormal = NormalMatrix * teNormal[1];
    
    gPatchDistance = tePatchDistance[0];
    gTriDistance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position; 
    gNormal = NormalMatrix *teNormal[0]; 	
    EmitVertex();
	
    gPatchDistance = tePatchDistance[1];
    gTriDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position;
    gNormal = NormalMatrix *teNormal[1]; 	
    EmitVertex();

    gPatchDistance = tePatchDistance[2];
    gTriDistance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position;
    gNormal = NormalMatrix *teNormal[2]; 	
    EmitVertex();

    EndPrimitive();
}

-- Fragment

out vec4 FragColor;
in vec3 gNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in float gPrimitive;
uniform vec3 LightPosition;
uniform vec3 DiffuseMaterial;
uniform vec3 AmbientMaterial;
const bool DrawLines = true;
const vec3 InnerLineColor = vec3(1, 1, 1);

float amplify(float d, float scale, float offset)
{
    d = scale * d + offset;
    d = clamp(d, 0, 1);
    d = 1 - exp2(-2*d*d);
    return d;
}

void main()
{
    vec3 N = normalize(gNormal);
    vec3 L = LightPosition;
    float df = abs(dot(N, L));
    vec3 color = AmbientMaterial + df * DiffuseMaterial;

if (DrawLines) {
        float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
        float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
        d1 = 1 - amplify(d1, 50, -0.5);
        d2 = amplify(d2, 50, -0.5);
        color = d2 * color + d1 * d2 * InnerLineColor;
    }

 


    FragColor = vec4(color, 1.0);
}

