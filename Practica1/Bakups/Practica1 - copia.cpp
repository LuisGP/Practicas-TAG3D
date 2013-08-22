/*
 *  Practica1.cpp
 *  Practica1
 *
 *  Created by Alejandro Ribao and Luis Gasco on 11/03/08.
 *  2008
 *  Based on "Práctica 3 GPGPU C code"
 *
 */

#include <stdio.h>    /* for printf and NULL */
#include <stdlib.h>   /* for exit */
#include <math.h>     /* for sqrt, sin, and cos */
#include <assert.h>   /* for assert */
#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "perlin.h"

/* An OpenGL 1.2 define */
#define GL_CLAMP_TO_EDGE                    0x812F

/* A few OpenGL 1.3 defines */
#define GL_TEXTURE_CUBE_MAP                 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP         0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X      0x8515

// Algunos defines para navegacion
#define DEF_dollyStepSize                   1.0
#define DEF_angleSensitivity                1.0

/* Forward declared GLUT callbacks registered by main. */
static void display(void);
static void keyboard(unsigned char c, int x, int y);
static void menu(int item);
void specialFunc( int key, int x, int y );
void enterMouseDrag( int x, int y );
void exitMouseDrag( int x, int y );
void mouseFunc( int button, int state, int x, int y );
void allMotionFunc( int x, int y );
void applyBumpMapping();

static const char *myProgramName = "Practica1";

// La vista
int fullscreen;
int	viewPortCenterX;
int	viewPortCenterY;

GLfloat	camX;
GLfloat	camY;
GLfloat	camZ;
GLfloat	camYaw;
GLfloat	camPitch;

// Para el ratón
int	isInMouseDrag;
int	oldCursorID;
int	oldCursorX;
int	oldCursorY;

int** alturas;
rgbint* paleta;
rgbfloat* paletaf;

GLfloat luzambiente[]= { 1, 1, 1, 1.0 };	// Definimos valores de luz ambiente

static float M_PI = 3.141592653;

enum{
	ALTO = 256,
	ANCHO = 256,
	NIVEL_LAGO = 10
};

// GL_POINT
// GL_FILL
// GL_LINE
enum{
	glLINE,
	glPOINT,
	glFLAT,
	glSMOOTH
};

int modoRender = glLINE;
int real_renderMode = GL_LINE;

void init(){
	glLightModelfv (GL_LIGHT_MODEL_AMBIENT, luzambiente);
	glEnable(GL_LIGHTING);

	paleta = loadPalInt("paletas/landcolor.ppm", 256);
	paletaf = intPal2Float(paleta, 256);
	applyBumpMapping();
}

void applyBumpMapping(){
	int i, j;
	int maximo = -(((unsigned int)-1)/2);
	int minimo = 36738;
	alturas = genPerlinNoise(256, 7, 30, 1);

	for(i = 0; i < ALTO; i++){
		for(j = 0; j < ANCHO; j++){
			if(maximo < alturas[i][j])
				maximo = alturas[i][j];
			if(minimo > alturas[i][j])
				minimo = alturas[i][j];
		}
	}

	maximo -= minimo;
	// "Normalizamos"
	for(i = 0; i < ALTO; i++){
		for(j = 0; j < ANCHO; j++){
			alturas[i][j] = alturas[i][j] - minimo; // Bajamos la altura de todo
			if(NIVEL_LAGO >= alturas[i][j]) // El lago
				alturas[i][j] = NIVEL_LAGO;
		}
	}

	camY = maximo;
}

void reshape(int w, int h)
{
 if (!h)
	return;
 glViewport(0, 0,  (GLsizei) w, (GLsizei) h);
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 0.1, 1000.0);
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 viewPortCenterX = w / 2;
 viewPortCenterY = h / 2;
}

int main(int argc, char **argv)
{
  // La vista
  fullscreen = 0;
  viewPortCenterX = -1;
  viewPortCenterY = -1;

  camX = 0.0;
  camY = 8.0;
  camZ = 0.0;
  camYaw = 0.0;
  camPitch = 0.0;

  // Para el ratón
  isInMouseDrag = 0;
  oldCursorID = 0;
  oldCursorX = 0;
  oldCursorY = 0;
  
  init();

  glutInitWindowSize(800, 600);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInit(&argc, argv);

  glutCreateWindow(myProgramName);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  // Navegacion
  glutKeyboardFunc(keyboard);
  glutSpecialFunc( specialFunc );
  glutMouseFunc( mouseFunc );
  glutMotionFunc( allMotionFunc );
  glutPassiveMotionFunc( allMotionFunc );

  glClearColor(0.1, 0.3, 0.6, 0.0);  /* Blue background */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(
    60.0,   /* Field of view in degree */
    1.0,    /* Aspect ratio */ 
    0.1,    /* Z near */
    1000.0); /* Z far */
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);
  //glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
 
  glEnable(GL_TEXTURE_2D);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); /* Tightly packed texture data. */

  //glBindTexture(GL_TEXTURE_2D, TO_NORMAL_MAP);

  // Cosas de texturas

  //glBindTexture(GL_TEXTURE_CUBE_MAP, TO_NORMALIZE_VECTOR_CUBE_MAP);
  
  // Mas texturas

  // Aqui venia lo de Cg

  glutCreateMenu(menu);
  glutAddMenuEntry("WIREFRAME", glLINE);
  glutAddMenuEntry("POINTS", glPOINT);
  glutAddMenuEntry("FLAT", glFLAT);
  glutAddMenuEntry("SMOOTH", glSMOOTH);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
  return 0;
}

/* Draw the scene. */
void
draw()
{
  int i, j, alt;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
 
  glRotatef( -camPitch, 1.0, 0.0, 0.0 );
  glRotatef( -camYaw, 0.0, 1.0, 0.0 );
  glTranslatef( -camX, -camY, -camZ );

  glPolygonMode(GL_FRONT_AND_BACK, real_renderMode);
  
  alt = 0;
  glNormal3f(1,-1,1);            // <--- OJO, ES TEMPORAL

  // LINE o POINT
  if(modoRender < 2){ 
     for(i = -127; i < 128; i++){
		 glBegin(GL_TRIANGLE_STRIP);  
	     for(j = -127; j < 128; j++){
			 alt = alturas[i+127][j+128];
			 setFloatColorMaterial (paletaf[alt], 0);
		     glVertex3f(i-1,alt,j);
			 alt = alturas[i+128][j+128];
			 setFloatColorMaterial (paletaf[alt], 0);
		     glVertex3f(i,alt,j);
	     }
	     glEnd();
     }
  }else{
	  if(modoRender == glFLAT){ 
          for(i = -127; i < 128; i++){
		     glBegin(GL_TRIANGLE_STRIP); 
	         for(j = -127; j < 128; j++){
				alt = alturas[i+127][j+128];
				setFloatColorMaterial (paletaf[alt], 1);
			    //glNormal3f(0,1,0);            // <--- OJO, ES TEMPORAL
		        glVertex3f(i-1,alt,j);
				alt = alturas[i+128][j+128];
				setFloatColorMaterial (paletaf[alt], 1);
		        glVertex3f(i,alt,j);
	         }
			 glEnd();
          }
	  }
	  if(modoRender == glSMOOTH){  
          for(i = -127; i < 128; i++){
		     glBegin(GL_TRIANGLE_STRIP);
	         for(j = -127; j < 128; j++){
				alt = alturas[i+127][j+128];
				setFloatColorMaterial (paletaf[alt], 0);
			    //glNormal3f(0,1,0);            // <--- OJO, ES TEMPORAL
		        glVertex3f(i-1,alt,j);
				alt = alturas[i+128][j+128];
				setFloatColorMaterial (paletaf[alt], 0);
				//glNormal3f(0,1,0);            // <--- OJO, ES TEMPORAL
		        glVertex3f(i,alt,j);
	         }
             glEnd();
          }
	  }
  }

  glPopMatrix();

  glFlush();
}

static void display(void)
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  draw();

  glutSwapBuffers();
}

static void idle(void)
{
	glutPostRedisplay();
}

static void menu(int item)
{
	switch(item){
		case glPOINT:
			   modoRender = glPOINT;
			   real_renderMode = GL_POINT;
			   break;
		case glFLAT:
			   modoRender = glFLAT;
			   real_renderMode = GL_FILL;
			   glShadeModel(GL_FLAT);
			   break;
		case glSMOOTH:
			   modoRender = glSMOOTH;
			   real_renderMode = GL_FILL;
			   glShadeModel(GL_SMOOTH);
			   break;
		case glLINE:
			   modoRender = glLINE;
			   real_renderMode = GL_LINE;
			   break;
		default: break; 
	}
}


// Funciones de navegacion
void clampCamera()
{
 if( camPitch > 90.0 )
	camPitch = 90.0;
 else if( camPitch < -90.0 )
	camPitch = -90.0;
 while( camYaw < 0.0 )
	camYaw += 360.0;
 while( camYaw >= 360.0 )
	camYaw -= 360.0;
}

void dollyCamera( GLfloat dollyBy, GLfloat dollyAngle )
{
 GLfloat actualAngleInRadians;
 actualAngleInRadians = ( ( camYaw + dollyAngle ) * M_PI / 180.0 );
 camX -= sin( actualAngleInRadians ) * dollyBy * DEF_dollyStepSize;
 camZ -= cos( actualAngleInRadians ) * dollyBy * DEF_dollyStepSize;
}

static void keyboard(unsigned char key, int x, int y)
{
   static int animating = 0;

   switch (key) {
// Movimiento hacia adelante
	case 'w':
	case 'W':
		dollyCamera( DEF_dollyStepSize, 0.0 );
		break;

// Movimiento hacia atras
	case 's':
	case 'S':
		dollyCamera( DEF_dollyStepSize, 180.0 );
		break;

// Strafe hacia la izquierda
	case 'a':
	case 'A':
		dollyCamera( DEF_dollyStepSize, 90.0 );
		break;

// Strafe derecha
	case 'd':
	case 'D':
		dollyCamera( DEF_dollyStepSize, 270.0 );
		break;

// Fullscreen
   case 'f':
   case 'F':
	    if(fullscreen){
			fullscreen = 0;
		}else{
			fullscreen = 1;
		}
        if(fullscreen){
           glutFullScreen();
        }else{
           glutReshapeWindow(800,600);
        }
		break;


		// Cambia modos de visualizado
    case ' ':
		switch(modoRender){
	       case glLINE:
			   modoRender = glPOINT;
			   real_renderMode = GL_POINT;
			   break;
		   case glPOINT:
			   modoRender = glFLAT;
			   real_renderMode = GL_FILL;
			   break;
		   case glFLAT:
			   modoRender = glSMOOTH;
			   real_renderMode = GL_FILL;
			   break;
		   case glSMOOTH:
			   modoRender = glLINE;
			   real_renderMode = GL_LINE;
			   break;
		   default: break;

		}
        break;

//Esc salir
	case 27:				
		exit(0);
		break;
  }
  glutPostRedisplay();
}

void specialFunc(int key, int x, int y)
{
	switch(key)
	{
	//giro  a la izquierda
	case GLUT_KEY_LEFT:
		camYaw += 1.0;
		clampCamera();
		break;

	// giro a la derecha.
	case GLUT_KEY_RIGHT:
		camYaw -= 1.0;
		clampCamera();
		break;

	// mirar hacia arriba.
	case GLUT_KEY_UP:
		camY += 1.0;
		break;

	// mirar hacia abajo
	case GLUT_KEY_DOWN:
		camY -= 1.0;
		break;
	}
    glutPostRedisplay();
}

// Ratón

void enterMouseDrag( int x, int y )
{
 if( isInMouseDrag )
	return;
 isInMouseDrag = 1;
 if( viewPortCenterX < 0 )
	{
	viewPortCenterX = glutGet( GLUT_WINDOW_WIDTH ) / 2;
	viewPortCenterY = glutGet( GLUT_WINDOW_HEIGHT ) / 2;
	}
 oldCursorID = glutGet( GLUT_WINDOW_CURSOR );
 oldCursorX = x;
 oldCursorY = y;
 glutSetCursor( GLUT_CURSOR_NONE );
 glutWarpPointer( viewPortCenterX, viewPortCenterY );
}

void exitMouseDrag( int x, int y )
{
 if( !isInMouseDrag )
	return;
 isInMouseDrag = 0;
 glutSetCursor( oldCursorID );
 glutWarpPointer( oldCursorX, oldCursorY );
}

void mouseFunc( int button, int state, int x, int y )
{
 if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
	{
	if( !isInMouseDrag )
		enterMouseDrag( x, y );
	else
		exitMouseDrag( x, y );
	}
}

void allMotionFunc( int x, int y )
{
 int deltaX, deltaY;

 if( !isInMouseDrag )
	return;
 deltaX = x - viewPortCenterX;
 deltaY = y - viewPortCenterY;
 if( deltaX == 0 && deltaY == 0 )
	return;
 glutWarpPointer( viewPortCenterX, viewPortCenterY );
 camYaw -= DEF_angleSensitivity * deltaX;
 camPitch -= DEF_angleSensitivity * deltaY * ( 1.0 );
 clampCamera();
 glutPostRedisplay();
}