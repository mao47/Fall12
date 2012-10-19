// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>
#include "Assignment5TemplateClasses.h"

using namespace std;

Vertex::Vertex()
{
	x = y = z = 0;
	h = 1;
}

void Vertex::Normalize()
{
	x = x/h;
	y = y/h;
	z = z/h;
	h = 1;
}

Object::Object()
{
	pBoundingBox = new Vertex[8];
	// Load the identity for the initial modeling matrix
	ModelMatrix[0] = ModelMatrix[5] = ModelMatrix[10] = ModelMatrix[15] = 1;
	ModelMatrix[1] = ModelMatrix[2] = ModelMatrix[3] = ModelMatrix[4] =
		ModelMatrix[6] = ModelMatrix[7] = ModelMatrix[8] = ModelMatrix[9] =
		ModelMatrix[11] = ModelMatrix[12] = ModelMatrix[13] = ModelMatrix[14]= 0;
}

Object::~Object()
{
	delete [] pBoundingBox;
	delete [] pVertexList;
	delete [] pFaceList;
}

// Load an object (.obj) file
void Object::Load(char* file, float s, float rx, float ry, float rz,
				  float tx, float ty, float tz)
{
	FILE* pObjectFile = fopen(file, "r");
	if(!pObjectFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char DataType;
	float a, b, c;

	// Scan the file and count the faces and vertices
	VertexCount = FaceCount = 0;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%c %f %f %f\n", &DataType, &a, &b, &c);
		if(DataType == 'v')
            VertexCount++;
		else if(DataType == 'f')
			FaceCount++;
	}
	pVertexList = new Vertex[VertexCount];
	pFaceList = new Face[FaceCount];

	fseek(pObjectFile, 0L, SEEK_SET);

	cout << "Number of vertices: " << VertexCount << endl;
	cout << "Number of faces: " << FaceCount << endl;

	// Load and create the faces and vertices
	int CurrentVertex = 0, CurrentFace = 0;
	float MinimumX, MaximumX, MinimumY, MaximumY, MinimumZ, MaximumZ;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%c %f %f %f\n", &DataType, &a, &b, &c);
		if(DataType == 'v')
		{
			pVertexList[CurrentVertex].x = a;
			pVertexList[CurrentVertex].y = b;
			pVertexList[CurrentVertex].z = c;
			// Track maximum and minimum coordinates for use in bounding boxes
			if(CurrentVertex == 0)
			{
				MinimumX = MaximumX = a;
				MinimumY = MaximumY = b;
				MinimumZ = MaximumZ = c;
			}
			else
			{
				if(a < MinimumX)
					MinimumX = a;
				else if(a > MaximumX)
					MaximumX = a;
				if(b < MinimumY)
					MinimumY = b;
				else if(b > MaximumY)
					MaximumY = b;
				if(c < MinimumZ)
					MinimumZ = c;
				else if(c > MaximumZ)
					MaximumZ = c;
			}
			CurrentVertex++;
		}
		else if(DataType == 'f')
		{
			// Convert to a zero-based index for convenience
			pFaceList[CurrentFace].v1 = (int)a - 1;
			pFaceList[CurrentFace].v2 = (int)b - 1;
			pFaceList[CurrentFace].v3 = (int)c - 1;
			CurrentFace++;
		}
	}

	// Initialize the bounding box vertices
	pBoundingBox[0].x = MinimumX; pBoundingBox[0].y = MinimumY; pBoundingBox[0].z = MinimumZ;
	pBoundingBox[1].x = MaximumX; pBoundingBox[1].y = MinimumY; pBoundingBox[1].z = MinimumZ;
	pBoundingBox[2].x = MinimumX; pBoundingBox[2].y = MaximumY; pBoundingBox[2].z = MinimumZ;
	pBoundingBox[3].x = MaximumX; pBoundingBox[3].y = MaximumY; pBoundingBox[3].z = MinimumZ;
	pBoundingBox[4].x = MinimumX; pBoundingBox[4].y = MinimumY; pBoundingBox[4].z = MaximumZ;
	pBoundingBox[5].x = MaximumX; pBoundingBox[5].y = MinimumY; pBoundingBox[5].z = MaximumZ;
	pBoundingBox[6].x = MinimumX; pBoundingBox[6].y = MaximumY; pBoundingBox[6].z = MaximumZ;
	pBoundingBox[7].x = MaximumX; pBoundingBox[7].y = MaximumY; pBoundingBox[7].z = MaximumZ;

	// Apply the initial transformations in order
	LocalScale(s);
	WorldRotate((float)(M_PI*rx/180.0), (float)(M_PI*ry/180.0), (float)(M_PI*rz/180.0));
	WorldTranslate(tx, ty, tz);	
}

// Do world-based translation
void Object::WorldTranslate(float tx, float ty, float tz)
{
	ModelMatrix[12] += tx;
	ModelMatrix[13] += ty;
	ModelMatrix[14] += tz;
}

// Perform world-based rotations in x,y,z order (intended for one-at-a-time use)
void Object::WorldRotate(float rx, float ry, float rz)
{
	float temp[16];

	if(rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[1] = temp[1]*cosx - temp[2]*sinx;
		ModelMatrix[2] = temp[2]*cosx + temp[1]*sinx;
		ModelMatrix[5] = temp[5]*cosx - temp[6]*sinx;
		ModelMatrix[6] = temp[6]*cosx + temp[5]*sinx;
		ModelMatrix[9] = temp[9]*cosx - temp[10]*sinx;
		ModelMatrix[10] = temp[10]*cosx + temp[9]*sinx;
		ModelMatrix[13] = temp[13]*cosx - temp[14]*sinx;
		ModelMatrix[14] = temp[14]*cosx + temp[13]*sinx;
	}

	if(ry != 0)
	{
		float cosy = cos(ry), siny = sin(ry);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosy + temp[2]*siny;
		ModelMatrix[2] = temp[2]*cosy - temp[0]*siny;
		ModelMatrix[4] = temp[4]*cosy + temp[6]*siny;
		ModelMatrix[6] = temp[6]*cosy - temp[4]*siny;
		ModelMatrix[8] = temp[8]*cosy + temp[10]*siny;
		ModelMatrix[10] = temp[10]*cosy - temp[8]*siny;
		ModelMatrix[12] = temp[12]*cosy + temp[14]*siny;
		ModelMatrix[14] = temp[14]*cosy - temp[12]*siny;
	}

	if(rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosz - temp[1]*sinz;
		ModelMatrix[1] = temp[1]*cosz + temp[0]*sinz;
		ModelMatrix[4] = temp[4]*cosz - temp[5]*sinz;
		ModelMatrix[5] = temp[5]*cosz + temp[4]*sinz;
		ModelMatrix[8] = temp[8]*cosz - temp[9]*sinz;
		ModelMatrix[9] = temp[9]*cosz + temp[8]*sinz;
		ModelMatrix[12] = temp[12]*cosz - temp[13]*sinz;
		ModelMatrix[13] = temp[13]*cosz + temp[12]*sinz;
	}
}

// Perform locally-based rotations in x,y,z order (intended for one-at-a-time use)
void Object::LocalRotate(float rx, float ry, float rz)
{
	float temp[16];

	if(rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[4] = temp[4]*cosx + temp[8]*sinx;
		ModelMatrix[5] = temp[5]*cosx + temp[9]*sinx;
		ModelMatrix[6] = temp[6]*cosx + temp[10]*sinx;
		ModelMatrix[7] = temp[7]*cosx + temp[11]*sinx;
		ModelMatrix[8] = temp[8]*cosx - temp[4]*sinx;
		ModelMatrix[9] = temp[9]*cosx - temp[5]*sinx;
		ModelMatrix[10] = temp[10]*cosx - temp[6]*sinx;
		ModelMatrix[11] = temp[11]*cosx - temp[7]*sinx;
	}

	if(ry != 0)
	{
        float cosy = cos(ry), siny = sin(ry);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosy - temp[8]*siny;
		ModelMatrix[1] = temp[1]*cosy - temp[9]*siny;
		ModelMatrix[2] = temp[2]*cosy - temp[10]*siny;
		ModelMatrix[3] = temp[3]*cosy - temp[11]*siny;
		ModelMatrix[8] = temp[8]*cosy + temp[0]*siny;
		ModelMatrix[9] = temp[9]*cosy + temp[1]*siny;
		ModelMatrix[10] = temp[10]*cosy + temp[2]*siny;
		ModelMatrix[11] = temp[11]*cosy + temp[3]*siny;
	}

	if(rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosz + temp[4]*sinz;
		ModelMatrix[1] = temp[1]*cosz + temp[5]*sinz;
		ModelMatrix[2] = temp[2]*cosz + temp[6]*sinz;
		ModelMatrix[3] = temp[3]*cosz + temp[7]*sinz;
		ModelMatrix[4] = temp[4]*cosz - temp[0]*sinz;
		ModelMatrix[5] = temp[5]*cosz - temp[1]*sinz;
		ModelMatrix[6] = temp[6]*cosz - temp[2]*sinz;
		ModelMatrix[7] = temp[7]*cosz - temp[3]*sinz;
	}
}

// Do locally-based uniform scaling
void Object::LocalScale(float s)
{
	for(int i = 0; i <= 11; i++)
        ModelMatrix[i] = s*ModelMatrix[i];
}

// Load a Scene (.dat) file
void Scene::Load(char* file)
{
	FILE* pSceneFile = fopen(file, "r");
	if(!pSceneFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char MeshFile[255];
	float Scaling;
	float RotationX, RotationY, RotationZ;
	float TranslationX, TranslationY, TranslationZ;

	// Step through the file and count the objects
	ObjectCount = 0;
	while(!feof(pSceneFile))
	{
		fscanf(pSceneFile, "%s %f %f %f %f %f %f %f\n", MeshFile, &Scaling,
			&RotationX, &RotationY, &RotationZ, &TranslationX, &TranslationY, &TranslationZ);
		ObjectCount++;
	}
	pObjectList = new Object[ObjectCount];

	fseek(pSceneFile, 0L, SEEK_SET);

	// Step through the file and create/load the objects
    for(int i = 0; i < ObjectCount; i++)
	{
		fscanf(pSceneFile, "%s %f %f %f %f %f %f %f\n", MeshFile, &Scaling,
			&RotationX, &RotationY, &RotationZ, &TranslationX, &TranslationY, &TranslationZ);
		pObjectList[i].Load(MeshFile, Scaling, RotationX, RotationY, RotationZ,
			TranslationX, TranslationY, TranslationZ);
	}

	cout << "Number of Objects Loaded: " << ObjectCount << endl;
}

Camera::Camera()
{
	Position.x = 10.0;
	Position.y = 0.0;
	Position.z = 0.0;

	Pitch = ((float) M_PI)/2;
	Yaw = 0.0;
	v.i = 0; v.j = 0; v.k = 1;
	EnforceVectors();

	ViewWidth = 8.0;
	ViewHeight = 8.0;

	FarPlane = 10.0;
	NearPlane = 2.0;
	ViewPlane = 10.0;

	minDistance = 1.0;

	LookAt();
}

// Move the camera with respect to its viewing vectors
void Camera::Move(float tx, float ty, float tz)
{
	Position.x += tz*n.i + ty*v.i + tx*u.i;
	Position.y += tz*n.j + ty*v.j + tx*u.j;
	Position.z += tz*n.k + ty*v.k + tx*u.k;
	LookAt();
}

// Pan the camera about its local axes
void Camera::Pan(float y, float p)
{
	Yaw += y;
	Pitch += p;
	EnforceVectors();
	LookAt();
}

// Scale the viewing plane
void Camera::ScaleView(float s)
{
	ViewWidth = s*ViewWidth;
	ViewHeight = s*ViewHeight;
}

// Move the viewing plane toward/away from the camera
void Camera::MoveView(float d)
{
	if(ViewPlane + d > 1.0)
        ViewPlane = ViewPlane + d;
}

// Moves the far plane toward/away from the camera
void Camera::MoveFarPlane(float move)
{
	if(FarPlane + move > NearPlane + minDistance)
        FarPlane = FarPlane + move;
}

// Moves the near plane toward/away from the camera
void Camera::MoveNearPlane(float move)
{
	if(NearPlane + move > minDistance && NearPlane + move < FarPlane - minDistance)
        NearPlane = NearPlane + move;
}

// Set (and normalize) the camera vectors based on the viewing angles
void Camera::EnforceVectors()
{
	float magnitude;
	Vector temp;

	n.i = sin(Pitch)*cos(Yaw);
	n.j = sin(Pitch)*sin(Yaw);
	n.k = cos(Pitch);

	if(((n.i == 0) && (n.j == 0) && (n.k == 1)) || ((n.i == 0) && (n.j == 0) && (n.k == -1)))
	{
        v.i = 1; v.j = 0; v.k = 0;
	}
	else
	{
        v.i = 0; v.j = 0; v.k = 1;
	}

	temp.i = v.j*n.k - v.k*n.j;
	temp.j = v.k*n.i - v.i*n.k;
	temp.k = v.i*n.j - v.j*n.i;
	magnitude = sqrt(temp.i*temp.i + temp.j*temp.j + temp.k*temp.k);
	u.i = temp.i/magnitude; u.j = temp.j/magnitude; u.k = temp.k/magnitude;

	v.i = n.j*u.k - n.k*u.j;
	v.j = n.k*u.i - n.i*u.k;
	v.k = n.i*u.j - n.j*u.i;
}

// Calculate the new perspective projection matrix; maps into (-1,1)x(-1,1)x(0,1)
void Camera::Perspective()
{
    ProjectionMatrix[0] = ProjectionMatrix[5] = ProjectionMatrix[10] = ProjectionMatrix[15] = 1;
	ProjectionMatrix[1] = ProjectionMatrix[2] = ProjectionMatrix[3] = ProjectionMatrix[4] =
		ProjectionMatrix[6] = ProjectionMatrix[7] = ProjectionMatrix[8] = ProjectionMatrix[9] =
		ProjectionMatrix[11] = ProjectionMatrix[12] = ProjectionMatrix[13] = ProjectionMatrix[14]= 0;

	// Set the non-identity elements
	ProjectionMatrix[0] = 2*ViewPlane/ViewWidth;
	ProjectionMatrix[5] = 2*ViewPlane/ViewHeight;
	ProjectionMatrix[10] = FarPlane/(NearPlane - FarPlane);
	ProjectionMatrix[11] = -1.0;
	ProjectionMatrix[14] = FarPlane*NearPlane/(NearPlane - FarPlane);
	ProjectionMatrix[15] = 0.0;
}

// Calculate the new orthographic projection matrix; maps into (-1,1)x(-1,1)x(0,1)
void Camera::Orthographic()
{
	ProjectionMatrix[0] = ProjectionMatrix[5] = ProjectionMatrix[10] = ProjectionMatrix[15] = 1;
	ProjectionMatrix[1] = ProjectionMatrix[2] = ProjectionMatrix[3] = ProjectionMatrix[4] =
		ProjectionMatrix[6] = ProjectionMatrix[7] = ProjectionMatrix[8] = ProjectionMatrix[9] =
		ProjectionMatrix[11] = ProjectionMatrix[12] = ProjectionMatrix[13] = ProjectionMatrix[14]= 0;

	// Set the non-identity elements
	ProjectionMatrix[0] = 2/ViewWidth;
	ProjectionMatrix[5] = 2/ViewHeight;
	ProjectionMatrix[10] = NearPlane/(NearPlane - FarPlane);
}

// Calculate the new viewing transform matrix
void Camera::LookAt()
{
	ViewingMatrix[0] = u.i; ViewingMatrix[4] = u.j; ViewingMatrix[8] = u.k;
		ViewingMatrix[12] = -(u.i*Position.x + u.j*Position.y + u.k*Position.z);
	ViewingMatrix[1] = v.i; ViewingMatrix[5] = v.j; ViewingMatrix[9] = v.k;
		ViewingMatrix[13] = -(v.i*Position.x + v.j*Position.y + v.k*Position.z);
	ViewingMatrix[2] = n.i; ViewingMatrix[6] = n.j; ViewingMatrix[10] = n.k;
		ViewingMatrix[14] = -(n.i*Position.x + n.j*Position.y + n.k*Position.z);
	ViewingMatrix[3] = ViewingMatrix[7] = ViewingMatrix[11] = 0; ViewingMatrix[15] = 1;
}

// Transform a point with an arbitrary matrix
Vertex Transform(float* matrix, Vertex& point)
{
	Vertex temp;
	temp.x = matrix[0]*point.x + matrix[4]*point.y + matrix[8]*point.z + matrix[12]*point.h;
	temp.y = matrix[1]*point.x + matrix[5]*point.y + matrix[9]*point.z + matrix[13]*point.h;
	temp.z = matrix[2]*point.x + matrix[6]*point.y + matrix[10]*point.z + matrix[14]*point.h;
	temp.h = matrix[3]*point.x + matrix[7]*point.y + matrix[11]*point.z + matrix[15]*point.h;
	return temp;
}

// Select a new object by intersecting the selection ray with all object faces
int Select(int previous, Scene* pScene, Camera* pCamera, float x, float y)
{
	int select = previous;
	float zp = 10000, z;
	float s, t;
	Vertex temp1, temp2, temp3;

	for(int i = 0; i < pScene->ObjectCount; i++)
	{
		for(int j = 0; j < pScene->pObjectList[i].FaceCount; j++)
		{
			// Project each vertex into the standard view volume
			temp1 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v1];
			temp1 = Transform(pScene->pObjectList[i].ModelMatrix, temp1);
			temp1 = Transform(pCamera->ViewingMatrix, temp1);
			temp1 = Transform(pCamera->ProjectionMatrix, temp1);
			temp1.Normalize();

			temp2 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v2];
			temp2 = Transform(pScene->pObjectList[i].ModelMatrix, temp2);
			temp2 = Transform(pCamera->ViewingMatrix, temp2);
			temp2 = Transform(pCamera->ProjectionMatrix, temp2);
			temp2.Normalize();

			temp3 = pScene->pObjectList[i].pVertexList[pScene->pObjectList[i].pFaceList[j].v3];
			temp3 = Transform(pScene->pObjectList[i].ModelMatrix, temp3);
			temp3 = Transform(pCamera->ViewingMatrix, temp3);
			temp3 = Transform(pCamera->ProjectionMatrix, temp3);
			temp3.Normalize();

			// Use parametric equations to find the intersection between the selection ray
			// and the current (transformed) face
			t = ((temp1.x - temp3.x)*(y - temp3.y) - (temp1.y - temp3.y)*(x - temp3.x))/
				((temp1.x - temp3.x)*(temp2.y - temp3.y) + (temp1.y - temp3.y)*(temp3.x - temp2.x));
			if((temp1.x - temp3.x) != 0)
                s = (x - temp3.x + (temp3.x - temp2.x)*t)/(temp1.x - temp3.x);
			else
				s = (y - temp3.y+(temp3.y - temp2.y)*t)/(temp1.y - temp3.y);
			// Use the intersection if its valid (within the triangle) and closer
			if((s >= 0) && (t >= 0) && ((s + t) <= 1))
			{
				z = temp1.z*s + temp2.z*t + (1 - s - t)*temp3.z;
				if(z < zp)
				{
					zp = z;
					select = i;
				}
			}
		}
	}

	return select;
}

// Clip a polygon against view volume borders
// ADD CODE HERE: dummy function only copies polygons
Vertex* ClipPolygon(int count, Vertex* input, int* out_count)
{


	Vertex f, s, p;

	//make sure the array is long enough. There is some upper bound considering
	//our input is always a triangle, but I'm not sure what it is, but pretty
	//sure it is less than 16 vertices.
	Vertex* intermediate = new Vertex[16];
	
	int outputCounter = 0;


	Vertex intersect;




	//check -h<=x
	for(int i = 0; i < count; i++)
	{
		p = input[i];
		if(i == 0)
		{
			s = p;
			f = p;
		}
		else
		{
			//cross plane? (-h <= x)
			if((s.x < -s.h && p.x > -p.h)
				|| (s.x > -s.h && p.x < -p.h))
			{
				float alpha = (s.x - (-s.h))/(s.x-(-s.h)-(p.x-(-p.h)));
				intersect.x = s.x + alpha*(p.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
				intersect.y = s.y + alpha*(p.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
				intersect.z = s.z + alpha*(p.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
				intersect.h = s.h + alpha*(p.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
				s = p;
				//todo: output intersection
				intermediate[outputCounter] = intersect;
				outputCounter++;
			}
			else
			{
				s = p;
			}
		}
		//is s on visible side? (-h <= x)
		if(s.x >= -s.h)
		{
			//output s
			intermediate[outputCounter] = s;
			outputCounter++;
		}
	}
	if(outputCounter > 0)
	{
		//does SF cross plane? (-h <= x)
		if((s.x < -s.h && f.x > -f.h)
			|| (s.x > -s.h && f.x < -f.h))
		{
			//todo: compute intersection
			float alpha = (s.x - (-s.h))/(s.x-(-s.h)-(f.x-(-f.h)));
			intersect.x = s.x + alpha*(f.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
			intersect.y = s.y + alpha*(f.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
			intersect.z = s.z + alpha*(f.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
			intersect.h = s.h + alpha*(f.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
			//output intersection
			intermediate[outputCounter] = intersect;
			outputCounter++;
		}
	}




	Vertex* intermediate2 = new Vertex[16];
	int outputCounter2 = 0;
	//check x<=h
	for(int i = 0; i < outputCounter; i++)
	{
		p = intermediate[i];
		if(i == 0)
		{
			s = p;
			f = p;
		}
		else
		{
			//cross plane? (x<=h)
			if((s.x < s.h && p.x > p.h)
				|| (s.x > s.h && p.x < p.h))
			{
				float alpha = (s.x - (s.h))/(s.x-(s.h)-(p.x-(p.h)));
				intersect.x = s.x + alpha*(p.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
				intersect.y = s.y + alpha*(p.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
				intersect.z = s.z + alpha*(p.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
				intersect.h = s.h + alpha*(p.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);	
				s = p;
				//output intersection
				intermediate2[outputCounter2] = intersect;
				outputCounter2++;
			}
			else
			{
				s = p;
			}
		}
		//is s on visible side? (x<=h)
		if(s.x < s.h)
		{
			//output s
			intermediate2[outputCounter2] = s;
			outputCounter2++;
		}
	}
	if(outputCounter2 > 0)
	{
		//does SF cross plane? (x<=h)
		if((s.x < s.h && f.x > f.h)
			|| (s.x > s.h && f.x < f.h))
		{
			//todo: compute intersection
			float alpha = (s.x - (s.h))/(s.x-(s.h)-(f.x-(f.h)));
			intersect.x = s.x + alpha*(f.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
			intersect.y = s.y + alpha*(f.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
			intersect.z = s.z + alpha*(f.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
			intersect.h = s.h + alpha*(f.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
			//output intersection
			intermediate2[outputCounter2] = intersect;
			outputCounter2++;
		}
	}




	Vertex* intermediate3 = new Vertex[16];
	int outputCounter3 = 0;
	//check y<=h
	for(int i = 0; i < outputCounter2; i++)
	{
		p = intermediate2[i];
		if(i == 0)
		{
			s = p;
			f = p;
		}
		else
		{
			//cross plane? (y<=h)
			if((s.y < s.h && p.y > p.h)
				|| (s.y > s.h && p.y < p.h))
			{
				float alpha = (s.y - (s.h))/(s.y-(s.h)-(p.y-(p.h)));
				intersect.x = s.x + alpha*(p.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
				intersect.y = s.y + alpha*(p.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
				intersect.z = s.z + alpha*(p.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
				intersect.h = s.h + alpha*(p.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
				s = p;
				//output intersection
				intermediate3[outputCounter3] = intersect;
				outputCounter3++;
			}
			else
			{
				s = p;
			}
		}
		//is s on visible side? (y<=h)
		if(s.y < s.h)
		{
			//output s
			intermediate3[outputCounter3] = s;
			outputCounter3++;
		}
	}
	if(outputCounter3 > 0)
	{
		//does SF cross plane? (y<=h)
		if((s.y < s.h && f.y > f.h)
			|| (s.y > s.h && f.y < f.h))
		{
			//todo: compute intersection
			float alpha = (s.y - (s.h))/(s.y-(s.h)-(f.y-(f.h)));
			intersect.x = s.x + alpha*(f.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
			intersect.y = s.y + alpha*(f.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
			intersect.z = s.z + alpha*(f.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
			intersect.h = s.h + alpha*(f.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
			//output intersection
			intermediate3[outputCounter3] = intersect;
			outputCounter3++;
		}
	}





	Vertex* intermediate4 = new Vertex[16];
	int outputCounter4 = 0;
	//check -h<=y
	for(int i = 0; i < outputCounter3; i++)
	{
		p = intermediate3[i];
		if(i == 0)
		{
			s = p;
			f = p;
		}
		else
		{
			//cross plane? (-h<=y)
			if((s.y < -s.h && p.y > -p.h)
				|| (s.y > -s.h && p.y < -p.h))
			{
				float alpha = (s.y - (-s.h))/(s.y-(-s.h)-(p.y-(-p.h)));
				intersect.x = s.x + alpha*(p.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
				intersect.y = s.y + alpha*(p.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
				intersect.z = s.z + alpha*(p.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
				intersect.h = s.h + alpha*(p.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
				s = p;
				//output intersection
				intermediate4[outputCounter4] = intersect;
				outputCounter4++;
			}
			else
			{
				s = p;
			}
		}
		//is s on visible side? (-h<=y)
		if(s.y >= -s.h)
		{
			//output s
			intermediate4[outputCounter4] = s;
			outputCounter4++;
		}
	}
	if(outputCounter4 > 0)
	{
		//does SF cross plane? (-h<=y)
		if((s.y < -s.h && f.y > -f.h)
			|| (s.y > -s.h && f.y < -f.h))
		{
			//todo: compute intersection
			float alpha = (s.y - (-s.h))/(s.y-(-s.h)-(f.y-(-f.h)));
			intersect.x = s.x + alpha*(f.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
			intersect.y = s.y + alpha*(f.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
			intersect.z = s.z + alpha*(f.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
			intersect.h = s.h + alpha*(f.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
			//output intersection
			intermediate4[outputCounter4] = intersect;
			outputCounter4++;
		}
	}










	

	Vertex* intermediate5 = new Vertex[16];
	int outputCounter5 = 0;
	//check z<=h
	for(int i = 0; i < outputCounter4; i++)
	{
		p = intermediate4[i];
		if(i == 0)
		{
			s = p;
			f = p;
		}
		else
		{
			//cross plane? (y<=h)
			if((s.z < s.h && p.z > p.h)
				|| (s.z > s.h && p.z < p.h))
			{
				float alpha = (s.z - (s.h))/(s.z-(s.h)-(p.z-(p.h)));
				intersect.x = s.x + alpha*(p.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
				intersect.y = s.y + alpha*(p.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
				intersect.z = s.z + alpha*(p.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
				intersect.h = s.h + alpha*(p.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
				s = p;
				//output intersection
				intermediate5[outputCounter5] = intersect;
				outputCounter5++;
			}
			else
			{
				s = p;
			}
		}
		//is s on visible side? (y<=h)
		if(s.z < s.h)
		{
			//output s
			intermediate5[outputCounter5] = s;
			outputCounter5++;
		}
	}
	if(outputCounter5 > 0)
	{
		//does SF cross plane? (y<=h)
		if((s.z < s.h && f.z > f.h)
			|| (s.z > s.h && f.z < f.h))
		{
			//todo: compute intersection
			float alpha = (s.z - (s.h))/(s.z-(s.h)-(f.z-(f.h)));
			intersect.x = s.x + alpha*(f.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
			intersect.y = s.y + alpha*(f.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
			intersect.z = s.z + alpha*(f.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
			intersect.h = s.h + alpha*(f.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
			//output intersection
			intermediate5[outputCounter5] = intersect;
			outputCounter5++;
		}
	}





	Vertex* intermediate6 = new Vertex[16];
	int outputCounter6 = 0;
	//check 0<=z
	for(int i = 0; i < outputCounter5; i++)
	{
		p = intermediate5[i];
		if(i == 0)
		{
			s = p;
			f = p;
		}
		else
		{
			//cross plane? (0<=z)
			if((s.z < 0 && p.z > 0)
				|| (s.z > 0 && p.z < 0))
			{
				float alpha = (s.z)/(s.z-p.z);
				intersect.x = s.x + alpha*(p.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
				intersect.y = s.y + alpha*(p.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
				intersect.z = s.z + alpha*(p.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
				intersect.h = s.h + alpha*(p.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
				s = p;
				//output intersection
				intermediate6[outputCounter6] = intersect;
				outputCounter6++;
			}
			else
			{
				s = p;
			}
		}
		//is s on visible side? (0<=z)
		if(s.z >= 0)
		{
			//output s
			intermediate6[outputCounter6] = s;
			outputCounter6++;
		}
	}
	if(outputCounter6 > 0)
	{
		//does SF cross plane? (0<=z)
		if((s.z < 0 && f.z > 0)
			|| (s.z > 0 && f.z < 0))
		{
			//todo: compute intersection
			float alpha = (s.z)/(s.z-f.z);
			intersect.x = s.x + alpha*(f.x-s.x);//alpha*p.x - (1-alpha)*s.x + alpha*(p.x-s.x);
			intersect.y = s.y + alpha*(f.y-s.y);//alpha*p.y - (1-alpha)*s.y + alpha*(p.y-s.y);
			intersect.z = s.z + alpha*(f.z-s.z);//alpha*p.z - (1-alpha)*s.z + alpha*(p.z-s.z);
			intersect.h = s.h + alpha*(f.h-s.h);//alpha*p.h - (1-alpha)*s.h + alpha*(p.h-s.h);
			//output intersection
			intermediate6[outputCounter6] = intersect;
			outputCounter6++;
		}
	}


	Vertex* output = new Vertex[outputCounter6];
	for(int i = 0; i < outputCounter6; i++)
		output[i] = intermediate6[i];
	*out_count = outputCounter6;
	delete [] intermediate;
	delete [] intermediate2;
	delete [] intermediate3;
	delete [] intermediate4;
	delete [] intermediate5;
	delete [] intermediate6;

	return output;


	

}