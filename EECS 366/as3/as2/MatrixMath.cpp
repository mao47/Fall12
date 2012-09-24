#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <array>
#include "MatrixMath.h"

MATRIX translateW(MATRIX M, point p)
{
	float t [4][4] = {{1,0,0,p.x},
					  {0,1,0,p.y},
					  {0,0,1,p.z},
					  {0,0,0,1}};
	return multiply(t, M);
}