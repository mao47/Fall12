/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 6-Local Illumination and Shading

*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>
#include "glut.h"
#include <windows.h>
//#include "glu.h"
#include "gl.h"
#include "glprocs.h"


#define PI 3.14159265359

using namespace std;

//Illimunation and shading related declerations
char *shaderFileRead(char *fn);
GLuint vertex_shader,fragment_shader,p;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;


//Projection, camera contral related declerations
int WindowWidth,WindowHeight;
bool LookAtObject = false;
bool ShowAxes = true;



float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;


float lpx, lpy, lpz; //light position
float lir, lig, lib; //light intensity
int colorIndex;

//phong surface parameters:
float ambR, ambG, ambB;
float difR, difG, difB;
float speR, speG, speB;
int specExp;

//cook-torrance surface parameters:
float ctaR, ctaG, ctaB;//cook-torrance ambient;
float ctrdR, ctrdG, ctrdB;//diffues reflection;
float ctfR, ctfG, ctfB;//fresnel function
float m1, w1, m2, w2; //beckman parameters/weights;




void DisplayFunc(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  0,0,0,
			  0,0,1);
	glEnable(GL_DEPTH_TEST);
	
	//I think this only needs to occur when the shader changes for most variables
	GLint specExpLoc = glGetUniformLocationARB(p, "specEx");
	glUniform1iARB(specExpLoc, specExp);
	GLint ambientLoc = glGetUniformLocationARB(p, "ambient");
	glUniform3fARB(ambientLoc, ambR, ambG, ambB);
	GLint diffuseLoc = glGetUniformLocationARB(p, "diffuse");
	glUniform3fARB(diffuseLoc, difR, difG, difB);
	GLint specLoc = glGetUniformLocationARB(p, "specular");
	glUniform3fARB(specLoc, speR, speG, speB);

	GLint lightPosLoc = glGetUniformLocationARB(p, "LightPosition");
	if(lightSource == 0)
	{
		lpx = CameraRadius*cos(CameraTheta)*sin(CameraPhi);
		lpy = CameraRadius*sin(CameraTheta)*sin(CameraPhi);
		lpz = CameraRadius*cos(CameraPhi);
	}
	glUniform3fARB(lightPosLoc, lpx, lpy, lpz);

	GLint lightIntensityLoc = glGetUniformLocationARB(p, "lightIntensity");
	glUniform3fARB(lightIntensityLoc, lir, lig, lib);
	
	
	glutSolidTeapot(1);
	glutSwapBuffers();
}

void ReshapeFunc(int x,int y)
{
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}


void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y)
{
	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
	}
	if(MouseRight)
	{
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}


void setShaders() {

	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	

	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("gourard_phong.vert");//"gourard_phong.vert");//"sampleshader.vert");
	fs = shaderFileRead("gourard_phong.frag");//"gourard_phong.frag");//"sampleshader.frag");

	const char * vv = vs;
	const char * ff = fs;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSourceARB(vertex_shader, 1, &vv,NULL);
	glShaderSourceARB(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShaderARB(vertex_shader);
	glCompileShaderARB(fragment_shader);


	//create an empty program object to attach the shader objects
	p = glCreateProgramObjectARB();

	//attach the shader objects to the program object
	glAttachObjectARB(p,vertex_shader);
	glAttachObjectARB(p,fragment_shader);

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteObjectARB(vertex_shader);
	glDeleteObjectARB(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling 
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgramARB(p);

	//Start to use the program object, which is the part of the current rendering state
	glUseProgramObjectARB(p);

}

void assertLightColor() {
	if(lightSource == 1) {
		if(colorIndex == 0)
			lir = lig = lib = 1.0;
		else if(colorIndex == 1)
		{
			lir = 1.0;
			lig = lib = 0.0;
		}
		else if(colorIndex == 2) {
			lig = 1.0;
			lir = lib = 0.0;
		}
		else
		{
			lib = 1.0;
			lir = lig = 0.0;
		}
	}
	else
		lir = lig = lib = 1.0;
}

void toggleLightSource() {
	
	if(lightSource == 0)
	{
		lightSource = 1;
		lpx = lpy = lpz = 1.0;
		
	}
	else
	{
		lightSource = 0;
		lpx = lpy  = lpz = 7.0;

	}
	assertLightColor();
}

//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'w':
	case 'W':
		if (illimunationMode == 0)
		{
			illimunationMode = 1;
		}
		else
		{
			illimunationMode = 0;
		}
		break;
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode =1;
		}
		else
		{
			shadingMode =0;
		}
		break;
	case 'd':
	case 'D':
		toggleLightSource();
		break;
	case 'f':
	case 'F':
		if (lightSource == 1)
		{
			colorIndex = (colorIndex + 1) % 4;
			assertLightColor();
			printf("%d\n", colorIndex);
			//change color of the secondary light source at each key press, 
			//light color cycling through pure red, green, blue, and white.
		}
		break;

    default:
		break;
    }

	glutPostRedisplay();
}



void loadMaterial(char *file) {
	FILE* matFile = fopen(file, "r");
	if(!matFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	if(!feof(matFile))
	{
		fscanf(matFile, "P %f %f %f %f %f %f %f %f %f %f\n", &ambR, &ambG, &ambB,
			&difR, &difG, &difB, &speR, &speG, &speB, &specExp);
	}


	if(!feof(matFile))
	{
		fscanf(matFile, "C %f %f %f %f %f %f %f %f %f %f %f %f %f\n", &ctaR, &ctaG, &ctaB,
			&ctrdR, &ctrdG, &ctrdB, &ctfR, &ctfG, &ctfB, &m1, &w1, &m2, &w2);
	}

}

int main(int argc, char **argv) 
{			  
	lpx = lpy = lpz = 7.0;
	lir = lig = lib = 1.0;
	colorIndex = 0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Assignment 6");



	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);



	

	setShaders();

	loadMaterial("material.dat");


	glutMainLoop();

	return 0;
}


//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {


	FILE *fp = fopen(fn,"r");
	if(!fp)
	{
		cout<< "Failed to load " << fn << endl;
		return " ";
	}
	else
	{
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);

		if (count > 0) 
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}
