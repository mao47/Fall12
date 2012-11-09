uniform vec3 LightPosition;
uniform vec3 lightIntensity;
uniform int specEx;
uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform vec2 m;
uniform vec2 w;
uniform vec3 fresnel;

varying vec3 intensity;

const vec3 ambLight = vec3(0.02, 0.02, 0.02);
const float pi = 3.1415926535;

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
	float VdotH		= dot(ViewVec, Halfway);
	float NdotV		= dot(tnorm, ViewVec);
	float alpha		= acos(HdotN / (length(Halfway) * length(tnorm)));
	float cos4alpha = pow(cos(alpha), 4O);
	float tanalpha  = tan(alpha);
	
	float theta		= acos(VdotH / (length(Halfway) * length(ViewVec)));
	
	//weighted average of beckmann function with different parameters
	float D			= w[0] / (m[0] * m[0] * cos4alpha) * exp(-pow(tanalpha/m[0], 2))
					+ w[1] / (m[1] * m[1] * cos4alpha) * exp(-pow(tanalpha/m[1], 2));
	
	float G			= min(
						1, min(
						2 * HdotN * NdotV / VdotH,
						2 * HdotN * NdotL / VdotH);
	
	intensity		= ambient + D * G * (fresnel * theta)
								/ (pi * NdotV);
	
	gl_Position = ftransform();

}