

#include "camera.h"

Camera::Camera()
{
	point N = {0,0,-1};
	point U = {0,1,0};
	point P = {0,0,5};
	RATIO = 0.1;
}

void Camera::swivel(float x, float y)
{
	//TODO: SUPER CONFUSING VARIABLE NAMES

	float angle = x * RATIO;
	//rotate N around (object) y axis (around U) by x

	// N = N * Ry
	float ty [4][4] = {{cos(angle),0,sin(angle),0},
					  {0,1,0,0},
					  {-1*sin(angle),0,cos(angle),0},
					  {0,0,0,1}};
	N = multiplyP(ty, N);

	//rotate N and U around x axis (horizontal) by y
	angle = y * RATIO;
	float tx [4][4] = {{1,0,0,0},
					  {0,cos(angle),-1*sin(angle),0},
					  {0,sin(angle),cos(angle),0},
					  {0,0,0,1}};
	// N = N * Rx
	N = multiplyP(tx, N);

	// U = U * Rx
	U = multiplyP(tx, U);
}

MATRIX Camera::ViewTransform()
{
	point negN = {-N.x,-N.y,-N.z};

	//compute n:
	float sizeN = magnitude(N);
	point n = {negN.x / sizeN, negN.y / sizeN, negN.z / sizeN};

	//compute u:
	point uXminusN = crossProd(U, negN);	// U x (-N)
	float len = magnitude(uXminusN);		// | U x (-N) |
	point u = { uXminusN.x / len, uXminusN.y / len, uXminusN.z / len};

	//compute v:
	point v = crossProd(n, u);

	MATRIX Mvw;
	Mvw[0][0] = u.x;
	Mvw[0][1] = u.y;
	Mvw[0][2] = u.z;
	Mvw[0][3] = - (u.x*P.x + u.y*P.y + u.z*P.z);	//-ut*P
	Mvw[1][0] = v.x;
	Mvw[1][1] = v.y;
	Mvw[1][2] = v.z;
	Mvw[1][3] = - (v.x*P.x + v.y*P.y + v.z*P.z);	//-vt*P
	Mvw[2][0] = n.x;
	Mvw[2][1] = n.y;
	Mvw[2][2] = n.z;
	Mvw[2][3] = - (n.x*P.x + n.y*P.y + n.z*P.z);	//-nt*P
	Mvw[3][0] = 0;
	Mvw[3][1] = 0;
	Mvw[3][2] = 0;
	Mvw[3][3] = 1;

	return Mvw;
}

