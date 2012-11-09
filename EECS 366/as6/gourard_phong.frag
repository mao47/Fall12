varying vec3 intensity;


void main()
{
	gl_FragColor = vec4( intensity[0], intensity[1], intensity[2], 1.0 );	
	
}
