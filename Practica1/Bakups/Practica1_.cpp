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
	}else if (aux[1]*2 > aux[2] && aux[1]*2 > aux[0]){ // Algo inclinado
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
	aux[2] -= ind*(2/aux[2]);  // La orientación pierde importancia con la altura
	
	if (aux[2] < 0 && aux[1] > aux[2] && aux[1] > aux[0]){
		colores[i][j][0] = 1;
		colores[i][j][1] = 1;
		colores[i][j][2] = 1;
	}
}


/**
 * Asigna los colores a los vértices en función de la paleta de terreno y del agua
 * El margen indica la aleatoriedad con la que se toman las muestras de color
 */
void genColores(int margen){
	bool underwater = false;
	int ind;
	rgbfloat* pal;
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
			underwater = false;
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
	}
}


/**
 * Lee y genera las texturas utilizadas
 */
void genTexturas() {
	paleta = loadPalInt("paletas/landcolor3.ppm", 256);
	paletaf = intPal2Float(paleta, 256);
	paleta_agua = loadPalInt("paletas/watercolor7.ppm", 256);
	paleta_aguaf = intPal2Float(paleta_agua, 256);
	paleta_grass = loadPalInt("paletas/grasscolor2.ppm", 256);
	paleta_grassf = intPal2Float(paleta_grass, 256);
	tree = readPPM("paletas/tree.ppm");
	snowtree = readPPM("paletas/treesnow.ppm");
	treealpha = readPGM("paletas/treealpha.pgm");
	datatree = genTextIntAlpha(tree, treealpha, 256);
	datasnowtree = genTextIntAlpha(snowtree, treealpha, 256);
	paletanubes = loadPalInt("paletas/skyalpha.ppm", 256);
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
	datanubes = new int*[FRAMES_NUBES];
	for (int i=0; i<FRAMES_NUBES; i++){
		datanubes[i] = genTextIntAlpha(nubes,nubesalpha[i],256);
	}
	//writePGMint("../../nubes3D.pgm", nubesalpha[0], 256);
	
	// Genera los nombres de textura 
	glGenTextures(15, texNames);
	glBindTexture(GL_TEXTURE_2D, texNames[ARBOL]);
	genText(datatree, false, 256);
	
	glBindTexture(GL_TEXTURE_2D, texNames[ARBOLNIEVE]);
	genText(datasnowtree,false, 256);	
	
	for (int i=0; i<FRAMES_NUBES; i++){
		glBindTexture(GL_TEXTURE_2D, texNames[NUBES+i]);
		genText(datanubes[i], true, 256);		
	}
	
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
	databrujula = genTextIntAlpha(brujula, brujulaalpha, 128);
	aguja = readPPM("paletas/aguja.ppm");
	agujaalpha = readPGM("paletas/agujaalpha.pgm");
	dataaguja = genTextIntAlpha(aguja, agujaalpha, 128);
	
	glBindTexture(GL_TEXTURE_2D, texNames[BRUJULA]);
	genText(databrujula,false, 128);	
	
	glBindTexture(GL_TEXTURE_2D, texNames[AGUJA]);
	genText(dataaguja,false, 128);	
	
}

/**
 * Genera las posiciones de los árboles. 
 * La cercania indica cómo de lejos deben estar entre sí
 */
void genArboles(int cercania){
	for (int i=0; i<ALTO; i++){
		for (int j=0; j<ANCHO; j++){
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
				arboles[i][j] = ((rand()%(alturas[i][j]*2)) == (NIVEL_LAGO+7)*2); 
				if (arboles[i][j]){
					// Mira si el árbol está en una zona de nieve
					if (colores[i][j][0] == 1 && colores[i][j][1] == 1 && colores[i][j][2] == 1){
						//	cout << "arbol NEVADO en " << i << ","<<j<< endl;
					}else{ // Pinta la sombra del árbol
						colores[i][j][0] -= 0.2;
						colores[i][j][1] -= 0.2;
						colores[i][j][2] -= 0.2;
					}
				}
			}
		}
	}
}


/**
 * Genera posiciones para las estrellas de forma aleatoria
 */
void genEstrellas(){
	int cercania = 3;
	estrellas = new rgbint*[ALTO];
	for (int i=0; i<ALTO; i++){
		estrellas[i] = new rgbint[ANCHO];
		for (int j=0; j<ANCHO; j++){
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
	writePPMint("../../estrellas.ppm", estrellas, ALTO);
	dataestrellas = genTextInt(estrellas, 255, ALTO);
	glBindTexture(GL_TEXTURE_2D, texNames[ESTRELLAS]);
	genText(dataestrellas, true, ALTO);
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

	deformarTerreno(n_alt, acamX, acamZ, 5, true);
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
GLfloat* ant_alturas = NULL;
float* pesos = NULL;

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
				//cout << "Peso[" << pos << "]: " << pesos[pos] << endl;
			}
		}
	}

	//cout << "Altura" << endl;
	// Modificamos la altura del terreno segun los pesos y el area de influencia
	for(int i = x-influencia; i < x+influencia; i++){
		for(int j = y-influencia; j < y+influencia; j++){
			if(i >= 0 && j >= 0 && i < ALTO && j < ANCHO){
				//cout << "I: " << i << "J: " << j << endl;
				pos = auxIndiceInfluencia(i,j,x,y,influencia);
				//cout << "Antigua: ";
				ant_alturas[pos] = vertices[i][j][1];
				//cout << ant_alturas[pos] << endl;
				/*if(suave)
					vertices[i][j][1] = suavizar5(i,j,ALTO,ANCHO,ant_alturas[pos] + (n_altura * pesos[pos])) * RELIEVE;
				else*/
				//cout << "Nueva: ";
				vertices[i][j][1] = ant_alturas[pos] + ((n_altura * pesos[pos]) * RELIEVE);
				//cout << vertices[i][j][1] << endl;
				//cout << "Hecho" << endl;
			}
		}
	}

	//delete[] ant_alturas;
	//delete[] pesos;
	if(andando){
		setAlturaPj();
	}

	delete[] ant_alturas;
	delete[] pesos;
	ant_alturas = NULL;
	pesos = NULL;
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
				ind = int(vertices[i][j][1]-(minutos%10))%10; 
				if (ind < 0) ind = -ind;
				normaleslago[i][j][0] = olas[ind];
				normaleslago[i][j][1] = 1;
				normaleslago[i][j][2] = olas[ind];
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
			// No se normaliza porque ya está activado GL_NORMALIZE
			normalesv[i][j][0] = media[0]; 
			normalesv[i][j][1] = media[1];
			normalesv[i][j][2] = media[2];
		}
	}
}


/************************************************************************************************
 Inicializaciones
 ************************************************************************************************/
/**
 * Genera los parámetros iniciales del terreno
 */
void genTerreno(){
	int i, j;
	int maximo = -(((unsigned int)-1)/2);
	int minimo = 2147483647;
	
	
	// Alturas generadas con ruido Perlin
	//alturas = genPerlinNoise(256, 12, 30, 1);	suave = false;	
	alturas = genPerlinNoise(ANCHO, 38, 400);	suave = true;	
	//writePGMint("../../alturas.pgm", alturas, ANCHO);
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
	
	// Pone la cámara en el punto más alto
	camY = maximo*RELIEVE;
	
	// Genera todos los parámetros en el orden necesario
	genVertices(alturas, suave);
	genNormalesLago();
	genLago();
	genNormalesT();
	genNormalesV();
	genColores(10);
	genArboles(1);
	genEstrellas();
}


/**
 * Inicializa todos los parámetros
 */
void init(){
	
	srand(time(NULL));
	
	// luces
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
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 	
	
	// Texturas
	genTexturas();
	
	// Genera el terreno 
	genTerreno();
	
	// Dejamos a OpenGL normalizar las normales
	glEnable(GL_NORMALIZE); 
	
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
}

/**
* Mueve la cámara por el terreno a modo de visita vitual
*/
void viajeCamara(){
	if(animarCamara){
		dollyCamera(DEF_dollyStepSize, 0);
		switch (estadoAnimCamara){
			case INICIO_SUAVE:
				// Ir despacio hacia la posición inicial
				break;
			case VUELTA:
				// Dar un par de vueltas al terreno
				break;
			case ZOOM_IN:
				// Hacer un Zoom
				break;
			case ZOOM_OUT:
				// Y deshacerlo
				break;
			case TRAVELING:
				// Moverse un poco por el terreno mirando
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
	clock_t act = clock()*CLK_TCK;
	float diff = act - t_fps;
	_fps++;
	
	if(diff > 1000000){ // 1 segundo
		fps = _fps;
		_fps = 0;
		t_fps = act;
	}
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
		cout << framenubes << endl;
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
	
	glColor4f(luzdifusa[0], luzdifusa[1], luzdifusa[2], luzdifusa[3]);
	glTranslatef (posicion_luz[0], posicion_luz[1], posicion_luz[2]); 
	float grande = 0;
	
	// Agrandarlo al acercarse a los límites
	if (horas == 17 && minutos > 30)
		grande = (minutos-30);
	if (horas == 6 && minutos < 30)
		grande = (30-minutos);
	
	glutSolidSphere(40.0+grande, 32, 32);
	
	glEnable(GL_LIGHTING);
    glPopMatrix ();
}


/**
 * Dibujar el terreno
 */
void dibujarTerreno(){
	//Pintamos el terreno
	
	// Orden contrario para BACKFACECULLING
	setTypeMaterial(LAND);
	GLfloat color[3];
	
	for(int i = M_ALTO-1; i > -M_ALTO; i--){
		glEnable(GL_COLOR_MATERIAL);
		glBegin(GL_TRIANGLE_STRIP);
		
		for(int j = M_ANCHO-1; j > -M_ANCHO; j--){
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
	
	bool nieve = false;
	if (colores[i][j][0] == 1 && colores[i][j][1] == 1 && colores[i][j][2] == 1){ // nieve
		nieve = true;
		glBindTexture(GL_TEXTURE_2D, texNames[ARBOLNIEVE]);
		alt -= 2;
		
	}
	
	glBegin(GL_QUADS);
	glTexCoord2f(x, 0);	glVertex3f(vertices[i][j][0]+cosang, alt+tam, vertices[i][j][2]-sinang);
	glTexCoord2f(x2, 0); glVertex3f(vertices[i][j][0]-cosang, alt+tam, vertices[i][j][2]+sinang);
	glTexCoord2f(x2, 1); glVertex3f(vertices[i][j][0]-cosang, alt, vertices[i][j][2]+sinang);
	glTexCoord2f(x, 1);	glVertex3f(vertices[i][j][0]+cosang, alt, vertices[i][j][2]-sinang);
	glEnd();
	
	if (nieve) // Volver a la textura del árbol normal
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
	bool noche = horas<6 || horas>17;
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
	GLfloat rot = -2*camYaw/360.0;
	//rot = 0;
	GLfloat off = minutos/120.0+(horas%2)*.5;
	off = 0;
	glScalef(ALTO*ESCALA, ANCHO*RELIEVE, ANCHO*ESCALA);
	glRotatef(camYaw, 0, 1, 0); // orientar a la cámara
	
	
	glBegin(GL_QUADS);		
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
    glTexCoord2f(1+rot+off, 1); glVertex3f( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Top)
    glTexCoord2f(rot+off, 1); glVertex3f(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Top)
    glTexCoord2f(rot+off, 0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Bottom Left Of The Quad (Top)
    glTexCoord2f(1+rot+off, 0); glVertex3f( 1.0f, 1.0f, 1.0f);	// Bottom Right Of The Quad (Top)
	
    glTexCoord2f(1+rot+off, 1); glVertex3f( 1.0f,-1.0f,-1.0f);	// Top Right Of The Quad (Back)
    glTexCoord2f(rot+off, 1); glVertex3f(-1.0f,-1.0f,-1.0f);	// Top Left Of The Quad (Back)
    glTexCoord2f(rot+off, 0); glVertex3f(-1.0f, 1.0f,-1.0f);	// Bottom Left Of The Quad (Back)
    glTexCoord2f(1+rot+off, 0); glVertex3f( 1.0f, 1.0f,-1.0f);	// Bottom Right Of The Quad (Back)
	
    glTexCoord2f(1+rot+off, 1); glVertex3f(-1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Left)
    glTexCoord2f(0+rot+off, 1); glVertex3f(-1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Left)
    glTexCoord2f(0+rot+off, 0); glVertex3f(-1.0f, 1.0f, 1.0f);	// Top Right Of The Quad (Left)
    glTexCoord2f(1+rot+off, 0); glVertex3f(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Left)
	
    glTexCoord2f(1+rot+off, 1);    glVertex3f( 1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Right)
    glTexCoord2f(rot+off, 1);    glVertex3f( 1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Right)
    glTexCoord2f(rot+off, 0);    glVertex3f( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Right)
    glTexCoord2f(1+rot+off, 1);    glVertex3f( 1.0f, 1.0f, 1.0f);	// Top Left Of The Quad (Right)
    glEnd();			// End Drawing The Cube
	
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
				if (arboles[i][j])	quadArbol(i,j,cosang,sinang);
		
	}else if (cosang < 0 && abscosang >= abssinang){
		for (int j=ANCHO-1; j>=0; j--)
			for (int i=0; i<ALTO; i++)
				if (arboles[i][j])	quadArbol(i,j,cosang,sinang);
		
	}else if (cosang >= 0 && abscosang >= abssinang){
		for (int j=0; j<ANCHO; j++)
			for (int i=ALTO-1; i>=0; i--)
				if (arboles[i][j])	quadArbol(i,j,cosang,sinang);
		
	}else if (sinang >= 0 && abscosang <= abssinang){
		for (int i=0; i<ALTO; i++)
			for (int j=ANCHO-1; j>=0; j--)
				if (arboles[i][j])	quadArbol(i,j,cosang,sinang);
	}
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
 * Dibuja la escena completa
 */
void draw(){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	
	// Posiciona la cámara
	glRotatef( -camPitch, 1.0, 0.0, 0.0 );
	glRotatef( -camYaw, 0.0, 1.0, 0.0 );
	glTranslatef( -camX, -camY, -camZ );
	
	// Jugando con la niebla aún
	glFogf (GL_FOG_START,  near);
	glFogf (GL_FOG_END,    far);
	glFogfv(GL_FOG_COLOR,  colorcielo);      
	glFogi (GL_FOG_MODE,   fog_mode);
	glFogf (GL_FOG_DENSITY,fog_density);
	glEnable(GL_FOG);
	
	
	// Modo de render de la escena
	glPolygonMode(GL_FRONT_AND_BACK, real_renderMode);
	
	
	// Pintamos el cielo
	dibujarCielo();
	
	// Iluminamos de forma 'realista'
	setSunLight(horas, minutos, nubosidad, posicion_luz, luzdifusa, colorcielo, ESCALA);
	plantarSol();
	
	// Pintamos el terreno
	dibujarTerreno();	
	
	// Pintamos el lago detallado
	dibujarLago();
	
	// Pintamos los arboles
	dibujarArboles();	
	
	
	glPopMatrix();		
	
	// Pintamos la brujula
	glPushMatrix();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width , 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	
	//	TEXTURA DEL FONDO DE LA BRUJULA
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
	
	//	TEXTURA DE LA AGUJA DE LA BRUJULA
	glBindTexture(GL_TEXTURE_2D, texNames[AGUJA]);
	setTypeMaterial(SNOW);
	setIntColorMaterial(c, 255);
	
	glTranslatef(64,64,0);
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
	////////////////////////////////////////////////////////////////////////////////////////////
	
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
	calculaFPS();
	glutSwapBuffers();
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
		case 'W':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 0.0 );
			break;
			
			case 's':	// Movimiento hacia atras
			case 'S':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 180.0 );
			break;
			
			case 'a':	// Strafe hacia la izquierda
			case 'A':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 90.0 );
			break;
			
			case 'd':	// Strafe derecha
			case 'D':
			if(andando){
				setAlturaPj();
			}
			dollyCamera( DEF_dollyStepSize, 270.0 );
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
			cout << "v: " << velocidad << endl;
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

			case 'x': case 'X': // Animación cámara
				animarCamara = !animarCamara;
				estadoAnimCamara = INICIO_SUAVE;
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
	
	
	// Inicializa los parámetros
	init();
	
	// Establece las funciones por defecto
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc( specialFunc );
	glutMouseFunc( mouseFunc );
	glutMotionFunc( allMotionFunc );
	glutPassiveMotionFunc( allMotionFunc );
	glutIdleFunc(idle);
	
	
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
	glutCreateMenu(menu);
	glutAddMenuEntry("WIREFRAME", glLINE);
	glutAddMenuEntry("POINTS", glPOINT);
	glutAddMenuEntry("FLAT", glFLAT);
	glutAddMenuEntry("SMOOTH", glSMOOTH);
	glutAddMenuEntry("[p] Andar/Flotar", ANDAR_FLOTAR);
	glutAddMenuEntry("[x] Animacion", ANIMAR_CAMARA);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutTimerFunc(100,timer,1);
	
	glutMainLoop();
	return 0;
}


