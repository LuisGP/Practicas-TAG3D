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
#if __APPLE__
#define DEF_dollyStepSize                   1.0
#define DEF_angleSensitivity                0.02
#else
#define DEF_dollyStepSize                   1.0
#define DEF_angleSensitivity                1.0
#endif

/* Forward declared GLUT callbacks registered by main. */
static void display(void);
static void keyboard(unsigned char c, int x, int y);
static void menu(int item);
void specialFunc( int key, int x, int y );
void enterMouseDrag( int x, int y );
void exitMouseDrag( int x, int y );
void mouseFunc( int button, int state, int x, int y );
void allMotionFunc( int x, int y );
//void applyBumpMapping();
void genTerreno();
static void idle(void);
static void timer(int value);


static const char *myProgramName = "Practica1";

// La vista
int fullscreen;
int	viewPortCenterX;
int	viewPortCenterY;
static float zoomFactor = 1.0;

static float alturaPj = 10;
static float alturaPjNado = 0.7;
bool andando = false;

GLfloat	camX;
GLfloat	camY;
GLfloat	camZ;
GLfloat	camYaw;
GLfloat	camPitch;

// Para el ratÛn
int	isInMouseDrag;
int	oldCursorID;
int	oldCursorX;
int	oldCursorY;

int** alturas;
rgbint* paleta;
rgbfloat* paletaf;
rgbint* paleta_agua;
rgbfloat* paleta_aguaf;
rgbint* paleta_grass;
rgbfloat* paleta_grassf;
// Luz
GLfloat luzdifusa[]= { 1.0, 1.0, 1.0, 1.0 };	// Definimos valores de luz difusa, m·xima intensidad de luz blanca
GLfloat luzambiente[]= { 0, 0, 0, 1.0 };	// Definimos valores de luz ambiente
GLfloat luzspecular[]= { 0.0, 0.0, 0.0, 0.0};	// Definimos el valor specular de la luz
GLfloat luzemision[]= { 1.0, 1.0, 1.0, 1.0};	// Definimos el valor de emisiÛn de la luz
GLfloat posicion_luz[]= { 5.0, 300.0, 5.0 , 1.0};	    // La posiciÛn de la luz
GLfloat colorcielo[] = {1,1,1,0};
GLfloat brillo = 10;				// Definimos el brillo de la luz (exponente de una ecuaciÛn)

// Materiales
GLfloat material_difuso[]={1.0, 1.0, 1.0, 1.0};		// DefiniciÛn de un material
GLfloat material_ambiente[]={1.0, 1.0, 1.0, 1.0};	// DefiniciÛn de un material
GLfloat material_specular[]={0.0, 0.0, 0.0, 0.0};	// DefiniciÛn de un material
GLfloat material_emission[]={0.0, 0.0, 0.0, 0.0};	// DefiniciÛn de un material
GLfloat material_brillo = 90;

int horas = 12;
int minutos = 0;
float velocidad = 1;
bool animar = true;
clock_t tiempo;
clock_t t_fps;
int fps;
int _fps; // auxiliar
char wtitle[40];

//static float M_PI = 3.141592653;

enum{
	ALTO = 256,
	ANCHO = 256,
	NIVEL_LAGO = 20,
	NIVEL_NIEVE = 50
};
static float RELIEVE = 3.0; // Controla el tamaño vertical del terreno
static float ESCALA = 5.0; // Controla el tamaño horizontal del terreno
static int M_ALTO = ALTO/2;
static int M_ANCHO = ANCHO/2;

// GL_POINT
// GL_FILL
// GL_LINE
enum{
	glLINE,
	glPOINT,
	glFLAT,
	glSMOOTH,
	ANDAR_FLOTAR
};

int modoRender = glSMOOTH;
int real_renderMode = GL_SMOOTH;


GLfloat normalest[(ALTO-1)*(ANCHO-1)*2][3];
GLfloat normalesv[ALTO][ANCHO][3];
GLfloat vertices[ALTO][ANCHO][3];
GLfloat colores[ALTO][ANCHO][3];
GLfloat normaleslago[ALTO][ANCHO][3];
GLfloat lago[ALTO][ANCHO][3];
float** rugosidades;
bool suave = true;



int anguloLago ( float vx, float vy, float vz ){
	float ang;
	
	GLfloat v0[3] = {camX-vx, camY-vy, camZ-vz };
	float mv0;
	mv0 = sqrt(v0[0]*v0[0] + v0[1]*v0[1] + v0[2]*v0[2]);
	
	ang = 1.0 - v0[1]/mv0;
	int alpha = int(ang * ang * 200 + 30);
	//if (alpha < 0) cout << alpha << endl;//alpha = -alpha;
	//cout << alpha << endl;
	return alpha;
}


GLfloat* calculateNormal(GLfloat* v0, GLfloat* v1, GLfloat* v2){
	//arista0=vector que va de v0 a v1
	GLfloat arista0[3];
	arista0[0] = v1[0] - v0[0]; 
	arista0[1] = v1[1] - v0[1];
	arista0[2] = v1[2] - v0[2];
	
    //arista1=vector que va de v0 a v2
	GLfloat arista1[3];
	arista1[0] = v2[0] - v0[0]; 
	arista1[1] = v2[1] - v0[1];
	arista1[2] = v2[2] - v0[2];
	
    GLfloat * normal = new GLfloat[3];
	normal[0] = arista0[1]*arista1[2] - arista0[2]*arista1[1];
	normal[1] = arista0[2]*arista1[0] - arista0[0]*arista1[2];
	normal[2] = arista0[0]*arista1[1] - arista0[1]*arista1[0];
	
	return normal;
}



GLfloat suavizar3(int i, int j){
	int f[6] = {4,2,1};
	int n = f[0];
	GLfloat acum = alturas[i][j]*f[0];
	if(i > 0){ 
		if (j > 0){	acum += alturas[i-1][j-1]*f[2];	n+=f[2]; }
		acum += alturas[i-1][j]*f[1];	n+=f[1];
		if (j < ANCHO-1){ acum += alturas[i-1][j+1]*f[2];	n+=f[2]; }
	}
	
	if (j > 0){	acum += alturas[i][j-1]*f[1];	n+=f[1]; }
	
	if (j < ANCHO-1){ acum += alturas[i][j+1]*f[1];	n+=f[1]; }
	
	if(i < ALTO-1){ 
		if (j > 0){	acum += alturas[i+1][j-1]*f[2];	n+=f[2]; }
		acum += alturas[i+1][j]*f[1];	n+=f[1];
		if (j < ANCHO-1){ acum += alturas[i+1][j+1]*f[2];	n+=f[2]; }			
	}
	return acum/n;
}


GLfloat suavizar5(int i, int j){
	int f[6] = {51,26,16,7,4,1};
	//	int f[6] = {100,0,0,0,1,1};
	
	int n = f[0];
	GLfloat acum = alturas[i][j]*f[0];
	
	if (j > 0){	acum += alturas[i][j-1]*f[1]; n+=f[1]; }
	if (j > 1){	acum += alturas[i][j-2]*f[3]; n+=f[3]; }
	if (j < ANCHO-1){ acum += alturas[i][j+1]*f[1]; n+=f[1]; }
	if (j < ANCHO-2){ acum += alturas[i][j+2]*f[3]; n+=f[3]; }
	
	if(i > 0){ 
		if (j > 0){	acum += alturas[i-1][j-1]*f[2];	n+=f[2]; }
		if (j > 1){	acum += alturas[i-1][j-2]*f[4];	n+=f[4]; }
		acum += alturas[i-1][j]*f[1];	n+=f[1];
		if (j < ANCHO-1){ acum += alturas[i-1][j+1]*f[2]; n+=f[2]; }
		if (j < ANCHO-2){ acum += alturas[i-1][j+2]*f[4]; n+=f[4]; }
	}
	
	if(i < ALTO-1){ 
		if (j > 0){	acum += alturas[i+1][j-1]*f[2];	n+=f[2]; }
		if (j > 1){	acum += alturas[i+1][j-2]*f[4];	n+=f[4]; }
		acum += alturas[i+1][j]*f[1];	n+=f[1];
		if (j < ANCHO-1){ acum += alturas[i+1][j+1]*f[2]; n+=f[2]; }			
		if (j < ANCHO-1){ acum += alturas[i+1][j+2]*f[4]; n+=f[4]; }			
	}
	
	
	if(i > 1){
		if (j > 0){	acum += alturas[i-2][j-1]*f[4];	n+=f[4]; }
		if (j > 1){	acum += alturas[i-2][j-2]*f[5];	n+=f[5]; }
		acum += alturas[i-2][j]*f[3];	n+=f[3];
		if (j < ANCHO-1){ acum += alturas[i-2][j+1]*f[4]; n+=f[4]; }
		if (j < ANCHO-2){ acum += alturas[i-2][j+2]*f[5]; n+=f[5]; }
	} 
	
	if(i < ALTO-2){
		if (j > 0){	acum += alturas[i+2][j-1]*f[4];	n+=f[4]; }
		if (j > 1){	acum += alturas[i+2][j-2]*f[5];	n+=f[5]; }
		acum += alturas[i+2][j]*f[3];	n+=f[3];
		if (j < ANCHO-1){ acum += alturas[i+2][j+1]*f[4]; n+=f[4]; }
		if (j < ANCHO-2){ acum += alturas[i+2][j+2]*f[5]; n+=f[5]; }
	} 
	
	
	return acum/n;
}









float modulo2(float* v){
	return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}


float differ(float* v1, float* v2){
	return v1[0]-v2[0] + v1[1]-v2[1] + v1[2]-v2[2];
}




/*
 GLfloat rugosidad(int i, int j){
 int f[6] = {1,1,1,1,1,1};
 //	int f[6] = {100,0,0,0,1,1};
 
 int n = f[0];
 GLfloat acum = modulo2(normalesv[i][j])*f[0];
 if(i > 0){ 
 if (j > 0){	acum += modulo2(normalesv[i-1][j-1])*f[2];	n+=f[2]; }
 if (j > 1){	acum += modulo2(normalesv[i-1][j-2])*f[4];	n+=f[4]; }
 acum += modulo2(normalesv[i-1][j])*f[1];	n+=f[1];
 if (j < ANCHO-1){ acum += modulo2(normalesv[i-1][j+1])*f[2]; n+=f[2]; }
 if (j < ANCHO-2){ acum += modulo2(normalesv[i-1][j+2])*f[4]; n+=f[4]; }
 }
 if (j > 0){	acum += modulo2(normalesv[i][j-1])*f[1]; n+=f[1]; }
 if (j > 1){	acum += modulo2(normalesv[i][j-2])*f[3]; n+=f[3]; }
 if (j < ANCHO-1){ acum += modulo2(normalesv[i][j+1])*f[1]; n+=f[1]; }
 if (j < ANCHO-2){ acum += modulo2(normalesv[i][j+2])*f[3]; n+=f[3]; }
 
 if(i < ALTO-1){ 
 if (j > 0){	acum += modulo2(normalesv[i+1][j-1])*f[2];	n+=f[2]; }
 if (j > 1){	acum += modulo2(normalesv[i+1][j-2])*f[4];	n+=f[4]; }
 acum += modulo2(normalesv[i+1][j])*f[1];	n+=f[1];
 if (j < ANCHO-1){ acum += modulo2(normalesv[i+1][j+1])*f[2]; n+=f[2]; }			
 if (j < ANCHO-1){ acum += modulo2(normalesv[i+1][j+2])*f[4]; n+=f[4]; }			
 }
 
 
 if(i > 1){
 if (j > 0){	acum += modulo2(normalesv[i-2][j-1])*f[4];	n+=f[4]; }
 if (j > 1){	acum += modulo2(normalesv[i-2][j-2])*f[5];	n+=f[5]; }
 acum += modulo2(normalesv[i-2][j])*f[3];	n+=f[3];
 if (j < ANCHO-1){ acum += modulo2(normalesv[i-2][j+1])*f[4]; n+=f[4]; }
 if (j < ANCHO-2){ acum += modulo2(normalesv[i-2][j+2])*f[5]; n+=f[5]; }
 } 
 
 if(i < ALTO-2){
 if (j > 0){	acum += modulo2(normalesv[i+2][j-1])*f[4];	n+=f[4]; }
 if (j > 1){	acum += modulo2(normalesv[i+2][j-2])*f[5];	n+=f[5]; }
 acum += modulo2(normalesv[i+2][j])*f[3];	n+=f[3];
 if (j < ANCHO-1){ acum += modulo2(normalesv[i+2][j+1])*f[4]; n+=f[4]; }
 if (j < ANCHO-2){ acum += modulo2(normalesv[i+2][j+2])*f[5]; n+=f[5]; }
 } 
 
 
 return acum/(n*5000);
 }
 */



GLfloat rugosidad(int i, int j){
	int f[6] = {1,1,1,1,1,1};
	//	int f[6] = {100,0,0,0,1,1};
	
	int n = f[0];
	GLfloat acum = differ(normalesv[i][j],normalesv[i][j])*f[0];
	if(i > 0){ 
		if (j > 0){	acum += differ(normalesv[i][j],normalesv[i-1][j-1])*f[2];	n+=f[2]; }
		if (j > 1){	acum += differ(normalesv[i][j],normalesv[i-1][j-2])*f[4];	n+=f[4]; }
		acum += differ(normalesv[i][j],normalesv[i-1][j])*f[1];	n+=f[1];
		if (j < ANCHO-1){ acum += differ(normalesv[i][j],normalesv[i-1][j+1])*f[2]; n+=f[2]; }
		if (j < ANCHO-2){ acum += differ(normalesv[i][j],normalesv[i-1][j+2])*f[4]; n+=f[4]; }
	}
	if (j > 0){	acum += differ(normalesv[i][j],normalesv[i][j-1])*f[1]; n+=f[1]; }
	if (j > 1){	acum += differ(normalesv[i][j],normalesv[i][j-2])*f[3]; n+=f[3]; }
	if (j < ANCHO-1){ acum += differ(normalesv[i][j],normalesv[i][j+1])*f[1]; n+=f[1]; }
	if (j < ANCHO-2){ acum += differ(normalesv[i][j],normalesv[i][j+2])*f[3]; n+=f[3]; }
	
	if(i < ALTO-1){ 
		if (j > 0){	acum += differ(normalesv[i][j],normalesv[i+1][j-1])*f[2];	n+=f[2]; }
		if (j > 1){	acum += differ(normalesv[i][j],normalesv[i+1][j-2])*f[4];	n+=f[4]; }
		acum += differ(normalesv[i][j],normalesv[i+1][j])*f[1];	n+=f[1];
		if (j < ANCHO-1){ acum += differ(normalesv[i][j],normalesv[i+1][j+1])*f[2]; n+=f[2]; }			
		if (j < ANCHO-1){ acum += differ(normalesv[i][j],normalesv[i+1][j+2])*f[4]; n+=f[4]; }			
	}
	
	
	if(i > 1){
		if (j > 0){	acum += differ(normalesv[i][j],normalesv[i-2][j-1])*f[4];	n+=f[4]; }
		if (j > 1){	acum += differ(normalesv[i][j],normalesv[i-2][j-2])*f[5];	n+=f[5]; }
		acum += differ(normalesv[i][j],normalesv[i-2][j])*f[3];	n+=f[3];
		if (j < ANCHO-1){ acum += differ(normalesv[i][j],normalesv[i-2][j+1])*f[4]; n+=f[4]; }
		if (j < ANCHO-2){ acum += differ(normalesv[i][j],normalesv[i-2][j+2])*f[5]; n+=f[5]; }
	} 
	
	if(i < ALTO-2){
		if (j > 0){	acum += differ(normalesv[i][j],normalesv[i+2][j-1])*f[4];	n+=f[4]; }
		if (j > 1){	acum += differ(normalesv[i][j],normalesv[i+2][j-2])*f[5];	n+=f[5]; }
		acum += differ(normalesv[i][j],normalesv[i+2][j])*f[3];	n+=f[3];
		if (j < ANCHO-1){ acum += differ(normalesv[i][j],normalesv[i+2][j+1])*f[4]; n+=f[4]; }
		if (j < ANCHO-2){ acum += differ(normalesv[i][j],normalesv[i+2][j+2])*f[5]; n+=f[5]; }
	} 
	
	//cout << acum/(n*50) << endl; 
	acum = acum/(n*50);
	if (acum < 0)
		acum = -acum;
	return acum;
}




// Funcion para deformar el terreno
void deformar(int n_alt, int i, int j, int alcance, void (*f), bool suave){
	/*
		n_alt: nueva altura
		i, j: vertice objetivo
		alcance: vertices vecinos afectados
		f: funcion que determina la deformacion
		suave: aplicar suavizado
	*/
}


void genVertices(int** alts, bool suav){
	
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			vertices[i][j][0] = (i-(M_ALTO)+1)*ESCALA;
			if (suav)
				vertices[i][j][1] = suavizar5(i,j)* RELIEVE;
			else
				vertices[i][j][1] = alts[i][j]* RELIEVE;
			vertices[i][j][2] = (j-(M_ANCHO)+1)*ESCALA;
		}
	}
}


void genLago(){
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			lago[i][j][0] = (i-(M_ALTO)+1)*ESCALA;
			lago[i][j][1] = NIVEL_LAGO*RELIEVE+0.5; // Evita errores con Z-Buffer
			lago[i][j][2] = (j-(M_ANCHO)+1)*ESCALA;
		}
	}
}



void cesped(int i, int j, int ind){
	ind = ind+(rand()%5-(5/2));
	float aux[3] = {normalesv[i][j][0],normalesv[i][j][1],normalesv[i][j][2]};
	if (aux[0] < 0) aux[0] = -aux[0];
	if (aux[2] < 0) aux[2] = -aux[2];
	
	if (aux[1] > aux[2] && aux[1] > aux[0]){
		colores[i][j][0] = paleta_grassf[ind].r;
		colores[i][j][1] = paleta_grassf[ind].g;
		colores[i][j][2] = paleta_grassf[ind].b;
	}else if (aux[1]*2 > aux[2] && aux[1]*2 > aux[0]){
		colores[i][j][0] = colores[i][j][0]*.25 + paleta_grassf[ind].r*.75;
		colores[i][j][1] = colores[i][j][1]*.25 + paleta_grassf[ind].g*.75;
		colores[i][j][2] = colores[i][j][2]*.25 + paleta_grassf[ind].b*.75;
	}
}



void nevar(int i, int j, int ind){
	float aux[3] = {normalesv[i][j][0],normalesv[i][j][1],normalesv[i][j][2]};
	if (aux[0] < 0) aux[0] = -aux[0];
	aux[1] += ind*.5; 
	aux[2] += ind*.3; 
	
	if (aux[2] > 0 && aux[1] > aux[2] && aux[1] > aux[0]){
		colores[i][j][0] = 1;
		colores[i][j][1] = 1;
		colores[i][j][2] = 1;
	}
}



void genColores(int margen){
	bool underwater = false;
	int ind;
	rgbfloat* pal;
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			underwater = false;
			int alt = vertices[i][j][1];
			if (margen != 0) {
				ind = vertices[i][j][1]+(rand()%margen-(margen/2));
				//colores[i][j][0] = pal[ind].r;
				//colores[i][j][1] = pal[ind].g;
				//colores[i][j][2] = pal[ind].b;
			} else {
				ind = alt;
			}
			if (ind > NIVEL_LAGO*RELIEVE){
				pal = paletaf;
				ind -= NIVEL_LAGO*RELIEVE;
			}else{
				pal = paleta_aguaf;
				ind = NIVEL_LAGO*RELIEVE-ind;
				underwater = true;
			}
			if (ind < 0) ind=0;
			if (ind > 255) ind=255;
			colores[i][j][0] = InterPol(pal[int(ind)].r, pal[int(ind)+1].r, alt-int(alt)); 
			colores[i][j][1] = InterPol(pal[int(ind)].g, pal[int(ind)+1].g, alt-int(alt)); 
			colores[i][j][2] = InterPol(pal[int(ind)].b, pal[int(ind)+1].b, alt-int(alt)); 
			
			if (!underwater && ind > NIVEL_LAGO*RELIEVE){
				cesped(i, j, ind);
			}
			
			if (!underwater && alt > NIVEL_NIEVE*RELIEVE){
				nevar(i,j, ind);
			}
		}
	}
}



void genRugosidad(){
	rugosidades = new float*[ALTO];
	for (int i=0; i<ALTO; i++){
		rugosidades[i] = new float[ANCHO];
		for (int j=0; j<ANCHO; j++){
			rugosidades[i][j]= rugosidad(i, j);
		}
	}
}



void aplicarRugosidad(){
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			float valor = 1-(rugosidades[i][j]);//(1-(rugosidades[i][j]-.1))*2;
			if (valor > 1) valor = 1;
			if (rugosidades[i][j] < .1) {
				colores[i][j][0] *= valor;
				colores[i][j][1] *= valor;
				colores[i][j][2] *= valor;
			}
		}
	}
}



void genNormalesLago(){
	float randoms[20];
	for (int i=0; i<20; i++){
		randoms[i] = (rand()%100)*.005;
	}
	
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			if (vertices[i][j][1] < NIVEL_LAGO*RELIEVE+2){
				normaleslago[i][j][0] = randoms[int(vertices[i][j][1])%20];
				normaleslago[i][j][1] = 1;
				normaleslago[i][j][2] = randoms[int(vertices[i][j][1])%20];
			}
		}
	}
	
}










void genNormalesT(){
	int n2 = 2*(ANCHO-1);
	int ind1, ind2;
	for (int i=0; i<(ALTO-1)*(ANCHO-1)*2; i+=2){ // PARES
		// 0 (0 2n+0) = (0,0)(1,1)(1,0)
		// 12(1 2n+4) = (1,2)(2,3)(2,2)
		// 14(1 2n+6) = (1,3)(2,4)(2,3)
		ind1 = i/(n2);
		ind2 = (i%(n2))/2;
		GLfloat* v0 = vertices[ind1][ind2];
		GLfloat* v1 = vertices[ind1+1][ind2+1];
		GLfloat* v2 = vertices[ind1+1][ind2];
		
		GLfloat* normal = calculateNormal(v0,v1,v2);
		normalest[i][0] = normal[0];
		normalest[i][1] = normal[1];
		normalest[i][2] = normal[2];
		delete normal;
	}
	
	
	for (int i=1; i<(ALTO-1)*(ANCHO-1)*2; i+=2){ // IMPARES
		// 13(1 2n+5) = (1,2)(1,3)(2,3)
		// 15(1 2n+7) = (1,3)(1,4)(2,4)
		ind1 = i/(n2);
		ind2 = (i%(n2))/2;
		GLfloat* v0 = vertices[ind1][ind2];
		GLfloat* v1 = vertices[ind1][ind2+1];
		GLfloat* v2 = vertices[ind1+1][ind2+1];
		
		GLfloat* normal = calculateNormal(v0,v1,v2);
		normalest[i][0] = normal[0];
		normalest[i][1] = normal[1];
		normalest[i][2] = normal[2];
		delete normal;
	}
	
}

void genNormalesV(){
	// 1,1 = 0,1,2,9,10,11
	// 1,2 = 2,3,4,11,12,13
	// 1,3 = 4,5,6,13,14,15
	// 2,1 = 8,9,10,17,18,19
	int n2 = 2*(ANCHO-1);
	int ind;
	GLfloat media[3];
	
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			media[0] = 0;
			media[1] = 0;			
			media[2] = 0;
			
			ind = n2*(i-1)+2*(j-1);
			if(i > 0){ 
				if (j > 0){
					media[0] += normalest[ind][0];
					media[1] += normalest[ind][1];
					media[2] += normalest[ind][2];
					media[0] += normalest[ind+1][0];
					media[1] += normalest[ind+1][1];
					media[2] += normalest[ind+1][2];
				}
				if (j < ANCHO-1){
					media[0] += normalest[ind+2][0];
					media[1] += normalest[ind+2][1];
					media[2] += normalest[ind+2][2];
				}
			}
			
			if(i < ALTO-1){ 
				if (j > 0){
					media[0] += normalest[ind+n2][0];
					media[1] += normalest[ind+n2][1];
					media[2] += normalest[ind+n2][2];
				}
				if (j < ANCHO-1){
					media[0] += normalest[ind+n2+1][0];
					media[1] += normalest[ind+n2+1][1];
					media[2] += normalest[ind+n2+1][2];
					media[0] += normalest[ind+n2+2][0];
					media[1] += normalest[ind+n2+2][1];
					media[2] += normalest[ind+n2+2][2];
				}
			}
			
			normalesv[i][j][0] = media[0];						// PASO DE DIVIDIR PORQUE HAY QUE NORMALIZAR IGUAL
			normalesv[i][j][1] = media[1];
			normalesv[i][j][2] = media[2];
		}
	}
}




// La altura del Pj no será más que la altura de la cámara...
void setAlturaPj(){
	// la posicion viene dada por camX y camZ... sólo si estamos en el terreno
	float acamX = (camX / ESCALA)+M_ALTO; // Normalizamos
	float acamZ = (camZ / ESCALA)+M_ANCHO;
	
	/*
	 Para suavizar, hacemos la media entre la anterior altura y
	 la del vertice correspondiente
	 */
	float aux, auxl;
	
	if(acamX > 2 && acamX < ALTO-2 && acamZ > 2 && acamZ < ANCHO-2){
		aux = ((vertices[(int)floor(acamX)][(int)floor(acamZ)][1]+alturaPj)+(camY))/2;
		auxl = lago[(int)floor(acamX)][(int)floor(acamZ)][1]+alturaPjNado;
		camY = max(auxl, aux);
	}
	//cout << camY << " vs " << vertices[(int)floor(acamX)][(int)floor(acamZ)][1]+alturaPj << endl;
}





void init(){
	srand(time(NULL));
	
	//glEnable(GL_COLOR_MATERIAL);
	glLightModelfv (GL_LIGHT_MODEL_AMBIENT, luzambiente);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzdifusa);	// Ponemos valores a la luz 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzambiente);	// Ponemos valores a la luz 0
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzspecular); // Ponemos valores a la luz 0
	glLightfv(GL_LIGHT0, GL_EMISSION, luzemision);	// Ponemos valores a la luz 0
	glLightf(GL_LIGHT0, GL_SHININESS, brillo);	    // Ponemos valores a la luz 0
	glLightfv(GL_LIGHT0, GL_POSITION, posicion_luz);	// Ponemos la posiciÛn para gl_light0
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	// Ahora ponemos la luz que ya definimos
	
	setSunLight(horas, minutos, 0, posicion_luz, luzdifusa, colorcielo, ESCALA);
	
	
	
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	
	paleta = loadPalInt("paletas/landcolor3.ppm", 256);
	paletaf = intPal2Float(paleta, 256);
	paleta_agua = loadPalInt("paletas/watercolor7.ppm", 256);
	paleta_aguaf = intPal2Float(paleta_agua, 256);
	paleta_grass = loadPalInt("paletas/grasscolor2.ppm", 256);
	paleta_grassf = intPal2Float(paleta_grass, 256);
	
	genTerreno();
	
	glEnable(GL_NORMALIZE); // Dejamos a OpenGL normalizar las normales
	
	t_fps = clock() * CLK_TCK ;
	fps = 0;
	tiempo = clock() * CLK_TCK; //time(NULL);
}

void calculaFPS(){
	clock_t act = clock()*CLK_TCK;
	float diff = act - t_fps;
	_fps++;
	
	if(diff > 1000000){ // 1 segundo
		fps = _fps;
		_fps = 0;
		t_fps = act;
	}
}

void genTerreno(){
	int i, j;
	int maximo = -(((unsigned int)-1)/2);
	int minimo = 2147483647;
	//alturas = genPerlinNoise(256, 12, 30, 1);	suave = false;	
	alturas = genPerlinNoise(256, 25, 37, 1);	suave = true;	
	//alturas = genPerlinNoise(256, 3, 30, 1);	suave = true;
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
		}
	}
	
	camY = maximo*RELIEVE;
	
	///////// NORMALIZAR
	genVertices(alturas, suave);
	genNormalesLago();
	genLago();
	genNormalesT();
	genNormalesV();
	genColores(8);
	
	//	genRugosidad();
	//	normalizarFloat(rugosidades, ANCHO);
	//	aplicarRugosidad();
}

void reshape(int w, int h)
{
	if (!h)
		return;
	glViewport(0, 0,  (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0*zoomFactor, (GLfloat) w/(GLfloat) h, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	viewPortCenterX = w / 2;
	viewPortCenterY = h / 2;
	//cout << zoomFactor << endl;
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
	
	// Para el ratÛn
	isInMouseDrag = 0;
	oldCursorID = 0;
	oldCursorX = 0;
	oldCursorY = 0;
	
	
	
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInit(&argc, argv);
	
	glutCreateWindow(myProgramName);
	
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	// Navegacion
	glutKeyboardFunc(keyboard);
	glutSpecialFunc( specialFunc );
	glutMouseFunc( mouseFunc );
	glutMotionFunc( allMotionFunc );
	glutPassiveMotionFunc( allMotionFunc );
	glutIdleFunc(idle);
	
	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
				   60.0*zoomFactor,   /* Field of view in degree */
				   1.0,    /* Aspect ratio */ 
				   0.01,    /* Z near */
				   10000.0*ESCALA); /* Z far */
	
	
	// Z-Buffer
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	// Back face culling
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
	glutAddMenuEntry("[p] Andar/Flotar", ANDAR_FLOTAR);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutTimerFunc(100,timer,1);
	
	glutMainLoop();
	return 0;
}



// Pintamos una esfera a modo de Sol ^^
void plantarSol(){
	glPushMatrix ();
	glDisable(GL_LIGHTING);
	
	glColor4f(luzdifusa[0], luzdifusa[1], luzdifusa[2], luzdifusa[3]);
	glTranslatef (posicion_luz[0], posicion_luz[1], posicion_luz[2]); 
	float grande = 0;
	if (horas == 17 && minutos > 30)
		grande = (minutos-30);
	if (horas == 6 && minutos < 30)
		grande = (30-minutos);
	glutSolidSphere(40.0+grande, 32, 32);
	
	glEnable(GL_LIGHTING);
    glPopMatrix ();
}



// Calculo de la luz Solar
void actualizaSol(bool forzar){
	// Otro metodo mas... sera multiplataforma?
	if(!forzar){
		minutos++;
		if(minutos == 60){
			minutos = 0;
			horas++;
			if (horas == 24) horas = 0;
		}
	}else{
		horas = horas%24;
	}
	
	//Animar el agua
	genNormalesLago();
	
	sprintf(wtitle, "%s - %d:%.2d  FPS: %d", myProgramName, horas%24, minutos, fps);
	glutSetWindowTitle(wtitle);
	
	
	/* LIMPIEZA GENERAL. VER LOGS :P */
}





/* Draw the scene. */
void
draw()
{
	int i, j;
	GLfloat color[3];
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	
	glRotatef( -camPitch, 1.0, 0.0, 0.0 );
	glRotatef( -camYaw, 0.0, 1.0, 0.0 );
	glTranslatef( -camX, -camY, -camZ );
	
	// Iluminamos de forma 'realista'
	setSunLight(horas, minutos, 0, posicion_luz, luzdifusa, colorcielo, ESCALA);
	plantarSol();
	
	
	// Modo de render de la escena
	glPolygonMode(GL_FRONT_AND_BACK, real_renderMode);
	
	
	//Pintamos el terreno
	
	// Orden contrario para BACKFACECULLING
	setTypeMaterial(LAND);
	
	for(i = M_ALTO-1; i > -M_ALTO; i--){
		glEnable(GL_COLOR_MATERIAL);
		glBegin(GL_TRIANGLE_STRIP);
		
		for(j = M_ANCHO-1; j > -M_ANCHO; j--){
			glNormal3fv(normalesv[i+M_ALTO-1][j+M_ANCHO]);
			color[0] = colores[i+M_ALTO-1][j+M_ANCHO][0]; 
			color[1] = colores[i+M_ALTO-1][j+M_ANCHO][1]; 
			color[2] = colores[i+M_ALTO-1][j+M_ANCHO][2];
			glColor3fv(color);
			glVertex3fv(vertices[i+M_ALTO-1][j+M_ANCHO]);
			
			glNormal3fv(normalesv[i+M_ALTO][j+M_ANCHO]);
			color[0] = colores[i+M_ALTO][j+M_ANCHO][0]; 
			color[1] = colores[i+M_ALTO][j+M_ANCHO][1]; 
			color[2] = colores[i+M_ALTO][j+M_ANCHO][2];
			glColor3fv(color);
			
			glVertex3fv(vertices[i+M_ALTO][j+M_ANCHO]);
		}
		glEnd();
	}
	
	setTypeMaterial(WATER);
	int alpha;
	GLfloat* aux;
	GLfloat* aux2;
	for(i = M_ALTO-1; i > -M_ALTO; i--){
		glEnable(GL_COLOR_MATERIAL);
		glBegin(GL_TRIANGLE_STRIP);
		
		for(j = M_ANCHO-1; j > -M_ANCHO; j--){
			aux = lago[i+M_ALTO-1][j+M_ANCHO];
			aux2 = lago[i+M_ALTO][j+M_ANCHO];			
			if(vertices[i+M_ALTO-1][j+M_ANCHO][1] < aux[1]+2 || 
				vertices[i+M_ALTO][j+M_ANCHO][1] < aux2[1]+2){
				glNormal3fv(normaleslago[i+M_ALTO-1][j+M_ANCHO]);
				alpha = anguloLago(aux[0], aux[1], aux[2]);
				//setIntColorMaterial (paleta_agua[0], alpha);
				glColor4f(colorcielo[0]+.5,colorcielo[1]+.3,colorcielo[2],alpha/255.0);
				glVertex3fv(aux);
				glNormal3fv(normaleslago[i+M_ALTO][j+M_ANCHO]);
				alpha = anguloLago(aux2[0], aux2[1], aux2[2]);
				//setIntColorMaterial (paleta_agua[0], alpha);
				glColor4f(colorcielo[0]+.5,colorcielo[1]+.3,colorcielo[2],alpha/255.0);
				glVertex3fv(aux2);
			}
		}
		glEnd();
	}
	
	glPopMatrix();
	
	glFlush();
	
	
	
	
	
	
	
	
	/*
	 
	 ////////////////////////////////////////////////////////////////////////////////////////////////
	 //	TEXTURA EXPERIMENTAL	
	 
	 // let's hardcode an image to illustrate texture application
	 // I'll use 4-bit per component values (16 bits per pixel) for a few rgba colors
	 #define TEX_INTERNAL GL_RGBA4
	 #define TEX_FORMAT   GL_RGBA
	 #define TEX_TYPE     GL_UNSIGNED_SHORT_4_4_4_4
	 
	 // here's a few hardcoded RGBA color values
	 #define R 0xf30f
	 #define W 0xffff
	 #define X 0x0000
	 #define G 0x5c6c
	 #define B 0x111f
	 
	 // and here's a hardcoded image
	 mushroom_texture [(16 * 16)] = {
	 X,X,X,B,B,B,B,B,B,B,B,B,B,X,X,X,
	 X,X,B,B,W,W,W,W,W,W,W,W,B,B,X,X,
	 X,X,B,W,W,W,W,W,W,W,W,W,W,B,X,X,
	 X,B,B,W,W,W,R,W,W,R,W,W,W,B,B,X,
	 B,B,B,B,W,W,R,W,W,R,W,W,B,B,B,B,
	 B,W,W,B,B,B,B,B,B,B,B,B,B,W,W,B,
	 B,W,W,W,W,W,G,G,G,G,W,W,W,W,W,B,
	 B,G,G,W,W,G,G,G,G,G,G,W,W,G,G,B,
	 B,G,G,G,W,G,G,G,G,G,G,W,G,G,G,B,
	 B,G,G,G,W,G,G,G,G,G,G,W,G,G,G,B,
	 B,B,G,G,W,W,G,G,G,G,W,W,G,G,B,B,
	 X,B,G,W,W,W,W,W,W,W,W,W,W,G,B,X,
	 X,B,B,W,G,G,W,W,W,W,G,G,W,B,B,X,
	 X,X,B,B,G,G,G,W,W,G,G,G,B,B,X,X,
	 X,X,X,B,B,B,G,W,W,G,B,B,B,X,X,X,
	 X,X,X,X,X,B,B,B,B,B,B,X,X,X,X,X,
	 };
	 
	 // now that we have a buffer containing an image, let's pass it to OpenGL
	 glTexImage2D(GL_TEXTURE_2D, 0, TEX_INTERNAL, 16, 16, 0, TEX_FORMAT, TEX_TYPE, mushroom_texture);
	 
	 // by default, OpenGL uses mipmaps. We must disable that for this texture to be "complete"
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	 
	 
	 glDisable(GL_CULL_FACE);
	 
	 
	 // now draw a quad with the texture (this assumes your projection etc is already setup)
	 glEnable(GL_TEXTURE_2D);
	 glBegin(GL_QUADS);
	 glTexCoord2f(0, 0); 	glVertex3f((M_ALTO-1)*ESCALA, NIVEL_LAGO*RELIEVE+0.5, -M_ANCHO*ESCALA);
	 
	 glTexCoord2f(1, 0); 	glVertex3f(-M_ALTO*ESCALA, NIVEL_LAGO*RELIEVE+0.5, -M_ANCHO*ESCALA);
	 
	 glTexCoord2f(1, 1); 	glVertex3f(-M_ALTO*ESCALA, NIVEL_LAGO*RELIEVE+0.5, (M_ANCHO-1)*ESCALA);
	 
	 glTexCoord2f(0, 1); 	glVertex3f((M_ALTO-1)*ESCALA, NIVEL_LAGO*RELIEVE+0.5, (M_ANCHO-1)*ESCALA);
	 
	 glEnd();
	 glDisable(GL_TEXTURE_2D);
	 glEnable(GL_CULL_FACE);
	 
	 glPopMatrix();	
	 
	 glFlush();
	 
	 
	 ////////////////////////////////////////////////////////////////////////////////////////////
	 
	 */
	
	
	
	
	
	
	
	
	
}

static void display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Aplicamos zoom?
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0*zoomFactor, 1.0, 0.1, 2000.0*ESCALA);
	glMatrixMode(GL_MODELVIEW);
	
	glLoadIdentity();
	
	draw();
	
	calculaFPS();
	
	glutSwapBuffers();
}

static void idle(void)
{
	glutPostRedisplay();
}

static void timer(int value){
	if (animar)
		actualizaSol(false);
	glutPostRedisplay();
	glutTimerFunc(1000/velocidad, timer, 1);
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
		case ANDAR_FLOTAR:
			keyboard('p',0,0);
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
	
	switch (key) {
			// Movimiento hacia adelante
		case 'w':
		case 'W':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 0.0 );
			break;
			
			// Movimiento hacia atras
			case 's':
			case 'S':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 180.0 );
			break;
			
			// Strafe hacia la izquierda
			case 'a':
			case 'A':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 90.0 );
			break;
			
			// Strafe derecha
			case 'd':
			case 'D':
			if(andando){
				setAlturaPj();
			}
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
			
			//Mirar abajo
			case 'z':
			camPitch -= 1.0;
			clampCamera();
			break;
			
			// Mirar arriba
			case 'Z':
			camPitch += 1.0;
			clampCamera();
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
					glShadeModel(GL_FLAT);
					break;
				case glFLAT:
					modoRender = glSMOOTH;
					real_renderMode = GL_FILL;
					glShadeModel(GL_SMOOTH);
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
			glutIdleFunc(NULL);
			
			exit(0);
			break;
			
			case '+': case '.': // Zoom in
			if(zoomFactor > 0.1)
				zoomFactor -= 0.1;
			break;
			
			case '-': case ',': // Zoom out
			if(zoomFactor < 2.4)
				zoomFactor += 0.1;
			break;
			
			case '0': // Zoom reset
			zoomFactor = 1.0;
			break;
			
			case '1': // Avanzar 1 hora
			horas++;
			actualizaSol(true);
			break;
			
			case '2': // Retroceder 1 hora
			horas--;
			actualizaSol(true);
			break;
			
			case 'p': case 'P': // Cambia entre andar y flotar
			andando = !andando;
			if(andando){
				setAlturaPj();
			}
			break;
			
			case 'o': case 'O': // Activar paso del tiempo
			animar = !animar;
			break;
			
			case 't':// Aumentar la velocidad del tiempo
			velocidad *= 2;
			if (velocidad > 500 ) velocidad = 512;
			cout << "v: " << velocidad << endl;
			break;
			
			case 'T':// Disminuir la velocidad del tiempo
			velocidad /= 2;
			if (velocidad < .125 ) velocidad = .125;
			break;			
			
			default: break;
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
			
			// mover hacia arriba.
		case GLUT_KEY_UP:
			if(!andando)
				camY += 1.0;
			break;
			
			// mover hacia abajo
			case GLUT_KEY_DOWN:
			if(!andando)
				camY -= 1.0;
			break;
	}
    glutPostRedisplay();
}

// RatÛn

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