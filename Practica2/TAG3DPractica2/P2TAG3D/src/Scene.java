/**
 * Raytracing scene.
 */
public class Scene {
	/**
	 * Camera.
	 */
	public Camera				camera;
	/**
	 * Object surfaces.
	 */
	public Surface[]			surfaces;
	/**
	 * Lights.
	 */
	public Light[]				lights;
	/**
	 * Texturas. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public ColorImage[]			textimg;
	/**
	 * Nombres de las texturas. ATRIBUTO A„ADIDO POR NOSOTROS
	 */
	public Texture textures[] = null; 

	/**
	 * Default constructor.
	 * Scene is initialized in the Loader.
	 */
	public Scene() {
		camera = new Camera();
		surfaces = new Surface[0];
		lights = new Light[0];
	}

	/**
	 * Cargar las texturas en memoria.
	 * Use by the parser.
	 * METODO CREADO POR NOSOTROS
	 */
	public void initFromParser() {
		if (textures != null){
			// Leer los archivos de texturas
			this.textimg = new ColorImage[this.textures.length];
			for (int i=0; i<textures.length; i++ ){
				this.textimg[i] = textures[i].loadTexture();
			}
			for (int i=0; i<surfaces.length; i++){
				if (surfaces[i].material.hasTexture())
					surfaces[i].material.texImg = this.textimg[surfaces[i].material.texture];
				if (surfaces[i].hasBump())
					surfaces[i].setBumpMapping(this.textimg[surfaces[i].bTexture]);
			}
		}
	}

	/**
	 * Intersect the given ray with this scene storing the result in intersection and
	 * result true if there was an intersection.
	 */
	boolean intersect(Ray ray, Intersection intersection) {
		// Calcular la intersecci—n entre el rayo y todos los objetos
		// Quedarse con el m‡s cercano (o devolver false si no hay)
		boolean hasInter = false;
		Intersection inter = new Intersection();
		for (int i=0; i<surfaces.length; i++){
			if (surfaces[i].intersectBBox(ray)){ // Intersecci—n con la BBox
				if (surfaces[i].intersect(ray, inter)){ // Hay interseccion!
					if ( !hasInter || inter.distance < intersection.distance){ // Es mas cercana que la ultima
						hasInter = true;
						intersection.set(inter);
					}				
				}
			}
		}
		return (hasInter);
	}

	/**
	 * Intersecciones producidas por la refracci—n
	 * @param ray
	 * @param s
	 * @param mesh
	 * @param intersection
	 * @return
	 */ 
	boolean intersectRefr(Ray ray, Surface s, Mesh mesh, Intersection intersection) {
		boolean hasInter = false;
		Intersection inter = new Intersection();
		if(mesh == null){
			if (s.intersectRefr(ray, s, inter)){ // Hay interseccion!
				if ( !hasInter || inter.distance < intersection.distance){ // Es mas cercana que la ultima
					hasInter = true;
					intersection.set(inter);
				}				
			}
		}else{ // Es una malla
			if (mesh.intersectRefr(ray, s, inter)){ // Hay interseccion!
				if ( !hasInter || inter.distance < intersection.distance){ // Es mas cercana que la ultima
					hasInter = true;
					intersection.set(inter);
				}				
			}
		}
		return (hasInter);
	}



	/**
	 * METODO CREADO POR NOSOTROS
	 */
	public String toString(){
		String out = "SCENE:\n";
		out += "Lights:\t";
		for (int i=0; i<lights.length; i++)
			out += lights[i].toString()+"\t";
		out += "\n";
		out += "Surfaces:\n";
		for (int i=0; i<surfaces.length; i++){
			String name = surfaces[i].toString();
			String mat = surfaces[i].material.toString();
			out += "\t"+name+"\t"+mat+"\n";
		}
		out += "Resolution:\t";
		out += camera.xResolution+"\t"+camera.yResolution+"\n";

		return out;
	}
}
