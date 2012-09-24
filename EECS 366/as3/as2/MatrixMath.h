#include <array>

typedef std :: array <std :: array <float, 4>,4> MATRIX;
typedef struct _point {
  float x,y,z;
} point;

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

MATRIX translateW(MATRIX M, point p);