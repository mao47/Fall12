

#include "camera.h"
#include <math.h>

Camera::Camera()
{
	
	N.x=0;	N.y=0;	N.z=-1; //why neg?
	U.x=0;	U.y=1;	U.z=0;
	P.x=0;	P.y=0;	P.z=5;

	x.x=1;	x.y=0;	x.z=5;
	y.x=0;	y.y=1;	y.z=5;
	z.x=0;	z.y=0;	z.z=4; //?
	RATIO = 0.025;
}

void Camera::lookAt(point p)
{
	//TODO: MORE TERRIBLE NAMES
	point direction = {p.x - P.x, p.y - P.y, p.z - P.z};	//subtract camera position from target so we don't need to multiply by -1 (look at -N direction)
	float dist = magnitude(direction);
	N.x = direction.x / dist;
	N.y = direction.y / dist;
	N.z = direction.z / dist;
}
void Camera::swivel(float xa, float ya)
{
	float angle = xa * RATIO;


	//TODO: SUPER CONFUSING VARIABLE NAME
	float transO [4][4] = {{1,0,0,-P.x},
	{0,1,0,-P.y},
	{0,0,1,-P.z},
	{0,0,0,1}};

	////translate to origin
	point tempx = multiplyP(transO, x);
	point tempy = multiplyP(transO, y);
	point tempz = multiplyP(transO, z);

	/*
	goal:
	up -> y
	look at -> z
	u x n -> x


	the (horizontal) rotation vector is now tempy to rotate horizontally in the camera's frame (where tempy is up)
	the vertical rotation vector will be tempx
	*/
	//  http://inside.mines.edu/~gmurray/ArbitraryAxisRotation/
	//float zdist = sqrt(tempy.x*tempy.x + tempy.y*tempy.y);
	//float d = magnitude(tempy);
	//float transXYplane2Zaxis [4][4] = {{tempy.z/d, 0, -zdist/d, 0},
	//								{0, 1, 0, 0},
	//								{zdist/d, 0, tempy.z/d, 0},
	//								{0, 0, 0, 1}};
	//MATRIX T;
	//if(zdist > 0){
	//	float trans2xyplane [4][4] = {{tempy.x/zdist, tempy.y/zdist,0,0},
	//								{-tempy.y/zdist, tempy.x/zdist,0,0},
	//								{0,0,1,0},
	//								{0,0,0,1}};
	//	T = multiply(transXYplane2Zaxis, trans2xyplane);
	//}
	//else
	//{
	//	for(int i=0; i<4; i++)
	//		for(int j=0; j<4; j++)
	//			T[i][j] = transXYplane2Zaxis[i][j];
	//}

	//at this point T will rotate the coords to the z axis





	///wait...


	float cosx = cos(angle);
	float sinx = sin(angle);
	float x1 = tempy.x;
	float y1 = tempy.y;
	float z1 = tempy.z;
	float x2 = tempy.x * tempy.x;
	float y2 = tempy.y * tempy.y;
	float z2 = tempy.z * tempy.z;
	float t[4][4] = {
		{x2 + (y2+z2)*cosx,			x1*y1*(1-cosx)-z1*sinx,		x1*z1*(1-cosx)+y1*sinx,		(P.x*(y2+z2)-x1*(P.y*y1+P.z*z1))*(1-cosx)+(P.y*z1-P.z*y1)*sinx},
		{x1*y1*(1-cosx)+z1*sinx,	y2+(x2+z2)*cosx,			y1*z1*(1-cosx)-x1*sinx,		(P.y*(x2+z2)-y1*(P.x*x1+P.z*z1))*(1-cosx)+(P.z*x1-P.x*z1)*sinx},
		{x1*z1*(1-cosx)-y1*sinx,	y1*z1*(1-cosx)+x1*sinx,		z2 + (x2+y2)*cosx,			(P.z*(x2+y2)-z1*(P.x*x1+P.y*y1))*(1-cosx)+(P.x*y1-P.y*x1)*sinx},
		{0,0,0,1}};

	//
	//x = multiplyP(t, x);
	//y = multiplyP(t, y);
	//z = multiplyP(t, z);





	angle = -ya*RATIO;

	cosx = cos(angle);
	sinx = sin(angle);
	x1 = tempx.x;
	y1 = tempx.y;
	z1 = tempx.z;
	x2 = tempx.x * tempx.x;
	y2 = tempx.y * tempx.y;
	z2 = tempx.z * tempx.z;
	float v[4][4] = {
		{x2 + (y2+z2)*cosx,			x1*y1*(1-cosx)-z1*sinx,		x1*z1*(1-cosx)+y1*sinx,		(P.x*(y2+z2)-x1*(P.y*y1+P.z*z1))*(1-cosx)+(P.y*z1-P.z*y1)*sinx},
		{x1*y1*(1-cosx)+z1*sinx,	y2+(x2+z2)*cosx,			y1*z1*(1-cosx)-x1*sinx,		(P.y*(x2+z2)-y1*(P.x*x1+P.z*z1))*(1-cosx)+(P.z*x1-P.x*z1)*sinx},
		{x1*z1*(1-cosx)-y1*sinx,	y1*z1*(1-cosx)+x1*sinx,		z2 + (x2+y2)*cosx,			(P.z*(x2+y2)-z1*(P.x*x1+P.y*y1))*(1-cosx)+(P.x*y1-P.y*x1)*sinx},
		{0,0,0,1}};

	MATRIX T = multiply(v, t);

	x = multiplyP(v, x);
	y = multiplyP(v, y);
	z = multiplyP(v, z);


	//point yp = {-U.x, U.y, -U.x};	//where the y axis goes

	////rotate U to world y
	//float roty [4][4] = {{0,-U.x,0,0},
	//{0,U.y,0,0},
	//{0,-U.z,0,0},
	//{0,0,0,1}};
	//

	
	//rotate N around (object) y axis (around U) by x

	// N = N * Ry
	//float ty [4][4] = {{cos(angle),0,sin(angle),0},
	//				  {0,1,0,0},
	//				  {-1*sin(angle),0,cos(angle),0},
	//				  {0,0,0,1}};
	//N = multiplyP(ty, N);

	////rotate N and U around x axis (horizontal) by y
	//angle = -ya * RATIO;
	//float tx [4][4] = {{1,0,0,0},
	//				  {0,cos(angle),-1*sin(angle),0},
	//				  {0,sin(angle),cos(angle),0},
	//				  {0,0,0,1}};
	//// N = N * Rx
	//N = multiplyP(tx, N);

	//// U = U * Rx
	//U = multiplyP(tx, U);
}



MATRIX Camera::ViewTransform()
{
	N.x = z.x - P.x;
	N.y = z.y - P.y;
	N.z = z.z - P.z;

	U.x = y.x - P.x;
	U.y = y.y - P.y;
	U.z = y.z - P.z;

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

