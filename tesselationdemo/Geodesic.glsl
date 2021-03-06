


-- Vertex

in vec3 Position;
in vec3 Normal;
in vec3 Tag;
in vec3 Delta;
out vec3 vPosition;
out vec3 vNormal;
out vec3 vTag;
out vec3 vDelta;
void main()
{
    vPosition = Position.xyz;
    vNormal = Normal.xyz;
    vTag = Tag.xyz;
    vDelta = Delta.xyz;
}

-- TessControl

layout(vertices = 3) out;
in vec3 vPosition[];
in vec3 vNormal[];
in vec3 vDelta[];
in vec3 vTag[];

out vec3 tcPosition[];
out vec3 tcNormal[];
out vec3 tcDelta[];
out vec3 tcNormalMinus[];
out vec3 tcTag[];
out vec3 tcColor[];

uniform float TessLevel;
uniform float TessLevelOuter;
uniform float Tesselation;
uniform float Tagg;
uniform float DCol;

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = vPosition[ID];
    tcNormal[ID] = vNormal[ID];
    tcDelta[ID] = vDelta[ID];
    tcNormalMinus[ID] = vec3(0.0,1.0,0.0);

    tcColor[ID] = vec3(1.0,1.0,1.0);

    tcTag[ID] = vTag[ID];

    int m = (ID%3);
    int i = ID-m;
    
    tcNormal[ID] = normalize(tcNormal[ID]);
    
    if (Tesselation > 0.0){ 
    
    float d01 = sqrt(1.0-(dot(vNormal[i], vNormal[i+1])+1.0)/2.0);
    float d02 = sqrt(1.0-(dot(vNormal[i], vNormal[i+2])+1.0)/2.0);
    float d12 = sqrt(1.0-(dot(vNormal[i+1], vNormal[i+2])+1.0)/2.0);

       gl_TessLevelOuter[2] = ceil(d01*TessLevel);
       gl_TessLevelOuter[0] = ceil(d12*TessLevel);
       gl_TessLevelOuter[1] = ceil(d02*TessLevel);
      gl_TessLevelInner[0] = max(gl_TessLevelOuter[2],max(gl_TessLevelOuter[1],gl_TessLevelOuter[0]));    
     }else{
	gl_TessLevelOuter[0] = 1;
	gl_TessLevelOuter[1] = 1;
	gl_TessLevelOuter[2] = 1;
	gl_TessLevelInner[0] = 1;
    }
    
    if (Tagg > 0.0){
    
    if (length(vTag[ID]) > 0.000001){ 
     
       // vertex is tagged !
       int n = ((m+1)%3);
       int p = ((m+2)%3);

       // compute face normals
       vec3 fn = normalize(vNormal[i]+ vNormal[i+1]+ vNormal[i+2]);

       // compute dot product between face normals and delta
       float dp = dot(fn, vDelta[ID]);
       if (dp < 0.0){
       if (DCol> 0.0)
           tcColor[ID] = vec3(0.0,0.0,1.0);
       	  tcNormalMinus[ID] = vNormal[ID]-vTag[ID].x*vDelta[ID];
       }else{
       if(DCol > 0.0)
         tcColor[ID] = vec3(1.0,0.0,0.0);
	  tcNormalMinus[ID] = vNormal[ID]+vTag[ID].x*vDelta[ID]; 
       }

      }
    }

    tcNormalMinus[ID] = normalize(tcNormalMinus[ID]);

     if (Tesselation > 0.0){
    float d01 = sqrt(1.0-(dot(tcNormal[i], tcNormal[i+1])+1.0)/2.0);
    float d02 = sqrt(1.0-(dot(tcNormal[i], tcNormal[i+2])+1.0)/2.0);
    float d12 = sqrt(1.0-(dot(tcNormal[i+1], tcNormal[i+2])+1.0)/2.0);
       gl_TessLevelOuter[2] = max(gl_TessLevelOuter[2], ceil(d01*TessLevel));
       gl_TessLevelOuter[0] = max(ceil(d12*TessLevel),gl_TessLevelOuter[0]);
       gl_TessLevelOuter[1] = max(ceil(d02*TessLevel),gl_TessLevelOuter[1]);
      gl_TessLevelInner[0] = max(max(gl_TessLevelOuter[2],max(gl_TessLevelOuter[1],gl_TessLevelOuter[0])), gl_TessLevelInner[0]);    
  
     }


	   
}

-- TessEval

layout(triangles, equal_spacing, cw) in;
in vec3 tcPosition[];
in vec3 tcNormal[];
in vec3 tcNormalMinus[];
in vec3 tcDelta[];
in vec3 tcColor[];
in vec3 tcTag[];

out vec3 tePosition;
out vec3 tePatchDistance;
out vec3 teNormal;
out vec3 teColor;
uniform mat4 Projection;
uniform mat4 Modelview;
uniform float NormalModel;
uniform float Tagg;
uniform float ColNorm;
float py(vec3 p, vec3 n, vec3 q){
     return -dot(n, (q-p));
}


vec3 stitch(vec3 p, vec3 n, vec3 nt, vec3 de, vec3 tag, vec3 po, vec3 tagT){

     vec3 d = p + (1.0f-tag[1])/3.0f * (po-p);

     vec3 e = py(p,n,d) * n;
  
     if (length(tag)> 0.000001 && length(tagT)> 0.000001){
     	// both are tagged
	e = py(p,n,d) * normalize(n + tag[2]*de);
     }else if (length(tag)> 0.000001){
        // only one is tagged
	vec3 x = normalize((1.0f - tag[0])*n + tag[0]*nt);
	e = py(p,x,d) * x;
     }

   return (d + e);
}




void main()
{

    vec3 p0 = tcPosition[0];
    vec3 p1 = tcPosition[1];
    vec3 p2 = tcPosition[2];

    vec3 n0 = tcNormal[0];
    vec3 n1 = tcNormal[1];
    vec3 n2 = tcNormal[2];

    vec3 nt0 = tcNormalMinus[0];
    vec3 nt1 = tcNormalMinus[1];
    vec3 nt2 = tcNormalMinus[2];

    vec3 d0 = tcDelta[0];
    vec3 d1 = tcDelta[1];
    vec3 d2 = tcDelta[2];

    vec3 c0 = tcColor[0];
    vec3 c1 = tcColor[1];
    vec3 c2 = tcColor[2];

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
	
	if (Tagg > 0.0){
	
	b210 = stitch(p0, n0,nt0,d0, tcTag[0], p1, tcTag[1]);

 	b201 = stitch(p0, n0,nt0,d0, tcTag[0], p2, tcTag[2]);

	b120 = stitch(p1, n1,nt1,d1, tcTag[1], p0, tcTag[0]);
        b021 = stitch(p1, n1,nt1,d1, tcTag[1], p2, tcTag[2]);
	b102 = stitch(p2, n2,nt2,d2, tcTag[2], p0, tcTag[0]);
        b012 = stitch(p2, n2,nt2,d2, tcTag[2], p1, tcTag[1]);
	
	}

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





	if (NormalModel == 0.0){

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
	}else if (NormalModel == 1.0){
// castelau

	vec3 w = (b001-b100);
	vec3 z = (b010-b100);


	teNormal = normalize(cross(w,z));
	}else {
	    vec3 n110 = normalize(n0 + n1);
	    vec3 n101 = normalize(n0 + n2);
	    vec3 n011 = normalize(n1 + n2);

	    vec3 n100 = phi.x*n0   + phi.y*n110 + phi.z*n101;
	    vec3 n010 = phi.x*n110 + phi.y*n1   + phi.z*n011;
	    vec3 n001 = phi.x*n101 + phi.y*n011 + phi.z*n2;

	    teNormal = phi.x*n100 + phi.y*n010 + phi.z*n001;  
	}
    
    vec3 c110 = normalize(c0 + c1);
    vec3 c101 = normalize(c0 + c2);
    vec3 c011 = normalize(c1 + c2);
    vec3 c100 = phi.x*c0   + phi.y*c110 + phi.z*c101;
    vec3 c010 = phi.x*c110 + phi.y*c1   + phi.z*c011;
    vec3 c001 = phi.x*c101 + phi.y*c011 + phi.z*c2;

	    teColor = phi.x*c100 + phi.y*c010 + phi.z*c001;  
	    if(ColNorm > 0.0)
	    teColor = teNormal;
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
in vec3 teColor[3];
out vec3 gPatchDistance;
out vec3 gTriDistance;
out vec3 gNormal;
out vec3 gColor;

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
    gColor = teColor[0];
    EmitVertex();
	
    gPatchDistance = tePatchDistance[1];
    gTriDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position;
    gNormal = NormalMatrix *teNormal[1]; 	
    gColor = teColor[1];
    EmitVertex();

    gPatchDistance = tePatchDistance[2];
    gTriDistance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position;
    gNormal = NormalMatrix *teNormal[2]; 	
    gColor = teColor[2];
    EmitVertex();

    EndPrimitive();
}

-- Fragment

out vec4 FragColor;
in vec3 gNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in vec3 gColor;
in float gPrimitive;
uniform vec3 LightPosition;
uniform vec3 DiffuseMaterial;
uniform vec3 AmbientMaterial;
uniform float Wireframe;
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
   vec3 color = AmbientMaterial + df * gColor;// DiffuseMaterial;
//   vec3 color = AmbientMaterial + df * DiffuseMaterial;

//    color = (vec3(1)+N) / 2.0;

if (Wireframe > 0.0) {
        float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
        float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
        d1 = 1 - amplify(d1, 50, -0.5);
        d2 = amplify(d2, 50, -0.5);
        color = d2 * color + d1 * d2 * InnerLineColor;
    }


    FragColor = vec4(color, 1.0);
}

