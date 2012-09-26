#include <array>
#ifndef DEFINE_TEMPLATES
#define DEFINE_TEMPLATES

typedef std :: array <std :: array <float, 4>,4> MATRIX;
typedef struct _point {
  float x,y,z;
} point;


//returns multiplication of two 4x4 matrices
template <typename l, typename r>
MATRIX multiply (l left, r right)
{
	MATRIX result;
	for (int i = 0; i < 4; i ++)
		for (int j =0; j < 4; j ++)
		{
			result[i][j] = 0;
			for (int k = 0; k < 4; k ++)
			{
				result[i][j] += left[i][k] * right[k][j];
			}
		}
	return result;
}
//multiplies point by the matrix
template <typename l>
point multiplyP (l mat, point p)
{
	point result;
	result.x = mat[0][0] * p.x + mat[0][1] * p.y + mat[0][2] * p.z + mat[0][3];
	result.y = mat[1][0] * p.x + mat[1][1] * p.y + mat[1][2] * p.z + mat[1][3];
	result.z = mat[2][0] * p.x + mat[2][1] * p.y + mat[2][2] * p.z + mat[2][3];
	return result;
}

point crossProd(point a, point b);
float magnitude(point a);
MATRIX translate(MATRIX M, point p, bool isWorld);
MATRIX rotateX(MATRIX M, float angle, bool isWorld);
MATRIX rotateY(MATRIX M, float angle, bool isWorld);
MATRIX rotateZ(MATRIX M, float angle, bool isWorld);
MATRIX scale(MATRIX M, float scaling);
#endif