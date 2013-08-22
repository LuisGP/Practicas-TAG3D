/*
 *  Practica1.h
 *  Practica1
 *
 *  Created by Alejandro Ribao and Luis Gasco on 21/03/08.
 *
 */

#include <stdio.h>    /* for printf and NULL */
#include <stdlib.h>   /* for exit */
#include <math.h>     /* for sqrt, sin, and cos */
#include <assert.h>   /* for assert */
#if __APPLE__
#include <GLUT/glut.h> /* OpenGL Utility Toolkit */
#else
#include <GL/glut.h>
#endif
#include "maps.h"

/* An OpenGL 1.2 define */
#define GL_CLAMP_TO_EDGE                    0x812F

/* A few OpenGL 1.3 defines */
#define GL_TEXTURE_CUBE_MAP                 0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP         0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X      0x8515

/* Algunos defines para suavizar la navegacion */
#if __APPLE__
#define DEF_dollyStepSize                   1.0
#define DEF_angleSensitivity                0.2
#else
#define DEF_dollyStepSize                   1.0
#define DEF_angleSensitivity                1.0
#endif


/************************************************************************************************
 
 Variables
 
 ************************************************************************************************/




// Nombre del programa
static const char *myProgramName = "Practica 1";


/* Medidas del terreno y cotas */
enum{
	ALTO = 256,
	ANCHO = 256,
	NIVEL_LAGO = 20,
	NIVEL_NIEVE = 130,
	FRAMES_NUBES = 10
};
static float RELIEVE = 3.0; // Controla el tamaño vertical del terreno
static float ESCALA = 5.0; // Controla el tamaño horizontal del terreno
static int M_ALTO = ALTO/2; // Usados para generar las tiras de triángulos
static int M_ANCHO = ANCHO/2;


/* La cámara */
int fullscreen = 0;
int viewPortCenterX = -1;
int viewPortCenterY = -1;
GLfloat	camX = 0.0;
GLfloat	camY = 8.0;
GLfloat	camZ = 0.0;
GLfloat	camYaw = 0.0;
GLfloat	camPitch = 0.0;
static float zoomFactor = 1.0;
static float alturaPj = 3.4;
static float alturaPjNado = 0.7;
bool andando = false;
bool animarCamara = false;
// Animación de la cámara
enum{
	INICIO_SUAVE,
	VUELTA,
	ZOOM_IN,
	ZOOM_OUT,
	TRAVELING
};
int estadoAnimCamara = INICIO_SUAVE;

GLfloat aspect=1.3;
int width=800;
int height=600;
#if __APPLE__
GLfloat near=10;
#else
GLfloat near=1;
#endif
GLfloat far=620*ESCALA;

// Para el ratón
int	isInMouseDrag = 0;
int	oldCursorID = 0;
int	oldCursorX = 0;
int	oldCursorY = 0;


/* Texturas y datos del terreno */
// Texturas
int** alturas;		// terreno
rgbint* paleta;
rgbfloat* paletaf;
rgbint* paleta_agua;
rgbfloat* paleta_aguaf;
rgbint* paleta_grass;
rgbfloat* paleta_grassf;
rgbint** tree;		// arboles
rgbint** snowtree;
int** treealpha;
rgbint* paletanubes; // nubes
rgbint** nubes;
int*** nubesalpha;
int framenubes = 0;
bool sentidonubes = true;
rgbint** estrellas; // estrellas
rgbint** brujula; // brujula
int** brujulaalpha;
rgbint** aguja; // aguja de la brujula
int** agujaalpha;
rgbint** bump;
rgbint** palm;		// palmeras
int** palmalpha;
rgbint** texterreno;
bool usartextura = false;

static GLuint texNames[	FRAMES_NUBES+8]; // Array de texturas usadas
enum {
	ARBOL = 0,
	ARBOLNIEVE = 1,
	ESTRELLAS = 2,	
	BRUJULA = 3,
	AGUJA = 4,
	BUMP = 5,
	TERRENO = 6,
	PALM = 7,
	NUBES = 8
	
};


// Geometría
GLfloat normalest[(ALTO-1)*(ANCHO-1)*2][3];
GLfloat normalesv[ALTO][ANCHO][3];
GLfloat vertices[ALTO][ANCHO][3];
GLfloat colores[ALTO][ANCHO][3];
GLfloat normaleslago[ALTO][ANCHO][3];
GLfloat lago[ALTO][ANCHO][3];
bool arboles[ALTO][ANCHO];
int margen=6; // para determinar cómo de aleatorios son los accesos a las paletas
int influencia=5; // tamaño del cursor de deformación
GLfloat* ant_alturas = NULL; //Para las deformaciones
float* pesos = NULL; //Para las deformaciones
bool suave = true; // Se aplica un filtrado al terreno si esta activado
float olas[13] = {0.2, 0.7, 0.6, 0.5, 0.4, 0.3, 
					0.2, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0}; // nums aleatorios auxiliares
float nubosidad = .1; // Grado de cobertura del cielo


/* Luz */
GLfloat luzdifusa[]= { 1.0, 1.0, 1.0, 1.0 };	// Luz del sol
GLfloat luzambiente[]= { 0, 0, 0, 1.0 };	// Definimos valores de luz ambiente
GLfloat luzspecular[]= { 0.0, 0.0, 0.0, 0.0};	// Definimos el valor specular de la luz
GLfloat luzemision[]= { 1.0, 1.0, 1.0, 1.0};	// Definimos el valor de emisión de la luz
GLfloat posicion_luz[]= { 5.0, 300.0, 5.0 , 1.0};	    // La posición de la luz
GLfloat brillo = 10;				// Definimos el brillo de la luz (exponente)
GLfloat colorcielo[] = {1,1,1,0};	// El color del cielo


/* hora simulada */
int horas = 12;
int minutos = 0;
float velocidad = 1;
bool animar = true;
clock_t tiempo;
clock_t t_fps;
int fps;
int _fps; // auxiliar
char wtitle[40];


/* Niebla */
float  fog_density  = nubosidad*nubosidad*nubosidad*nubosidad*nubosidad; //0.003f
GLenum fog_mode     = GL_EXP;
float far_distance  = 500.0*ESCALA;
float near_distance = 400.05f;


/* Modos de visualización */
enum{
	glLINE,
	glPOINT,
	glFLAT,
	glSMOOTH,
	ANDAR_FLOTAR,
	ANIMAR_CAMARA,
	TEXT_TERR
};

int modoRender = glSMOOTH;
int real_renderMode = GL_SMOOTH;


/************************************************************************************************
 
  Operaciones
 
 ************************************************************************************************/


/************************************************************************************************
		Asignar colores
 ************************************************************************************************/
void cesped(int i, int j, int ind);
void nevar(int i, int j, int ind);
void genColores(int margen);
void genTexturas();
void genArboles(int cercania);
void genEstrellas();

/************************************************************************************************
		Generar geometría
 ************************************************************************************************/
void genVertices(int** alts, bool suav);
void realizarDeformacion(int n_alt);
int auxIndiceInfluencia(int i, int j, int x, int y, int influencia);
void deformarTerreno(int n_altura, int x, int y, int influencia, bool suave);
void genLago();

/************************************************************************************************
		Generar normales
 ************************************************************************************************/
void genNormalesLago();
void genNormalesT();
void genNormalesV();

/************************************************************************************************
		Inicializaciones
 ************************************************************************************************/
void genTerreno();
void init();

/************************************************************************************************
		Modificaciones de la cámara
 ************************************************************************************************/
void setAlturaPj();
void clampCamera();
void dollyCamera( GLfloat dollyBy, GLfloat dollyAngle );
void viajeCamara();

/************************************************************************************************
		Animaciones
 ************************************************************************************************/
void calculaFPS();
void actualizaSol(bool forzar);

 /************************************************************************************************
		Dibujado
 ************************************************************************************************/
void plantarSol();
void dibujarTerreno();
void quadArbol(int i, int j, GLfloat cosang, GLfloat sinang);
void dibujarCielo();
void dibujarArboles();
int alphaLago ( float vx, float vy, float vz );
void dibujarLago();
void dibujarBrujula();
void draw();

/************************************************************************************************
		Callbacks propios de OpenGL
 ************************************************************************************************/
void reshape(int w, int h);
static void display(void);
static void idle(void);
static void timer(int value);
static void keyboard(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
void enterMouseDrag( int x, int y );
void exitMouseDrag( int x, int y );
void mouseFunc( int button, int state, int x, int y );
void allMotionFunc( int x, int y );
static void menu(int item);
 