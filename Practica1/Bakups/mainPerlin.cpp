#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <perlin.h>

// Based on http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
// and http://pgrafica.webideas4all.com/perlin.html

using namespace std;


/********************************************************************

	Programa principal

/********************************************************************/


int mainPerlin ( int argc, char* argv[] ) {
	int** tabla;
	int size;
	float persistence;
	int octaves;
	int seed;
	char* output;
	
	if (argc > 5){
		size = atoi(argv[1]);
	    octaves = atoi(argv[2]);
    	persistence = atof(argv[3]) <= 1 ? atof(argv[3]) : 1;
		seed = atoi(argv[4]);
		output = argv[5];
	}else{
		cout << "[1] : dimensiones del lado de la imagen" << endl;
		cout << "[2] : numero de octavas a combinar[1,20]" << endl;
		cout << "[3] : persistencia [0,1]" << endl;
		cout << "[4] : semilla" << endl;
		cout << "[5] : fichero de salida (incluir .pgm)" << endl;
		cout << "[6] : (opcional) normalizar si vale 'n'" << endl;	
		exit(1);
	}
	
	cerr << size << " " << octaves << " " << seed << " " << persistence << " " << argv[5] << endl;
	tabla = genPerlinNoise(size, octaves, seed, persistence);
	
	if (argc > 6)
		if (argv[6][0] == 'n')
			normalizarInt(tabla,size);
	
	writePGMint(argv[5], tabla, size);

	freeTablaInt(tabla, size);
	exit(0);
}

