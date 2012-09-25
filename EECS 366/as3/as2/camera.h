#include "MatrixMath.h"

class Camera
{
private:	
	point N, U, P;
public:
	int f;

	Camera();
	void swivel(float horizontal, float vertical);
	MATRIX ViewTransform();

};