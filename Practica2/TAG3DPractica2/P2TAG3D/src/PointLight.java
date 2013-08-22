/**
 * Point light source
 */
public class PointLight extends Light {
	/**
	 * Light position.
	 */
	public Vec3						position;
	
	/**
	 * Default constructor.
	 *
	 */
	public PointLight() {
		super();
		position = new Vec3();
	}

	/**
	 * Compute light direction.
	 * @param surfacePoint Surface position.
	 */
	public Vec3 computeLightDirection(Vec3 surfacePoint) {
		Vec3 ldir = new Vec3();
		ldir = position.sub(surfacePoint);
		ldir.setToNormalize();
		return ldir;
	}

	public Vec3[] computeLightDirectionShadow(Vec3 surfacePoint) {
		Vec3 ldir[] = new Vec3[1];
		ldir[0] = position.sub(surfacePoint);
		ldir[0].setToNormalize();
		return ldir;
	}
	
	/**
	 * Compute light internsity.
	 * @param surfacePoint Surface position.
	 */
	public Color computeLightIntensity(Vec3 surfacePoint) {
		Vec3 ldir = position.sub(surfacePoint);
		double dist = ldir.length();
		
		double fallout = 1-(dist/computeShadowDistance(surfacePoint));
		
		fallout = Math.pow(fallout, 2);
		
		return this.intensity.scale(fallout);
	}

	/**
	 * Compute shadow distance, i.e. max distance (in world space) a ray should 
	 * travel when checking for shadows. 
	 * @param surfacePoint Surface position.
	 */
	public double computeShadowDistance(Vec3 surfacePoint) {
		return surfacePoint.sub(this.position).length();
	}
	
	/**
	 * Para las estad’sticas.
	 * METODO CREADO POR NOSOTROS
	 */
	public String toString(){
		return "PointLight";
	}
	
	/**
	 * Comprobar par‡metros.
	 */
	public void initFromParser() {
		super.initFromParser();
		// Comprobar par‡metros
		if (this.position == null){
			System.err.println("PointLight: Falta la intensidad de la luz <intensity>");
			System.exit(1);
		}
	}
}
