uniform vec3 LightPosition;
uniform vec3 lightIntensity;
uniform int specEx;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;

varying vec3 lpos;
varying vec3 lintensity;
varying vec3 Norm;
varying vec3 EcPos;
varying int spEx;
varying vec3 amb;
varying vec3 dif;
varying vec3 spe;


void main()
{
	amb = ambient;
	dif = diffuse;
	spe = specular;
	EcPos 		= vec3(gl_ModelViewMatrix * gl_Vertex);
	Norm 		= normalize(gl_NormalMatrix * gl_Normal);
	lintensity = lightIntensity;
	lpos = LightPosition;
	//vec3 lightVec 	= normalize(LightPosition - ecPos);
	//float NdotL 	= dot(tnorm, lightVec);
	//vec3 ViewVec 	= normalize(-ecPos);
	//vec3 LplusV 	= lightVec + ViewVec;
	//vec3 Halfway 	= LplusV / length(LplusV);
	//float HdotN 	= dot(Halfway, tnorm);
	
//	intensity = ambient * ambLight 				//ambient
//		+ lightIntensity * (
//			diffuse * NdotL 					//diffuse
//			+ specular * clamp(pow(HdotN, specEx), 0, 1)
//		);
	gl_Position = ftransform();

}