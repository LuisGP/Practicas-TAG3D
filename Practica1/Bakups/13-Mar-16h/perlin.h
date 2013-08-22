/*
 *  perlin.h
 *  Practica1
 *
 *  Created by Alejandro Ribao and Luis Gasco on 11/03/08.
 *  2008
 *
 */


#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#include <time.h>
#define M_PI 3.141592653
#endif

// Based on http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
// and http://pgrafica.webideas4all.com/perlin.html

using namespace std;
const int SIZE = 256;

enum{
	SNOW,
	LAND,
	GRASS1,
	GRASS2,
	SAND,
	WATER
};




typedef struct rgbint rgbint;
typedef struct rgbint {
	int r;
	int g;
	int b;
};

typedef struct rgbfloat rgbfloat;
typedef struct rgbfloat {
	float r;
	float g;
	float b;
};




/********************************************************************
 
 Funciones para calcular el ruido Perlin en cada punto
 
 /********************************************************************/

/*
 Pseudo random
 */
float Noise(register int x);
/*
 Interpolacion
 */
float InterPol(float a, float b, float x);
/*
 Ruido Perlin para la posicion x,y
 */
float PerlinNoise(float x,float y,int width,int octaves,float seed, float persistence);
/*
 Funcion para generar Ruido Perlin con unas dimensiones determinadas
 */
int** genPerlinNoise (int size, int octaves, int seed, float persistence);




/********************************************************************
 
 Funciones para normalizar la imagen
 
 /********************************************************************/

/*
 Para enteros
 */
void normalizarInt(int** tabla, int size);
/*
 Para float
 */
void normalizarFloat(float** tabla, int size);






/********************************************************************
 
 Funciones para escribir por pantalla el archivo PGM
 
 /********************************************************************/
/*
 Para enteros
 */
void printTablaInt ( int** tabla, int size );


/*
 Para floats
 */
void printTablaFloat ( float** tabla, int size );





/********************************************************************
 
 Funcion para leer un archivo PGM
 
 /********************************************************************/

int** readPGM ( char* filename );


/********************************************************************
 
 Funcion para leer un archivo PPM
 
 /********************************************************************/

rgbint** readPPM ( char* filename );





/********************************************************************
 
 Funciones para escribir en archivo PGM la imagen
 
 /********************************************************************/
/*
 Para enteros
 */
void writePGMint ( char* filename, int** tabla, int size );


/*
 Para floats
 */
void writePGMfloat ( char* filename, float** tabla, int size );



/********************************************************************
 
 Funciones para escribir en archivo PPM la imagen
 
 /********************************************************************/
/*
 Para enteros
 */
void writePPMint ( char* filename, rgbint** tabla, int size );


/*
 Para floats
 */
void writePPMfloat ( char* filename, rgbfloat** tabla, int size );





/********************************************************************
 
 Conversion del espacio de [0,255] a [0,1] y viceversa
 
 /********************************************************************/

/*
 De 0,255 a 0,1
 */
float** int2float ( int** tabla, int size );


/*
 De 0,1 a 0,255
 */
int** float2int ( float** tabla, int size );

/*
 De 0,255 a 0,1 RGB
 */ 
rgbfloat** rgbint2float ( rgbint** tabla, int size );


/*
 De 0,1 a 0,255 RGB
 */
rgbint** rgbfloat2int ( rgbfloat** tabla, int size );






/********************************************************************
 
 Funcion para liberar la memoria ocupada por la imagen
 
 /********************************************************************/
/*
 Para enteros
 */
void freeTablaInt(int** tabla, int size);

/*
 Para floats
 */
void freeTablaFloat(float** tabla, int size);

/*
 Para enteros RGB
 */
void freeTablaRGBInt(rgbint** tabla, int size);

/*
 Para floats RGB
 */
void freeTablaRGBFloat(rgbfloat** tabla, int size);



/********************************************************************
 
 Funcion para obtener uno de los canales
 
 /********************************************************************/
/*
 Para enteros
 */
int** rgbintChannel(rgbint** tabla, int chan, int size);

/*
 Para floats
 */
float** rgbfloatChannel(rgbfloat** tabla, int chan, int size);

/********************************************************************
 
 Funcion para fusionar tres canales
 
 /********************************************************************/
/*
 Para enteros
 */
rgbint** int2rgb(int** r, int** g, int** b, int size);

/*
 Para floats
 */
rgbfloat** float2rgb(float** r, float** g, float** b, int size);


/********************************************************************
 
 Funciones para paletas
 
 /********************************************************************/
/*
 Para enteros
 */
rgbint* loadPalInt(char* filename, int size);

/*
 Convertir a floats
 */
rgbfloat* intPal2Float(rgbint* pal, int size);



/********************************************************************
 
 Funcion para generar un cielo
 
 /********************************************************************/
/*
 Para enteros
 */
rgbint** skyrgb(rgbint* pal, int nivel, int size);



/********************************************************************
 
 Asigna una paleta a un mapa en escala de grises (es recomendable normalizar antes)
 
 /********************************************************************/
/*
 Para enteros
 */
 rgbint** applyPal(rgbint* pal, int** tabla, int size);

/*
 Para floats
 */
 rgbfloat** applyPal(rgbfloat* pal, float** tabla, int size);



/*************************************

Materiales

**************************************/
/*
 Asigna un material segun todas sus caracteristicas
*/
void setMaterial (GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat alpha,
				  GLfloat sr, GLfloat sg, GLfloat sb, GLfloat er, GLfloat eg, GLfloat eb, GLfloat shine);

/*
 Asigna un material estandar de color c y alpha a
*/
void setIntColorMaterial (rgbint c, int a);

/*
 Asigna un material estandar de color c y alpha a
*/
void setFloatColorMaterial (rgbfloat c, float a);

/*
 Asigna un material predefinido
*/
void setTypeMaterial (int index);



/*************************************

Luz

**************************************/
void setSunLight (int h, int m, float clouds);
