//#include <stdio.h>
#include <stdlib.h>
//#include "glut.h"
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <math.h>
#include <array>
#include "MatrixMath.h"

point crossProd(point a, point b)
{
	point result;
	result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;

    float len = sqrt(result.x*result.x + result.y*result.y + result.z*result.z);

    result.x = -result.x/len;
    result.y = -result.y/len;
    result.z = -result.z/len;

	return result;
}
float magnitude(point a)
{
	return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

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