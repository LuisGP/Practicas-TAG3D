/*
 *  perlin.cpp
 *  Practica1
 * 
 *  Created by Alejandro Ribao and Luis Gasco on 11/03/08.
 *  2008
 *
 */

#include "perlin.h"


/********************************************************************
 
 Funciones para calcular el ruido Perlin en cada punto
 
 /********************************************************************/

/*
 Pseudo random
 */
float Noise(register int x)
{
    x = (x<<13)^x;
    float r = (((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0);
	
	return r;
}



/*
 Interpolacion
 */
float InterPol(float a, float b, float x){     //a altura 1a
    return a+(b-a)*x*x*(3-2*x);                //b altura 2a
}                            




/*
 Ruido Perlin para la posicion x,y
 */
float PerlinNoise(float x,float y,int width,int octaves,float seed, float persistence){
	double a,b,valor=0,freq,cachox,cachoy;
	int casilla,num_pasos,pasox,pasoy;
	float amplitud=256*persistence;            //La amplitud es 128,64,32,16... para cada pasada
	float periodo=256;                         //El periodo es similar a la amplitud
	if (octaves>12) 
		octaves=12;
	
	
	for (int s=0;s<octaves;s++){
		amplitud/=2;                      
		periodo/=2;
		freq=1/float(periodo);             //Optimizacion para dividir 1 vez y multiplicar luego
		num_pasos=int(width*freq);         //Para el const que vimos en IntNoise
		pasox=int(x*freq);                 //Indices del vértice superior izquerda del cuadrado
		pasoy=int(y*freq);                 //en el que nos encontramos
		cachox=x*freq-pasox;               //frac_x y frac_y en el ejemplo
		cachoy=y*freq-pasoy;
		casilla=pasox+pasoy*num_pasos;     // índice final del IntNoise
		a=InterPol(Noise(casilla+seed),Noise(casilla+1+seed),cachox);
		b=InterPol(Noise(casilla+num_pasos+seed),Noise(casilla+1+num_pasos+seed),cachox);
		valor+=InterPol(a,b,cachoy)*amplitud;   //superposicion del valor final con anteriores
	}
	
	return valor;      
	
}





/*
 Funcion para generar Ruido Perlin con unas dimensiones determinadas
 */
int** genPerlinNoise (int size, int octaves, int seed, float persistence){
	int** tabla;
	
	tabla = new int*[size];
	
	for (int x=0; x<size; x++){
		tabla[x] = new int[size];
		for (int y=0; y<size; y++){
			tabla[x][y] = int (PerlinNoise(x,y,size,octaves, seed, persistence));
		}
	}	
	return tabla;
}









/********************************************************************
 
 Funciones para normalizar la imagen
 
 /********************************************************************/

/*
 Para enteros
 */
void normalizarInt(int** tabla, int size){
	int maxv = -1000;
	int minv = 9999;
	int rango;
	float factor;
	
	for (int x=0; x<size; x++){
		for (int y=0; y<size; y++){
			if (tabla[x][y] > maxv)
				maxv = tabla[x][y];
			if (tabla[x][y] <  minv)
				minv = tabla[x][y];
		}
	}
	
	rango = maxv-minv;
	factor = 255.0/rango;
	
	for (int x=0; x<size; x++){
		for (int y=0; y<size; y++){
			tabla[x][y] = (tabla[x][y]-minv) * factor;
		}
	}
	
}


/*
 Para float
 */
void normalizarFloat(float** tabla, int size){
	int maxv = -1000;
	int minv = 9999;
	float rango;
	float factor;
	
	for (int x=0; x<size; x++){
		for (int y=0; y<size; y++){
			if (tabla[x][y] > maxv)
				maxv = tabla[x][y];
			if (tabla[x][y] <  minv)
				minv = tabla[x][y];
		}
	}
	
	cerr << "max: " << maxv << " min: " << minv << endl; 
	
	rango = maxv-minv;
	factor = 1.0/rango;
	
	for (int x=0; x<size; x++){
		for (int y=0; y<size; y++){
			tabla[x][y] = (tabla[x][y]-minv) * factor;
		}
	}
	
}






/********************************************************************
 
 Funciones para escribir por pantalla el archivo PGM
 
 /********************************************************************/
/*
 Para enteros
 */
void printTablaInt ( int** tabla, int size ){
	cout << "P2"<< endl << size << " "<< size << endl << "255" << endl;
	
	for (int x=0; x<size; x++){
		for (int y=0; y<size; y++){
			cout << int(tabla[x][y]) << " ";	
		}
		cout << endl;
	}
}


/*
 Para floats
 */
void printTablaFloat ( float** tabla, int size ){
	cout << "P2"<< endl << size << " "<< size << endl << "255" << endl;
	
	for (int x=0; x<SIZE; x++){
		for (int y=0; y<SIZE; y++){
			cout << int(tabla[x][y]) << " ";	
		}
		cout << endl;
	}
	
}





/********************************************************************
 
 Funcionpara leer un archivo PGM
 
 /********************************************************************/

int** readPGM ( char* filename ){
	ifstream fich1;
	int** tablaAux;
	
	char readed[15];
	int n;
	
	fich1.open(filename, ios::in | ios::binary);
	
	if ( !fich1.is_open() ){
		cout << "No se ha podido abrir el archivo:"<< filename << endl;
		exit(1);
	}
	
	do {
		fich1 >> readed;	
	} while (strcmp(readed,"P2"));
	
	do {
		fich1 >> readed;	
	} while (readed[0]=='#');
	
	fich1 >> n;
	fich1 >> readed;
	
	tablaAux = new int*[n];
	for (int i=0; i<n; i++){
		tablaAux[i] = new int[n];
	}
	
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			fich1 >> tablaAux[i][j];
		}
	}
	
	fich1.close();
	
	return tablaAux;
}




/********************************************************************
 
 Funcionpara leer un archivo PPM
 
 /********************************************************************/

rgbint** readPPM ( char* filename ){
	ifstream fich1;
	rgbint** tablaAux;
	
	char readed[15];
	int n;
	
	fich1.open(filename, ios::in | ios::binary);
	
	if ( !fich1.is_open() ){
		cout << "No se ha podido abrir el archivo:"<< filename << endl;
		exit(1);
	}
	
	do {
		fich1 >> readed;	
	} while (strcmp(readed,"P3"));
	
	do {
		fich1 >> readed;	
	} while (readed[0]=='#');
	
	fich1 >> n;
	fich1 >> readed;
	
	tablaAux = new rgbint*[n];
	for (int i=0; i<n; i++){
		tablaAux[i] = new rgbint[n];
	}
	
	for (int i=0; i<n; i++){
		for (int j=0; j<n; j++){
			fich1 >> tablaAux[i][j].r;
			fich1 >> tablaAux[i][j].g;
			fich1 >> tablaAux[i][j].b;
		}
	}
	
	fich1.close();
	
	return tablaAux;
}




/********************************************************************
 
 Funciones para escribir en archivo PGM la imagen
 
 /********************************************************************/
/*
 Para enteros
 */
void writePGMint ( char* filename, int** tabla, int size ){
	ofstream fich1;
	
	fich1.open(filename, ios::out | ios::binary);
	
	if ( !fich1.is_open() ){
		cout << "No se ha podido crear el archivo:"<< filename << endl;
		exit(1);
	}
	
	fich1 << "P2"<< endl << "# log.ppm"<< endl << size << " "<< size << endl << "255" << endl;
	
	for (int i=0; i<size; i++){
		for (int j=0; j<size; j++){
			fich1 << tabla[i][j] << " ";
		}
		fich1 << endl;
	}
	
	fich1.close();
} 


/*
 Para floats
 */
void writePGMfloat ( char* filename, float** tabla, int size ){
	int** tablatemp = float2int(tabla, size);
	
	writePGMint(filename, tablatemp, size);
	
	freeTablaInt(tablatemp, size);
} 




/********************************************************************
 
 Funciones para escribir en archivo PPM la imagen
 
 /********************************************************************/
/*
 Para enteros
 */
void writePPMint ( char* filename, rgbint** tabla, int size ){
	ofstream fich1;
	
	fich1.open(filename, ios::out | ios::binary);
	
	if ( !fich1.is_open() ){
		cout << "No se ha podido crear el archivo:"<< filename << endl;
		exit(1);
	}
	
	fich1 << "P3"<< endl << "# log.ppm"<< endl << size << " "<< size << endl << "255" << endl;
	
	for (int i=0; i<size; i++){
		for (int j=0; j<size; j++){
			fich1 << tabla[i][j].r << " ";
			fich1 << tabla[i][j].g << " ";
			fich1 << tabla[i][j].b << " ";
		}
		fich1 << endl;
	}
	
	fich1.close();
} 


/*
 Para floats
 */
void writePPMfloat ( char* filename, rgbfloat** tabla, int size ){
	rgbint** tablatemp = rgbfloat2int(tabla, size);
	
	writePPMint(filename, tablatemp, size);
	
	freeTablaRGBInt(tablatemp, size);
} 






/********************************************************************
 
 Conversion del espacio de [0,255] a [0,1] y viceversa
 
 /********************************************************************/

/*
 De 0,255 a 0,1
 */
float** int2float ( int** tabla, int size ){
	float** tablaOut;
	
	tablaOut = new float*[size];
	
	for (int x=0; x<size; x++){
		tablaOut[x] = new float[size];
		for (int y=0; y<size; y++){
			tablaOut[x][y] = tabla[x][y] / 255.0;
		}
	}
	
	return tablaOut;
}


/*
 De 0,1 a 0,255
 */
int** float2int ( float** tabla, int size ){
	int** tablaOut;
	
	tablaOut = new int*[size];
	
	for (int x=0; x<size; x++){
		tablaOut[x] = new int[size];
		for (int y=0; y<size; y++){
			tablaOut[x][y] = int (tabla[x][y] * 255);
		}
	}
	
	return tablaOut;
}


/*
 De 0,255 a 0,1 RGB
 */
rgbfloat** rgbint2float ( rgbint** tabla, int size ){
	rgbfloat** tablaOut;
	
	tablaOut = new rgbfloat*[size];
	
	for (int x=0; x<size; x++){
		tablaOut[x] = new rgbfloat[size];
		for (int y=0; y<size; y++){
			tablaOut[x][y].r = tabla[x][y].r / 255.0;
			tablaOut[x][y].g = tabla[x][y].g / 255.0;
			tablaOut[x][y].b = tabla[x][y].b / 255.0;
		}
	}
	
	return tablaOut;
}


/*
 De 0,1 a 0,255 RGB
 */
rgbint** rgbfloat2int ( rgbfloat** tabla, int size ){
	rgbint** tablaOut;
	
	tablaOut = new rgbint*[size];
	
	for (int x=0; x<size; x++){
		tablaOut[x] = new rgbint[size];
		for (int y=0; y<size; y++){
			tablaOut[x][y].r = int (tabla[x][y].r * 255);
			tablaOut[x][y].g = int (tabla[x][y].g * 255);
			tablaOut[x][y].b = int (tabla[x][y].b * 255);
		}
	}
	
	return tablaOut;
}





/********************************************************************
 
 Funcion para liberar la memoria ocupada por la imagen
 
 /********************************************************************/
/*
 Para enteros
 */
void freeTablaInt(int** tabla, int size){
	for (int i=0; i<size; i++){
		delete[] tabla[i];
	}
	delete[] tabla;
}


/*
 Para floats
 */
void freeTablaFloat(float** tabla, int size){
	for (int i=0; i<size; i++){
		delete[] tabla[i];
	}
	delete[] tabla;
}


/*
 Para enteros RGB
 */
void freeTablaRGBInt(rgbint** tabla, int size){
	for (int i=0; i<size; i++){
		delete[] tabla[i];
	}
	delete[] tabla;
}


/*
 Para floats RGB
 */
void freeTablaRGBFloat(rgbfloat** tabla, int size){
	for (int i=0; i<size; i++){
		delete[] tabla[i];
	}
	delete[] tabla;
}



/********************************************************************
 
 Funcion para obtener uno de los canales
 
 /********************************************************************/
/*
 Para enteros
 */
int** rgbintChannel(rgbint** tabla, int chan, int size){
	int** tablaOut;
	
	tablaOut = new int*[size];
	
	for (int x=0; x<size; x++){
		tablaOut[x] = new int[size];
		for (int y=0; y<size; y++){
			switch (chan) {
				case 0 : 
					tablaOut[x][y] = tabla[x][y].r;
					break;
				case 1 : 
					tablaOut[x][y] = tabla[x][y].g;
					break;
				case 2 : 
					tablaOut[x][y] = tabla[x][y].b;
					break;
			}
		}
	}
	return tablaOut;
}

/*
 Para floats
 */
float** rgbfloatChannel(rgbfloat** tabla, int chan, int size){
	float** tablaOut;
	
	tablaOut = new float*[size];
	
	for (int x=0; x<size; x++){
		tablaOut[x] = new float[size];
		for (int y=0; y<size; y++){
			switch (chan) {
				case 0 : 
					tablaOut[x][y] = tabla[x][y].r;
					break;
				case 1 : 
					tablaOut[x][y] = tabla[x][y].g;
					break;
				case 2 : 
					tablaOut[x][y] = tabla[x][y].b;
					break;
			}
		}
	}
	return tablaOut;
}


/********************************************************************
 
 Funcion para fusionar tres canales
 
 /********************************************************************/
/*
 Para enteros
 */
rgbint** int2rgb(int** r, int** g, int** b, int size){
	rgbint** tablaOut;
	
	tablaOut = new rgbint*[size];
	
	for (int x=0; x<size; x++){
		tablaOut[x] = new rgbint[size];
		for (int y=0; y<size; y++){
			tablaOut[x][y].r = r[x][y];
			tablaOut[x][y].g = r[x][y];
			tablaOut[x][y].b = b[x][y];
		}
	}
	return tablaOut;
}

/*
 Para floats
 */
rgbfloat** float2rgb(float** r, float** g, float** b, int size){
	rgbfloat** tablaOut;
	
	tablaOut = new rgbfloat*[size];
	
	for (int x=0; x<size; x++){
		tablaOut[x] = new rgbfloat[size];
		for (int y=0; y<size; y++){
			tablaOut[x][y].r = r[x][y];
			tablaOut[x][y].g = r[x][y];
			tablaOut[x][y].b = b[x][y];
		}
	}
	return tablaOut;
}


/********************************************************************
 
 Funcion para cargar una paleta
 
 /********************************************************************/
/*
 Para enteros
 */
rgbint* loadPalInt(char* filename, int size){
	ifstream fich1;
	rgbint* tablaAux;
	
	char readed[15];
	int n = size;
	
	fich1.open(filename, ios::in | ios::binary);
	
	if ( !fich1.is_open() ){
		cout << "No se ha podido abrir el archivo:"<< filename << endl;
		exit(1);
	}
	
	do {
		fich1 >> readed;	
	} while (strcmp(readed,"P3"));
	
	do {
		fich1 >> readed;	
	} while (readed[0]=='#');
	
	fich1 >> readed;
	fich1 >> readed;
	
	tablaAux = new rgbint[n];
	
	for (int i=0; i<n; i++){
		fich1 >> tablaAux[i].r;
		fich1 >> tablaAux[i].g;
		fich1 >> tablaAux[i].b;
	}
	
	fich1.close();
	
	return tablaAux;
}




/*
 Convertir a floats
 */
rgbfloat* intPal2Float(rgbint* pal, int size){
	rgbfloat* tablaOut = new rgbfloat[size];
	
	for (int i=0; i<size; i++){
		tablaOut[i].r = pal[i].r / 255.0; 
		tablaOut[i].g = pal[i].g / 255.0; 
		tablaOut[i].b = pal[i].b / 255.0; 
		
	}
	
	return tablaOut;
}





/********************************************************************
 
 Funcion para generar un cielo
 
 /********************************************************************/
/*
 Segun la paleta pal y la densidad de nubes 'nivel' [0,255] indicando como de despejado esta
 */
rgbint** skyrgb(rgbint* pal, int nivel, int size){
	int** tabla = genPerlinNoise(size, 6, ((long)time(NULL))%1024, 1);
	
	normalizarInt(tabla, size);
	
	rgbint** out = new rgbint*[size];
	for (int i=0; i<size; i++){
		out[i] = new rgbint[size];
		for (int j=0; j<size; j++){
			int v = tabla[i][j];
			if (v < nivel){
				out[i][j] = pal[0];
			}else if (nivel != 0) {
				out[i][j] = pal[int((v-nivel)*(255.0/(255-nivel)))];
			}else{
				out[i][j] = pal[v];
			}
		}
		
	}
	freeTablaInt(tabla, size);
	return out;
}



/********************************************************************
 
 Asigna una paleta a un mapa en escala de grises (es recomendable normalizar antes)
 
 /********************************************************************/
/*
 Para enteros
 */
rgbint** applyPal(rgbint* pal, int** tabla, int size){
	rgbint** out = new rgbint*[size];
	for (int i=0; i<size; i++){
		out[i] = new rgbint[size];
		for (int j=0; j<size; j++){
			out[i][j] = pal[tabla[i][j]];
		}
	}
	return out;
}


/*
 Para floats
 */
rgbfloat** applyPal(rgbfloat* pal, float** tabla, int size){
	rgbfloat** out = new rgbfloat*[size];
	for (int i=0; i<size; i++){
		out[i] = new rgbfloat[size];
		for (int j=0; j<size; j++){
			out[i][j] = pal[int(tabla[i][j]*255)];
		}
	}
	return out;
}


/*************************************

 Materiales

**************************************/

void setMaterial (GLfloat ar, GLfloat ag, GLfloat ab, GLfloat dr, GLfloat dg, GLfloat db, GLfloat alpha,
				  GLfloat sr, GLfloat sg, GLfloat sb, GLfloat er, GLfloat eg, GLfloat eb, GLfloat shine){
	
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
//	glColor4f(ar,ag,ab,alpha);
	glDisable(GL_COLOR_MATERIAL);
	GLfloat color[4];
	color[0] = ar;
	color[1] = ag;
	color[2] = ab;
	color[3] = 1;
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
	color[0] = dr;
	color[1] = dg;
	color[2] = db;
	color[3] = alpha;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
//	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glColor4f(dr,dg,db,alpha);
	color[0] = sr;
	color[1] = sg;
	color[2] = sb;
	color[3] = 1;
    glMaterialfv(GL_FRONT, GL_SPECULAR, color);
	color[0] = er;
	color[1] = eg;
	color[2] = eb;
	color[3] = 1;
    glMaterialfv(GL_FRONT, GL_EMISSION, color);
	glMaterialf(GL_FRONT, GL_SHININESS, shine * 128.0);
	
}


void setIntColorMaterial (rgbint c, int a){
	setMaterial(0.0, 0.0, 0.0,						// amb
				c.r/255.0, c.g/255.0, c.b/255.0, a/255.0,	//diff
				0.2, 0.2, 0.2,		//spec
				0, 0, 0, .2);		//emi shn
}


void setFloatColorMaterial (rgbfloat c, float a){
	setMaterial(0.0, 0.0, 0.0,						// amb
				c.r, c.g, c.b, a,	//diff
				0.2, 0.2, 0.2,		//spec
				0, 0, 0, .2);		//emi shn
}


void setTypeMaterial (int index){
	switch (index){
		case SNOW :
			setMaterial(0.6, 0.6, 0.6,		// amb
						0.7, 0.7, 0.7, 1,	//diff
						1, 1, 1,			//spec
						0, 0, 0, .03);		//emi shn
			break;
		
		case LAND :
			setMaterial(0.0, 0.0, 0.0,		// amb   
						0.6, 0.55, 0.5, 1,	//diff
						0.05, .05, 0.05,	//spec
						0, 0, 0, .7);		//emi shn
			break;

		case GRASS1 :
			setMaterial(0, 0, 0,			// amb
						0.1, 0.35, 0.1, 1,	//diff
						0.45, .75, .15,		//spec
						0, 0, 0, .25);		//emi shn
			break;

		case GRASS2 :
			setMaterial(0, 0, 0,			// amb
						0.2, 0.35, 0.1, 1,	//diff
						.85, .85, .45,		//spec
						0, 0, 0, .25);		//emi shn
			break;

		case SAND :
			setMaterial(0, 0, 0,			// amb
						0.5, 0.4, 0.4, 1,	//diff
						.7, .55, .2,		//spec
						0, 0, 0, .02);		//emi shn
			break;

		case WATER :
			setMaterial(0, 0, 0,			// amb
						0.65, 0.9, 1, .3,	//diff
						1, 1, 1,			//spec
						0, 0, 0, .8);		//emi shn
			break;
	}
}





/*************************************

Luz

**************************************/
void setSunLight (int h, int m, float clouds, GLfloat* position, GLfloat* diffuse, GLfloat* clearColor,  float escala){
	int mins = (h%24)*60+m%60;
	float ang = -2*M_PI*mins/1440.0;
	float altura = 512;
	// 00:00 = 0 mins = 1440 mins = 0 = 2PI = (128,-1024,128)
	// 12:00 = 720 mins = PI = (128,1024,128)
	// 6:00 = 360 mins = PI/2 = (128+1024,0,128)
	// 18:00 = 1080 mins = 3PI/2 = (128-1024,0,128)
   GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
   position[0] = -sin(ang)*altura*escala;
   position[1] = -cos(ang)*altura*escala;
   position[2] = 0;
   position[3] = 1.0;
   
   //GLfloat clearColor[4]; // Color del cielo
   clearColor[3] = 1;

   // Color en funcion del angulo
   GLfloat r = 1;
   float posf = (position[1])/altura;
   if (h == 17 && m > 50){
		r = (60-m)/10.0;
	}
   if (h == 6 && m < 10){
		r = ((m))/10.0;
	}	
   if (position[1] < 0) r=0;
   GLfloat g = posf*2;
   if ( h == 6 && m > 5 || h>6 && h<17 || h == 17 && m < 55 ){
		g=g*4;
   }
   if (g > 1) g=1;
   if (g < 0) g=0;

   GLfloat b = posf*4;
   if (h == 17 && m > 1){
		b = (60-m)/59.0*b;
	}
   if (h == 6 && m < 59){
		b = ((m))/59.0*b;
	}	
   if (b > 1) b=1;
   if (b < 0) b=-b;

   // efecto de las nubes
   float atenuar = posf*clouds/10;
   r = (r+0.6*clouds)*(1-clouds)+atenuar;
   g = (g+1*clouds)*(1-clouds)+atenuar;
   b = (b+1.4*clouds)*(1-clouds)+atenuar;
  
   diffuse[0] = r;
   diffuse[1] = g;
   diffuse[2] = b;
   
   
   
   if (h > 6 && h < 17 || h==6 && m>20 || h == 17 && m<40){
		diffuse[3] = 1.0;
   }else{
		diffuse[3] = 0.0;
		if (h==6 && m <= 20)
				diffuse[3] = m/20.0+.1;

		if (h==17 && m >= 40)
				diffuse[3] = (60-m)/20.0+.1;
   }

   
   
   
   
   
   // ambient
   /*
   r = posf+r/2+.1;
   if (r<0.1) r=0.1;
   if (position[1] < -0.5) r=0.1;
   g = posf+g/2+.1;
   if (g<0.1) g=0.1;
   if (position[1] < -0.5) g=0.1;
   b = posf+b/2+.15;
   if (b<0.15) b=0.15;
   if (position[1] < -0.5) b=0.15;
*/
	r = 0.1;
	g = .1;
	b = .15;
	
   GLfloat ambient[] = {r, g, b, 1.0};

   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
   glLightfv(GL_LIGHT0, GL_POSITION, position);
	


	// ROJO
	clearColor[0] = 0.3;
	if ( h==6 && m < 30){
		clearColor[0] = (1-((mins-360)/30.0))*.1+.3;
		if (clearColor[0] > 1) clearColor[0] = 1;
		if (m < 10)
			clearColor[0] = ((mins-360)/10.0)*.4;
	}
	if ( h==17 && m> 30 ){ /////// FALTA PROBARLO
		int submins = 1440-mins;
		clearColor[0] = (1-((submins-360)/30.0))*.1+.3;
		if (clearColor[0] > 1) clearColor[0] = 1;
		if (m > 50)
			clearColor[0] = ((submins-360)/10.0)*.4;
	}

	// VERDE
	clearColor[1] = 0.7;
	if ( h==6 && m < 38){
		clearColor[1] = ((mins-362)/35.0)*.7;
		if (clearColor[1] > 1) clearColor[1] = 1;
	}
	if ( h==17 && m > 22){ /////// FALTA PROBARLO
		int submins = 1440-mins;
		clearColor[1] = ((submins-362)/35.0)*.7;
		if (clearColor[1] > 1) clearColor[1] = 1;
	}

	// AZUL
	clearColor[2] = 1;
	if ( h==6 && m < 58){
		clearColor[2] = ((mins-365)/55.0);
		if (clearColor[2] < 0) clearColor[2] = 0;
		if (clearColor[2] > 1) clearColor[2] = 1;
	}
	if ( h==17 && m > 2 ){ /////// FALTA PROBARLO
		int submins = 1440-mins;
		clearColor[2] = ((submins-365)/55.0);
		if (clearColor[2] < 0) clearColor[2] = 0;
		if (clearColor[2] > 1) clearColor[2] = 1;
	}


  //if (h==17)
	//cout << 1440-mins << ":" << clearColor[0] << " "<< clearColor[1] << " "<< clearColor[2] << " " << endl;

   glClearColor(clearColor[0],clearColor[1],clearColor[2],clearColor[3]);
	
	if (h < 6 || h > 17)
		glClearColor(0, 0, 0,1.0);  /* night */


	if (h > 6 && h < 17 || h==6 && m>58 || h == 17 && m<2 )
		glClearColor(0.3, 0.7, 1,1.0);  /* Blue background */

	
   return;
}


