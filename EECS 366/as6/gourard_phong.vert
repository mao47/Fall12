uniform vec3 LightPosition;
uniform vec3 lightIntensity;
uniform int specEx;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

varying vec3 intensity;

const vec3 ambLight = vec3(0.02, 0.02, 0.02);

//varying float NdotL;
//varying vec3 ReflectVec;
//varying vec3 ViewVec;
//varying vec3 Halfway;


void main()
{
	vec3 ecPos 		= vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 tnorm 		= normalize(gl_NormalMatrix * gl_Normal);
	vec3 lightVec 	= normalize(LightPosition - ecPos);
	vec3 NdotL 		= dot(tnorm, lightVec);
	vec3 ViewVec 		= normalize(-ecPos);
	vec3 LplusV 	= lightVec + ViewVec;
	vec3 Halfway 	= LplusV / length(LplusV);
	vec3 HdotN 		= dot(Halfway, tnorm);
	
	intensity = ambient * ambLight 				//ambient
		+ lightIntensity * (
			diffuse * NdotL 					//diffuse
			+ specular * pow(HdotN, specEx)
		);
	gl_Position = ftransform();

}