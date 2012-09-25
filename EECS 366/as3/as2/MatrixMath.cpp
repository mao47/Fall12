#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <array>
#include "MatrixMath.h"

MATRIX translate(MATRIX M, point p, bool isWorld)
{
	float t [4][4] = {{1,0,0,p.x},
					  {0,1,0,p.y},
					  {0,0,1,p.z},
					  {0,0,0,1}};
	if(isWorld)
		return multiply(t, M);
	else
		return multiply(M, t);
}

MATRIX rotateX(MATRIX M, float angle, bool isWorld)
{
	float t [4][4] = {{1,0,0,0},
					  {0,cos(angle),-1*sin(angle),0},
					  {0,sin(angle),cos(angle),0},
					  {0,0,0,1}};
	if(isWorld)
		return multiply(t, M);
	else
		return multiply(M, t);
}

MATRIX rotateY(MATRIX M, float angle, bool isWorld)
{
	float t [4][4] = {{cos(angle),0,sin(angle),0},
					  {0,1,0,0},
					  {-1*sin(angle),0,cos(angle),0},
					  {0,0,0,1}};
	if(isWorld)
		return multiply(t, M);
	else
		return multiply(M, t);
}

MATRIX rotateZ(MATRIX M, float angle, bool isWorld)
{
	float t [4][4] = {{cos(angle),-1*sin(angle),0,0},
					  {sin(angle),cos(angle),0,0},
					  {0,0,1,0},
					  {0,0,0,1}};
	if(isWorld)
		return multiply(t, M);
	else
		return multiply(M, t);
}

MATRIX scale(MATRIX M, float scaling)
{
	float t [4][4] = {{scaling,0,0,0},
					  {0,scaling,0,0},
					  {0,0,scaling,0},
					  {0,0,0,1}};
	return multiply(t, M);
}