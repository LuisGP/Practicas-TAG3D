import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

/**
 * Mesh.
 * CLASE CREADA POR NOSOTROS
 */
public class Mesh extends Surface {
	/**
	 * Posición de la referencia de la malla.
	 */
	public Vec3					position;
	/**
	 * Escala.
	 */
	public Vec3					size=new Vec3(1,1,1);
	/**
	 * Lista de vertices.
	 */
	public List<Vec3>			vs;
	/**
	 * Lista de triangulos.
	 */
	public List<TriangleInd>	ts;
	/**
	 * Lista de coordenadas de textura.
	 */
	public List<Vec3>			uvs;
	/**
	 * Lista de normales.
	 */
	public List<Vec3>			normals;
	/**
	 * Interpolar o no las normales.
	 */
	public double				breakang=0.0;
	/**
	 * Nombre del archivo OFF.
	 */
	public String				filename;
	/**
	 * Nombre del archivo con las coord. UV.
	 */
	public String				UVfile;
	/**
	 * Numero de vertices.
	 */
	public int				nvertices;
	/**
	 * Numero de triangulos.
	 */
	public int				ntriangles;
	/**
	 * La última coordenada UV leída.
	 */
	private int				lastUV=0;
	/**
	 * Invertir las normales.
	 */
	public int				invNormal=0;


	/**
	 * Calcula a qué triángulos pertenece cada vértice
	 * @return
	 */
	private List<ArrayList<Integer>> calcAds(){
		// Método que calcula las adyacencias
		List<ArrayList<Integer>> l = new ArrayList<ArrayList<Integer>>();
		// Comprobar para todos los vértices
		for (int i=0; i<this.nvertices; i++){
			ArrayList<Integer> ts = new ArrayList<Integer>();
			// Mirar en todos los triángulos
			for (int j=0; j<this.ntriangles; j++){
				TriangleInd t = this.ts.get(j);
				if (t.in(i)){ // Condicion de pertenencia
					ts.add(j);
				}
			}
			l.add(ts);
		}
		return l;
	}

	/**
	 * Calcula las normales por vértice
	 * @param ads
	 */
	private void calcNormales(List<ArrayList<Integer>> ads){
		// Método que calcula las normales en los vértices 
		this.normals = new ArrayList<Vec3>();

		// Calcular para todos los vértices
		for (int i=0; i<this.nvertices; i++){
			ArrayList<Integer> vad = ads.get(i);
			Vec3 normal = new Vec3();

			// Mirar en todos los triángulos
			for (int j=0; j<vad.size(); j++){
				TriangleInd t = this.ts.get(vad.get(j));
				normal.setToAdd(t.normal);
			}
			this.normals.add(normal.normalize());
		}
	}

	/**
	 * Inicializar y comprobar.
	 * Use by the parser.
	 */
	public void initFromParser() {
		this.vmin = new Vec3().scale(Double.MAX_VALUE);
		this.vmax = new Vec3().scale(Double.MIN_VALUE);
		if (this.position == null){
			System.err.println("Mesh: Falta la posición <position>");
			System.exit(1);
		}
		if (this.material.hasTexture() || this.hasBump()){
			try {

				if (this.UVfile == null){
					System.err.println("Mesh: Falta el archivo de UVs <UVfile>");
					System.exit(1);
				}
				this.readUV(this.UVfile);
			} catch (IOException e) {
				e.printStackTrace(); 
			}
		} 

		try {
			if (this.filename == null){
				System.err.println("Mesh: Falta el archivo de la malla OFF <filename>");
				System.exit(1);
			}
			this.readOFF(this.filename, this.position);
		} catch (IOException e) {
			e.printStackTrace();
		}

		this.calcNormales(this.calcAds());
	}

	/**
	 * Lee las coordenadas UV de los vértices desde el fichero
	 * @param vfile
	 * @throws IOException 
	 */
	private void readUV(String vfile) throws IOException {
		this.uvs = new ArrayList<Vec3>();
		File file = new File(vfile);
		FileReader fr = null;
		try {
			fr = new FileReader(file);
		} catch (FileNotFoundException e) {
			System.err.println("Mesh: "+e);
			System.exit(1);
		}
		BufferedReader reader = new BufferedReader(fr);
		String line = null;

		int ln = 0;

		while ((line = reader.readLine()) != null) {
			ln++;
			StringTokenizer tok = new StringTokenizer(line);
			if (!line.equals("")){
				Vec3 uv = new Vec3();
				uv.x = Double.parseDouble(tok.nextToken());
				uv.y = Double.parseDouble(tok.nextToken());
				this.uvs.add(uv);
			}
		}
		reader.close();
	}

	/**
	 * Interpreta la cabecera del archivo .off
	 * Inicializa vértices y triángulos
	 * @param line
	 */
	private void leerCabecera( String line ){
		StringTokenizer tok = new StringTokenizer(line);
		int i = 0;
		while (tok.hasMoreTokens()) {
			switch (i){
			case 0 :
				this.nvertices = Integer.parseInt(tok.nextToken());
				break;

			case 1 :
				this.ntriangles = Integer.parseInt(tok.nextToken()); 
				this.ts = new ArrayList<TriangleInd>(this.ntriangles);
				break;

			default:
				tok.nextToken();
			break;
			}
			i++;
		}
		if ( i < 2 )
			System.err.println("No hay suficientes valores: [" + line + "]");
	}

	/**
	 * Crea un vértice a partir de su línea
	 * @param s
	 */
	private Vec3 parseV ( String s ){
		Vec3 v = new Vec3();
		StringTokenizer tok = new StringTokenizer(s);
		int i = 0;
		while (tok.hasMoreTokens()) {
			if (i >= 3) {
				System.err.println("Demasiados valores: [" + s + "]");
			}
			switch(i){
			case 0:
				v.x = Double.parseDouble(tok.nextToken());
				break;
			case 1:
				v.y = Double.parseDouble(tok.nextToken());				
				break;
			case 2:
				v.z = Double.parseDouble(tok.nextToken());
				break;
			}
			i++;
		}
		if ( i < 3 )
			System.err.println("No hay suficientes valores: [" + s + "]");

		v.x *= this.size.x;
		v.y *= this.size.y;
		v.z *= this.size.z;
		v.setToAdd(this.position);
		if (v.x < vmin.x) vmin.x = v.x;
		if (v.x > vmax.x) vmax.x = v.x;
		if (v.y < vmin.y) vmin.y = v.y;
		if (v.y > vmax.y) vmax.y = v.y;
		if (v.z < vmin.z) vmin.z = v.z;
		if (v.z > vmax.z) vmax.z = v.z;
		return v;
	}

	/**
	 * Crea un triángulo a partir de la línea correspondiente del fichero .off
	 * @param line
	 * @return
	 */
	private TriangleInd readTriang(String line, List<Vec3> vs2) {
		StringTokenizer tok = new StringTokenizer(line);
		int i = 0;
		int ind[] = new int[3];

		while (tok.hasMoreTokens()) {
			switch (i){
			case 0 :
				int n = Integer.parseInt(tok.nextToken());
				if (n != 3)
					System.err.println("NO SON TRIANGULOS: n = "+n);
				break;

			case 1 :
			case 2 :
			case 3 :
				ind[i-1] = Integer.parseInt(tok.nextToken());
				break;

			default:
				tok.nextToken();
			break;
			}
			i++;
		}
		// Crear el triángulo real
		TriangleInd t = new TriangleInd();
		t.material = this.material;
		t.i0 = ind[0];
		t.i1 = ind[1];
		t.i2 = ind[2];
		t.v0 = vs2.get(t.i0);
		t.v1 = vs2.get(t.i1);
		t.v2 = vs2.get(t.i2);	
		if (this.material.hasTexture() || this.hasBump()){
			t.compUVs = true;
			t.v0t = this.uvs.get(lastUV);
			t.v1t = this.uvs.get(lastUV+1);
			t.v2t = this.uvs.get(lastUV+2);
			lastUV += 3;
		}else{
			t.compUVs = false;
		}

		t.initFromParser();
		return t;
	}

	/**
	 * Genera la malla a partir del archivo .off indicado
	 * @param fname
	 * @throws IOException 
	 */
	private void readOFF ( String fname, Vec3 pos ) throws IOException{
		File file = new File(fname);
		FileReader fr = null;
		try {
			fr = new FileReader(file);
		} catch (FileNotFoundException e) {
			System.err.println("Mesh: "+e);
			System.exit(1);
		}
		BufferedReader reader = new BufferedReader(fr);
		String line = null;
		int ln = 0;
		int vreaded = 0;
		int treaded = 0;
		vs = new ArrayList<Vec3>();

		while ((line = reader.readLine()) != null) {
			ln++;
			// Formato OFF
			if (ln == 1)
				continue;
			// Num de vértices y triángulos
			if (ln == 2){
				leerCabecera(line);
				continue;
			}
			// Líneas vacías
			if (line.length() == 0)
				continue;
			// Lectura de vértices
			if (vreaded < this.nvertices){
				vs.add(parseV(line));
				vreaded++;
				continue;
			}
			// Lectura de triángulos
			if (treaded < this.ntriangles){
				this.ts.add(readTriang(line,vs));
				treaded++;
			}
		}
		reader.close();
	}

	/**
	 * Intersect the given ray with this scene storing the result in intersection and
	 * result true if there was an intersection.
	 */
	public boolean intersect(Ray ray, Intersection intersection) {
		// Calcular el triángulo de intersección e interpolar la normal
		boolean hasInter = false;
		int nt = -1;
		Intersection inter = new Intersection();
		for (int i=0; i<this.ntriangles; i++){
			if (this.ts.get(i).intersect(ray, inter)){ // Hay interseccion!
				if ( !hasInter || inter.distance < intersection.distance){ // Es mas cercana que la ultima
					hasInter = true;
					nt = i;
					intersection.set(inter);
					intersection.mesh = this;
				}				
			}
		}
		if (hasInter && this.breakang>0)
			intersection.normal = interpNormal(nt, intersection.position);
		if (hasInter && this.invNormal == 1)
			intersection.normal = intersection.normal.negate();
		if(hasInter && this.hasBump()){
			intersection.normal = this.applyBump(intersection.normal, intersection.uv);
		}
		return (hasInter);
	}

	/**
	 * Intersecciones por refracción
	 */
	public boolean intersectRefr(Ray ray, Surface s, Intersection intersection) {
		// Calcular el triángulo de intersección e interpolar la normal
		boolean hasInter = false;
		int nt = -1;
		Intersection inter = new Intersection();
		for (int i=0; i<this.ntriangles; i++){
			if (this.ts.get(i) != s && this.ts.get(i).intersectRefr(ray, s, inter)){ // Hay interseccion!
				if ( !hasInter || inter.distance < intersection.distance){ // Es mas cercana que la ultima
					hasInter = true;
					nt = i;
					intersection.set(inter);
				}				
			}
		}
		if (hasInter && this.breakang>0)
			intersection.normal = interpNormal(nt, intersection.position);

		if (hasInter && this.invNormal == 1)
			intersection.normal = intersection.normal.negate();

		if(hasInter && this.hasBump()){
			intersection.normal = this.applyBump(intersection.normal, intersection.uv);
		}
		return (hasInter);
	}

	/**
	 * Interpola la normal según los vértices del triángulo (coord. baricéntricas)
	 * @param nt
	 * @param position
	 * @return
	 */
	private Vec3 interpNormal(int nt, Vec3 position) {
		// metodo que calcule la normal interpolada
		Vec3 n = null;
		Vec3 coord = null;
		TriangleInd t = this.ts.get(nt);

		Vec3[] ns = new Vec3[3];
		ns[0] = this.normals.get(t.i0);
		ns[1] = this.normals.get(t.i1);
		ns[2] = this.normals.get(t.i2);

		/*
		 * Calcular las coordenadas baricéntricas
		 */
		coord = t.barCoords(position);

		n = ns[0].scale(coord.x);
		n.setToAdd(ns[1].scale(coord.y));
		n.setToAdd(ns[2].scale(coord.z));
		return n.normalize();
	}

	/**
	 * Devolver cilindros que representen las normales
	 * @return
	 */
	public Surface[] displayNormals(){
		Lambert mat = new Lambert();
		mat.diffuse = new Color(1,1,1);
		Surface cils[] = new Surface[this.normals.size()]; 
		for (int i=0; i<this.normals.size(); i++){
			Vec3 n = this.normals.get(i);
			Cylinder c = new Cylinder();
			c.center1 = this.vs.get(i);
			c.center2 = c.center1.add(n.scale(0.1));
			c.radius = 0.01;
			c.material = mat;
			c.initFromParser();
			cils[i] = c;
		}
		return cils;
	}

	/**
	 * Representación de la malla. Para las estadísticas
	 */
	public String toString(){
		return "Mesh("+this.nvertices+"/"+this.ntriangles+")";
	}
}
