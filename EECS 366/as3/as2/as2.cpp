// The template code for Assignment 2 
//

#include <stdio.h>
#include <stdlib.h>
#include "glut.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <array>
#include "MatrixMath.h"
#include "camera.h"

//typedef std :: array <std :: array <float, 4>,4> MATRIX;

#define ON 1
#define OFF 0

// Global variables
int window_width, window_height;    // Window dimensions
int PERSPECTIVE = OFF;


// Vertex and Face data structure sued in the mesh reader
// Feel free to change them
//typedef struct point {
//  float x,y,z;
//} point;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

//pi
float PI = atan((float)1)*4;

float TRANSLATE_SPEED = .1;
float ROTATE_SPEED = 0.1;
float SCALE_RATIO = 0.1;
float PAN_RATIO = 0.4;
MATRIX M;
float IDENTITY[][4] = {{1,0,0,0},
					   {0,1,0,0},
					   {0,0,1,0},
					   {0,0,0,1}};

Camera camera;
bool camToggle = false;
//mouse movement vars
int lastx=0; int lasty = 0;
int swivelMouse = OFF;
int strafeMouse = OFF;
int viewMouse = OFF;

//what to display
int showAxes = ON;

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
	
	MATRIX view = camera.ViewTransform();

	MATRIX final = multiply(view, M);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(showAxes){
		{
			point o= {0,0,0};
			point x= {1,0,0};
			point y= {0,1,0};
			point z= {0,0,1};
		
			o = multiplyP(final, o);
			x = multiplyP(final, x);
			y = multiplyP(final, y);
			z = multiplyP(final, z);

			//try drawing some axes
			glBegin (GL_LINES);
			glColor3f (0,1,1); // X axis color.
			glVertex3f (o.x,o.y,o.z);
			glVertex3f (x.x,x.y,x.z ); 
			glColor3f (1,0,1); // Y axis color.
			glVertex3f (o.x,o.y,o.z);
			glVertex3f (y.x,y.y,y.z );
			glColor3f (1,1,0); // z axis color.
			glVertex3f (o.x,o.y,o.z);
			glVertex3f (z.x,z.y,z.z ); 
			glEnd();
		}
		{
			point o= {0,0,0};
			point x= {1,0,0};
			point y= {0,1,0};
			point z= {0,0,1};
		
			o = multiplyP(view, o);
			x = multiplyP(view, x);
			y = multiplyP(view, y);
			z = multiplyP(view, z);

			//try drawing some axes
			glBegin (GL_LINES);
			glColor3f (0,1,1); // X axis color.
			glVertex3f (o.x,o.y,o.z);
			glVertex3f (x.x,x.y,x.z ); 
			glColor3f (1,0,1); // Y axis color.
			glVertex3f (o.x,o.y,o.z);
			glVertex3f (y.x,y.y,y.z );
			glColor3f (1,1,0); // z axis color.
			glVertex3f (o.x,o.y,o.z);
			glVertex3f (z.x,z.y,z.z ); 
			glEnd();
		}
	}


	// Draw a blue object
	glColor3f(0,0,1);
	glBegin(GL_TRIANGLES);
	for(int face = 0; face < faces; face++)
	{
		faceStruct myFace = faceList[face];

		point v = multiplyP(final, vertList[myFace.v1]);
		glVertex3f(v.x,v.y,v.z);
		
		v = multiplyP(final, vertList[myFace.v2]);
		glVertex3f(v.x,v.y,v.z);
		
		v = multiplyP(final, vertList[myFace.v3]);
		glVertex3f(v.x,v.y,v.z);
	}


	glEnd();
	

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
		if(state==0)
		{
			swivelMouse=ON;
			strafeMouse=OFF;
			viewMouse=OFF;
		}
		else swivelMouse=OFF;
	}
	else if(button == 2)
	{
		if(state==0)
		{
			swivelMouse=OFF;
			strafeMouse=ON;
			viewMouse=OFF;
		}
		else strafeMouse=OFF;
	}
	else if(button == 1)
	{
		if(state==0)
		{
			swivelMouse=OFF;
			strafeMouse=OFF;
			viewMouse=ON;
		}
		else viewMouse=OFF;
	}
	printf("swivel: %d, strafe: %d, view: %d\n", swivelMouse, strafeMouse, viewMouse);
//	printf("long Angle: %f, lat angle: %f, Distance: %f\n",cameraLongAngle,cameraLatAngle,cameraDistance);
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void	mouseMotion(int x, int y)
{
	if(swivelMouse)
	{
		camera.swivel(x-lastx,y-lasty);
	}
	else if(strafeMouse)
	{
		camera.moveX(PAN_RATIO*(lastx-x));
		camera.moveY(PAN_RATIO*(lasty-y));
	}
	else if(viewMouse)
	{
		camera.moveZ(y-lasty);
	}


	lastx = x;
	lasty = y;
	glutPostRedisplay();
}


void reset()
{
	camera = Camera();
	for (int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j ++)
			M[i][j] = IDENTITY[i][j];
}

// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void	keyboard(unsigned char key, int x, int y)
{
	point p = {0,0,0};
	MATRIX V = camera.ViewTransform();
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
	case 'a':
	case 'A':
		if(showAxes) showAxes = OFF;
		else showAxes = ON;
		break;
	case 'r':
		reset();
		break;
	case 'q':
	case 'Q':
		exit(0);
		break;
	case 'c':
		if(camToggle)
		{
			camera.lookAt(p);
		}
		else
		{
			//todo: does this work
			p.x = M[0][3];	p.y = M[1][3];	p.z = M[2][3];
			camera.lookAt(p);
		}
		camToggle = !camToggle;
		break;
	//world transformations
	case '4':
		p.x = -TRANSLATE_SPEED;
		M = translate(M, p, true);
		break;
	case '6':
		p.x = TRANSLATE_SPEED;
		M = translate(M, p, true);
		break;
	case '8':
		p.y = TRANSLATE_SPEED;
		M = translate(M, p, true);
		break;
	case '2':
		p.y = -TRANSLATE_SPEED;
		M = translate(M, p, true);
		break;
	case '9':
		p.z = TRANSLATE_SPEED;
		M = translate(M, p, true);
		break;
	case '1':
		p.z = -TRANSLATE_SPEED;
		M = translate(M, p, true);
		break;
	case '[':
		M = rotateX(M, -ROTATE_SPEED, true);
		break;
	case ']':
		M = rotateX(M, ROTATE_SPEED, true);
		break;
	case ';':
		M = rotateY(M, -ROTATE_SPEED, true);
		break;
	case '\'':
		M = rotateY(M, ROTATE_SPEED, true);
		break;
	case '.':
		M = rotateZ(M, -ROTATE_SPEED, true);
		break;
	case '/':
		M = rotateZ(M, ROTATE_SPEED, true);
		break;
	case '=':
		M = scale(M, 1+SCALE_RATIO);
		break;
	case '-':
		M = scale(M, 1-SCALE_RATIO);
		break;

	//object transformations
	case 'i':
		M = rotateX(M, -ROTATE_SPEED, false);
		break;
	case 'o':
		M = rotateX(M, ROTATE_SPEED, false);
		break;
	case 'k':
		M = rotateY(M, -ROTATE_SPEED, false);
		break;
	case 'l':
		M = rotateY(M, ROTATE_SPEED, false);
		break;
	case 'm':
		M = rotateZ(M, -ROTATE_SPEED, false);
		break;
	case ',':
		M = rotateZ(M, ROTATE_SPEED, false);
		break;
	case 'd':
		printf("\nM:\n");
		for(int i = 0; i < 4; i ++){
			printf("%d, %d, %d, %d\n", M[i][0], M[i][1], M[i][2], M[i][3]);
		}
		
		
		printf("\nV:\n");
		for(int i = 0; i < 4; i ++){
			printf("%d, %d, %d, %d\n", V[i][0], V[i][1], V[i][2], V[i][3]);
		}
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}

// Here's the main
int main(int argc, char* argv[])
{

	reset();

//	cameraDistance=5;
	meshReader("helicopter.obj", 1);
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
