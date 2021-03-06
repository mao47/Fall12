uniform vec3 LightPosition;
uniform vec3 lightIntensity;
uniform int specEx;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

varying vec3 intensity;

const vec3 ambLight = vec3(0.02, 0.02, 0.02);

void main()
{
	vec3 ecPos 		= vec3(gl_ModelViewMatrix * gl_Vertex);
	vec3 tnorm 		= normalize(gl_NormalMatrix * gl_Normal);
	vec3 lightVec 	= normalize(LightPosition - ecPos);
	float NdotL 	= dot(tnorm, lightVec);
	vec3 ViewVec 	= normalize(-ecPos);
	vec3 LplusV 	= lightVec + ViewVec;
	vec3 Halfway 	= LplusV / length(LplusV);
	float HdotN 	= dot(Halfway, tnorm);
	
	intensity = ambient * ambLight 				//ambient
		+ lightIntensity * (
			diffuse * NdotL 					//diffuse
			+ specular * clamp(pow(HdotN, specEx), 0, 1)
		);
	gl_Position = ftransform();

}