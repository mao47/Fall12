#include "MatrixMath.h"

class Camera
{
private:	
	point N, U, P;
	point x, y, z;
	float RATIO;
public:
	int f;

	Camera();
	void swivel(float horizontal, float vertical);
	void lookAt(point p);
	MATRIX ViewTransform();
	void moveX(float shift);
	void moveY(float shift);
	void moveZ(float shift);
};