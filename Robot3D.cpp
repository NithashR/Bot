//Nithash Rajendram
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "cube.h"
#include "QuadMesh.h"

const int vWidth = 650;    // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels


float robotradius = 1.5;
float robotheight = 3.0;

// Control Robot body rotation on base
float robotAngle = 0.0;
float xpos = 0.0;
float zpos = 0.0;

// Control arm rotation
float wheelAngle = 0.0;
float cannonAngle = 90.0;
float cannonAnglex = 0.0;

// Spin Cube Mesh
float cubeAngle = 0.0;

GLfloat blue_ambient[] = { 0.02f, 0.0f, 0.8f, 1.0f };
GLfloat blue_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat blue_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat blue_shininess[] = { 90.0F };


GLfloat red_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat red_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat red_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat red_shininess[] = { 32.0F };

GLfloat grey_ambient[] = { 0.2f, 0.2f, 0.2f, 0.9f };
GLfloat grey_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat grey_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat grey_shininess[] = { 100.0F };

GLfloat black_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat black_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat black_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat black_shininess[] = { 100.0F };


// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };


// Mouse button
int currentButton;

// A template cube mesh
CubeMesh* cubeMesh = createCubeMesh();

// A flat open mesh
QuadMesh* groundMesh = NULL;

// Structure defining a bounding box, currently unused
typedef struct BoundingBox {
	VECTOR3D min;
	VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 16;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler(int param);
void drawRobot();
void drawBody();
void drawHead();
void drawWheel();
void drawLegs();
void drawcannons();

int main(int argc, char** argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("3D Hierarchical Example");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// Other initializatuion
	// Set up ground quad mesh
	VECTOR3D origin = VECTOR3D(-16.0f, 0.0f, 16.0f);
	VECTOR3D dir1v = VECTOR3D(1.0f, 0.0f, 0.0f);
	VECTOR3D dir2v = VECTOR3D(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 32.0);
	groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1v, dir2v);

	VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
	VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
	VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	drawRobot();
	// Draw ground
	glPushMatrix();
	glTranslatef(0.0, -10.0, 0.0);
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}

void drawRobot()
{
	glPushMatrix();
	// spin robot on base. 
	glTranslatef(xpos, 0, zpos);
	glRotatef(robotAngle, 0.0, 1.0, 0.0);

	drawBody();
	drawHead();
	drawLegs();
	drawWheel();
	drawcannons();

	glTranslatef(robotheight, robotheight * 1.5, robotheight);

	glPushMatrix();
	glTranslatef(-robotheight, -robotheight * 1.5, 0);
	glRotatef(cubeAngle, 0.0, 1.0, 0.0);
	glTranslatef(robotheight, robotheight * 1.5, 0);
	drawCubeMesh(cubeMesh);
	glPopMatrix();

	glPopMatrix();

	// don't want to spin fixed base

	glPopMatrix();
}


void drawBody()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, blue_shininess);

	//body
	glPushMatrix();
	glTranslatef(0, 0, 0); // this will be done last
	glScalef(robotradius, robotheight, robotradius);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 1.0, 20, 10);
	glPopMatrix();

	//bottom
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glScalef(robotradius, robotradius, robotradius);
	glutSolidSphere(1.0, 30, 10);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, grey_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, grey_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, grey_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, grey_shininess);

	//leg axel
	glPushMatrix();
	glTranslatef(robotheight, robotradius, 0); // this will be done last
	glScalef(robotheight*2, robotradius/2, robotradius/2);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 1.0, 20, 10);
	glPopMatrix();
}

void drawHead()
{
	// Set robot material properties per body part. Can have seperate material properties for each part
	glMaterialfv(GL_FRONT, GL_AMBIENT, blue_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, blue_shininess);

	glPushMatrix();
	glTranslatef(0, robotheight, 0);
	glScalef(robotradius, robotradius, robotradius);
	glutSolidSphere(1.0, 30, 10);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, grey_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, grey_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, grey_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, grey_shininess);

	//face
	glPushMatrix();
	glTranslatef(0, robotheight, robotradius/2); // this will be done last
	glScalef(robotradius, robotradius, robotradius);
	glutSolidCube(1.0);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, red_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, red_shininess);

	glPushMatrix();
	glTranslatef(0, robotheight, robotradius*0.4); // this will be done last
	glScalef(robotradius*1.5, robotradius*0.8, robotradius);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, robotheight, robotradius *0.7); // this will be done last
	glScalef(robotradius/2, robotradius/2, robotradius);
	glutSolidCube(1.0);
	glPopMatrix();

}

void drawWheel()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, grey_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, grey_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, grey_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, grey_shininess);

	//wheel axel
	glPushMatrix();
	glTranslatef(robotheight, -robotheight*2, 0); // this will be done last
	glScalef(robotheight * 2, robotradius/2, robotradius / 2);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	gluCylinder(gluNewQuadric(), 1.0, 1.0, 1.0, 20, 10);
	glPopMatrix();


	//wheel left
	glPushMatrix();
	glTranslatef(robotheight*0.5, -robotheight * 2, 0); // this will be done last
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	gluDisk(gluNewQuadric(), 0.0, robotheight, 20, 10);
	glPopMatrix();

	//wheel right
	glPushMatrix();
	glTranslatef(-robotheight * 0.5, -robotheight * 2, 0); // this will be done last
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	gluDisk(gluNewQuadric(), 0.0, robotheight, 20, 10);
	glPopMatrix();

	//tire tread
	glPushMatrix();
	glTranslatef(0, -robotheight * 2, 0); // this will be done last
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	// rotate gun
	glRotatef(wheelAngle, 0.0, 0.0, 1.0);

	glutWireTorus(robotradius, robotheight, 20, 20);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT, black_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, black_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, black_shininess);

	//tire
	glPushMatrix();
	glTranslatef(0, -robotheight * 2, 0); // this will be done last
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glutSolidTorus(robotradius, robotheight, 20, 20);
	glPopMatrix();

}

void drawLegs()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, red_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, red_shininess);

	glPushMatrix();
	// Position arm with respect to parent body

	//left upper leg
	glPushMatrix();
	glTranslatef(robotheight*0.8, 0, -robotradius); // this will be done last
	glRotatef(45.0, 1.0, 0.0, 0.0);
	glScalef(robotradius/2, robotheight*2.5, robotradius);
	glutSolidCube(1.0);
	glPopMatrix();

	//right upper leg
	glPushMatrix();
	glTranslatef(-robotheight * 0.8, 0, -robotradius); // this will be done last
	glRotatef(45.0, 1.0, 0.0, 0.0);
	glScalef(robotradius / 2, robotheight * 2.5, robotradius);
	glutSolidCube(1.0);
	glPopMatrix();

	//left lower leg
	glPushMatrix();
	glTranslatef(robotheight * 0.8, -robotheight*1.5, -robotradius); // this will be done last
	glRotatef(-45.0, 1.0, 0.0, 0.0);
	glScalef(robotradius / 2, robotheight * 2.5, robotradius);
	glutSolidCube(1.0);
	glPopMatrix();

	//right lower leg
	glPushMatrix();
	glTranslatef(-robotheight * 0.8, -robotheight * 1.5, -robotradius); // this will be done last
	glRotatef(-45.0, 1.0, 0.0, 0.0);
	glScalef(robotradius / 2, robotheight * 2.5, robotradius);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawcannons()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, red_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, red_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, red_shininess);


	//left connon connection
	glPushMatrix();
	glTranslatef(robotradius*0.9, 0, robotradius * 0.5);
	glScalef(robotradius/2, robotradius/2, robotradius/2);
	glutSolidSphere(1.0, 30, 10);
	glPopMatrix();

	//right connon connection
	glPushMatrix();
	glTranslatef(-robotradius * 0.9, 0, robotradius * 0.5);
	glScalef(robotradius / 2, robotradius / 2, robotradius / 2);
	glutSolidSphere(1.0, 30, 10);
	glPopMatrix();

	//cannon
	glMaterialfv(GL_FRONT, GL_AMBIENT, black_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, black_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, black_shininess);

	//left cannon
	glPushMatrix();
	glTranslatef(robotradius * 0.9, 1, robotheight / 2);
	// rotate gun
	glTranslatef(-1, -1 , 0.0);
	glRotatef(cannonAngle, 0.1, 0.0, 0.0);
	glRotatef(cannonAnglex, 0.0, 0.0, 1);
	glTranslatef(1, 1, 0.0);
	// build gun
	glScalef(robotradius/4, robotheight, robotradius/4);
	glutSolidCube(1.0);
	glPopMatrix();

	//right cannon
	glPushMatrix();
	glTranslatef(-robotradius * 0.9, 1, robotheight / 2);
	// rotate gun
	glTranslatef(-1, -1, 0.0);
	glRotatef(cannonAngle, 0.1, 0.0, 0.0);
	glRotatef(cannonAnglex, 0.0, 0.0, 0.1);
	glTranslatef(1, 1, 0.0);
	// build gun
	glScalef(robotradius / 4, robotheight, robotradius / 4);
	glutSolidCube(1.0);
	glPopMatrix();



}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

bool stop = false;

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		if (cannonAngle >= 40)
		{
			cannonAngle -= 2.0;
		}
		break;
	case 's':
		if (cannonAngle <= 120)
		{
			cannonAngle += 2.0;
		}
		break;
	case 'd':
		if (cannonAnglex <= 20)
		{
			cannonAnglex += 2.0;
		}
		break;
	case 'a':
		if (cannonAnglex >= -20)
		{
			cannonAnglex -= 2.0;
		}
		break;

	case 'm':
		glutTimerFunc(10, animationHandler, 0);
		break;
	case 'M':
		stop = true;
		break;
	case 'Q':
		exit(0);
	}

	glutPostRedisplay();   // Trigger a window redisplay
}
int count = 90;
int temp = 2;

void animationHandler(int param)
{
	if (!stop)
	{
		if (count == 120)
		{
			temp = -2;
		}
		else if (count == 40)
		{
			temp = 2;
		}
				
		cannonAngle += temp;
		count += temp;

		cubeAngle += 2.0;

		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);


	}
}



// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{
		printf("------------------------- CPS 511 Assignment 1------------------------ -\n");
		printf("------------------------Bot buttons and purposes---------------------- -\n");
		printf("Arrow Up : Used to make robot move forward.\n");
		printf("Arrow Down : Used to make robot move backwards.\n");
		printf("Left Arrow : Rotates robot coutnerclockwise.\n");
		printf("Right Arrow : Rotates robot clockwise.\n");
		printf("w : Moves cannons up.\n");
		printf("s : Moves cannons down.\n");
		printf("a : Moves cannons to the left.\n");
		printf("d : Moves cannons to the right.\n");
		printf("m : Moves cannons up and down automatically.\n");
		printf("M : Stops auto movement of cannons.\n");
		printf("Q : Quit.\n");
	}
	// Do transformations with arrow keys
	else if (key == GLUT_KEY_LEFT)   
	{
		robotAngle += 2.0;
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		robotAngle -= 2.0;
	}
	else if (key == GLUT_KEY_UP)
	{
		xpos += sin(3.14 * robotAngle / 180) * 0.5;
		zpos += cos(3.14 * robotAngle / 180) * 0.5;
		wheelAngle -= 2.0;
	}
	else if (key == GLUT_KEY_DOWN)
	{
		xpos -= sin(3.14 * robotAngle / 180) * 0.5;
		zpos -= cos(3.14 * robotAngle / 180) * 0.5;
		wheelAngle += 2.0;
	}
	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			break;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

