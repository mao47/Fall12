varying vec3 lpos;
varying vec3 lintensity;
varying int spEx;
varying vec3 amb;
varying vec3 dif;
varying vec3 spe;

varying vec3 EcPos;
varying vec3 Norm;

const vec3 ambLight = vec3(0.02, 0.02, 0.02);

void main()
{
	//vec3 ecPos 		= vec3(gl_ModelViewMatrix * gl_Vertex);
	//vec3 Norm 		= normalize(gl_NormalMatrix * gl_Normal);
	vec3 lightVec 	= normalize(lpos - EcPos);
	float NdotL 	= dot(Norm, lightVec);
	vec3 ViewVec 	= normalize(-EcPos);
	vec3 LplusV 	= lightVec + ViewVec;
	vec3 Halfway 	= LplusV / length(LplusV);
	float HdotN 	= dot(Halfway, Norm);
	
	vec3 intensity = amb * ambLight 				//amb
		+ lintensity * (
			dif * NdotL 					//dif
			+ spe * clamp(pow(HdotN, spEx), 0, 1)
		);
	gl_FragColor = vec4(intensity[0], intensity[1], intensity[2], 1.0);

}