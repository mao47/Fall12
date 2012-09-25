// The template code for Assignment 2 
//

#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#define ON 1
#define OFF 0


// Global variables
int window_width, window_height;    // Window dimensions
int PERSPECTIVE = OFF;


// Vertex and Face data structure sued in the mesh reader
// Feel free to change them
typedef struct _point {
  float x,y,z;
} point;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

//pi
float PI = atan((float)1)*4;

//mouse movement vars
int lastx=0; int lasty = 0;
int panMouse = OFF;
int zoomMouse = OFF;

//what to display
int showAxes = ON;
int showObj = ON;

//camera vars
float cameraDistance = 5;
float cameraLatAngle = 1.5;
float cameraLongAngle = 1.5;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List

// The mesh reader itself
// It can read *very* simple obj files
void meshReader (char *filename,int sign)
{
  float x,y,z,len;
  int i;
  char letter;
  point v1,v2,crossP;
  int ix,iy,iz;
  int *normCount;
  FILE *fp;

  fp = fopen(filename, "r");
  if (fp == NULL) { 
    printf("Cannot open %s\n!", filename);
    exit(0);
  }

  // Count the number of vertices and faces
  while(!feof(fp))
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      if (letter == 'v')
	{
	  verts++;
	}
      else
	{
	  faces++;
	}
    }

  fclose(fp);

  printf("verts : %d\n", verts);
  printf("faces : %d\n", faces);

  // Dynamic allocation of vertex and face lists
  faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
  vertList = (point *)malloc(sizeof(point)*verts);
  normList = (point *)malloc(sizeof(point)*verts);

  fp = fopen(filename, "r");

  // Read the veritces
  for(i = 0;i < verts;i++)
    {
      fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
      vertList[i].x = x;
      vertList[i].y = y;
      vertList[i].z = z;
    }

  // Read the faces
  for(i = 0;i < faces;i++)
    {
      fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
      faceList[i].v1 = ix - 1;
      faceList[i].v2 = iy - 1;
      faceList[i].v3 = iz - 1;
    }
  fclose(fp);


  // The part below calculates the normals of each vertex
  normCount = (int *)malloc(sizeof(int)*verts);
  for (i = 0;i < verts;i++)
    {
      normList[i].x = normList[i].y = normList[i].z = 0.0;
      normCount[i] = 0;
    }

  for(i = 0;i < faces;i++)
    {
      v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
      v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
      v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
      v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
      v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
      v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

      crossP.x = v1.y*v2.z - v1.z*v2.y;
      crossP.y = v1.z*v2.x - v1.x*v2.z;
      crossP.z = v1.x*v2.y - v1.y*v2.x;

      len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

      crossP.x = -crossP.x/len;
      crossP.y = -crossP.y/len;
      crossP.z = -crossP.z/len;

      normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
      normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
      normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
      normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
      normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
      normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
      normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
      normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
      normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
      normCount[faceList[i].v1]++;
      normCount[faceList[i].v2]++;
      normCount[faceList[i].v3]++;
    }
  for (i = 0;i < verts;i++)
    {
      normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
      normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
      normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
    }

}



// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void	display(void)
{
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   

	glLoadIdentity();

	// Set the camera position, orientation and target
	gluLookAt(
		cameraDistance*cos(cameraLongAngle)*sin(cameraLatAngle),	//x pos
		cameraDistance*sin(cameraLongAngle)*sin(cameraLatAngle),	//y pos
		cameraDistance*cos(cameraLatAngle),							//z pos
		0,0,0,														//target (origin)
		0,0,1														//"up" vector (z)
		);


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(showAxes){
		//try drawing some axes
		glBegin (GL_LINES);
		glColor3f (0,1,1); // X axis color.
		glVertex3f (0,0,0);
		glVertex3f (1,0,0 ); 
		glColor3f (1,0,1); // Y axis color.
		glVertex3f (0,0,0);
		glVertex3f (0,1,0 );
		glColor3f (1,1,0); // z axis color.
		glVertex3f (0,0,0);
		glVertex3f (0,0,1 ); 
		glEnd();
	}

	if(showObj){
		// Draw a blue object
		glColor3f(0,0,1);
		glBegin(GL_TRIANGLES);
		for(int face = 0; face < faces; face++)
		{
			faceStruct myFace = faceList[face];

			point v = vertList[myFace.v1];
			glVertex3f(v.x,v.y,v.z);
		
			v = vertList[myFace.v2];
			glVertex3f(v.x,v.y,v.z);
		
			v = vertList[myFace.v3];
			glVertex3f(v.x,v.y,v.z);
		}


		glEnd();
	}

    // (Note that the origin is lower left corner)
    // (Note also that the window spans (0,1) )
    // Finish drawing, update the frame buffer, and swap buffers
    glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void	resize(int x,int y)
{
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    if (PERSPECTIVE) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
    }
    printf("Resized to %d %d\n",x,y);
}


// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseButton(int button,int state,int x,int y)
{
	lastx = x;
	lasty = y;
	if(button == 0) //left button: pan
	{
		if(state==0) panMouse=ON;
		else panMouse=OFF;
	}
	else if(button == 2)
	{
		if(state==0) zoomMouse=ON;
		else zoomMouse=OFF;
	}
	printf("long Angle: %f, lat angle: %f, Distance: %f\n",cameraLongAngle,cameraLatAngle,cameraDistance);
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
	if(panMouse)
	{
		cameraLongAngle -= 0.01 * (x-lastx);
		if(cameraLongAngle < 0) cameraLongAngle += 2*PI;
		else if(cameraLongAngle > 2*PI) cameraLongAngle -= 2*PI;

		cameraLatAngle -= 0.005 * (y-lasty);
		if(cameraLatAngle < 0.1) cameraLatAngle = 0.1;
		else if (cameraLatAngle > PI-.1) cameraLatAngle = PI-.1;
	}
	if(zoomMouse)
	{
		cameraDistance += 0.05 * (y-lasty);
		if (cameraDistance < .1) cameraDistance = .1;
	}
	lastx = x;
	lasty = y;
	glutPostRedisplay();
}


// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
    switch(key) {
    case '':                           /* Quit */
		exit(1);
		break;
    case 'p':
    case 'P':
	// Toggle Projection Type (orthogonal, perspective)
        if(PERSPECTIVE) {
			// Orthogonal Projection 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
			glutSetWindowTitle("Assignment 2 Template (orthogonal)");
			PERSPECTIVE = OFF;
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		} else {
			// Perpective Projection 
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
			glutSetWindowTitle("Assignment 2 Template (perspective)");
			PERSPECTIVE = ON;
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		break;
	case 's':
	case 'S':
		if(showObj) showObj = OFF;
		else showObj = ON;
		break;
	case 'a':
	case 'A':
		if(showAxes) showAxes = OFF;
		else showAxes = ON;
		break;
	case 'q':
	case 'Q':
		exit(0);
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}


// Here's the main
int main(int argc, char* argv[])
{

	meshReader("helicopter.obj", 1);

	cameraDistance=5;
	
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignment 2 Template (orthogonal)");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);

    // Switch to main loop
    glutMainLoop();
    return 0;        
}
