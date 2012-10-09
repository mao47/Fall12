// Assignment4Template.cpp
// Computer Graphics - EECS 366/466 - Spring 2006

// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "glut.h"
#include "Assignment4TemplateClasses.h"

using namespace std;

// User Interface Variables
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseMiddle = false;
bool MouseRight = false;
bool PerspectiveMode = true;
bool ShowAxes = true;
bool SelectionMode = false;
int SelectedObject = 0;
bool ShowBoundingBoxes = true;
int WindowWidth = 300, WindowHeight = 300;

// Scene Content Variables
Scene* pDisplayScene;
Camera* pDisplayCamera;

Vertex** boundBoxes;


#define EPSILON 0.000001
#define CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#define SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 

int intersect_triangle(float orig[3], float dir[3],
                   float vert0[3], float vert1[3], float vert2[3],
                   float *t, float *u, float *v)
{
   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   float det,inv_det;

   /* find vectors for two edges sharing vert0 */
   SUB(edge1, vert1, vert0);
   SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = DOT(edge1, pvec);

#ifdef TEST_CULL           /* define TEST_CULL if culling is desired */
   if (det < EPSILON)
      return 0;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec);
   if (*u < 0.0 || *u > det)
      return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

    /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec);
   if (*v < 0.0 || *u + *v > det)
      return 0;

   /* calculate t, scale parameters, ray intersects triangle */
   *t = DOT(edge2, qvec);
   inv_det = 1.0 / det;
   *t *= inv_det;
   *u *= inv_det;
   *v *= inv_det;
#else                    /* the non-culling branch */
   if (det > -EPSILON && det < EPSILON)
     return 0;
   inv_det = 1.0 / det;

   /* calculate distance from vert0 to ray origin */
   SUB(tvec, orig, vert0);

   /* calculate U parameter and test bounds */
   *u = DOT(tvec, pvec) * inv_det;
   if (*u < 0.0 || *u > 1.0)
     return 0;

   /* prepare to test V parameter */
   CROSS(qvec, tvec, edge1);

   /* calculate V parameter and test bounds */
   *v = DOT(dir, qvec) * inv_det;
   if (*v < 0.0 || *u + *v > 1.0)
     return 0;

   /* calculate t, ray intersects triangle */
   *t = DOT(edge2, qvec) * inv_det;
#endif
   return 1;
}

void DisplayFunc()
{
	
	Vertex* input;
	Vertex	temp,temp1,temp2,temp3;
	Vertex  orig,xunit,yunit,zunit;
	
	// vertices to be used in drawing the coordiante axes
	orig.x=orig.y=orig.z=xunit.y=xunit.z=yunit.x=yunit.z=zunit.x=zunit.y=0.0;
	orig.h=xunit.x=xunit.h=yunit.y=yunit.h=zunit.z=zunit.h=1.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	
	//MAY WANT TO MODIFY THIS BASED ON ASSIGNMENT REQUIREMENTS
	gluOrtho2D(-1,1,-1,1);
	

	if(PerspectiveMode)
	{
		pDisplayCamera->Perspective();
	}
		
	else
		pDisplayCamera->Orthographic();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// draw world coordinate frames
	if(ShowAxes)
	{
		glLineWidth(3.0);
		glBegin(GL_LINES);

		temp = Transform(pDisplayCamera->ViewingMatrix,orig);
		temp1 = Transform(pDisplayCamera->ProjectionMatrix,temp);
		temp = Transform(pDisplayCamera->ViewingMatrix,xunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(1, 0, 0);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);

		temp = Transform(pDisplayCamera->ViewingMatrix,yunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(0, 1, 0);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);

		temp = Transform(pDisplayCamera->ViewingMatrix,zunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(0, 0, 1);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
		glEnd();
		glLineWidth(1.0);
	}

	boundBoxes = new Vertex*[pDisplayScene->ObjectCount];
	// draw objects
	for(int i = 0; i < pDisplayScene->ObjectCount; i++)
	{
		boundBoxes[i] = new Vertex[8];
		// Color the selected object yellow and others blue
		if(i == SelectedObject)
			glColor3f(1, 1, 0);
		else
			glColor3f(0, 0, 1);
		//  draw object faces
		for(int j = 0; j < pDisplayScene->pObjectList[i].FaceCount; j++)
		{
			input = new Vertex[3];
			input[0] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v1];
			input[1] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v2];
			input[2] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v3];

			for (int k=0; k<3; k++){
				temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,input[k]);
				temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
				input[k]= Transform(pDisplayCamera->ProjectionMatrix,temp2);
			}

			

			glBegin(GL_POLYGON);
			for(int k = 0; k < 3; k++)
				glVertex2f(input[k].x/input[k].h, input[k].y/input[k].h);
			glEnd();

			delete [] input;
			input = NULL;
		
		}

		// Draw object coordinate frames
		if(ShowAxes)
		{
			glLineWidth(3.0);
			glBegin(GL_LINES);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,orig);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp2 = Transform(pDisplayCamera->ProjectionMatrix,temp1);
			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,xunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,yunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,zunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);
			glEnd();
			glLineWidth(1.0);
		}

		
		//ADD YOUR CODE HERE: Draw the bounding boxes
		//Color the selected box red and others blue
		if(i == SelectedObject)
			glColor3f(1, 0, 0);
		else
			glColor3f(0, 0, 1);
		//  draw top and bottom faces of box
		//TODO: Grab vertices of bounding box in View Coord
		for(int j = 0; j < 2; j++)
		{
			input = new Vertex[4];
			input[0] = pDisplayScene->pObjectList[i].pBoundingBox[0+4*j];
			input[1] = pDisplayScene->pObjectList[i].pBoundingBox[1+4*j];
			input[2] = pDisplayScene->pObjectList[i].pBoundingBox[3+4*j];
			input[3] = pDisplayScene->pObjectList[i].pBoundingBox[2+4*j];

			for (int k=0; k<4; k++){
				temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,input[k]);
				temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
				boundBoxes[i][j*4+k] = temp2;
				input[k]= Transform(pDisplayCamera->ProjectionMatrix,temp2);
			}

			if(ShowBoundingBoxes){
				glBegin(GL_POLYGON);
				for(int k = 0; k < 4; k++)
					glVertex2f(input[k].x/input[k].h, input[k].y/input[k].h);
				glEnd();
			}

			delete [] input;
			input = NULL;
		}
		//draw vertical edges
		for(int j = 0; j < 4; j++)
		{
			input = new Vertex[2];
			input[0] = pDisplayScene->pObjectList[i].pBoundingBox[j];
			input[1] = pDisplayScene->pObjectList[i].pBoundingBox[j+4];
				
			for (int k=0; k<2; k++){
				temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,input[k]);
				temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
				input[k]= Transform(pDisplayCamera->ProjectionMatrix,temp2);
			}
			if(ShowBoundingBoxes){
				glBegin(GL_LINES);
				for(int k = 0; k < 2; k++)
					glVertex2f(input[k].x/input[k].h, input[k].y/input[k].h);
				glEnd();
			}

			delete [] input;
			input = NULL;
			}
 
	}

	glutSwapBuffers();
}


void ReshapeFunc(int x,int y)
{
	// Get the correct view proportionality for the new window size
	pDisplayCamera->ViewWidth = pDisplayCamera->ViewWidth*((float)x/WindowWidth);
	pDisplayCamera->ViewHeight = pDisplayCamera->ViewHeight*((float)y/WindowHeight);
	glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}

void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !state;


	if(MouseLeft && SelectionMode)
	{
		// Select a new object with (x,y) 
		//ADD YOUR CODE HERE: Select a new object by intersecting the selection ray

		//transform viewport coordinates to image frame coordinates
		float viewX = (float)x / (float)WindowWidth * pDisplayCamera->ViewWidth - pDisplayCamera->ViewWidth / (float)2;
		float viewY = -(float)y / (float)WindowHeight * pDisplayCamera->ViewHeight + pDisplayCamera->ViewHeight / (float)2;

		//DECLARE ALL THE INPUTS FOR THE INTERSECTION ALGORITHM
		Vertex* input;
		Vertex	temp,temp1,temp2,temp3;
		//Vertex  orig,xunit,yunit,zunit;
		float orig[3], dir[3], vert0[3], vert1[3], vert2[3];
		float t, u, v;
		
		orig[0] = 0; orig[1] = 0; orig[2] = 0;
		dir[0] = viewX; dir[1] = viewY; dir[2] = -pDisplayCamera->ViewPlane;


		//assume the ray is calculated at this point:
		for(int i = 0; i < pDisplayScene->ObjectCount; i++)
		{
			for(int j = 0; j < pDisplayScene->pObjectList[i].FaceCount; j++)
			{
				input = new Vertex[3];
				input[0] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v1];
				input[1] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v2];
				input[2] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v3];

				for (int k=0; k<3; k++){
					temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,input[k]);
					input[k]	= Transform(pDisplayCamera->ViewingMatrix,temp);
				}
				vert0[0] = input[0].x; vert0[1] = input[0].y; vert0[2] = input[0].z;
				vert1[0] = input[1].x; vert1[1] = input[1].y; vert1[2] = input[1].z;
				vert2[0] = input[2].x; vert2[1] = input[1].y; vert2[2] = input[2].z;

				if(intersect_triangle(orig, dir, vert0, vert1, vert2, &t, &u, &v) == 1)
				{
					SelectedObject = i;
					delete [] input;
					input = NULL;
					glutPostRedisplay();
					return;
				}

				delete [] input;
				input = NULL;
			}
		}

		glutPostRedisplay();
	}
}

void MotionFunc(int x, int y)
{
	if(MouseLeft && !SelectionMode)
		pDisplayCamera->Pan(((float) x - MouseX)/128, ((float) y - MouseY)/128);

    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'O':
	case 'o':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(M_PI/32, 0, 0);
		break;
	case 'I':
	case 'i':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(-M_PI/32, 0, 0);
		break;
	case 'L':
	case 'l':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, M_PI/32, 0);
		break;
	case 'K':
	case 'k':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, -M_PI/32, 0);
		break;
	case ',':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, 0, M_PI/32);
		break;
	case 'M':
	case 'm':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, 0, -M_PI/32);
		break;
	case '6':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(M_PI/32, 0, 0);
		break;
	case '4':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(-M_PI/32, 0, 0);
		break;
	case '8':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, M_PI/32, 0);
		break;
	case '2':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, -M_PI/32, 0);
		break;
	case '9':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, 0, M_PI/32);
		break;
	case '1':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, 0, -M_PI/32);
		break;
	case ']':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(M_PI/32, 0, 0);
		break;
	case '[':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(-M_PI/32, 0, 0);
		break;
	case 39:	// Apostrophe
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, M_PI/32, 0);
		break;
	case 59:	// Semicolon
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, -M_PI/32, 0);
		break;
	case '/':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, 0, M_PI/32);
		break;
	case '.':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, 0, -M_PI/32);
		break;
	case '=':
		pDisplayScene->pObjectList[SelectedObject].LocalScale(1.05);
		break;
	case '-':
		pDisplayScene->pObjectList[SelectedObject].LocalScale(0.95);
		break;
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'B':
	case 'b':
		ShowBoundingBoxes = !ShowBoundingBoxes;
		break;
	case 'N':
	case 'n':
		SelectionMode = !SelectionMode;
		if(SelectionMode)
			cout << "Selection Mode" << endl;
		else
			cout << "Camera Mode" << endl;
		break;
	case 'P':
	case 'p':
		PerspectiveMode = !PerspectiveMode;
		if(PerspectiveMode)
			glutSetWindowTitle("Assignment 4 (Perspective)");
		else
			glutSetWindowTitle("Assignment 4 (Orthogonal)");
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'Y':
	case 'y':
		pDisplayCamera->MoveView(0.5);
		break;
	case 'U':
	case 'u':
		pDisplayCamera->MoveView(-0.5);
		break;
	case 'H':
	case 'h':
		pDisplayCamera->ScaleView(0.95);
		break;
	case 'J':
	case 'j':
		pDisplayCamera->ScaleView(1.05);
		break;
    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    pDisplayScene = new Scene;
	//YOU MAY ENTER YOUR OWN SCENE FILE OR PASS IT AS AN ARGUMENT TO THE PROGRAM
	pDisplayScene->Load("scene2.dat");
	pDisplayCamera = new Camera;
	pDisplayCamera->ViewWidth = (float)WindowWidth/32;
	pDisplayCamera->ViewHeight = (float)WindowHeight/32;

	// Initialize GLUT
    glutInit(&argc, argv);
	glutInitWindowSize(WindowWidth, WindowHeight);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignment 4 (Perspective)");
    glutDisplayFunc(DisplayFunc);
    glutReshapeFunc(ReshapeFunc);
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

    // Switch to main loop
    glutMainLoop();

	return 0;
}