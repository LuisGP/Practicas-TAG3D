/*
 *  Practica1.cpp
 *  Practica1
 *
 *  Created by Alejandro Ribao on 11/03/08.
 *  2008
 *  Based on "Práctica 3 GPGPU C code"
 *
 */
#include "Practica1.h"


/************************************************************************************************
 Asignar colores
 ************************************************************************************************/
/**
 * Asigna los colores a los vértices en función de la paleta de césped
 * No crece césped en las pendientes demasiado escarpadas
 */
void cesped(int i, int j, int ind){
	ind = ind+(rand()%5-(5/2));
	if (ind > 255) ind = 255;
	float aux[3] = {normalesv[i][j][0],normalesv[i][j][1],normalesv[i][j][2]};
	if (aux[0] < 0) aux[0] = -aux[0];
	if (aux[2] < 0) aux[2] = -aux[2];
	
	if (aux[1] > aux[2] && aux[1] > aux[0]){ // Poco inclinado
		colores[i][j][0] = paleta_grassf[ind].r;
		colores[i][j][1] = paleta_grassf[ind].g;
		colores[i][j][2] = paleta_grassf[ind].b;
	}else if (aux[1]*1.5 > aux[2] && aux[1]*1.5 > aux[0]){ // Algo inclinado
		colores[i][j][0] = colores[i][j][0]*.25 + paleta_grassf[ind].r*.75;
		colores[i][j][1] = colores[i][j][1]*.25 + paleta_grassf[ind].g*.75;
		colores[i][j][2] = colores[i][j][2]*.25 + paleta_grassf[ind].b*.75;
	}
}


/**
 * Asigna los colores a los vértices en función de la paleta de nieve y su cota
 * No hay nieve en las pendientes demasiado escarpadas ni en las caras que dan al sur
 * Pero eso varía en función de la altura
 */
void nevar(int i, int j, int ind){
	float aux[3] = {normalesv[i][j][0],normalesv[i][j][1],normalesv[i][j][2]};
	if (aux[0] < 0) aux[0] = -aux[0];
	aux[1] += ind*(5/aux[1]);  // Probabilidad aumenta con la altura
	if (aux[2] < 0) 
		aux[2] += ind*(2/aux[2]);  // La orientación pierde importancia con la altura
	else
		aux[2] *= 2*aux[2];
	
	
	if (aux[2] < 0 && aux[1] > aux[2] && aux[1] > aux[0]){
		colores[i][j][0] = 1;
		colores[i][j][1] = 1;
		colores[i][j][2] = 1;
	}
}


/** 
 * Asigna un color al vértice (1) en función de la paleta de terreno y agua
 * El margen indica la aleatoriedad con la que se toman las muestras de color
 */
void genColorv(int i, int j, int margen){
	int ind;
	bool underwater = false;
	rgbfloat* pal;
	int alt = vertices[i][j][1];
	if (margen != 0) { // Color aleatorio dentro del margen
		ind = vertices[i][j][1]+(rand()%margen-(margen/2));
	} else { // Color coincide con altura si no se especifica margen
		ind = alt;
	}
	if (ind > NIVEL_LAGO*RELIEVE){ //Sobre el nivel del agua
		pal = paletaf;
		ind -= NIVEL_LAGO*RELIEVE;
	}else{							//Bajo el nivel del agua
		pal = paleta_aguaf;
		ind = NIVEL_LAGO*RELIEVE-ind;
		underwater = true;
	}
	if (ind < 0) ind=0;
	if (ind > 254) ind=254;
	// Elegir el tono intermedio correspondiente
	colores[i][j][0] = interpolar(pal[int(ind)].r, pal[int(ind)+1].r, alt-int(alt)); 
	colores[i][j][1] = interpolar(pal[int(ind)].g, pal[int(ind)+1].g, alt-int(alt)); 
	colores[i][j][2] = interpolar(pal[int(ind)].b, pal[int(ind)+1].b, alt-int(alt)); 
	
	if (!underwater && ind > NIVEL_LAGO+3){ // Pintar con césped
		cesped(i, j, alt);
	}
	if (!underwater && ind > NIVEL_NIEVE){ // Pintar con nieve
		nevar(i,j, ind);
	}
	
}

/**
 * Asigna los colores a los vértices en función de la paleta de terreno y del agua
 * El margen indica la aleatoriedad con la que se toman las muestras de color
 */
void genColores(int margen){
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			genColorv(i,j,margen);
		}
	}
}


/**
 * Lee y genera las texturas utilizadas
 */
void genTexturas() {
	cout << "CARGANDO: " ;
	cout << "landcolor3.ppm";
	paleta = loadPalInt("paletas/landcolor3.ppm", 256);
	paletaf = intPal2Float(paleta, 256);
	cout << "watercolor7.ppm";
	paleta_agua = loadPalInt("paletas/watercolor7.ppm", 256);
	paleta_aguaf = intPal2Float(paleta_agua, 256);
	cout << "grasscolor2.ppm";
	paleta_grass = loadPalInt("paletas/grasscolor2.ppm", 256);
	paleta_grassf = intPal2Float(paleta_grass, 256);
	cout << "tree.ppm";
	tree = readPPM("paletas/tree.ppm");
	cout << "treesnow.ppm";
	snowtree = readPPM("paletas/treesnow.ppm");
	cout << "treealpha.ppm";
	treealpha = readPGM("paletas/treealpha.pgm");
	cout << "palm.ppm";
	palm = readPPM("paletas/palm.ppm");
	cout << "palmalpha.ppm";
	palmalpha = readPGM("paletas/palmalpha.pgm");
	
	int* datatree = genTextIntAlpha(tree, treealpha, 256);
	int* datasnowtree = genTextIntAlpha(snowtree, treealpha, 256);
	int* datapalm = genTextIntAlpha(palm, palmalpha, 256);
	cout << "skyalpha.ppm" << endl;
	paletanubes = loadPalInt("paletas/skyalpha.ppm", 256);

	cout << "Generando nubes" << endl;
	nubesalpha = skyalpha3D(paletanubes, FRAMES_NUBES, int((nubosidad)*128.0), 256);
	nubes = new rgbint*[256]; // nubes blancas
	for (int i=0; i<256; i++){
		nubes[i] = new rgbint[256];
		for (int j=0; j<256; j++){
			nubes[i][j].r = 255;
			nubes[i][j].g = 255;
			nubes[i][j].b = 255;		
		}
	}
	int** datanubes = new int*[FRAMES_NUBES];
	for (int i=0; i<FRAMES_NUBES; i++){
		datanubes[i] = genTextIntAlpha(nubes,nubesalpha[i],256);
	}
	
	// Genera los nombres de textura 
	cout << "ENLAZANDO NOMBRES" << endl;
	glGenTextures(FRAMES_NUBES+10, texNames);
	glBindTexture(GL_TEXTURE_2D, texNames[ARBOL]);
	genText(datatree, false, 256);
	delete[] datatree;
	
	glBindTexture(GL_TEXTURE_2D, texNames[ARBOLNIEVE]);
	genText(datasnowtree,false, 256);	
	delete[] datasnowtree;
	
	glBindTexture(GL_TEXTURE_2D, texNames[PALM]);
	genText(datapalm,false, 256);	
	delete[] datapalm
	;
	
	
	for (int i=0; i<FRAMES_NUBES; i++){
		glBindTexture(GL_TEXTURE_2D, texNames[NUBES+i]);
		genText(datanubes[i], true, 256);
		delete[] datanubes[i];	
	}
	
	cout << "BRUJULA" << endl;
	brujula = new rgbint*[128]; // brujula negra
	for (int i=0; i<128; i++){
		brujula[i] = new rgbint[128];
		for (int j=0; j<128; j++){
			brujula[i][j].r = 0;
			brujula[i][j].g = 0;
			brujula[i][j].b = 0;		
		}
	}
	brujulaalpha = readPGM("paletas/brujulaalpha.pgm");
	int* databrujula = genTextIntAlpha(brujula, brujulaalpha, 128);
	aguja = readPPM("paletas/aguja.ppm");
	agujaalpha = readPGM("paletas/agujaalpha.pgm");
	int* dataaguja = genTextIntAlpha(aguja, agujaalpha, 128);
	
	glBindTexture(GL_TEXTURE_2D, texNames[BRUJULA]);
	genText(databrujula,false, 128);	
	delete[] databrujula;
	
	glBindTexture(GL_TEXTURE_2D, texNames[AGUJA]);
	genText(dataaguja,false, 128);	
	delete[] dataaguja;
	
	cout << "LLUVIA" << endl;
	rgbint** lluvia = new rgbint*[128];
	for (int i=0; i<128; i++){
		lluvia[i] = new rgbint[128];
		for (int j=0; j<128; j++){
			lluvia[i][j].r = 255;
			lluvia[i][j].g = 255;
			lluvia[i][j].b = 255;	
		}
	}
	int** lluviaalpha = new int*[128];
	for (int i=0; i<128; i++){
		lluviaalpha[i] = new int[128];
		for (int j=0; j<128; j++){
			lluviaalpha[i][j] = rand()%255;
		}
	}
	
	int* datalluvia = genTextIntAlpha(lluvia, lluviaalpha, 128);
	glBindTexture(GL_TEXTURE_2D, texNames[LLUVIA]);
	genText(datalluvia, true, 128);
	delete[] datalluvia;	
	
	cout << "colores.ppm" << endl;
	texterreno = readPPM("paletas/colores.ppm");
	int* dataterreno = genTextInt(texterreno, 255, 2048);
	glBindTexture(GL_TEXTURE_2D, texNames[TERRENO]);
	genText(dataterreno, true, 2048);
	delete[] dataterreno;	
	
	cout << "Liberando recursos" << endl;
	freeTablaRGBInt(texterreno, 2048);
	freeTablaRGBInt(tree, 256);
	freeTablaRGBInt(snowtree, 256);
	freeTablaInt(treealpha, 256);
	freeTablaRGBInt(nubes, 256);
	freeTablaRGBInt(brujula, 128);
	freeTablaRGBInt(aguja, 128);
	freeTablaInt(agujaalpha, 128);
	freeTablaInt(brujulaalpha, 128);
	
}

/**
 * Genera las posiciones de los árboles. 
 * La cercania indica cómo de lejos deben estar entre sí
 */
void genArboles(int cercania){
	for (int i=1; i<ALTO-1; i++){
		for (int j=1; j<ANCHO-1; j++){
			for (int k=i-cercania; k<i; k++){
				for (int l=j-cercania; k<j; k++){
					if (k>=0 && l>=0){
						if (arboles[k][l]){
							arboles[i][j] = false;
							//cout << "vecino en " << k << ","<< l << " a: " << i-k<< "," << j-l<< endl;
							continue;
						}
					}
				}
			}
			
			// No planta árboles en colinas
			float aux[3] = {normalesv[i][j][0],normalesv[i][j][1],normalesv[i][j][2]};
			if (aux[0] < 0) aux[0] = -aux[0];
			if (aux[2] < 0) aux[2] = -aux[2];
			
			if (aux[1] > aux[2] && aux[1] > aux[0] && alturas[i][j] > 0){
				arboles[i][j] = ((rand()%(alturas[i][j]*2)) == (NIVEL_LAGO+3)*2); 
			}
		}
	}
}


/**
 * Genera posiciones para las estrellas de forma aleatoria
 */
void genEstrellas(){
	int cercania = 3;
	estrellas = new rgbint*[512];
	for (int i=0; i<512; i++){
		estrellas[i] = new rgbint[512];
		for (int j=0; j<512; j++){
			for (int k=i-cercania; k<i; k++){
				int v = rand()%3000 < 5;
				if (v){
					estrellas[i][j].r = 100+ 25*v;
					estrellas[i][j].g = 100+ 25*v;
					estrellas[i][j].b = 100+ 25*v;
				}else{
					estrellas[i][j].r = 0;
					estrellas[i][j].g = 0;
					estrellas[i][j].b = 0;
				}
			}
		}
	}
	//writePPMint("../../estrellas.ppm", estrellas, 512);
	int* dataestrellas = genTextInt(estrellas, 255, 512);
	glBindTexture(GL_TEXTURE_2D, texNames[ESTRELLAS]);
	genText(dataestrellas, true, 512);
	delete[] dataestrellas;
}

/**
 * Varía la cota del nivel del agua
 */
void cambiarCotaLago(int n){
	if (n<0)
		return;
	NIVEL_LAGO = n;
	
	// Genera todos los parámetros en el orden necesario
	genVertices(alturas, suave);
	genNormalesLago();
	genLago();
	genNormalesT();
	genNormalesV();
	genColores(margen);
}


/**
 * Varía la cota de nieve
 */
void cambiarCotaNieve(int n){
	if (n<0)
		return;
	NIVEL_NIEVE = n;
	
	// Genera todos los parámetros en el orden necesario
	genVertices(alturas, suave);
	//	genNormalesLago();
	//	genLago();
	genNormalesT();
	genNormalesV();
	genColores(margen);
}


/**
 * Varía la escala del terreno
 */
void cambiarEscala(float n){
	if (n<1)
		return;
	ESCALA = n;
	
	// Genera todos los parámetros en el orden necesario
	genVertices(alturas, suave);
	genNormalesLago();
	genLago();
	genNormalesT();
	genNormalesV();
	genColores(margen);
}


/**
 * Varía el factor de relieve del terreno
 */
void cambiarRelieve(float n){
	if (n<0.25)
		return;
	RELIEVE = n;
	
	// Genera todos los parámetros en el orden necesario
	genVertices(alturas, suave);
	genNormalesLago();
	genLago();
	genNormalesT();
	genNormalesV();
	genColores(margen);
}



/************************************************************************************************
 Generar geometría
 ************************************************************************************************/
/**
 * Genera la geometría del terreno en función de las alturas calculadas
 * Lo suaviza si se pide
 */
void genVertices(int** alts, bool suav){
	
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			vertices[i][j][0] = (i-(M_ALTO)+1)*ESCALA;
			if (suav)
				vertices[i][j][1] = suavizar5(i,j,ALTO,ANCHO,alturas)* RELIEVE;
			else
				vertices[i][j][1] = alts[i][j]* RELIEVE;
			vertices[i][j][2] = (j-(M_ANCHO)+1)*ESCALA;
		}
	}
}



/**
 * Deforma la geometría del terreno
 */
void realizarDeformacion(int n_alt){
	// la posicion viene dada por camX y camZ... sólo si estamos en el terreno
	float acamX = (camX / ESCALA)+M_ALTO; // Normalizamos
	float acamZ = (camZ / ESCALA)+M_ANCHO;
	
	deformarTerreno(n_alt, acamX, acamZ, influencia, true);
}

int auxIndiceInfluencia(int i, int j, int x, int y, int influencia){
	int auxX, auxY, aux, lado;
	
	lado = 2*influencia + 1; 
	
	// Posicion relativa: (0,0) sería (x,y)
	auxX = i-x;
	auxY = j-y;
	
	// Posicion auxiliar, con (0,0) en (-influencia, -influencia)
	auxX += influencia;
	auxY += influencia;
	
	// Posicion vectorial
	aux = (auxX * lado) + auxY;
	
	return aux;
}



void deformarTerreno(int n_altura, int x, int y, int influencia, bool suave){
	int pos;
	// Recogemos las alturas antiguas que serán modificadas
	ant_alturas = (GLfloat*)malloc(sizeof(GLfloat)*(2*influencia+1)*(2*influencia+1));
	
	// Asignamos pesos al area de influencia
	pesos = (float*)malloc(sizeof(float)*(2*influencia+1)*(2*influencia+1));
	
	float d2;
	
	//cout << "Pesos" << endl;
	for(int i = x-influencia; i < x+influencia; i++){
		for(int j = y-influencia; j < y+influencia; j++){
			if(i >= 0 && j >= 0 && i < ALTO && j < ANCHO){
				pos = auxIndiceInfluencia(i,j,x,y,influencia);
				d2 = ((x-i)*(x-i))+((y-j)*(y-j)); // Distancia al cuadrado
				pesos[pos] = (influencia-(d2/10))/influencia;
				if(pesos[pos] < 0){
					pesos[pos] = 0;
				}
			}
		}
	}
	
	// Modificamos la altura del terreno segun los pesos y el area de influencia
	for(int i = x-influencia; i < x+influencia; i++){
		for(int j = y-influencia; j < y+influencia; j++){
			if(i >= 0 && j >= 0 && i < ALTO && j < ANCHO){
				pos = auxIndiceInfluencia(i,j,x,y,influencia);
				alturas[i][j] = alturas[i][j] + ((n_altura * pesos[pos]));// * RELIEVE);
				vertices[i][j][1] = vertices[i][j][1] + ((n_altura * pesos[pos]) * RELIEVE);
				genColorv(i,j,margen);
				if (arboles[i][j] && (alturas[i][j] < (NIVEL_LAGO+7)))
					arboles[i][j] = false;
			}
		}
	}
	
	if(andando){
		setAlturaPj();
	}
	
	// Regenerar el terreno
	genLago();
	genNormalesLago();
	genNormalesT();
	genNormalesV();
	
	delete[] ant_alturas;
	delete[] pesos;
	ant_alturas = NULL;
	pesos = NULL;
}


/**
 * Plantar o arrancar un árbol de la posición actual
 */
void modificarArbol(bool plantar){
	int acamX = (camX / ESCALA)+M_ALTO; // Normalizamos
	int acamZ = (camZ / ESCALA)+M_ANCHO;

	arboles[acamX][acamZ] = plantar;
	if (!plantar){
		arboles[acamX+1][acamZ] = plantar;
		arboles[acamX][acamZ+1] = plantar;
		arboles[acamX+1][acamZ+1] = plantar;
		arboles[acamX-1][acamZ] = plantar;
		arboles[acamX][acamZ-1] = plantar;
		arboles[acamX-1][acamZ-1] = plantar;
		arboles[acamX-1][acamZ+1] = plantar;
		arboles[acamX+1][acamZ-1] = plantar;
	}
}


/**
 * Genera la geometría de la superficie del agua
 */
void genLago(){
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			lago[i][j][0] = (i-(M_ALTO)+1)*ESCALA;
			lago[i][j][1] = NIVEL_LAGO*RELIEVE+0.5; // Evita errores con Z-Buffer
			lago[i][j][2] = (j-(M_ANCHO)+1)*ESCALA;
		}
	}
}


/************************************************************************************************
 Generar normales
 ************************************************************************************************/
/**
 * Genera las normales a la superficie del agua, simulando su variación
 */
void genNormalesLago(){
	int ind;
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			if (vertices[i][j][1] < NIVEL_LAGO*RELIEVE+2){
				// Elegir orientación en función de la hora y de la distancia a la orilla 
				if (!llueve || llueve && intensidadlluvia < 0.8)
					ind = int(vertices[i][j][1]-(minutos%10))%10; 
				else
					ind = int(vertices[i][j][1]-(rand()%10))%10; 
				if (ind < 0) ind = -ind;
				float mod = sqrt(olas[ind]*olas[ind]*2+1);
				normaleslago[i][j][0] = olas[ind]/mod;
				normaleslago[i][j][1] = 1/mod;
				normaleslago[i][j][2] = normaleslago[i][j][0];
			}
		}
	}
	
}


/**
 * Genera las normales de los triángulos superficie del terreno
 */
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


/**
 * Genera las normales de los vértices de la superficie del terreno
 * Es necesario calcular antes las de los triángulos
 */
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
			normalesv[i][j][0] = media[0]; 
			normalesv[i][j][1] = media[1];
			normalesv[i][j][2] = media[2];
			// normalizar
			float mod = sqrt(modulo2(normalesv[i][j]));
			normalesv[i][j][0] = media[0]/mod; 
			normalesv[i][j][1] = media[1]/mod;
			normalesv[i][j][2] = media[2]/mod;
			
		}
	}
}


/************************************************************************************************
 Inicializaciones
 ************************************************************************************************/
/**
 * Genera los parámetros iniciales del terreno
 */
void genTerreno(bool read, char* fname){
	int i, j;
	int maximo = -(((unsigned int)-1)/2);
	int minimo = 2147483647;
	
	
	// Alturas generadas con ruido Perlin
	if (!read){
		alturas = genPerlinNoise(ANCHO, -666, 3000);		
	

//		writePGMint("../../alturas.pgm", alturas, ANCHO);
	}else{
		alturas = readPGM(fname);
	}
	
	for(i = 0; i < ALTO; i++){
		for(j = 0; j < ANCHO; j++){
			if(maximo < alturas[i][j])
				maximo = alturas[i][j];
			if(minimo > alturas[i][j])
				minimo = alturas[i][j];
		}
	}
	
	// "Normalizamos"
	maximo -= minimo;
	
	for(i = 0; i < ALTO; i++){
		for(j = 0; j < ANCHO; j++){
			alturas[i][j] = alturas[i][j] - minimo; // Bajamos la altura de todo
		}
	}
	
	// Genera todos los parámetros en el orden necesario
	genVertices(alturas, suave);
	genNormalesLago();
	genLago();
	genNormalesT();
	genNormalesV();
	genColores(margen);
	
	/*
	 // Volcar colores a archivo 	
	 float* r[ALTO];
	 float* g[ALTO];
	 float* b[ALTO];
	 
	 for(i = 0; i < ALTO; i++){
	 r[i] = new float[ANCHO];
	 g[i] = new float[ANCHO];
	 b[i] = new float[ANCHO];
	 
	 for(j = 0; j < ANCHO; j++){
	 r[i][j] = colores[i][j][0];
	 g[i][j] = colores[i][j][1];
	 b[i][j] = colores[i][j][2];
	 }
	 }
	 // Volcar los colores actuales a fichero
	 rgbfloat** tablacolores = float2rgb(r, g, b, ALTO);
	 writePPMfloat("../../coloresgenerados.ppm", tablacolores, ALTO);
	 */
	
	genArboles(1);
	genEstrellas();
	
	// Pone la cámara en el suelo
	camY = vertices[127][127][1]+3*RELIEVE;
	
}


/**
 * Inicializa todos los parámetros
 */
void init(bool read, char* fname){
	
	srand(time(NULL));
	
	// luces
	cout << "LUZ" << endl;
	glLightModelfv (GL_LIGHT_MODEL_AMBIENT, luzambiente);
	setSunLight(horas, minutos, 0, posicion_luz, luzdifusa, colorcielo, ESCALA);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzdifusa);	// Ponemos valores a la luz 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzambiente);	// Ponemos valores a la luz 0
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzspecular); // Ponemos valores a la luz 0
	glLightfv(GL_LIGHT0, GL_EMISSION, luzemision);	// Ponemos valores a la luz 0
	glLightf(GL_LIGHT0, GL_SHININESS, brillo);	    // Ponemos valores a la luz 0
	glLightfv(GL_LIGHT0, GL_POSITION, posicion_luz);	// Ponemos la posiciÛn para gl_light0
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	// Ahora ponemos la luz que ya definimos
	
	// Transparencia
	cout << "BLENDING" << endl;
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 	
	
	// Texturas
	cout << "TEXTURAS" << endl;
	genTexturas();
	
	// Genera el terreno 
	cout << "TERRENO" << endl;
	genTerreno(read, fname);
	
	// Dejamos a OpenGL normalizar las normales
	//glEnable(GL_NORMALIZE); 
	
	// Parámetros para calcular los fps
	t_fps = clock() * CLK_TCK ;
	fps = 0;
	tiempo = clock() * CLK_TCK; //time(NULL);
}


/************************************************************************************************
 Modificaciones de la cámara
 ************************************************************************************************/
/**
 * La altura del personaje en el modo ANDAR es la altura de la cámara
 */
void setAlturaPj(){
	// la posicion viene dada por camX y camZ... sólo si estamos en el terreno
	float acamX = (camX / ESCALA)+M_ALTO; // Normalizamos
	float acamZ = (camZ / ESCALA)+M_ANCHO;
	
	
	// Para suavizar, hacemos la media entre la anterior altura y
	// la del vertice correspondiente
	float aux, auxl;
	
	if(acamX > 2 && acamX < ALTO-2 && acamZ > 2 && acamZ < ANCHO-2){
		aux = ((vertices[(int)floor(acamX)][(int)floor(acamZ)][1]+alturaPj)+(camY))/2;
		auxl = lago[(int)floor(acamX)][(int)floor(acamZ)][1]+alturaPjNado;
		camY = max(auxl, aux);
	}
}


/**
 * Navegación - Giros de cámara
 */
void clampCamera(){
	if( camPitch > 90.0 )
		camPitch = 90.0;
	else if( camPitch < -90.0 )
		camPitch = -90.0;
	while( camYaw < 0.0 )
		camYaw += 360.0;
	while( camYaw >= 360.0 )
		camYaw -= 360.0;
}


/**
 * Más navegación - Movimientos de cámara
 */
void dollyCamera( GLfloat dollyBy, GLfloat dollyAngle ){
	GLfloat actualAngleInRadians;
	actualAngleInRadians = ( ( camYaw + dollyAngle ) * M_PI / 180.0 );
	camX -= sin( actualAngleInRadians ) * dollyBy * DEF_dollyStepSize;
	camZ -= cos( actualAngleInRadians ) * dollyBy * DEF_dollyStepSize;
	// Evitar que se salga mientras anda
	if(andando){
		if(camX > M_ALTO*ESCALA-10){
			camX = M_ALTO*ESCALA-10;
		}
		if(camX < -M_ALTO*ESCALA+10){
			camX = -M_ALTO*ESCALA+10;
		}
		if(camZ > M_ANCHO*ESCALA-10){
			camZ = M_ANCHO*ESCALA-10;
		}
		if(camZ < -M_ANCHO*ESCALA+10){
			camZ = -M_ANCHO*ESCALA+10;
		}
	}
}


/** 
 * Gira la cámara
 */
void travCamera(GLfloat dollyAngle){
	GLfloat actualAngleInRadians;
	actualAngleInRadians = dollyAngle * M_PI / 180.0;
	camX -= sin( actualAngleInRadians ) * DEF_dollyStepSize;
	camZ -= cos( actualAngleInRadians ) * DEF_dollyStepSize;
}


/**
 * Mueve la cámara por el terreno a modo de visita vitual
 */
void viajeCamara(){
	bool mov = false;
	float acamX = (camX / ESCALA); // Normalizamos
	float acamZ = (camZ / ESCALA);
	
	float absterm;
	if (camYaw-aux_vuelta < 0)
		absterm = - (camYaw-aux_vuelta);
	else
		absterm = camYaw-aux_vuelta;
	
	float absacamX = acamX < 0 ? -acamX : acamX;
	float absacamZ = acamZ < 0 ? -acamZ : acamZ;
	
	if(animarCamara){
		//dollyCamera(DEF_dollyStepSize, 0);
		switch (estadoAnimCamara){
			case INICIO_SUAVE:
				// Ir despacio hacia la posición inicial
				// Miramos hacia el centro
				if(absacamX > 5){
					if(acamX > 0){
						travCamera(90);
					}else{
						travCamera(270);
					}
					mov = true;
				}
				
				if(absacamZ > 5){
					if(acamZ > 0){
						travCamera(0);
					}else{
						travCamera(180);
					}
					mov = true;
				}
				if(!mov){
					estadoAnimCamara = VUELTA;
					vueltas_ = N_VUELTAS;
					aux_vuelta = camYaw;
					//cout << "OK" << endl;
				}/*else{
				 cout << "X: " << acamX << " Z: " << acamZ << endl;
				 }*/
				break;
				case VUELTA:
				// Dar un par de vueltas al terreno
				if(vueltas_ >= 0){
					
					if(absterm < 1){ // Contamos 1 vuelta
						vueltas_--;
					}
					camYaw++;
					if(camYaw >= 360){ // De 0 a 359...
						camYaw -= 360;
					}
				}else{
					estadoAnimCamara = ZOOM_IN;
					aux_zoom = N_ZOOM;
				}
				break;
				case ZOOM_IN:
				// Hacer un Zoom
				if(aux_zoom >= 0){
					zoomFactor -= 0.001;
					aux_zoom--;
				}else{
					estadoAnimCamara = ZOOM_OUT;
				}
				break;
				case ZOOM_OUT:
				// Y deshacerlo
				if(aux_zoom < N_ZOOM){
					zoomFactor += 0.001;
					aux_zoom++;
				}else{
					estadoAnimCamara = TRAVELING;
					aux_vuelta = camYaw+1;
				}
				break;
				case TRAVELING:
				// Moverse un poco por el terreno mirando
				keyboard('0',0,0); // Eliminamos cualquier zoom que haya quedado
				if(absterm >= 1){ // Contamos 1 vuelta
					camYaw--;
					dollyCamera(DEF_dollyStepSize*5,90);
					if(camYaw < 0){ // De 0 a 359...
						camYaw += 360;
					}
				}else{
					animarCamara = false;
				}
				break;
		}
	}
}


/************************************************************************************************
 Animaciones
 ************************************************************************************************/
/**
 * Calcula los FPS actuales
 */
void calculaFPS(){
#if __APPLE__
	// Calculate frame rate.
    if (((clock() - t_fps) / (double)CLOCKS_PER_SEC) >= 1.0){
        t_fps = clock();
        fps = _fps;
        _fps = 0;
    }else{
		_fps++;
	}
#else
	clock_t act = clock()*CLK_TCK; 
	float diff = act - t_fps;
	_fps++;
	
	if(diff > 1000000){ // 1 segundo
		fps = _fps;
		_fps = 0;
		t_fps = act;
	}
#endif
}


/**
 * Cálculo de la luz solar y animaciones en general
 */
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
	
	//Cambiar la hora
	sprintf(wtitle, "%s - %d:%.2d  FPS: %d", myProgramName, horas%24, minutos, fps);
	glutSetWindowTitle(wtitle);
	
	//Animar las nubes 
	if (sentidonubes){
		framenubes++;
		if (framenubes == FRAMES_NUBES){
			framenubes = FRAMES_NUBES-1;
			sentidonubes = false;
		}
	}else{
		framenubes--;
		if (framenubes == -1){
			framenubes = 0;
			sentidonubes = true;
		}
	}	
	
	//Animar la lluvia
	if (llueve){
		nubosidadlluvia += 0.05;
		if (nubosidadlluvia > 1.0) nubosidadlluvia = 1.0;
		intensidadlluvia += 0.025;
		if (intensidadlluvia > 1.0) intensidadlluvia = 1.0;
	}else{
		nubosidadlluvia -= 0.025;
		if (nubosidadlluvia < nubosidad) nubosidadlluvia = nubosidad;
		
		intensidadlluvia -= 0.05;
		if (intensidadlluvia < 0.0) intensidadlluvia = 0.0;
		
		
	}
}



/************************************************************************************************
 Dibujado
 ************************************************************************************************/

/**
 * Genera una esfera a modo de sol en su posición actual
 */
void plantarSol(){
	glPushMatrix ();
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	
	
	glColor4f(luzdifusa[0], luzdifusa[1], luzdifusa[2], luzdifusa[3]*(1-nubosidadlluvia));
	glTranslatef (posicion_luz[0], posicion_luz[1], posicion_luz[2]); 
	float grande = 0;
	
	// Agrandarlo al acercarse a los límites
	if (horas == 17 && minutos > 30)
		grande = (minutos-30);
	if (horas == 6 && minutos < 30)
		grande = (30-minutos);
	
	glutSolidSphere(40.0+grande, 32, 32);
	glEnable(GL_FOG);
	glEnable(GL_LIGHTING);
    glPopMatrix();
}


/**
 * Dibujar el terreno
 */
void dibujarTerreno(){
	//Pintamos el terreno
	
	// Posición de la cámara
	int acamX = (camX / ESCALA)+M_ALTO;
	int acamZ = (camZ / ESCALA)+M_ANCHO;
	
	
	// Orden contrario para BACKFACECULLING
	setTypeMaterial(LAND);
	if (usartextura){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texNames[TERRENO]);
	}else{
		glDisable(GL_TEXTURE_2D);
	}
	
	
	float color[3];
	float agua = (NIVEL_LAGO+3)*RELIEVE;
	// Tiras de triángulos
	for(int i = M_ALTO-1; i > -M_ALTO; i--){
		glEnable(GL_COLOR_MATERIAL);
		glBegin(GL_TRIANGLE_STRIP);
		
		for(int j = M_ANCHO-1; j > -M_ANCHO; j--){
			int v2x = i+M_ALTO;
			int v2y = j+M_ANCHO;
			int v1x = i+M_ALTO-1;
			int v1y = j+M_ANCHO;
			// Área ocupada por el cursor
			int dist1x = v1x - acamX >= 0 ? v1x - acamX : acamX - v1x;
			int dist1y = v1y - acamZ >= 0 ? v1y - acamZ : acamZ - v1y;
			int dist2x = v2x - acamX >= 0 ? v2x - acamX : acamX - v2x;
			int dist2y = v2y - acamZ >= 0 ? v2y - acamZ : acamZ - v2y;
			
			glNormal3fv(normalesv[v1x][v1y]);
			glTexCoord2f((v1y)/(float)(ANCHO-1),(v1x)/(float)(ALTO-1));
			// Color
			if (!usartextura){
				color[0] = colores[v1x][v1y][0];
				color[1] = colores[v1x][v1y][1];
				color[2] = colores[v1x][v1y][2];
				glColor3fv(colores[v1x][v1y]);
			}
			if (usartextura){
				color[0] = 1;
				color[1] = 1;
				color[2] = 1;
			}
			if (dist1x <= influencia && dist1y <= influencia){
				color[0] += 0.3;
				if (color[0] > 1) color[0] = 1;
				if (usartextura){
					color[1] -= 0.3;
					color[2] -= 0.3;
				}			}
			if (arboles[v1x][v1y] && vertices[v1x][v1y][1]> agua){
				color[0] -= .2;
				color[1] -= .2;
				color[2] -= .2;			
			}
			glColor3fv(color);
			glVertex3fv(vertices[v1x][v1y]);
			
			glTexCoord2f( (v2y)/(float)(ANCHO-1), (v2x)/(float)(ALTO-1));
			glNormal3fv(normalesv[v2x][v2y]);
			
			// Color
			if (!usartextura){
				color[0] = colores[v2x][v2y][0];
				color[1] = colores[v2x][v2y][1];
				color[2] = colores[v2x][v2y][2];
				glColor3fv(colores[v2x][v2y]);
			}
			if (usartextura){
				color[0] = 1;
				color[1] = 1;
				color[2] = 1;
			}
			if (dist2x <= influencia && dist2y <= influencia){
				color[0] += 0.3;
				if (color[0] > 1) color[0] = 1;
				if (usartextura){
					color[1] -= 0.3;
					color[2] -= 0.3;
				}
			}
			if (arboles[v2x][v2y] && vertices[v2x][v2y][1]> agua){
				color[0] -= .2;
				color[1] -= .2;
				color[2] -= .2;			
			}
			glColor3fv(color);
			glVertex3fv(vertices[v2x][v2y]);
			
		}
		glEnd();
	}
	if (!usartextura){
		glEnable(GL_TEXTURE_2D);
	}else{
		glDisable(GL_TEXTURE_2D);
	}
}


/**
 * Dibujar un árbol orientado a la cámara
 */
void quadArbol(int i, int j, GLfloat cosang, GLfloat sinang){
	// Dibujar
	GLfloat alt = vertices[i][j][1];
	GLfloat x = (i+j)%2;	// Algunos están reflejados respecto de x
	GLfloat x2 = 1-x;
	GLfloat tam = 20 + ((i+j)%4-2)*2; // Distinto tamaño para cada uno
	
	bool altText = false;
	if (colores[i][j][0] == 1 && colores[i][j][1] == 1 && colores[i][j][2] == 1){ // nieve
		altText = true;
		glBindTexture(GL_TEXTURE_2D, texNames[ARBOLNIEVE]);
		alt -= 2;
		
	}
	
	if (alt < (NIVEL_LAGO+7)*RELIEVE){	// Plantar una palmera mejor
		altText = true;
		glBindTexture(GL_TEXTURE_2D, texNames[PALM]);	
	}
	
	glBegin(GL_QUADS);
	glTexCoord2f(x, 0);	glVertex3f(vertices[i][j][0]+cosang, alt+tam, vertices[i][j][2]-sinang);
	glTexCoord2f(x2, 0); glVertex3f(vertices[i][j][0]-cosang, alt+tam, vertices[i][j][2]+sinang);
	glTexCoord2f(x2, 1); glVertex3f(vertices[i][j][0]-cosang, alt, vertices[i][j][2]+sinang);
	glTexCoord2f(x, 1);	glVertex3f(vertices[i][j][0]+cosang, alt, vertices[i][j][2]-sinang);
	glEnd();
	
	if (altText) // Volver a la textura del árbol normal
		glBindTexture(GL_TEXTURE_2D, texNames[ARBOL]);
}


/**
 * Dibujar el cielo
 */
void dibujarCielo(){
	// Dibujar
	glNormal3f(0, +1, 0);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	bool noche = (horas<6 || horas>17) && nubosidadlluvia<0.8;
	if (noche){	// Noche con estrellas
		glEnable(GL_LIGHTING);
		glBindTexture(GL_TEXTURE_2D, texNames[ESTRELLAS]);
		setTypeMaterial(STARS);
	}else{						// Nubes moviéndose
		glBindTexture(GL_TEXTURE_2D, texNames[NUBES+framenubes]);
		glDisable(GL_LIGHTING);
		glDisable(GL_FOG);
	}
	
	glDepthMask(GL_FALSE); // Para evitar problemas con el z-buffer
	
	
	
	glPushMatrix();
	GLfloat off = minutos/2.0+(horas*30);
	float scf = max(max(ALTO*ESCALA, RELIEVE*ANCHO), ANCHO*ESCALA);
	
	glScalef(2*scf, scf, 2*scf);
	glTranslatef(0,-0.5,0);
	
	
	if (!noche) glRotatef(off, 0, 1, 0); // pasar el tiempo
	
	if (horas >17 || horas < 6) // Noche de color negro
		glColor4f(0,0,0,1);
	else if (horas == 17){ // Oscurecer las nubes en las horas límite
		GLfloat c = (60-minutos)/60.0;
		glColor4f(c,c,c,1);
	}else if (horas == 6){
		GLfloat c = (minutos)/60.0;
		glColor4f(c,c,c,1);
	}else{
		glColor4f(1,1,1,1);
	}
	
	GLUquadric* qd = gluNewQuadric();
	gluQuadricTexture(qd, true);
    
	gluSphere(qd,1.0,20,20);
	gluDeleteQuadric(qd);
	
	glPopMatrix();
	
	
	glDepthMask(GL_TRUE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	if (!noche) glEnable(GL_FOG);
}


/**
 * Dibujar los árboles
 */
void dibujarArboles(){
	rgbint c;
	c.r = 255;
	c.g = 0;
	c.b = 0;
	
	// Orientar a la cámara
	float angulo = camYaw/180.0*M_PI;
	GLfloat cosang = 10*cos(angulo);
	GLfloat sinang = 10*sin(angulo);
	GLfloat abscosang = cosang > 0 ? cosang : -cosang;
	GLfloat abssinang = sinang > 0 ? sinang : -sinang;
	GLfloat normal[3] = {-cosang, posicion_luz[1]/50, -sinang}; //Normal a la pantalla
	glDisable(GL_CULL_FACE);
	glEnable(GL_COLOR_MATERIAL);
	setTypeMaterial(TREE);
	glBindTexture(GL_TEXTURE_2D, texNames[ARBOL]);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(GL_FALSE);	// Se evita que a veces se vea el QUAD
	glNormal3fv(normal);
	
	// Pintar los arboles desde el fondo hacia la camara
	if (sinang < 0 && abssinang >= abscosang){
		for (int i=ALTO-1; i>=0; i--)
			for (int j=ANCHO-1; j>=0; j--)
				if (arboles[i][j] && vertices[i][j][1]>(NIVEL_LAGO+3)*RELIEVE)
					quadArbol(i,j,cosang,sinang);
		
	}else if (cosang < 0 && abscosang >= abssinang){
		for (int j=ANCHO-1; j>=0; j--)
			for (int i=0; i<ALTO; i++)
				if (arboles[i][j] && vertices[i][j][1]>(NIVEL_LAGO+3)*RELIEVE)
					quadArbol(i,j,cosang,sinang);
		
	}else if (cosang >= 0 && abscosang >= abssinang){
		for (int j=0; j<ANCHO; j++)
			for (int i=ALTO-1; i>=0; i--)
				if (arboles[i][j] && vertices[i][j][1]>(NIVEL_LAGO+3)*RELIEVE)
					quadArbol(i,j,cosang,sinang);
		
	}else if (sinang >= 0 && abscosang <= abssinang){
		for (int i=0; i<ALTO; i++)
			for (int j=ANCHO-1; j>=0; j--)
				if (arboles[i][j] && vertices[i][j][1]>(NIVEL_LAGO+3)*RELIEVE)
					quadArbol(i,j,cosang,sinang);
	}
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);	
	glDisable(GL_TEXTURE_2D);
}


/**
 * Calcula el ángulo entre el punto de vista y cada vértice del lago 
 */
int alphaLago ( float vx, float vy, float vz ){
	float ang;
	
	GLfloat v0[3] = {camX-vx, camY-vy, camZ-vz };
	float mv0;
	mv0 = sqrt(v0[0]*v0[0] + v0[1]*v0[1] + v0[2]*v0[2]);
	
	if (v0[1] >= 0)
		ang = 1.0 - v0[1]/mv0;
	else
		ang = 1.0 + v0[1]/mv0;
	int alpha = int(ang * ang * 200 + 30);
	return alpha;
}


/**
 * Dibujar el agua
 */
void dibujarLago(){
	
	// Se vé la superficie tanto por encima como por debajo
	glDisable(GL_CULL_FACE);
	
	// Orden contrario para BACKFACECULLING
	setTypeMaterial(WATER);
	
	GLfloat* v;
	
	int alpha; 
	
	// Genera la malla
	for(int i = M_ALTO-1; i > -M_ALTO; i--){
		glEnable(GL_COLOR_MATERIAL);
		glBegin(GL_TRIANGLE_STRIP);
		
		for(int j = M_ANCHO-1; j > -M_ANCHO; j--){
			// Condición para generar superficie del lago
			int nivel = (NIVEL_LAGO+2)*RELIEVE;
			if (vertices[i+M_ALTO-1][j+M_ANCHO][1] < nivel || vertices[i+M_ALTO][j+M_ANCHO][1] < nivel){
				
				glNormal3fv(normaleslago[i+M_ALTO-1][j+M_ANCHO]);
				v = lago[i+M_ALTO-1][j+M_ANCHO];
				alpha = alphaLago(v[0], v[1], v[2]);
				glColor4f(colorcielo[0]+.5,colorcielo[1]+.3,colorcielo[2],alpha/255.0);
				glVertex3fv(lago[i+M_ALTO-1][j+M_ANCHO]);
				
				glNormal3fv(normaleslago[i+M_ALTO][j+M_ANCHO]);
				v = lago[i+M_ALTO][j+M_ANCHO];
				alpha = alphaLago(v[0], v[1], v[2]);
				glColor4f(colorcielo[0]+.5,colorcielo[1]+.3,colorcielo[2],alpha/255.0);
				glVertex3fv(lago[i+M_ALTO][j+M_ANCHO]);
			}
		}
		glEnd();
	}
	glEnable(GL_CULL_FACE);
}


/**
 * Dibuja la lluvia
 */
void dibujarLluvia(){
	if (intensidadlluvia == 0.0)
		return;
	
	if (llueve && rand()%100 == 1){
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}
	
	// Pintamos la lluvia
	glPushMatrix();
	
	//	Textura del fondo de la brújula
	glBindTexture(GL_TEXTURE_2D, texNames[LLUVIA]);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	setTypeMaterial(SNOW);
	rgbint c;
	GLfloat altura0 = ANCHO*RELIEVE*2;
	GLfloat altura1 = ANCHO*RELIEVE;
	GLfloat ancho0 = -ALTO*ESCALA;
	GLfloat paso = 1;//*ESCALA;
	GLfloat offset = (minutos/100.0+camYaw*0.1);
	
	
	glTranslatef(camX, 1, camZ);
	
	
	glRotatef(camYaw, 0, 1, 0);
	
	// Dibuja las capas de lluvia
	glBegin(GL_QUADS);
	for (int i=int(intensidadlluvia*5); i>=0; i--){
		GLfloat off = offset+0.1*i;
		c.r = 160-16*i;
		c.g = c.r;
		c.b = c.r;
		setIntColorMaterial(c, 0);
		glTexCoord2f(0, (4+off)*i); 	glVertex3f(ancho0, altura0, -i*paso);
		glTexCoord2f(4+i*20, (4+off)*i); 	glVertex3f(-ancho0, altura0, -i*paso);
		setIntColorMaterial(c, (100-i*10)*intensidadlluvia);
		glTexCoord2f(4+i*20, (2+off)*i); 	glVertex3f(-ancho0, altura1, -i*paso);
		glTexCoord2f(0, (2+off)*i); 	glVertex3f(ancho0, altura1, -i*paso);
		
		glTexCoord2f(0, (2+off)*i); 	glVertex3f(ancho0, altura1, -i*paso);
		glTexCoord2f(4+i*20, (2+off)*i);	glVertex3f(-ancho0, altura1, -i*paso);
		setIntColorMaterial(c, 0);
		glTexCoord2f(4+i*20, (+off)*i); 	glVertex3f(-ancho0, camY-100*RELIEVE, -near*2-i*paso);
		glTexCoord2f(0, (+off)*i); 	glVertex3f(ancho0, camY-100*RELIEVE, -near*2-i*paso);
	} // Por detrás de la cámara también
	for (int i=int(intensidadlluvia*5); i>=0; i--){
		GLfloat off = offset+0.1*i;
		c.r = 160-16*i;
		c.g = c.r;
		c.b = c.r;
		setIntColorMaterial(c, 0);
		glTexCoord2f(0, (.4+off)*i); 	glVertex3f(ancho0, altura0, near*2+i*paso);
		glTexCoord2f(4+i, (.4+off)*i); 	glVertex3f(-ancho0, altura0, near*2+i*paso);
		setIntColorMaterial(c, (100-i*10)*intensidadlluvia);
		glTexCoord2f(4+i, (.2+off)*i); 	glVertex3f(-ancho0, altura1, near*2+i*paso);
		glTexCoord2f(0, (.2+off)*i); 	glVertex3f(ancho0, altura1, near*2+i*paso);
		
		glTexCoord2f(0, (.2+off)*i); 	glVertex3f(ancho0, altura1, near*2+i*paso);
		glTexCoord2f(4+i, (.2+off)*i);	glVertex3f(-ancho0, altura1, near*2+i*paso);
		setIntColorMaterial(c, 0);
		glTexCoord2f(4+i, (+off)*i); 	glVertex3f(-ancho0, camY-100*RELIEVE, near*2+i*paso);
		glTexCoord2f(0, (+off)*i); 	glVertex3f(ancho0, camY-100*RELIEVE, near*2+i*paso);
	}
	glEnd();
	
	
	
	
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	
	glPopMatrix();
} 


/**
 * Dibuja la brújula en la esquina
 */
void dibujarBrujula(){
	// Pintamos la brújula
	glPushMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width , 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,1);	
	
	//	Textura del fondo de la brújula
	glBindTexture(GL_TEXTURE_2D, texNames[BRUJULA]);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	rgbint c;
	c.r = 255;
	c.g = 255;
	c.b = 255;
	setTypeMaterial(SNOW);
	setIntColorMaterial(c, 255);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); 	glVertex3f(0, 128, 0);
	glTexCoord2f(1, 0); 	glVertex3f(128,128, 0);
	glTexCoord2f(1, 1); 	glVertex3f(128, 0, 0);
	glTexCoord2f(0, 1); 	glVertex3f(0, 0, 0);
	glEnd();
	
	//	Textura de la aguja de la brújula
	glBindTexture(GL_TEXTURE_2D, texNames[AGUJA]);
	setTypeMaterial(SNOW);
	setIntColorMaterial(c, 255);
	
	glTranslatef(64,64,-.01);
	glRotatef(camYaw, 0,0,1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); 	glVertex3f(-64, 64, 0);
	glTexCoord2f(1, 0); 	glVertex3f(64, 64, 0);
	glTexCoord2f(1, 1); 	glVertex3f(64, -64, 0);
	glTexCoord2f(0, 1); 	glVertex3f(-64, -64, 0);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	
	glPopMatrix();
} 




/**
 * Dibuja la escena completa
 */
void draw(){
	
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	
	// Posiciona la cámara
	glRotatef( -camPitch, 1.0, 0.0, 0.0 );
	glRotatef( -camYaw, 0.0, 1.0, 0.0 );
	glTranslatef( -camX, -camY, -camZ );
	glCullFace(GL_BACK);
	glDisable(GL_CLIP_PLANE0);
	
	
	// Niebla
	fog_density  = nubosidadlluvia/1000;
	glFogf (GL_FOG_START,  near);
	glFogf (GL_FOG_END,    far);
	glFogfv(GL_FOG_COLOR,  colorcielo);      
	glFogi (GL_FOG_MODE,   fog_mode);
	glFogf (GL_FOG_DENSITY,fog_density);
	glDisable(GL_FOG);
	
	
	// Modo de render de la escena
	glPolygonMode(GL_FRONT_AND_BACK, real_renderMode);
	
	
	// Pintamos el cielo
	dibujarCielo();
	
	// Iluminamos de forma 'realista'
	setSunLight(horas, minutos, nubosidadlluvia, posicion_luz, luzdifusa, colorcielo, ESCALA);
	plantarSol();
	
	// Pintamos el terreno
	dibujarTerreno();	
	
	// Pintamos el lago detallado
	dibujarLago();
	
	// Pintamos los árboles
	dibujarArboles();	
	
	// Pintamos la lluvia
	dibujarLluvia();
	
	
	glPopMatrix();		
	
	// Pintamos la brújula
	dibujarBrujula();
	
	
	// Hacemos la visita virtual
	viajeCamara();
	
	glFlush();
	
}


/************************************************************************************************
 Callbacks propios de OpenGL
 ************************************************************************************************/
/**
 * Función de redimensión de ventana
 */
void reshape(int w, int h){
	if (!h)
		return;
	width = w;
	height = h;
	glViewport(0, 0,  (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	aspect = float(w)/float(h);
	gluPerspective(60.0*zoomFactor, aspect, near, far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	viewPortCenterX = w / 2;
	viewPortCenterY = h / 2;
}


/**
 * Secuencia de renderización
 */
static void display(void){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0*zoomFactor, aspect, near, far);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	draw();
	glutSwapBuffers();
	calculaFPS();
}


/**
 * Redibujar cuando no se está haciendo nada
 */
static void idle(void){
	glutPostRedisplay();
}


/**
 * Acción a realizar cada cierto tiempo determinado por la velocidad
 */
static void timer(int value){
	if (animar)
		actualizaSol(false);
	glutPostRedisplay();
	glutTimerFunc(1000/velocidad, timer, 1);
}


/**
 * Acciones de las teclas
 */
static void keyboard(unsigned char key, int x, int y) {
	
	switch (key) {
			
			case 'w':	// Movimiento hacia adelante
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 0.0 );
			break;
			case 'W':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize*2.0, 0.0 );
			break;
			
			case 's':	// Movimiento hacia atras
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 180.0 );
			break;
			case 'S':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize*2.0, 180.0 );
			break;
			
			case 'a':	// Strafe hacia la izquierda
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 90.0 );
			break;
			case 'A':	// Strafe hacia la izquierda (correr)
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize*2.0, 90.0 );
			break;
			
			case 'd':	// Strafe derecha
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 270.0 );
			break;
			case 'D':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize*2.0, 270.0 );
			break;
			
			case 'f':	// Fullscreen
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
			
			case 'z':	//Mirar abajo
			camPitch -= 1.0;
			clampCamera();
			break;
			
			case 'Z':	// Mirar arriba
			camPitch += 1.0;
			clampCamera();
			break;
			
			case ' ': // Cambia modos de visualizado
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
			
			case 27:	//Esc salir
			glutIdleFunc(NULL);
			
			
			freeTablaRGBInt(estrellas, 256);
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
			if (horas == -1)
				horas = 23;
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
			break;
			
			case 'T':// Disminuir la velocidad del tiempo
			velocidad /= 2;
			if (velocidad < .125 ) velocidad = .125;
			break;			
			
			case 'M': // Deformación positiva
			realizarDeformacion(1);
			break;	
			
			case 'm': // Deformación negativa
			realizarDeformacion(-1);
			break;
			
			case 'N': // Aumentar tamaño del cursor de deformación
			influencia++;
			break;	
			
			case 'n': // Disminuir tamaño del cursor de deformación
			influencia--;
			if (influencia < 1) influencia = 1;
			break;
			
			case 'r':
			case 'R': // Llover
			llueve = !llueve;
			if (llueve)
				nubosidadlluvia = nubosidad;
			break;
			
			case 'x': case 'X': // Animación cámara
			animarCamara = !animarCamara;
			estadoAnimCamara = INICIO_SUAVE;
			break;	
			
			case 'q': // Disminuye el nivel del agua
			cambiarCotaLago(NIVEL_LAGO-1);
			break;
			
			case 'Q': // Aumenta el nivel del agua
			cambiarCotaLago(NIVEL_LAGO+1);
			break;
			
			case 'e': // Disminuye la cota de nieve
			cambiarCotaNieve(NIVEL_NIEVE-1);
			break;
			
			case 'E': // Aumenta la cota de nieve
			cambiarCotaNieve(NIVEL_NIEVE+1);
			break;
			
			case 'g': // Disminuye el factor de escala
			cambiarEscala(ESCALA/2);
			break;
			
			case 'G': // Aumenta el factor de escala
			cambiarEscala(ESCALA*2);
			break;
			
			case 'h': // Disminuye el factor de escala
			cambiarRelieve(RELIEVE/2);
			break;
			
			case 'H': // Aumenta el factor de escala
			cambiarRelieve(RELIEVE*2);
			break;
			
			case 'c': // Elimina un árbol
			modificarArbol(false);
			break;
			
			case 'C': // Planta un árbol
			modificarArbol(true);
			break;
			
			default: break;
	}
	glutPostRedisplay();
}


/**
 * Más acciones de teclas
 */
void specialFunc(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_LEFT:	//giro  a la izquierda
			camYaw += 1.0;
			clampCamera();
			//cout << camYaw << " " << cos(camYaw/180.0*M_PI) << " " << sin(camYaw/180.0*M_PI) << endl;
			break;
			
		case GLUT_KEY_RIGHT: // giro a la derecha.
			camYaw -= 1.0;
			clampCamera();
			//cout << camYaw << " " << cos(camYaw/180.0*M_PI) << " " << sin(camYaw/180.0*M_PI) << endl;
			break;
			
		case GLUT_KEY_UP:	// mover hacia arriba.
			if(!andando)
				camY += 2.0;
			break;
			
			case GLUT_KEY_DOWN:	// mover hacia abajo
			if(!andando)
				camY -= 2.0;
			break;
	}
    glutPostRedisplay();
}


/**
 * Acción de pulsar un botón del ratón
 */
void enterMouseDrag( int x, int y ) {
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


/**
 * Acción de soltar el botón
 */
void exitMouseDrag( int x, int y ) {
	if( !isInMouseDrag )
		return;
	isInMouseDrag = 0;
	glutSetCursor( oldCursorID );
	glutWarpPointer( oldCursorX, oldCursorY );
}


/**
 * Acción del botón
 */
void mouseFunc( int button, int state, int x, int y ) {
	if( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
	{
		if( !isInMouseDrag )
			enterMouseDrag( x, y );
		else
			exitMouseDrag( x, y );
	}
}


/**
 * Más movimiento del ratón
 */
void allMotionFunc( int x, int y ) {
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


/**
 * Menú contextual
 */
static void menu(int item){
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
		case ANIMAR_CAMARA:
			animarCamara = !animarCamara;
			break;
		case TEXT_TERR:
			usartextura = !usartextura;
			break;
		case LLOVER:
			keyboard('r',0,0);
			break;					
		default: break; 
	}
}


/************************************************************************************************
 Programa principal
 ************************************************************************************************/
/**
 * Programa principal
 */
int main(int argc, char **argv){
	
	// Crea la ventana
	glutInitWindowSize(width, height);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInit(&argc, argv);
	glutCreateWindow(myProgramName);
	
	
	cout << "Init" << endl;
	// Inicializa los parámetros
	if (argc>1)
		init(true,argv[1]);
	else
		init(false,NULL);
	
	cout << "GLUT init" << endl;
	// Establece las funciones por defecto
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc( specialFunc );
	glutMouseFunc( mouseFunc );
	glutMotionFunc( allMotionFunc );
	glutPassiveMotionFunc( allMotionFunc );
	glutIdleFunc(idle);
	
	cout << "GL init" << endl;
	// Vista inicial
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0*zoomFactor, aspect, near, far);
	
	
	// Z-Buffer
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	
	// Sombreado Gouraud
	glShadeModel(GL_SMOOTH);
	
	// Back face culling
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	
	// Texturas
	glEnable(GL_TEXTURE_2D);
	
	
	// Menú contextual
	cout << "GLUT setup" << endl;
	glutCreateMenu(menu);
	glutAddMenuEntry("WIREFRAME", glLINE);
	glutAddMenuEntry("POINTS", glPOINT);
	glutAddMenuEntry("FLAT", glFLAT);
	glutAddMenuEntry("SMOOTH", glSMOOTH);
	glutAddMenuEntry("Textura del terreno", TEXT_TERR);
	glutAddMenuEntry("[r] Cambiar tiempo", LLOVER);
	glutAddMenuEntry("[p] Andar/Flotar", ANDAR_FLOTAR);
	glutAddMenuEntry("[x] Animacion", ANIMAR_CAMARA);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutTimerFunc(100,timer,1);
	
	cout << "START" << endl;
	glutMainLoop();
	return 0;
}


