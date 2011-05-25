varying vec3 Normal;
varying vec3 viewDir;

void main()
{

	float nDotLD;	// Normal - dot - light direction 
	float nDotHV;	// Normal - dot - light half vector
	float pf;		// power factor

	vec3 N = normalize(Normal);
	nDotLD = max(0.0, dot(N, normalize(vec3(gl_LightSource[0].position))));

	vec3 LP = (normalize(gl_LightSource[0].position).xyz);
	vec3 ED = normalize(viewDir);

	vec3 H = normalize(LP + vec3(0.0, 0.0, 1.0));

	nDotHV = max( 0.0, dot(N, H) );

	if(nDotLD == 0.0)
		pf = 0.0;
	else
		pf = pow(nDotHV, gl_FrontMaterial.shininess);

	vec4 ambient  = gl_LightSource[0].ambient;
	vec4 diffuse  = gl_LightSource[0].diffuse * nDotLD;
	vec4 specular = gl_LightSource[0].specular * pf;

	vec4 color = gl_FrontLightModelProduct.sceneColor +		// = materialEmission + globlAmbientLight * materialAmbient 
		ambient * gl_FrontMaterial.ambient +
		diffuse * gl_FrontMaterial.diffuse +
		specular * gl_FrontMaterial.specular;

gl_FragColor = vec4(color.xyz,1.0);

}