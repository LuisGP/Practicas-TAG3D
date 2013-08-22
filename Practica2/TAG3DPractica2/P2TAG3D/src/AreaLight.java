/**
 * Luz de area.
 * CLASE CREADA POR NOSOTROS
 */
public class AreaLight extends Light {
	/**
	 * Light position.
	 */
	public Vec3						position;
	/**
	 * Nœmero de muestras.
	 */
	public int 						nsamples = 3;
	/**
	 * Puntos que aportan luz.
	 */
	private Vec3 puntos[]=null;
	/**
	 * Forma de la luz.
	 */
	public String shape=null;
	/**
	 * Tama–o de la luz.
	 */
	public double size = -1;
	/**
	 * La forma de la luz
	 */
	public Surface object;

	/**
	 * Inicializa los par‡metros
	 */
	public void initFromParser(){
		super.initFromParser();
		if (this.position == null){
			System.err.println("AreaLight: Falta la posici—n de la luz <position>");
			System.exit(1);
		}
		if (object == null && shape == null){
			System.err.println("AreaLight: Falta la forma de la luz <object> — <shape>");
			System.exit(1);
		}
		if (object == null){ // Luz no definida
			if (size < 0){
				System.err.println("AreaLight: Falta el tama–o de la luz <size>");
				System.exit(1);
			}
			if (shape.toLowerCase().equals("sphere")){
				Sphere sp = new Sphere();
				sp.material = new Lambert();
				((Lambert)sp.material).diffuse = new Color(1,1,1);
				sp.material.initFromParser();
				sp.position = this.position;
				sp.radius = this.size/2.0;
				sp.initFromParser();
				this.object = sp;
			}else if (shape.toLowerCase().equals("quad")){
				Quad q = new Quad();
				q.material = new Lambert();
				((Lambert)q.material).diffuse = new Color(1,1,1);
				q.material.initFromParser();
				double s2 = this.size/2.0;
				q.v0 = this.position.sub(new Vec3(-s2, 0, -s2));
				q.v1 = this.position.sub(new Vec3(-s2, 0, s2));
				q.v2 = this.position.sub(new Vec3(s2, 0, s2));
				q.v3 = this.position.sub(new Vec3(s2, 0, -s2));
				q.initFromParser();
				this.object = q;
			}else{ // S—lo reconocidos esferas y quads como tipos de luz
				System.err.println("AreaLight: S—lo se reconocen Sphere y Quad como <shape>");
				System.exit(1);
			}
		}else{ // Luz definida
			if (this.object == null){
				this.object.material = new Lambert();
				((Lambert)this.object.material).diffuse = new Color(1,1,1);
			}
		}
		if (this.nsamples<3)
			this.nsamples=3;
	}

	/**
	 * Default constructor.
	 *
	 */
	public AreaLight() {
		super();
		position = new Vec3();
		object = null;
	}

	/**
	 * Constructor
	 * @param surface
	 * @param nsamples
	 */
	public AreaLight(Surface surface, int nsamples) {
		super();
		position = new Vec3();
		object = surface;
		this.nsamples = nsamples;
	}
	
	/**
	 * Calcula las distintas luces en las que se divide
	 */
	private void calculaPuntos(){		
		puntos = new Vec3[nsamples*nsamples*nsamples];

		double dx, dy, dz;
		dx = object.vmin.x - object.vmax.x;
		dy = object.vmin.y - object.vmax.y;
		dz = object.vmin.z - object.vmax.z;

		double ix, iy, iz;
		ix = dx / (nsamples-1);
		iy = dy / (nsamples-1);
		iz = dz / (nsamples-1);

		Vec3 pos = new Vec3();
		Vec3 inc;
		int ind;

		for(int i = 0; i < nsamples; i++){
			for(int j = 0; j < nsamples; j++){
				for(int k = 0; k < nsamples; k++){
					inc = new Vec3(ix*i,iy*j,iz*k);
					pos.set(object.vmin.sub(inc));
					ind = k+j*nsamples+i*nsamples*nsamples;

					puntos[ind] = new Vec3(pos);
				}				
			}
		}
	}

	/**
	 * Calcula la direcci—n entre el punto y la luz
	 */
	public Vec3 computeLightDirection(Vec3 surfacePoint) {
		Vec3 ldir = new Vec3();
		ldir = position.sub(surfacePoint);
		ldir.setToNormalize();
		return ldir;
	}

	/**
	 * Calcula las direcciones para las sombras
	 */
	public Vec3[] computeLightDirectionShadow(Vec3 surfacePoint) {
		Vec3 dirs[] = new Vec3[nsamples*nsamples*nsamples];

		Ray ray;
		Vec3 dir;
		int ind;
		Intersection inter = new Intersection();

		if(puntos == null)
			calculaPuntos();

		// Distancia entre 2 puntos
		for(int i = 0; i < nsamples; i++){
			for(int j = 0; j < nsamples; j++){
				for(int k = 0; k < nsamples; k++){
					ind = k+j*nsamples+i*nsamples*nsamples;
					dir = puntos[ind].sub(surfacePoint).normalize();
					ray = new Ray(surfacePoint,dir);
					if(this.object.intersect(ray, inter)){
						dirs[ind] = new Vec3(dir);
					}else{
						dirs[ind] = null;
					}
				}				
			}
		}

		return dirs;
	}

	/**
	 * Calcula la intensidad de la luz en un punto
	 */
	public Color computeLightIntensity(Vec3 surfacePoint) {
		Vec3 ldir = position.sub(surfacePoint);
		double dist = ldir.length();

		double fallout = 1-(dist/computeShadowDistance(surfacePoint));

		fallout = Math.pow(fallout, 2);

		return this.intensity.scale(fallout);
	}

	/**
	 * Calcula la distancia m‡xima de oclusi—n para sombras
	 */
	public double computeShadowDistance(Vec3 surfacePoint) {
		return surfacePoint.sub(this.position).length();
	}

	/**
	 * Para las estad’sticas
	 */
	public String toString(){
		return "AreaLight("+nsamples+")";
	}
}
